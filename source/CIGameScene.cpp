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
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _input.init(getBounds());
    
    // Create the game update manager
    _gameUpdateManager = GameUpdateManager::alloc();
    
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
    _stardustContainer = StardustQueue::alloc(MAX_STARDUST);
    _stardustContainer->setTexture(_assets->get<Texture>("photon"));
    std::shared_ptr<StardustNode> _stardustNode = StardustNode::alloc();
    _stardustNode->setStardustQueue(_stardustContainer);
    
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    _stardustContainer->addStardust(dimen);
    
    addChild(scene);
    addChild(_planet->getPlanetNode());
    addChild(_stardustNode);
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
    // Reset the planet and input
    _input.clear();
    
    // Reset the parallax
    Vec2 position = _farSpace->getPosition();
    _farSpace->setAnchor(Vec2::ANCHOR_CENTER);
    _farSpace->setPosition(position);
    _farSpace->setAngle(0.0f);
    
    _draggedStardust = NULL;
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
    
    _stardustContainer->update();
    
    if (rand() % 60 == 0){
        _stardustContainer->addStardust(dimen);
    }
    
    _massHUD->setText(to_string(_planet->getMass()) + "; "
        + CIColor::getString(_planet->getColor()));

    collisions::checkForCollision(_planet, _stardustContainer);
    collisions::checkInBounds(_stardustContainer, dimen);
    collisions::checkForCollisions(_stardustContainer);
    updateDraggedStardust();
    
    // send game updates to other players
    _gameUpdateManager->sendUpdate(_planet, _stardustContainer, dimen);
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
