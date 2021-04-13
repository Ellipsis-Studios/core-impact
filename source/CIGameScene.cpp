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
#pragma mark Level Layout

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

/** The parallax for each layer */
#define PARALLAX_AMT 0.1f

/** Maximum number of stardusts allowed on screen at a time. */
#define MAX_STARDUST 512

/** Maximum number of layers in planet model */
#define MAX_PLANET_LAYERS 3

/** Default value for the countdown (inactive) */
#define INACTIVE_COUNTDOWN -10.0f

/** Starting value for the countdown (active) */
#define START_COUNTDOWN 2.0f

/** Base stardust spawn rate */
#define BASE_SPAWN_RATE 40

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
 * @param gameId                The gameId for a client game
 * @param spawnRate             The rate for spawning new stardusts
 * @param gravStrength          The strength for planet's gravity
 * @param colorCount            The number of stardust colors available
 * @param gameLength            The game winning condition value
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
    const string gameId, const float spawnRate, const float gravStrength, const uint8_t colorCount, const uint16_t gameLength) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
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
    if (gameId.empty()) { // Host
        _networkMessageManager->createGame();
    }
    else {
        _networkMessageManager->joinGame(gameId);
    }
    
    // Acquire the scene built by the asset loader and resize it the scene
    auto scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    // Get the scene components.
    _allSpace  = _assets->get<scene2::SceneNode>("game_field");
    _farSpace  = _assets->get<scene2::SceneNode>("game_field_far");
    _nearSpace = _assets->get<scene2::SceneNode>("game_field_near");
    _massHUD  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("game_hud"));

    // Create the planet model
    _planet = PlanetModel::alloc(dimen.width / 2, dimen.height / 2, CIColor::getNoneColor(), MAX_PLANET_LAYERS, gravStrength, gameLength);
    auto coreTexture = _assets->get<Texture>("core");
    auto ringTexture = _assets->get<Texture>("innerRing");
    auto unlockedTexture = _assets->get<Texture>("unlockedOuterRing");
    auto lockedTexture = _assets->get<Texture>("lockedOuterRing");
    _planet->setTextures(coreTexture, ringTexture, unlockedTexture, lockedTexture);

    _draggedStardust = NULL;
    _stardustContainer = StardustQueue::alloc(MAX_STARDUST, coreTexture);

    // TODO: resize to number of players in the game and add opponent planet nodes to scene graph
    _opponent_planets.resize(5);

    // Game settings
    _spawnRate = spawnRate;
    _colorCount = colorCount;
    
    addChild(scene);
    addChild(_planet->getPlanetNode());
    addChild(_stardustContainer->getStardustNode());

    _countdown = INACTIVE_COUNTDOWN;
    
    CULog("Game Room Id: %s Spawn Rate: %f Gravity Strength: %f Color Count: %i Game Length (win cond): %i", gameId.c_str(), spawnRate, gravStrength, colorCount, gameLength);
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
    _massHUD = nullptr;
    _allSpace = nullptr;
    _farSpace = nullptr;
    _nearSpace = nullptr;
    _stardustContainer = nullptr;
    _planet = nullptr;
    _draggedStardust = NULL;
    _opponent_planets.clear();
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
    dimen *= SCENE_WIDTH/dimen.width;
    _input.update(timestep);
    
    _massHUD->setText("Room: " + _networkMessageManager->getRoomId()
        + " / Your Core: " + to_string(_planet->getMass()) + "; "
        + CIColor::getString(_planet->getColor()));
    
     // Handle counting down then switching to loading screen
     if (_networkMessageManager->getWinnerPlayerId() != -1) {
         if (_countdown <= INACTIVE_COUNTDOWN) {
             // handle winning. starts off win countdown
             CULog("Game won.");
             _countdown = START_COUNTDOWN;
         } else if (_countdown > 0.0f) {
             // handle win countdown
             _countdown -= timestep;
             return;
         } else if (_countdown > INACTIVE_COUNTDOWN) {
             // handle resetting game
             setActive(false);
             return;
         }
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
        _networkMessageManager->receiveMessages(dimen);
        _gameUpdateManager->setPlayerId(_networkMessageManager->getPlayerId());
    } else {
        // send and receive game updates to other players
        _gameUpdateManager->sendUpdate(_planet, _stardustContainer, dimen);
        _networkMessageManager->receiveMessages(dimen);
        _networkMessageManager->sendMessages();
        _gameUpdateManager->processGameUpdate(_stardustContainer, _planet, _opponent_planets, dimen);
        for (int ii = 0; ii < _opponent_planets.size() ; ii++) {
            std::shared_ptr<OpponentPlanet> opponent = _opponent_planets[ii];
            if (opponent != nullptr && getChildByName(to_string(ii)) == nullptr) {
                opponent->setTextures(_assets->get<Texture>("opponentProgress"), dimen);
                //TODO: call opponent->setName with name and font
                addChildWithName(opponent->getOpponentNode(), to_string(ii));
            }
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
    if (_stardustContainer->size() == MAX_STARDUST) {
        return;
    }
    
    // handle game settings
    size_t spawn_probability = BASE_SPAWN_RATE + (_stardustContainer->size() * BASE_SPAWN_RATE);
    spawn_probability = spawn_probability / _spawnRate;
    if (rand() % spawn_probability != 0) {
        return;
    }
    
    /** Finds the average mass of the planets in game */
    int avgMass = _planet->getMass();
    int planetCount = 1;
    for (const std::shared_ptr<OpponentPlanet> &op : _opponent_planets){
        if (op != nullptr){
            avgMass += op->getMass();
            planetCount++;
        }
    }
    avgMass = avgMass / planetCount;
    int massCorrection = avgMass - _planet->getMass();
    
    /** Pity mechanism: The longer you haven't seen a certain color, the more likely it will be to spawn that color */
    CIColor::Value c = CIColor::getRandomColor();
    int probSum = 0;
    // Sums up the total probability space of the stardust colors, augmented by a mass correction
    probSum = accumulate(_stardustProb, _stardustProb + _colorCount, probSum) + massCorrection;
    // Randomly selects a point in the probability space
    int spawnRand = rand() % max(1, probSum);
    for (int i = 0; i < _colorCount; i++) {
        spawnRand -= (CIColor::Value(i) == _planet->getColor()) ? _stardustProb[i] - massCorrection : _stardustProb[i];
        if (spawnRand <= 0){
            c = CIColor::Value(i);
            spawnRand = probSum;
            _stardustProb[i] = max(_stardustProb[i] - BASE_SPAWN_RATE, 0);
        } else {
            _stardustProb[i] += 10;
        }
    }

    _stardustContainer->addStardust(c, bounds);
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
                stardustQueue->addStardust(stardust->getColor(), bounds);
                stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                break;
            case StardustModel::Type::SHOOTING_STAR:
                CULog("SHOOTING STAR");
                stardustQueue->addShootingStardust(stardust->getColor(), bounds);
                stardustQueue->addShootingStardust(stardust->getColor(), bounds);
            case StardustModel::Type::GRAYSCALE:
                CULog("GRAYSCALE");
                stardustQueue->getStardustNode()->applyGreyScale();
            default:
                break;
        }
    }
    
    stardustQueue->clearPowerupQueue();
}
