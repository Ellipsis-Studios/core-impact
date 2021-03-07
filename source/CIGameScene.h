//
//  SDGameScene.h
//  Ship Demo
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
#ifndef __SD_GAME_SCENE_H__
#define __SD_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "CIPlanetModel.h"
#include "CIInput.h"
#include "CIDotsNode.h"
#include "CIDotsQueue.h"

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
    ShipInput _input;
    
    // VIEW
    /** Filmstrip representing the animated ship */
    std::shared_ptr<cugl::scene2::AnimationNode> _shipNode;
    /** Label for on-screen coordinate HUD */
    std::shared_ptr<cugl::scene2::Label> _coordHUD;
    /** Node to hold all of our graphics. Necesary for resolution indepedence. */
    std::shared_ptr<cugl::scene2::SceneNode> _allSpace;
    /** Background in animation parallax. Stores the field of stars */
    std::shared_ptr<cugl::scene2::SceneNode> _farSpace;
    /** Foreground in animation parallax. Stores the planets. */
    std::shared_ptr<cugl::scene2::SceneNode> _nearSpace;
    /** Shared memory pool for dots. (MODEL CLASS) */
    std::shared_ptr<DotsQueue> _dotsContainer;

    // MODEL
    // A page-out could dispose of the view as long as it just has this.
    /** The current coordinates of the ship */
    std::shared_ptr<PlanetModel>  _shipModel;
    
    /**
     * Returns an informative string for the position
     *
     * This function is for writing the current ship position to the HUD.
     *
     * @param coords The current ship coordinates
     *
     * @return an informative string for the position
     */
    std::string positionText(const cugl::Vec2& coords);

    
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
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    
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
     * Resets the status of the game so that we can play again.
     */
    void reset();

};

#endif /* __SD_GAME_SCENE_H__ */
