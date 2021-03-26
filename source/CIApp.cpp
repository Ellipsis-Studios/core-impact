//
//  CIApp.cpp
//  CoreImpact
//
//  This is the root class for your game.  The file main.cpp accesses this class
//  to run the application.  While you could put most of your game logic in
//  this class, we prefer to break the game up into player modes and have a
//  class for each mode.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "CIApp.h"

using namespace cugl;

static std::shared_ptr<OrthographicCamera> cam = nullptr;

#pragma mark -
#pragma mark Gameplay Control

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void CoreImpactApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    cam = OrthographicCamera::alloc(getDisplaySize());
    
    // Start-up basic input
#ifdef CU_MOBILE
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    Input::activate<Keyboard>();
    Input::activate<TextInput>();
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());

    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets);

    // Queue up the other assets
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    
    Application::onStartup(); // YOU MUST END with call to parent
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void CoreImpactApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    // Shutdown input
#ifdef CU_MOBILE
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Input::deactivate<Keyboard>();
    Input::deactivate<TextInput>();
    
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void CoreImpactApp::update(float timestep) {
    if (!_loaded && _loading.isActive()) {
        _loading.reset(); // sets the values to default
        _loading.update(0.01f);
    } else if (!_loaded) { 
        _loading.dispose(); // Disables the input listeners to this mode
        _gameplay.init(_assets, _loading._joinGame.empty(), _loading._joinGame);
        _loaded = true;
    } else if (_gameplay.isActive()) { 
        _gameplay.update(timestep);
    } else {
        // handle game reset
        _gameplay.reset();
        _assets = nullptr;
        _batch = nullptr;

        _assets = AssetManager::alloc();
        _batch = SpriteBatch::alloc();

        _assets->attach<Font>(FontLoader::alloc()->getHook());
        _assets->attach<Texture>(TextureLoader::alloc()->getHook());
        _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());

        _loaded = false;
        _loading.init(_assets);

        _assets->loadDirectoryAsync("json/assets.json", nullptr);
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void CoreImpactApp::draw() {
    if (!_loaded) {
        _loading.render(_batch);
    } else {
        _gameplay.render(_batch);
    }
}


