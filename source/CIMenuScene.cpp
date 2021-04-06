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
bool MenuScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("menu");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen

    _teamLogo = assets->get<scene2::SceneNode>("menu_teamLogo");
    _gameTitle = assets->get<scene2::SceneNode>("menu_title");
    _gamePlanet = assets->get<scene2::SceneNode>("menu_world");
    const float rend = dimen.getIWidth()/ 4.0f; // Set game title/planet position to one from end of loading scene
    _gameTitle->setPositionX(rend);
    _gamePlanet->setPositionX(rend);

    /** Main Menu buttons */
    _settingsBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsbutton"));
    _joinBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joinbutton"));
    _newBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_newbutton"));
    _tutorialBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_tutorialbutton"));

    /** Back button */
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_menubackbutton"));

    // TODO: integrate network manager to game lobby
    
    /**
     * Returns button listener to trigger scene switch.
     *
     * @param ms    MenuStatus to switch to on button click
     */
    auto mbuttonListener = [=](MenuStatus ms) {
        return  [=](const std::string& name, bool down) { if (!down) _status = ms; };
    };
    

    /** Add button listeners for triggering menu scene switching. */
    _joinBtn->addListener(mbuttonListener(MenuStatus::MainToJoin));
    _settingsBtn->addListener(mbuttonListener(MenuStatus::MainToSetting));
    _newBtn->addListener(mbuttonListener(MenuStatus::MainToLobby));
    _tutorialBtn->addListener(mbuttonListener(MenuStatus::MainToTutorial));

    /** Back button to trigger return to main menu */
    _backBtn->addListener([=](const std::string& name, bool down) {
        if (!down) {
            switch (_status) {
            case MenuStatus::Setting:
                _status = MenuStatus::SettingToMain;
                break;
            case MenuStatus::JoinRoom:
                _status = MenuStatus::JoinToMain;
                break;
            case MenuStatus::GameLobby:
                _status = MenuStatus::LobbyToMain;
                break;
            case MenuStatus::Tutorial:
                _status = MenuStatus::TutorialToMain;
                break;
            default:
                break;
            }
        }
        });
        
    /** Handle non-button input listeners */
    
    _status = MenuStatus::MainMenu;
    Application::get()->setClearColor(Color4(192, 192, 192, 255));
    addChildWithName(layer, "menuScene");
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _settingsBtn->deactivate();
        _joinBtn->deactivate();
        _newBtn->deactivate();
        _tutorialBtn->deactivate();
        _backBtn->deactivate();
    }
    _tutorial->setDisplay(false);
    _settings->setDisplay(false);
    _join->setDisplay(false);
    _lobby->setDisplay(false);

    _tutorial = nullptr;
    _settings = nullptr;
    _join = nullptr;
    _lobby = nullptr;

    _teamLogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _joinBtn = nullptr;
    _newBtn = nullptr;
    _tutorialBtn = nullptr;
    _settingsBtn = nullptr;

    _backBtn = nullptr;
    _assets = nullptr;

    _isLoaded = false;
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
    switch (_status) {
    case MenuStatus::MainMenu:
        if (isActive() && !_isLoaded) { // only on first call after init
            _menuSceneInputHelper(true, _tutorialBtn, _settingsBtn, _joinBtn, _newBtn);
            auto root = getChildByName("menuScene");

            /** Tutorial screen */
            _tutorial = TutorialMenu::alloc(_assets);
            _tutorial->setDisplay(false);
            root->addChild(_tutorial->getLayer());
            
            /** Settings screen */
            _settings = SettingsMenu::alloc(_assets);
            _settings->setDisplay(false);
            root->addChild(_settings->getLayer());
            
            /** Join Game screen */
            _join = JoinMenu::alloc(_assets);
            _join->setDisplay(false);
            root->addChild(_join->getLayer());
            
            /** Game Lobby screen */
            _lobby = LobbyMenu::alloc(_assets);
            _lobby->setDisplay(false);
            root->addChild(_lobby->getLayer());

            _isLoaded = true;
            Scene2::reset();
        }
        break;
    case MenuStatus::MainToSetting:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn);
        _settings->setDisplay(true);
        _status = MenuStatus::Setting;
        break;
    case MenuStatus::MainToJoin:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn);
        _join->setDisplay(true);
        _status = MenuStatus::JoinRoom;
        break;
    case MenuStatus::MainToLobby:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _lobby->setDisplay(true);
        _menuSceneInputHelper(true, _backBtn);
        _status = MenuStatus::GameLobby;
        break;
    case MenuStatus::MainToTutorial:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn);
        _tutorial->setDisplay(true);
        _status = MenuStatus::Tutorial;
        break;
    case MenuStatus::Setting:
        _settings->update(timestep);
        break;
    case MenuStatus::JoinRoom:
        _join->update(timestep);
        if (_join->getPressedJoin()) {
            _status = MenuStatus::JoinToLobby;
            _join->resetPress();
        }
        break;
    case MenuStatus::Tutorial:
        break;
    case MenuStatus::GameLobby:
        _lobby->setPlayerLabel1(_settings->getPlayerName());
        _lobby->update(timestep);
        if (_lobby->getPressedStart()) {
            _status = MenuStatus::LobbyToGame;
            _lobby->resetPress();
        }
        break;
    case MenuStatus::JoinToLobby:
        _join->setDisplay(false);
        _lobby->setDisplay(true);
        _status = MenuStatus::GameLobby;
        break;
    case MenuStatus::SettingToMain:
        _settings->setDisplay(false);
        _menuSceneInputHelper(false, _backBtn);
        
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::JoinToMain:
        _join->setDisplay(false);
        _menuSceneInputHelper(false, _backBtn);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::LobbyToMain:
        _menuSceneInputHelper(false, _backBtn);
        _lobby->setDisplay(false);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::TutorialToMain:
        _menuSceneInputHelper(false, _backBtn);
        _tutorial->setDisplay(false);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::LobbyToGame:
        _menuSceneInputHelper(false, _backBtn);
        _lobby->setDisplay(false);
        _active = false;
        break;
    }
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool MenuScene::isPending() const {
    return _settingsBtn != nullptr && _settingsBtn->isVisible();
}
