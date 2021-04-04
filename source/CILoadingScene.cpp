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
    
    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    _brand = assets->get<scene2::SceneNode>("load_name");
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
    _teamlogo->setVisible(false);
    _gameTitle->setVisible(false);
    _gamePlanet->setVisible(false);
    _brand = nullptr;
    _bar = nullptr;
    _assets = nullptr;
    _teamlogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _progress = 0.0f;
    _isLoaded = false;
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
    if (_isLoaded) {
        // move game title/planet
        auto root = getChildByName("loadingScene");
        float rend = root->getContentWidth() / 4.0f;

        if (_gamePlanet->getPositionX() > rend) {
            /** Move game planet/title to the left */
            auto newx = _gamePlanet->getPositionX() - (rend / 60.0f);
            _gamePlanet->setPositionX(newx);
            _gameTitle->setPositionX(newx);
        }
        else {
            /** Completed movement. Trigger main menu scene. */
            _gamePlanet->setPositionX(rend);
            _gameTitle->setPositionX(rend);
            _active = false;
        }
    }
    else if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            _progress = 1.0f;
            _bar->setVisible(false);
            _brand->setVisible(false);

            _gameTitle->setVisible(true);
            _gamePlanet->setVisible(true);
            _teamlogo->setVisible(true);
            _isLoaded = true;
        }
        _bar->setProgress(_progress);
    }
}
