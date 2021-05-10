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
#include "CIInputController.h"
#include "CIStardustQueue.h"
#include "CIGameUpdateManager.h"
#include "CINetworkMessageManager.h"
#include "CIOpponentPlanet.h"
#include "CIWinScene.h"
#include "CIGameSettings.h"
#include "CIPlayerSettings.h"
#include "CIGameConstants.h"
#include "CIPauseMenu.h"

/** Base stardust spawn rate */
#define BASE_PROBABILITY_SPACE 100

/** Default number of stardust color counts */
#define DEFAULT_COLOR_COUNTS 4

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
private:
    /** Handles stardust color probability */
    int _stardustProb[6];
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
    /** Node to hold all of our graphics. Necesary for resolution indepedence. */
    std::shared_ptr<cugl::scene2::SceneNode> _allSpace;
    /** Background in animation parallax. Stores the field of stars */
    std::shared_ptr<cugl::scene2::AnimationNode> _farSpace;
    /** Foreground in animation parallax. Stores the planets. */
    std::shared_ptr<cugl::scene2::SceneNode> _nearSpace;
    /** Shared memory pool for stardust. (MODEL CLASS) */
    std::shared_ptr<StardustQueue> _stardustContainer;
    /** Reference to the pause button */
    std::shared_ptr<cugl::scene2::Button> _pauseBtn;
    std::shared_ptr<PauseMenu> _pauseMenu;

    // MODEL
    /** The model representing the planet */
    std::shared_ptr<PlanetModel>  _planet;
    /** Pointer to the model of the stardust that is currently being dragged */
    StardustModel*  _draggedStardust;
    /** Vector of opponent planets */
    std::vector<std::shared_ptr<OpponentPlanet>> _opponentPlanets;

    // Game Settings
    std::shared_ptr<GameSettings> _gameSettings;

    // Player Settings
    std::shared_ptr<PlayerSettings> _playerSettings;

    /** Time since last animation frame update */
    float _timeElapsed;
    
    /** Pointer to the win scene */
    std::shared_ptr<WinScene> _winScene;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::Scene2() {
        for (int i = 0; i < 6; i++) {
            _stardustProb[i] = 0;
        }
    }
    
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
     * @param assets                The (loaded) assets for this game mode
     * @param networkMessageManager The reference to network message manager
     * @param gameSettings          The settings for the current game
     * @param playerSettings        The settings for the current player
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
        const std::shared_ptr<GameSettings>& gameSettings, 
        const std::shared_ptr<PlayerSettings>& playerSettings);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     * @param playerSettings    The player's saved settings value
     * 
     */
    void update(float timestep, const std::shared_ptr<PlayerSettings>& playerSettings);
    
    /**
     * This method updates the dragged stardust.
     *
     * It selects or deselects a dragged stardust stardust if applicable,
     * and updates the velocity a selected stardust if there is one.
     */
    void updateDraggedStardust();
    
    /**
     * This method attempts to add a stardust to the players screen.
     *
     *  Whether a stardust is added is determined by how many stardust are already on the screen.
     *  The color of the added stardust is determined by how close to finishing the player is.
     *
     *  @param bounds the bounds of the game screen
     */
    void addStardust(const cugl::Size bounds);
    
    /**
     * This method applies the power ups of special stardust.
     *
     * @param bounds the bounds of the game screen
     * @param stardustQueue the stardustQueue
     */
    void processSpecialStardust(const cugl::Size bounds, const std::shared_ptr<StardustQueue> stardustQueue);

    /**
     * Sets whether the pause menu is currently active and visible.
     *
     * @param onDisplay     Whether the pause menu is currently active and visible
     */
    void togglePause(bool onDisplay);

};

#endif /* __CI_GAME_SCENE_H__ */
