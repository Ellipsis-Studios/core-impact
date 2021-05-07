//
//  CIMenuScene.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 3/27/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIMenuScene.h"
#include "CINetworkMessageManager.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes the controller contents, making it ready for loading
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets                The (loaded) assets for this game mode
 * @param networkMessageManager The network message manager for the game
 * @param gameSettings          The settings for the current game
 * @param playerSettings        The player's saved settings value
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
    const std::shared_ptr<GameSettings>& gameSettings,
    const std::shared_ptr<PlayerSettings>& playerSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
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

    /** Back button to return to main menu */
    _backBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_menubackbutton"));
    _backBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            switch (_state)
            {
                case MenuState::Setting:
                    _state = MenuState::SettingToMain;
                    break;
                case MenuState::NameMenu:
                    _state = MenuState::NameToMain;
                    break;
                case MenuState::JoinRoom:
                    _state = MenuState::JoinToMain;
                    break;
                case MenuState::GameLobby:
                    _state = MenuState::LobbyToMain;
                    break;
                case MenuState::GameSetting:
                    _state = MenuState::GameSettingToLobby;
                    break;
                default:
                    break;
            }
        }
        });

    // game settings 
    _gameSettings = gameSettings;
    // player settings
    _playerSettings = playerSettings;

    _networkMessageManager = networkMessageManager;

    _state = MenuState::LoadToMain;

    Application::get()->setClearColor(Color4(192, 192, 192, 255));
    addChildWithName(layer, "menuScene");

    _mainmenu = MainMenu::alloc(_assets);
    _mainmenu->setDisplay(false);

    _settings = SettingsMenu::alloc(_assets, playerSettings);
    _settings->setDisplay(false);

    _namemenu = NameMenu::alloc(_assets, playerSettings);
    _namemenu->setDisplay(false);

    _join = JoinMenu::alloc(_assets, gameSettings);
    _join->setDisplay(false);

    _lobby = LobbyMenu::alloc(_assets, networkMessageManager, gameSettings, playerSettings);
    _lobby->setDisplay(false);

    _gsettingsmenu = GameSettingsMenu::alloc(_assets, networkMessageManager, gameSettings);
    _gsettingsmenu->setDisplay(false);

    _popupMenu = PopupMenu::alloc(_assets, networkMessageManager, gameSettings, playerSettings);
    _popupMenu->setDisplay(false);

    addChild(_mainmenu->getLayer(), 0);
    addChild(_settings->getLayer(), 1);
    addChild(_namemenu->getLayer(), 1);
    addChild(_join->getLayer(), 2);
    addChild(_lobby->getLayer(), 3);
    addChild(_gsettingsmenu->getLayer(), 4);
    addChild(_popupMenu->getLayer(), 5);

    return true;
}

/**
 * The method initializes the menu scene using its current values.
 *
 * Meant to be used to re-initialize the menu scene on game reset.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    return init(assets, _networkMessageManager, _gameSettings, _playerSettings);
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    removeAllChildren();
    
    // Deactivate the button (platform dependent)
    if (_backBtn != nullptr && _backBtn->isVisible()) {
        _backBtn->deactivate();
    } else if (_backBtn != nullptr) {
        _backBtn->clearListeners();
    }
    
    _mainmenu->setDisplay(false);
    _settings->setDisplay(false);
    _namemenu->setDisplay(false);
    _join->setDisplay(false);
    _lobby->setDisplay(false);
    _gsettingsmenu->setDisplay(false);
    _popupMenu->setDisplay(false);

    _mainmenu->dispose();
    _settings->dispose();
    _namemenu->dispose();
    _join->dispose();
    _lobby->dispose();
    _gsettingsmenu->dispose();
    _popupMenu->dispose();

    _mainmenu = nullptr;
    _settings = nullptr;
    _namemenu = nullptr;
    _join = nullptr;
    _lobby = nullptr;
    _gsettingsmenu = nullptr;
    _popupMenu = nullptr;

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
 * @param networkMessageManager  The network message manager for managing connections to other players
 */
void MenuScene::update(float timestep) {
    if (!isActive() || _backBtn == nullptr) {
        return;
    }

    // handle background display
    switch (_state)
    {
        case MenuState::LoadToMain:
        case MenuState::MainMenu:
            // display
            if (_gameTitle != nullptr) {
                _gameTitle->setVisible(true);
                _gamePlanet->setVisible(true);
            }
            break;
        default:
            // hidden
            _gameTitle->setVisible(false);
            _gamePlanet->setVisible(false);
            break;
    }

    // handle back button display
    switch (_state) 
    {
        case MenuState::Setting:
        case MenuState::NameMenu:
        case MenuState::JoinRoom:
        case MenuState::GameLobby:
        case MenuState::GameSetting:
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

    // handle back button positioning
    const float loffset = getChildByName("menuScene")->getContentWidth() * (0.1f);
    _backBtn->setPositionX(loffset);

    // handle menu screens 
    switch (_state) 
    {
        case MenuState::LoadToMain:
            // menu scene start
            _mainmenu->setDisplay(true);
            _state = MenuState::MainMenu;
            break;
        case MenuState::LobbyToGame:
        case MenuState::MainToTutorial:
            // menu scene end
            _lobby->setDisplay(false);
            _gsettingsmenu->setDisplay(false);
            _active = false;
            break;
        default:
            _popupMenu->update(_state, timestep);
            _lobby->update(_state);
            _gsettingsmenu->update(_state);
            _mainmenu->update(_state);
            _settings->update(_state);
            _namemenu->update(_state);
            _join->update(_state);
            break;
    }
}
