//
//  CILoadingScene.cpp
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
#include "CILoadingScene.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

#pragma mark -
#pragma mark Constructors

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
bool LoadingScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<scene2::SceneNode>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _teamlogo = assets->get<scene2::SceneNode>("load_teamlogo");
    _gameTitle = assets->get<scene2::SceneNode>("load_title");
    _gamePlanet = assets->get<scene2::SceneNode>("load_world");

    Application::get()->setClearColor(Color4(192, 192, 192, 255));
    addChildWithName(layer, "loadingScene");
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoadingScene::dispose() {
    if (_teamlogo != nullptr) {
        _teamlogo->setVisible(false);
        _gameTitle->setVisible(false);
        _gamePlanet->setVisible(false);
    }
    _assets = nullptr;
    _teamlogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _progress = 0.0f;
    _opacitiesIndex = 0;
}

#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LoadingScene::update(float progress) {
    _progress = _assets->progress();
    if (_progress >= 1) {
        _progress = 1.0f;
        _gameTitle->setVisible(true);
        _gamePlanet->setVisible(true);
        _teamlogo->setVisible(true);
        _active = false;
        _opacitiesIndex = 0;
    }
    Color4 color = _gameTitle->getColor();
    if (_progress < 1) {
        color.a = 255 * _opacities[_opacitiesIndex];
        _opacitiesIndex++;
        if (_opacitiesIndex >= _opacities.size()) {
            _opacitiesIndex = 0;
        }
    }
    _gameTitle->setColor(color);
}
