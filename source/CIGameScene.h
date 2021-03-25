//
//  CIGameScene.h
//  CoreImpact
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It is a relativeluy simple class as we are not
//  worried about collisions.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Author: Walker White
//  Version: 1/10/18
//
#ifndef __CI_GAME_SCENE_H__
#define __CI_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "CIPlanetModel.h"
#include "CIInput.h"
#include "CIStardustQueue.h"
#include "CIGameUpdateManager.h"
#include "CINetworkMessageManager.h"

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    InputController _input;
    /** The game update manager for managing messages from other players */
    std::shared_ptr<GameUpdateManager> _gameUpdateManager;
    /** The network message manager for managing connections to other players */
    std::shared_ptr<NetworkMessageManager> _networkMessageManager;
    
    // VIEW
    /** Label used to dispslay the planet's mass to the screen */
    std::shared_ptr<cugl::scene2::Label> _massHUD;
    /** Node to hold all of our graphics. Necesary for resolution indepedence. */
    std::shared_ptr<cugl::scene2::SceneNode> _allSpace;
    /** Background in animation parallax. Stores the field of stars */
    std::shared_ptr<cugl::scene2::SceneNode> _farSpace;
    /** Foreground in animation parallax. Stores the planets. */
    std::shared_ptr<cugl::scene2::SceneNode> _nearSpace;
    /** Shared memory pool for stardust. (MODEL CLASS) */
    std::shared_ptr<StardustQueue> _stardustContainer;

    // MODEL
    /** The model representing the planet */
    std::shared_ptr<PlanetModel>  _planet;
    /** Pointer to the model of the stardust that is currently being dragged */
    StardustModel*  _draggedStardust;
    
    /** Countdown to reset the game after winning/losing */
    float _countdown;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, bool isHost, std::string gameId);

    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    /**
     * This method updates the dragged stardust.
     *
     * It selects or deselects a dragged stardust stardust if applicable,
     * and updates the velocity a selected stardust if there is one.
     */
    void updateDraggedStardust();

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
};

#endif /* __CI_GAME_SCENE_H__ */
