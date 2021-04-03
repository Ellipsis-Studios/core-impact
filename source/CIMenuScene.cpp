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
    _assets->loadDirectory("json/menu.json");
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

    /** Settings labels */
    _settingsTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingstitle"));
    _pnameLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsnamelabel"));
    _musicLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsmusiclabel"));
    _volumeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsvolumelabel"));
    _parallaxLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsparallaxlabel"));
    /** Settings inputs */
    _musicBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsmusicbutton"));
    _parallaxBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsparallaxbutton"));
    _volumeSlider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("menu_settingsvolumeinput"));
    _pnameInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_settingsnameinput"));
    _playerName = _pnameInput->getText(); // set player name using name textfield in Settings

    /** Join Game inputs */
    _roomIdInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_joingameroomidinput"));
    _roomJoinBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joingamejoinbutton"));
    _joinGame = _roomIdInput->getText(); // set _joinGame value using room id textfield in Join Game

    /** Game Lobby labels */
    _lobbyRoomLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyroomidlabel"));
    _gamelobbyplayerlabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel1"));
    _gamelobbyplayerlabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel2"));
    _gamelobbyplayerlabel3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel3"));
    _gamelobbyplayerlabel4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel4"));
    _gamelobbyplayerlabel5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel5"));
    /** Game Lobby buttons */
    _gameStartBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_gamelobbystartbutton"));
    
    /** Tutorial title label */
    _tutorialTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_tutorialtitle"));

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
    _roomJoinBtn->addListener(mbuttonListener(MenuStatus::JoinToLobby));
    _gameStartBtn->addListener(mbuttonListener(MenuStatus::LobbyToGame));
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
    /** Volume slider listeners */
    _volume = _volumeSlider->getValue(); // update volume using Volume Slider in Settings 
    _volumeSlider->addListener([=](const std::string& name, float value) {
        if (value != _volume) {
            // TODO: Update global game volume
            _volume = value;
        }
        });
    /** Player name input listeners */
    _pnameInput->addTypeListener([=](const std::string& name, const std::string& value) {
        // Handle size limit on settings name textfield
        if (value.length() > 12) {
            _pnameInput->setText(value.substr(0, 12));
        }
        });
    _pnameInput->addExitListener([=](const std::string& name, const std::string& value) {
        // Set player name input
        _playerName = value;
        CULog("Player Name set to %s", value.c_str());
        });
    /** Join game room id input listeners */
    _roomIdInput->addTypeListener([=](const std::string& name, const std::string& value) {
        // Handle room id textfield with size limit
        if (value.length() > 5) {
            _roomIdInput->setText(value.substr(value.length() - 5, 5));
        }
        });
    _roomIdInput->addExitListener([=](const std::string& name, const std::string& value) {
        // Set game room id input 
        _joinGame = value;
        CULog("Room id set to %s", value.c_str());
        });

    /** Set both toggle buttons. */
    _musicBtn->setToggle(true);
    _parallaxBtn->setToggle(true);

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
        _roomIdInput->deactivate();
        _roomJoinBtn->deactivate();
        _gameStartBtn->deactivate();
        _pnameInput->deactivate();
        _musicBtn->deactivate();
        _volumeSlider->deactivate();
        _parallaxBtn->deactivate();
        _backBtn->deactivate();
    }

    _teamLogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _joinBtn = nullptr;
    _newBtn = nullptr;
    _tutorialBtn = nullptr;

    _settingsBtn = nullptr;
    _settingsTitle = nullptr;
    _pnameLabel = nullptr;
    _pnameInput = nullptr;
    _musicLabel = nullptr;
    _musicBtn = nullptr;
    _volumeLabel = nullptr;
    _volumeSlider = nullptr;
    _parallaxLabel = nullptr;
    _parallaxBtn = nullptr;

    _roomIdInput = nullptr;
    _roomJoinBtn = nullptr;

    _lobbyRoomLabel = nullptr;
    _gamelobbyplayerlabel1 = nullptr;
    _gamelobbyplayerlabel2 = nullptr;
    _gamelobbyplayerlabel3 = nullptr;
    _gamelobbyplayerlabel4 = nullptr;
    _gamelobbyplayerlabel5 = nullptr;
    _gameStartBtn = nullptr;

    _tutorialTitle = nullptr;

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
            _isLoaded = true;
            Scene2::reset();
        }
        break;
    case MenuStatus::MainToSetting:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn, _pnameInput, _musicBtn, _volumeSlider, _parallaxBtn);
        _settingsTitle->setVisible(true);
        _pnameLabel->setVisible(true);
        _musicLabel->setVisible(true);
        _volumeLabel->setVisible(true);
        _parallaxLabel->setVisible(true);
        _status = MenuStatus::Setting;
        break;
    case MenuStatus::MainToJoin:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn, _roomIdInput, _roomJoinBtn);
        _status = MenuStatus::JoinRoom;
        break;
    case MenuStatus::MainToLobby:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn, _gameStartBtn);
        _lobbyRoomLabel->setVisible(true);
        _gamelobbyplayerlabel1->setVisible(true);
        _gamelobbyplayerlabel2->setVisible(true);
        _gamelobbyplayerlabel3->setVisible(true);
        _gamelobbyplayerlabel4->setVisible(true);
        _gamelobbyplayerlabel5->setVisible(true);
        _status = MenuStatus::GameLobby;
        break;
    case MenuStatus::MainToTutorial:
        _menuSceneInputHelper(false, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _menuSceneInputHelper(true, _backBtn);
        _tutorialTitle->setVisible(true);
        _status = MenuStatus::Tutorial;
        break;
    case MenuStatus::Setting:
        if (_musicBtn->isActive()) {
            _musicOn = !_musicBtn->isDown();
        } 
        if (_parallaxBtn->isActive()) {
            _parallaxOn = !_parallaxBtn->isDown();
        }
        break;
    case MenuStatus::JoinRoom:
        break;
    case MenuStatus::Tutorial:
        break;
    case MenuStatus::GameLobby:
        _gamelobbyplayerlabel1->setText(_playerName);
        break;
    case MenuStatus::JoinToLobby:
        _menuSceneInputHelper(false, _backBtn, _roomJoinBtn, _roomIdInput);
        _menuSceneInputHelper(true, _backBtn, _gameStartBtn);
        _lobbyRoomLabel->setVisible(true);
        _gamelobbyplayerlabel1->setVisible(true);
        _gamelobbyplayerlabel2->setVisible(true);
        _gamelobbyplayerlabel3->setVisible(true);
        _gamelobbyplayerlabel4->setVisible(true);
        _gamelobbyplayerlabel5->setVisible(true);
        _status = MenuStatus::GameLobby;
        break;
    case MenuStatus::SettingToMain:
        _menuSceneInputHelper(false, _backBtn, _pnameInput, _musicBtn, _volumeSlider, _parallaxBtn);
        _settingsTitle->setVisible(false);
        _pnameLabel->setVisible(false);
        _musicLabel->setVisible(false);
        _volumeLabel->setVisible(false);
        _parallaxLabel->setVisible(false);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::JoinToMain:
        _menuSceneInputHelper(false, _backBtn, _roomIdInput, _roomJoinBtn);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::LobbyToMain:
        _menuSceneInputHelper(false, _backBtn, _gameStartBtn);
        _lobbyRoomLabel->setVisible(false);
        _gamelobbyplayerlabel1->setVisible(false);
        _gamelobbyplayerlabel2->setVisible(false);
        _gamelobbyplayerlabel3->setVisible(false);
        _gamelobbyplayerlabel4->setVisible(false);
        _gamelobbyplayerlabel5->setVisible(false);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::TutorialToMain:
        _menuSceneInputHelper(false, _backBtn);
        _tutorialTitle->setVisible(false);
        _menuSceneInputHelper(true, _joinBtn, _newBtn, _tutorialBtn, _settingsBtn);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::LobbyToGame:
        _menuSceneInputHelper(false, _backBtn, _gameStartBtn);
        _lobbyRoomLabel->setVisible(false);
        _gamelobbyplayerlabel1->setVisible(false);
        _gamelobbyplayerlabel2->setVisible(false);
        _gamelobbyplayerlabel3->setVisible(false);
        _gamelobbyplayerlabel4->setVisible(false);
        _gamelobbyplayerlabel5->setVisible(false);
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
