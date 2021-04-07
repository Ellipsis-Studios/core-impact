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

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

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
    _json = JsonValue::allocObject();
    CULog("%s", Application::getSaveDirectory().append("save.json").c_str());
    
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
    _networkMessageManager = nullptr;
    
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
 * The method called when you are running out of memory.
 *
 * When this method is called, you should immediately free memory or cause
 * the application to quit. Texture memory is generally the biggest
 * candidate for freeing memory; delete all textures you are not using.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void CoreImpactApp::onLowMemory() {
    CULog("Warning: Low Memory.");
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void CoreImpactApp::onSuspend() {
    if (_networkMessageManager) {
        std::shared_ptr<cugl::JsonWriter> _jsonWriter = JsonWriter::alloc(Application::getSaveDirectory().append("save.json"));
        int playerId = _networkMessageManager->getPlayerId();
        _json->appendValue("timestamp", std::to_string(_networkMessageManager->getTimestamp()));
        _json->appendValue("playerId", std::to_string(playerId));
        _json->appendValue("gameId", _networkMessageManager->getRoomId());

        // planet JSON
        std::shared_ptr<PlanetModel> planet = _gameplay.getPlanet();
        std::shared_ptr<cugl::JsonValue> planetJson = JsonValue::allocObject();
        planetJson->appendValue("currLayerStardustAmount", std::to_string(planet->getCurrLayerProgress()));
        planetJson->appendValue("currLayerLockInStardustNeeded", std::to_string(planet->getLayerLockinTotal()));
        planetJson->appendValue("radius", planet->getRadius());
        planetJson->appendValue("mass", planet->getMass());

        // layers within planet JSON
        std::shared_ptr<cugl::JsonValue> layers = JsonValue::allocArray();
        std::shared_ptr<cugl::JsonValue> layer;
        for (size_t ii = 0; ii < planet->getLayers().size(); ii++) {
            layer = JsonValue::allocObject();
            layer->appendValue("size", std::to_string(planet->getLayers()[ii].layerSize));
            layer->appendValue("color", std::to_string(planet->getLayers()[ii].layerColor));
            layer->appendValue("lockedIn", planet->getLayers()[ii].isLockedIn);
            layers->appendChild(layer);
        }
        planetJson->appendChild("layers", layers);
        _json->appendChild("planet", planetJson);

        // stardust queue JSON
        std::shared_ptr<cugl::JsonValue> stardustsJson = JsonValue::allocArray();
        std::shared_ptr<cugl::JsonValue> stardustJson;
        std::shared_ptr<StardustQueue> stardustQueue = _gameplay.getStardustContainer();
        StardustModel* stardust;
        for (size_t jj = 0; jj < stardustQueue->size(); jj++) {
            stardust = stardustQueue->get(jj);
            if (stardust != nullptr) {
                cugl::Vec2 pos = stardust->getPosition();
                cugl::Vec2 vel = stardust->getVelocity();
                stardustJson = JsonValue::allocObject();
                stardustJson->appendValue("color", std::to_string(stardust->getColor()));
                stardustJson->appendValue("xPos", pos.x);
                stardustJson->appendValue("yPos", pos.y);
                stardustJson->appendValue("xVel", vel.x);
                stardustJson->appendValue("yVel", vel.y);
                stardustsJson->appendChild(stardustJson);
            }
        }
        _json->appendChild("stardustQueue", stardustsJson);

        std::shared_ptr<cugl::JsonValue> opponentPlanetsJson = JsonValue::allocArray();
        std::shared_ptr<cugl::JsonValue> opponentPlanetJson;
        std::vector<std::shared_ptr<OpponentPlanet>> opponentPlanetsVector = _gameplay.getOpponentPlanets();
        std::shared_ptr<OpponentPlanet> opponentPlanet;
        for (size_t kk = 0; kk < opponentPlanetsVector.size(); kk++) {
            opponentPlanet = opponentPlanetsVector[kk];
            if (opponentPlanet != nullptr) {
                opponentPlanetJson = JsonValue::allocObject();
                opponentPlanetJson->appendValue("playerId", std::to_string(kk));
                opponentPlanetJson->appendValue("mass", std::to_string(opponentPlanet->getMass()));
                opponentPlanetJson->appendValue("color", std::to_string(opponentPlanet->getColor()));
                opponentPlanetsJson->appendChild(opponentPlanetJson);
            }
        }
        _json->appendChild("otherPlanets", opponentPlanetsJson);

        CULog("JSON saved: %s", _json->toString().c_str());
        CULog("Save location: %s", Application::getSaveDirectory().c_str());
        //CULog("DIR size: %u", Application::getSaveDirectory().size());
        // _jsonWriter = JsonWriter::alloc(Application::getSaveDirectory().append("save.json"));
        _jsonWriter->writeJson(_json);
        _gameplay.dispose();
//        _gameplay.removeChildByName("planet");
//        _gameplay.removeChildByName("stardustQueue");
        CULog("Finished saving!");
    }
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void CoreImpactApp::onResume() {
    CULog(_networkMessageManager == nullptr ? "true" : "false");
    _loaded = true;
    _gameplay.setActive(true);
//    _gameplay.init(_assets, _networkMessageManager, _loading._joinGame.empty(), _loading._joinGame);
    std::shared_ptr<cugl::JsonReader> _jsonReader = JsonReader::alloc(Application::getSaveDirectory().append("save.json"));
    //_jsonReader = JsonReader::alloc(Application::getSaveDirectory().append("save.json"));
    _json = _jsonReader->readJson();
    CULog("JSON retrieved: %s", _json->toString().c_str());
    CULog("Save location: %s", Application::getSaveDirectory().c_str());

    // if game has won or more than 50 time intervals
    if (_networkMessageManager->getWinnerPlayerId() != -1 || _networkMessageManager->getTimestamp() - _json->getInt("timestamp") > 50) {
        // redirect player to main screen
        _loading.setActive(true);
    }

    else {
        // recreate the planet model
        Size dimen = Application::get()->getDisplaySize();
        dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
        std::shared_ptr<cugl::JsonValue> layersJson = _json->get("planet")->get("layers");
        std::shared_ptr<PlanetModel> planet = PlanetModel::alloc(dimen.width / 2, dimen.height / 2, static_cast<CIColor::Value>(layersJson->get(layersJson->size()-1)->getInt("color")), layersJson->size());
        auto coreTexture = _assets->get<Texture>("core");
        auto ringTexture = _assets->get<Texture>("innerRing");
        auto unlockedTexture = _assets->get<Texture>("unlockedOuterRing");
        auto lockedTexture = _assets->get<Texture>("lockedOuterRing");
        planet->setTextures(coreTexture, ringTexture, unlockedTexture, lockedTexture);
        planet->setCurrLayerProgress(_json->getInt("currLayerStardustAmount"));
        planet->setLayerLockinTotal(_json->getInt("currLayerLockInStardustNeeded"));
        planet->setRadius(_json->getFloat("radius"));
        planet->setMass(_json->getFloat("mass"));

        // recreate the planet layers
        std::vector<PlanetLayer> layers;
        layers.resize(layersJson->size());
        PlanetLayer layer;
        for (size_t ii = 0; ii < layers.size(); ii++) {
            layer = {
            .layerSize = layersJson->get(ii)->getInt("size"),
            .layerColor = static_cast<CIColor::Value>(layersJson->get(ii)->getInt("color")),
            .isActive = true,
            .isLockedIn = layersJson->get(ii)->getBool("lockedIn")
            };
            layers[ii] = layer;
        }
        planet->setLayers(layers);

        // recreate the stardust queue
        std::shared_ptr<cugl::JsonValue> stardustsJson = _json->get("stardustQueue");
        std::shared_ptr<StardustQueue> stardustContainer = StardustQueue::alloc(stardustsJson->size(), _assets->get<Texture>("photon"));
        std::shared_ptr<cugl::JsonValue> stardustJson;
        std::shared_ptr<StardustModel> stardust;
        for (size_t jj = 0; jj < stardustsJson->size(); jj++) {
            stardustJson = stardustsJson->get(jj);
            Vec2 pos = Vec2(stardustJson->getFloat("xPos"), stardustJson->getFloat("yPos"));
            Vec2 vel = Vec2(stardustJson->getFloat("xVel"), stardustJson->getFloat("yVel"));
            stardust = StardustModel::alloc(pos, vel, static_cast<CIColor::Value>(stardustJson->getInt("color")));
            stardustContainer->addStardust(stardust);
        }

        // recreate the opponent planets
        std::shared_ptr<cugl::JsonValue> opponentPlanetsJson = _json->get("otherPlanets");
        std::vector<std::shared_ptr<OpponentPlanet>> opponent_planets;
        opponent_planets.resize(opponentPlanetsJson->size());
        std::shared_ptr<OpponentPlanet> opponent_planet;
        for (size_t kk = 0; kk < opponent_planets.size(); kk++) {
            opponent_planet = OpponentPlanet::alloc(0, 0, static_cast<CIColor::Value>(opponentPlanetsJson->get(kk)->getInt("color")));
            opponent_planet->setMass(opponentPlanetsJson->get(kk)->getFloat("mass"));
            opponent_planets[kk] = opponent_planet;
        }

//        _gameplay.addChildWithName(planet->getPlanetNode(), "planet");
//        _gameplay.addChildWithName(stardustContainer->getStardustNode(), "stardustQueue");
//        _gameplay.setOpponentPlanets(opponent_planets);
        
        _json = JsonValue::allocObject();
    }
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
        _loading.update(0.01f);
    } else if (!_loaded) { 
        _loading.dispose(); // Disables the input listeners to this mode
        if (_networkMessageManager == nullptr) {
            _networkMessageManager = NetworkMessageManager::alloc();
        }
        _gameplay.init(_assets, _networkMessageManager, _loading._joinGame.empty(), _loading._joinGame);
        _loaded = true;
    } else if (_gameplay.isActive()) { 
        _gameplay.update(timestep);
    } else {
        // handle game reset
        _gameplay.setActive(true);
        _gameplay.dispose();

        _networkMessageManager = nullptr;

        _loading.removeAllChildren();
        _loaded = false;
        _loading.init(_assets);
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


