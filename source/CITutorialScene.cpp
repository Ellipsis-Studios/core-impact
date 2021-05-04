//
//  CITutorialScene.cpp
//  CoreImpact
//
//  Created by Kevin Sun on 4/29/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CITutorialScene.h"

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <numeric>
#include "CICollisionController.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Animations
/** Seconds per frame for Game Scene background animation */
#define BACKGROUND_SPF .066
/** Game Scene background animation start frame */
#define BACKGROUND_START 0
/** Game Scene background animation end frame */
#define BACKGROUND_END 240


#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets                The (loaded) assets for this game mode
 * @param networkMessageManager The reference to network message manager
 * @param gameSettings          The settings for the current game
 * @param playerSettings        The settings for the current player
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool TutorialScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
    const std::shared_ptr<GameSettings>& gameSettings,
    const std::shared_ptr<PlayerSettings>& playerSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= CONSTANTS::SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler and managers
    _assets = assets;
    _input.init(getBounds());
    srand(time(NULL));
    
    // Set up tutorial system
    _tutorialStage = -1;
    _nextTutorialStage = 0;
    _tutorialTimer = 0;
    // Set the game update manager and network message managers
    _gameUpdateManager = GameUpdateManager::alloc();
    _networkMessageManager = networkMessageManager;
    _networkMessageManager->setGameUpdateManager(_gameUpdateManager);
    
    // Acquire the scene built by the asset loader and resize it the scene
    auto scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    // Get the scene components.
    _allSpace  = _assets->get<scene2::SceneNode>("game_field");
    _farSpace = std::dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_field_far"));
    _nearSpace = _assets->get<scene2::SceneNode>("game_field_near");
    _tutorialText  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("game_tutorial"));
    _tutorialText->setVisible(true);
    
    // create the win scene
    _winScene = WinScene::alloc(assets, dimen);

    // Create the planet model
    _planet = PlanetModel::alloc(dimen.width / 2, dimen.height / 2, CIColor::getNoneColor(),
        CONSTANTS::MAX_PLANET_LAYERS, gameSettings->getGravStrength(), gameSettings->getPlanetMassToWin());
    auto coreTexture = _assets->get<Texture>("core");
    auto ringTexture = _assets->get<Texture>("innerRing");
    auto unlockedTexture = _assets->get<Texture>("unlockedOuterRing");
    auto lockedTexture = _assets->get<Texture>("lockedOuterRing");
    auto planetProgressTexture = _assets->get<Texture>("playerProgress");
    _planet->setTextures(coreTexture, ringTexture, unlockedTexture, lockedTexture, planetProgressTexture);

    _draggedStardust = NULL;
    _stardustContainer = StardustQueue::alloc(CONSTANTS::MAX_STARDUSTS, coreTexture);

    // Game settings
    _gameSettings = gameSettings;
    // Player settings
    _playerSettings = playerSettings;
    
    // update stardustProb to match colorCount
    for (int i = 0; i < gameSettings->getColorCount(); i++) {
        _stardustProb[i] = BASE_PROBABILITY_SPACE;
    }
    CIColor::setNumColors(gameSettings->getColorCount());

    addChild(scene);
    addChild(_planet->getPlanetNode());
    addChild(_stardustContainer->getStardustNode());
    
    std::vector<string> opponentNames = networkMessageManager->getOtherNames();
    _opponentPlanets.resize((int) opponentNames.size());
    for (int ii = 0; ii < _opponentPlanets.size(); ii++) {
        if (opponentNames[ii] == "") {
            continue;
        }
        CILocation::Value location = CILocation::Value(ii+1);
        cugl::Vec2 pos = CILocation::getPositionOfLocation(location, dimen);
        std::shared_ptr<OpponentPlanet> opponent = OpponentPlanet::alloc(pos.x, pos.y, CIColor::getNoneColor(), location);
        opponent->setTextures(_assets->get<Texture>("opponentProgress"), _assets->get<Texture>("fog"), dimen);
        opponent->setName(opponentNames[ii], assets->get<Font>("saira20"));
        addChild(opponent->getOpponentNode());
        _opponentPlanets[ii] = opponent;
    }
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void TutorialScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _active = false;
    }
    _assets = nullptr;
    _gameUpdateManager = nullptr;
    _networkMessageManager = nullptr;
    _allSpace = nullptr;
    _farSpace = nullptr;
    _nearSpace = nullptr;
    _tutorialText = nullptr;
    _stardustContainer = nullptr;
    _planet = nullptr;
    _draggedStardust = NULL;
    _opponentPlanets.clear();
    _winScene = nullptr;
}


#pragma mark -
#pragma mark Gameplay Handling

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void TutorialScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= CONSTANTS::SCENE_WIDTH/dimen.width;
    
    _input.update(timestep);
    
    // Handle counting down then switching to loading screen
    if (_tutorialStage == 13) {
        if (!_winScene->displayActive()) {
            CULog("Game won.");
            _winScene->setWinner(0, 0, "");
            _winScene->setDisplay(true);
        }
        else if (_winScene->goBackToHome()) {
            // handle resetting game
            _winScene->setDisplay(false);
            setActive(false);
        }
        return;
    }

    _timeElapsed += timestep;
    if (_timeElapsed > BACKGROUND_SPF) {
        unsigned int bkgrdFrame = _farSpace->getFrame();
        _timeElapsed = 0;
        bkgrdFrame = (bkgrdFrame == BACKGROUND_END) ? BACKGROUND_START : bkgrdFrame + 1;
        _farSpace->setFrame(bkgrdFrame);
    }
    _stardustContainer->update(timestep);
    addStardust(dimen);

    collisions::checkForCollision(_planet, _stardustContainer, timestep);
    collisions::checkInBounds(_stardustContainer, dimen);
    collisions::checkForCollisions(_stardustContainer);
    updateDraggedStardust();
    
    /** Tutorialization Phase */
    // Advance Tutorial Stage
    if (_tutorialStage != _nextTutorialStage){
        _tutorialStage = _nextTutorialStage;
        switch (_tutorialStage) {
            case 0:
                _tutorialText->setText("Drag the stardust to your core");
                _tutorialTimer = 60;
                break;
            case 1:
                _tutorialTimer = 30;
            case 2:
                _tutorialText->setText("Stardust of different colors reduce your progress");
                break;
            case 3:
                _tutorialText->setText("Hold down on the core to lock in a layer");
                break;
            case 4:
                _tutorialText->setText("Continue building up your core with a new color");
                break;
            case 5: {
                _tutorialText->setText("Another planet appeared!");
                _tutorialTimer = 60;
                std::shared_ptr<OpponentPlanet> planet = OpponentPlanet::alloc(0, dimen.height, CIColor::Value((_planet->getColor()+2)%4), CILocation::TOP_LEFT);
                planet->setTextures(_assets->get<Texture>("opponentProgress"), _assets->get<Texture>("fog"), dimen);
                planet->setName("Opponent", _assets->get<Font>("saira20"));
                planet->setMass(55);
                addChild(planet->getOpponentNode());
                _opponentPlanets[0] = planet;
            } break;
            case 6: {
                _tutorialText->setText("Hey, they threw some stardust at you!");
                _tutorialTimer = 200;
            } break;
            case 7: {
                _tutorialText->setText("Send some back their way!");
            } break;
            case 8: {
                _tutorialText->setText("Nice hit, here's a reward!");
                _tutorialTimer = 200;
                _opponentPlanets[0]->startHitAnimation();
            } break;
            case 9: {
                _tutorialText->setText("Keep building up your planet!");
            } break;
            case 10: {
                _tutorialText->setText("When you lock in, there's a power up");
            } break;
            case 11: {
                _tutorialText->setText("Let's try out a new power up this time!");
            } break;
            case 12: {
                _tutorialText->setText("Cool! Finish the third layer to win the game");
                _tutorialText->setPositionY(_tutorialText->getPositionY()+80);
            } break;
            default:
                break;
        }
    }
    if (_tutorialTimer > 0){
        _tutorialTimer--;
    }
    
    switch (_tutorialStage) {
        case 0:
            // Once the player has collected 3 dots
            if (_planet->getMass() > 45){
                _nextTutorialStage++;
            }
            break;
        case 1:
            // Once a few different colored dots have spawned
            if (_tutorialTimer == 0){
                _nextTutorialStage++;
            }
            break;
        case 2:
            // Once the player has collected 3 dots
            if (_planet->getMass() > 65){
                _nextTutorialStage++;
            }
            break;
        case 3:
            // Once the player can lock in
            if (_planet->getMass() <= 65) {
                _nextTutorialStage--;
            }
            else if (_planet->getNumLayers() > 1) {
                _nextTutorialStage++;
            }
            break;
        case 4:
            // Build up a new color
            if (_planet->getCurrLayerProgress() > 1) {
                _nextTutorialStage++;
            }
            break;
        case 5:
            if (_tutorialTimer == 0 || _stardustContainer->size() < 1){
                _nextTutorialStage++;
            } break;
        case 6:
            if (_tutorialTimer == 0){
                _nextTutorialStage++;
            } break;
        case 7:
            // Throw a particle at the other planet
            for (size_t ii = 0; ii < _stardustContainer->size(); ii++) {
                // This returns a reference
                StardustModel* stardust = _stardustContainer->get(ii);
                if (stardust != nullptr) {
                    Vec2 stardustPos = stardust->getPosition();
                    Vec2 stardustVel = stardust->getVelocity();
                    if ((stardustPos.x < 10 && stardustPos.y > dimen.height + 10) &&
                        (stardustVel.x < 0 && stardustVel.y > 0)){
                        _nextTutorialStage++;
                        break;
                    }
                }
            }
            break;
        case 8:
            if (_tutorialTimer == 0){
                _nextTutorialStage++;
            }
            break;
        case 9:
            if (_planet->getCurrLayerProgress() > 3) {
                _nextTutorialStage++;
            }
            break;
        case 10:
            if (_planet->getCurrLayerProgress() > 4 && _tutorialTimer == 0) {
                _nextTutorialStage++;
            } else if (_planet->getCurrLayerProgress() <= 3) {
                _nextTutorialStage--;
            }
            break;
        case 11:
            if (_planet->getNumLayers() > 2) {
                _nextTutorialStage++;
            } break;
        case 12:
            if (_planet->getCurrLayerProgress() > 4 && _planet->getNumLayers() > 2){
                CULog("Tutorial Complete.");
                _nextTutorialStage++;
                _tutorialText->setVisible(false);
                _winScene->setWinner(0, 0, "");
                _winScene->setDisplay(true);
            } break;
        default:
            break;
    }
    
    if (collisions::checkForCollision(_planet, _input.getPosition())) {
        CIColor::Value planetColor = _planet->getColor();
        if (_planet->lockInLayer(timestep)) {
            // Layer Locked In
            CULog("LAYER LOCKED IN");
            _stardustContainer->addToPowerupQueue(planetColor, true);
        }
    } else if (_planet->isLockingIn()) {
        _planet->stopLockIn();
    }
    
    _planet->update(timestep);

    for (int ii = 0; ii < _opponentPlanets.size() ; ii++) {
        std::shared_ptr<OpponentPlanet> opponent = _opponentPlanets[ii];
        if (opponent != nullptr) {
            if (opponent->getMass() < _planet->getMass()*0.6){
                opponent->setMass(_planet->getMass()*0.6);
            }
            opponent->update(timestep);
        }
    }
    
    processSpecialStardust(dimen, _stardustContainer);
}

/**
 * This method updates the dragged stardust.
 *
 * It selects or deselects a dragged stardust stardust if applicable,
 * and updates the velocity a selected stardust if there is one.
 */
void TutorialScene::updateDraggedStardust() {
    if (_input.fingerDown()) {
        if (_draggedStardust == NULL) {
            _draggedStardust = collisions::getNearestStardust(_input.getPosition(), _stardustContainer);
        }
        // this is structured like this to update a recently dragged stardust
        if (_draggedStardust != NULL) {
            float sdRadius = _stardustContainer->getStardustRadius();
            collisions::moveDraggedStardust(_input.getPosition(), _draggedStardust, sdRadius);
        }
    } else if (!_input.fingerDown() && _draggedStardust != NULL) {
        // finger just released, flick dragged stardust
        Vec2 newVelocity = _draggedStardust->getVelocity() + _input.getPrevVelocity();
        _draggedStardust->setVelocity(newVelocity);
        _draggedStardust = NULL;
    }
}

/**
 * This method attempts to add a stardust to the players screen.
 *
 *  Whether a stardust is added is determined by how many stardust are already on the screen.
 *  The color of the added stardust is determined by how close to finishing the player is.
 *
 *  @param bounds the bounds of the game screen
 */
void TutorialScene::addStardust(const Size bounds) {
    if (_stardustContainer->size() == CONSTANTS::MAX_STARDUSTS) {
        return;
    }
    
    CIColor::Value c = CIColor::getRandomColor();
    int cornerProb[] = {10,10,10,10};
    switch (_tutorialStage) {
        case 0:
            if (_stardustContainer->size() > 0 || _tutorialTimer != 0) {
                return;
            }
            c = CIColor::red;
            break;
            
        case 1:
            if (_stardustContainer->size() > 1) {
                return;
            }
            while (c == CIColor::red){
                c = CIColor::getRandomColor();
            }
            break;
            
        case 2:
            if (_stardustContainer->size() > 0) {
                return;
            }
            break;
            
        case 3: return;
        case 6:
            if (_stardustContainer->size() > 2) {
                return;
            }
            while (c == _planet->getColor()){
                c = CIColor::getRandomColor();
            }
            cornerProb[0] = INT_MAX;
            break;
        case 8:
            if (_stardustContainer->size() > 2) {
                return;
            }
            if (_planet->getColor() == CIColor::getNoneColor()){
                c = CIColor::yellow;
            } else {
                c = _planet->getColor();
            }
            cornerProb[0] = INT_MAX;
            break;
        default:
            size_t spawn_probability = CONSTANTS::BASE_SPAWN_RATE + (_stardustContainer->size() * CONSTANTS::BASE_SPAWN_RATE);
            spawn_probability = spawn_probability / _gameSettings->getSpawnRate();
            if (rand() % spawn_probability != 0) {
                return;
            }
            
            /** Finds the average mass of the planets in game */
            int avgMass = _planet->getMass();
            int planetCount = 1;
            for (const std::shared_ptr<OpponentPlanet> &op : _opponentPlanets){
                if (op != nullptr){
                    avgMass += op->getMass();
                    planetCount++;
                }
            }
            avgMass = avgMass / planetCount;
            int massCorrection = avgMass - _planet->getMass();
            
            
            /** Pity mechanism: The longer you haven't seen a certain color, the more likely it will be to spawn that color */
            int probSum = 0, largestProb = 0;
            // Sums up the total probability space of the stardust colors, augmented by a mass correction
            largestProb = *max_element(_stardustProb, _stardustProb + _gameSettings->getColorCount());
            massCorrection = min(largestProb, max(-largestProb, massCorrection));
            probSum = accumulate(_stardustProb, _stardustProb + _gameSettings->getColorCount(), probSum) + massCorrection;

            // Randomly selects a point in the probability space
            int spawnRand = rand() % max(1, probSum);
            for (int i = 0; i < _gameSettings->getColorCount(); i++) {
                spawnRand -= (CIColor::Value(i) == _planet->getColor()) ? _stardustProb[i] - massCorrection : _stardustProb[i];
                // Primary Mechanism
                if (spawnRand <= 0){
                    c = CIColor::Value(i);
                    spawnRand = INT_MAX;
                    _stardustProb[i] = max(_stardustProb[i] - CONSTANTS::BASE_SPAWN_RATE, 0);
                } else {
                    // Maintains probability state size consistency
                    _stardustProb[i] += (CONSTANTS::BASE_SPAWN_RATE / (_gameSettings->getColorCount() - 1))
                        + (((BASE_PROBABILITY_SPACE * _gameSettings->getColorCount()) - probSum) / _gameSettings->getColorCount());
                }
            }
            
            while (c > _gameSettings->getColorCount()){
                // Something has gone terribly wrong
                CULog("深刻なエラーが発生しました");
                c = CIColor::getRandomColor();
            }
            break;
    }
    
    /** Looparound Mechanism: Tries to make it so that players can't just send it straight back */
    CILocation::Value spawnCorner = CILocation::Value(0);
    for (const std::shared_ptr<OpponentPlanet> &op : _opponentPlanets){
        if (op != nullptr){
            if (op->getColor() == c){
                cornerProb[op->getLocation()-1] += 60;
            }
        }
    }
    int cornerSum = 0;
    cornerSum = accumulate(cornerProb, cornerProb + 4, cornerSum);
    int cornerRand = rand() % cornerSum;
    for (int i = 0; i < 4; i++) {
        cornerRand -= cornerProb[i];
        if (cornerRand <= 0){
            spawnCorner = CILocation::Value(i+1);
            break;
        }
    }
    _stardustContainer->addStardust(c, bounds, spawnCorner);
}

/**
 * This method applies the power ups of special stardust.
 *
 * @param bounds the bounds of the game screen
 * @param stardustQueue the stardustQueue
 */
void TutorialScene::processSpecialStardust(const cugl::Size bounds, const std::shared_ptr<StardustQueue> stardustQueue) {
    std::vector<std::shared_ptr<StardustModel>> powerupQueue = stardustQueue->getPowerupQueue();
    for (size_t ii = 0; ii < powerupQueue.size(); ii++) {
        std::shared_ptr<StardustModel> stardust = powerupQueue[ii];

        switch (stardust->getStardustType()) {
            case StardustModel::Type::METEOR:
                CULog("METEOR SHOWER!");
                stardustQueue->addStardust(stardust->getColor(), bounds);
                stardustQueue->addStardust(stardust->getColor(), bounds);
                stardustQueue->addStardust(stardust->getColor(), bounds);
                stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                break;
            case StardustModel::Type::SHOOTING_STAR:
                CULog("SHOOTING STAR");
                stardustQueue->addShootingStardust(stardust->getColor(), bounds);
                stardustQueue->addShootingStardust(stardust->getColor(), bounds);
                break;
            case StardustModel::Type::GRAYSCALE:
                CULog("GRAYSCALE");
                stardustQueue->getStardustNode()->applyGreyScale();
                break;
            case StardustModel::Type::FOG: {
                CULog("FOG");
                std::shared_ptr<OpponentPlanet> opponent = _opponentPlanets[stardust->getPreviousOwner()];
                if (opponent != nullptr) {
                    opponent->getOpponentNode()->applyFogPower();
                }
                break;
            }
            default:
                break;
        }
    }
    
    stardustQueue->clearPowerupQueue();
}