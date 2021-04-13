//
//  CIMenuScene.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 3/27/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIMenuScene.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

#pragma mark -
#pragma mark Constructors

/**
 * Initializes the menu scene with the provided menu settings values.
 *
 * Used only as a helper to the other init methods.
 *
 * @param assets        The (loaded) assets for this game mode
 * @param playerName    The player name value
 * @param volume        The game volume setting value
 * @param musicOn       The musicOn setting value
 * @param parallaxOn    The parallax effect setting value
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
    string playerName, float volume, bool musicOn, bool parallaxOn) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("menu");
    layer->setContentSize(dimen);
    layer->doLayout();

    _teamLogo = assets->get<scene2::SceneNode>("menu_teamLogo");
    _gameTitle = assets->get<scene2::SceneNode>("menu_title");
    _gamePlanet = assets->get<scene2::SceneNode>("menu_world");
    const float rend = dimen.getIWidth()/ 4.0f; // Set game title/planet position to one from end of loading scene
    _gameTitle->setPositionX(rend);
    _gamePlanet->setPositionX(rend);

    /** Back button to return to main menu */
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_menubackbutton"));
    _backBtn->addListener([=](const std::string& name, bool down) {
        if (!down) {
            switch (_state) 
            {
                case MenuState::Setting:
                    _state = MenuState::SettingToMain;
                    break;
                case MenuState::JoinRoom:
                    _state = MenuState::JoinToMain;
                    break;
                case MenuState::GameLobby:
                    _state = MenuState::LobbyToMain;
                    break;
                case MenuState::Tutorial:
                    _state = MenuState::TutorialToMain;
                    break;
                default:
                    break;
            }
        }
        });

    // TODO: integrate network manager to game lobby
    
    /** Player settings */
    _playerName = playerName;
    _volume = volume;
    _musicOn = musicOn;
    _parallaxOn = parallaxOn;

    /** Default game settings */
    _otherNames = vector<string>{ "N/A", "N/A", "N/A", "N/A" };
    _joinGame = "";
    _spawnRate = 1.0f;
    _gravStrength = 1.0f;
    _colorCount = 6;
    _gameLength = 200;

    _state = MenuState::LoadToMain;

    Application::get()->setClearColor(Color4(192, 192, 192, 255));
    addChildWithName(layer, "menuScene");

    _mainmenu = MainMenu::alloc(_assets);
    _mainmenu->setDisplay(false);

    _tutorial = TutorialMenu::alloc(_assets);
    _tutorial->setDisplay(false);

    _settings = SettingsMenu::alloc(_assets);
    _settings->setDisplay(false);

    _join = JoinMenu::alloc(_assets);
    _join->setDisplay(false);

    _lobby = LobbyMenu::alloc(_assets);
    _lobby->setDisplay(false, _state);

    addChild(_mainmenu->getLayer(), 0);
    addChild(_tutorial->getLayer(), 1);
    addChild(_settings->getLayer(), 1);
    addChild(_join->getLayer(), 2);
    addChild(_lobby->getLayer(), 3);

    return true;
}

/**
 * The method initializes the menu scene using player settings json value.
 *
 * Meant to be used to initialize the menu scene only on initial load
 *
 * @param assets            The (loaded) assets for this game mode
 * @param playerSettings    The player's saved settings value
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<cugl::JsonValue>& playerSettings) {
    // Load in saved settings file
    _playerName = "Player Name";
    _volume = 0.5f;
    _musicOn = true;
    _parallaxOn = true;
    if (playerSettings != nullptr) {
        string pname = playerSettings->getString("PlayerName", "Player Name");
        if (!pname.empty()) {
            _playerName = pname;
        }
        _volume = playerSettings->getFloat("Volume", 0.5f);
        _musicOn = playerSettings->getBool("MusicOn", true);
        _parallaxOn = playerSettings->getBool("ParallaxOn", true);
    }

    return init(assets, _playerName, _volume, _musicOn, _parallaxOn);
}

/**
 * The method initializes the menu scene using its current values.
 *
 * Meant to be used to re-initialize the menu scene on game reset.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    return init(assets, _playerName, _volume, _musicOn, _parallaxOn);
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    // Deactivate the button (platform dependent)
    if (_backBtn != nullptr && _backBtn->isVisible()) {
        _backBtn->deactivate();
    }
    _mainmenu->setDisplay(false);
    _settings->setDisplay(false);
    _join->setDisplay(false);
    _tutorial->setDisplay(false);
    _lobby->setDisplay(false, _state);

    _mainmenu = nullptr;
    _settings = nullptr;
    _join = nullptr;
    _lobby = nullptr;
    _tutorial = nullptr;

    _teamLogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _backBtn = nullptr;
    _assets = nullptr;

    _active = false;
}

#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the menu mode and display the correct scene.
 *
 * This method handles taking down and setting up menu scenes based on 
 * MenuStatus value. There are 5 scenes within Menu Scene. For each 
 * scene to properly display, all its assets must be activated and set to visible 
 * on the previous frame. The transitional statuses handle this by deactivating/setting invisible
 * previous screen's assets then activating/setting visible the upcoming screen's assets.
 * 
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MenuScene::update(float timestep) {
    if (!isActive() || _backBtn == nullptr) {
        return;
    }
    // handle back button
    switch (_state) 
    {
        case MenuState::Setting:
        case MenuState::JoinRoom:
        case MenuState::GameLobby:
        case MenuState::Tutorial:
            // display back button
            if (!_backBtn->isVisible()) {
                _backBtn->setVisible(true);
                _backBtn->activate();
            }
            break;
        default:
            // hide back button
            if (_backBtn->isVisible()) {
                _backBtn->setVisible(false);
                _backBtn->deactivate();
            }
            break;
    }

    // handle menu screens 
    switch (_state) 
    {
        case MenuState::LoadToMain:
            // menu scene start
            _mainmenu->setDisplay(true);
            _state = MenuState::MainMenu;
            break;
        case MenuState::LobbyToGame:
            // menu scene end
            _lobby->setDisplay(false, _state);
            _active = false;
            break;
        default:
            _lobby->update(_state, _joinGame, _playerName, _otherNames,
                _spawnRate, _gravStrength, _colorCount, _gameLength);
            _mainmenu->update(_state);
            _settings->update(_state, _playerName, _volume, _musicOn, _parallaxOn);
            _join->update(_state, _joinGame);
            _tutorial->update(_state);
            break;
    }
}
