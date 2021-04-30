//
//  CIGameScene.cpp
//  CoreImpact
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It is a relatively simple class as we are not
//  worried about collisions.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <numeric>

#include "CIGameScene.h"
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
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
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
    // Set the game update manager and network message managers
    _gameUpdateManager = GameUpdateManager::alloc();
    _networkMessageManager = networkMessageManager;
    _networkMessageManager->setGameuUpdateManager(_gameUpdateManager);
    
    // Acquire the scene built by the asset loader and resize it the scene
    auto scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    // Get the scene components.
    _allSpace  = _assets->get<scene2::SceneNode>("game_field");
    _farSpace = std::dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_field_far"));
    _nearSpace = _assets->get<scene2::SceneNode>("game_field_near");
    
    // create the win scene
    _winScene = WinScene::alloc(assets, dimen);

    // create the pause menu
    _pauseMenu = PauseMenu::alloc(_assets, networkMessageManager, playerSettings);
    _pauseMenu->setDisplay(false);

    _pauseBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("game_pausebutton"));
    _pauseBtn->setColor(Color4::GRAY);
    _pauseBtn->setVisible(true);
    _pauseBtn->activate();

    _pauseBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _networkMessageManager->setGameState(GameState::GamePaused);
        }
        });

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
    addChild(_pauseMenu->getLayer(), 1);
    
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
void GameScene::dispose() {
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
    _stardustContainer = nullptr;
    _planet = nullptr;
    _draggedStardust = NULL;
<<<<<<< bce1c45fe3f36d0f563ab6057593442a982b69d8
    _opponentPlanets.clear();
    
=======
    _opponent_planets.clear();
    _pauseBtn = nullptr;
    _pauseMenu = nullptr;
>>>>>>> working pause menu
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
void GameScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= CONSTANTS::SCENE_WIDTH/dimen.width;
    
    _input.update(timestep);
    // Handle counting down then switching to loading screen
    if (_networkMessageManager->getWinnerPlayerId() != -1) {
        if (!_winScene->displayActive()) {
            CULog("Game won.");
            _winScene->setWinner(_networkMessageManager->getWinnerPlayerId(), _networkMessageManager->getPlayerId());
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
  
    // attempt to set player id of game update manager
    if (_gameUpdateManager->getPlayerId() < 0) {
        // need to make this call to attempt to connect to game
        _networkMessageManager->receiveMessages();
        _gameUpdateManager->setPlayerId(_networkMessageManager->getPlayerId());
    } else {
        // send and receive game updates to other players
        _gameUpdateManager->sendUpdate(_planet, _stardustContainer);
        _networkMessageManager->receiveMessages();
        _networkMessageManager->sendMessages();
        _gameUpdateManager->processGameUpdate(_stardustContainer, _planet, _opponentPlanets, dimen);
        for (int ii = 0; ii < _opponentPlanets.size() ; ii++) {
            std::shared_ptr<OpponentPlanet> opponent = _opponentPlanets[ii];
            if (opponent != nullptr) {
                opponent->update(timestep);
            }
        }
    }
    
    processSpecialStardust(dimen, _stardustContainer);
    togglePause(_networkMessageManager->getGameState() == GameState::GamePaused);
    _pauseMenu->update();
}

/**
 * This method updates the dragged stardust.
 *
 * It selects or deselects a dragged stardust stardust if applicable,
 * and updates the velocity a selected stardust if there is one.
 */
void GameScene::updateDraggedStardust() {
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
void GameScene::addStardust(const Size bounds) {
    if (_stardustContainer->size() == CONSTANTS::MAX_STARDUSTS) {
        return;
    }
    
    // handle game settings
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
    CIColor::Value c = CIColor::getRandomColor();
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
    
    /** Looparound Mechanism: Tries to make it so that players can't just send it straight back */
    int cornerProb[] = {10,10,10,10};
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
void GameScene::processSpecialStardust(const cugl::Size bounds, const std::shared_ptr<StardustQueue> stardustQueue) {
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

/**
 * Sets whether the pause menu is currently active and visible.
 *
 * @param onDisplay     Whether the pause menu is currently active and visible
 */
void GameScene::togglePause(bool onDisplay) {
    _pauseMenu->setDisplay(onDisplay);
    _pauseBtn->setVisible(!onDisplay);

    if (onDisplay) {
        _pauseBtn->deactivate();
    }
    else {
        _pauseBtn->activate();
    }
}