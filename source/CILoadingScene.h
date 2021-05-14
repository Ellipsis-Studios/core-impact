//
//  CILoadingScene.h
//  CoreImpact
//
//  This module provides a very barebones loading screen.  Most of the time you
//  will not need a loading screen, because the assets will load so fast.  But
//  just in case, this is a simple example you can use in your games.
//
//  We know from 3152 that you all like to customize this screen.  Therefore,
//  we have kept it as simple as possible so that it is easy to modify. In
//  fact, this loading screen uses the new modular JSON format for defining
//  scenes.  See the file "loading.json" for how to change this scene.
//
//  Author: Walker White
//  Version: 1/10/18
//
#ifndef __CI_LOADING_SCENE_H__
#define __CI_LOADING_SCENE_H__
#include <cugl/cugl.h>


/**
 * This class is a simple loading screen for asychronous asset loading.
 *
 * The screen will display a very minimal progress bar that displays the
 * status of the asset manager.  Make sure that all asychronous load requests
 * are issued BEFORE calling update for the first time, or else this screen
 * will think that asset loading is complete.
 *
 * Once asset loading is completed, it will display a play button.  Clicking
 * this button will inform the application root to switch to the gameplay mode.
 */
class LoadingScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // NO CONTROLLER (ALL IN SEPARATE THREAD)

    // VIEW
    /** Team logo */
    std::shared_ptr<cugl::scene2::SceneNode> _teamlogo;
    /** Game Title */
    std::shared_ptr<cugl::scene2::SceneNode> _gameTitle;
    /** Game Planet */
    std::shared_ptr<cugl::scene2::SceneNode> _gamePlanet;

    // MODEL
    /** The progress displayed on the screen */
    float _progress;
    
    /** Vector of opacities. Used for displaying the logo.  */
    std::vector<float> _opacities{ 1.0f, 1.0f, 0.98f, 0.98f, 0.95f, 0.95f, 0.90f, 0.90f, 0.82f, 0.82f, 0.75f, 0.75f, 0.65f, 0.65f, 0.5f, 0.5f, 0.3f, 0.3f, 0.08f, 0.08f, 0.3f, 0.3f, 0.5f, 0.5f, 0.65f, 0.65f, 0.75f, 0.75f, 0.82f, 0.82f, 0.90f, 0.90f, 0.95f, 0.95f, 0.98f, 0.98f, 1.0f, 1.0f };
    /** Current index in opacities vector. */
    int _opacitiesIndex = 0;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    LoadingScene() : cugl::Scene2(), _progress(0.0f) {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LoadingScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes the controller contents, making it ready for loading
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
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

};

#endif /* __CI_LOADING_SCENE_H__ */
