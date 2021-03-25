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

/** Starting value for the countdown */
#define START_COUNTDOWN -10.0f

#pragma mark -
#pragma mark Constructors


/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param isHost    Whether or not this instance is hosting the game
 * @param gameId    The gameId for a client game
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, bool isHost, std::string gameId) {
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
    startGame(isHost, gameId);
    
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
    _planet = PlanetModel::alloc(dimen.width/2, dimen.height/2, CIColor::getNoneColor(), 3);
    auto coreTexture = _assets->get<Texture>("core");
    auto ringTexture = _assets->get<Texture>("innerRing"); 
    auto unlockedTexture = _assets->get<Texture>("unlockedOuterRing");
    auto lockedTexture = _assets->get<Texture>("lockedOuterRing");
    _planet->setTextures(coreTexture, ringTexture, unlockedTexture, lockedTexture);
    
    _draggedStardust = NULL;
    _stardustContainer = StardustQueue::alloc(MAX_STARDUST, _assets->get<Texture>("photon"));
    
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    
    addChild(scene);
    addChild(_planet->getPlanetNode());
    addChild(_stardustContainer->getStardustNode());

    _countdown = START_COUNTDOWN;
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _gameUpdateManager = nullptr;
        _networkMessageManager = nullptr;
        _allSpace = nullptr;
        _farSpace = nullptr;
        _nearSpace = nullptr;
        _planet = nullptr;
        _draggedStardust = NULL;
        _active = false;
    }
}


#pragma mark -
#pragma mark Gameplay Handling

/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    _input.dispose();
    
    _gameUpdateManager->reset();

    _networkMessageManager->reset();
    _networkMessageManager->setGameuUpdateManager(_gameUpdateManager);

    _allSpace = _assets->get<scene2::SceneNode>("game_field");
    _farSpace = _assets->get<scene2::SceneNode>("game_field_far");
    _nearSpace = _assets->get<scene2::SceneNode>("game_field_near");
    _massHUD = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("game_hud"));

    _planet->reset();

    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
    _stardustContainer->reset(); // clears both queues and resets indices
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);

    _countdown = START_COUNTDOWN;

    _draggedStardust = NULL;

    _active = false;
    setActive(false);
}

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
     if (_planet->isWinner()) {
         if (_countdown <= START_COUNTDOWN) {
             // handle winning. starts off win countdown 
             CULog("Game won.");
             _countdown = 2.0f;
         } else if (_countdown > 0.0f) {
             // handle win countdown
             _countdown -= timestep;
             return;
         } else if (_countdown > START_COUNTDOWN) {
             // handle resetting game
             setActive(false);
             return;
         }
     }
    // if (_countdown > 0.0f) {
    //    _countdown -= timestep;
    //    return;
    //} else if (_countdown > START_COUNTDOWN) {
    //    setActive(false);
    //    return;
    //} 
    //if (_planet->isWinner()) {
    //    // handle winning
    //    CULog("Game won.");
    //    _countdown = 2.0f;
    //}
    
    _stardustContainer->update();
    
    if (rand() % 150 == 0){
        _stardustContainer->addStardust(dimen);
    }

    collisions::checkForCollision(_planet, _stardustContainer);
    collisions::checkInBounds(_stardustContainer, dimen);
    collisions::checkForCollisions(_stardustContainer);
    updateDraggedStardust();
    
    if (collisions::checkForCollision(_planet, _input.getPosition())) {
        _planet->lockInLayer(timestep);
    } else if (_planet->isLockingIn()) {
        _planet->stopLockIn();
    }
    
    // attempt to set player id of game update manager
    if (_gameUpdateManager->getPlayerId() < 0) {
        // need to make this call to attempt to connect to game
        _networkMessageManager->receiveMessages();
        _gameUpdateManager->setPlayerId(_networkMessageManager->getPlayerId());
    } else {
        // send and receive game updates to other players
        _gameUpdateManager->sendUpdate(_planet, _stardustContainer, dimen);
        _networkMessageManager->receiveMessages();
        _networkMessageManager->sendMessages();
        _gameUpdateManager->processGameUpdate(_stardustContainer, dimen);
    }
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
            float sdRadius = collisions::getStardustRadius(_stardustContainer);
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
 * Starts a new game instance with new input instance.
 */
void GameScene::startGame(bool isHost, std::string gameId) {
    // reinitialize the input 
    _input.init(getBounds());

    // Create the game update manager and network message managers
    _gameUpdateManager = GameUpdateManager::alloc();
    _networkMessageManager = NetworkMessageManager::alloc();
    _networkMessageManager->setGameuUpdateManager(_gameUpdateManager);
    if (isHost) {
        _networkMessageManager->createGame();
    }
    else {
        _networkMessageManager->joinGame(gameId);
    }
    _active = true;
    setActive(true);
}