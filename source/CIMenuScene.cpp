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

    _teamlogo = assets->get<scene2::SceneNode>("menu_teamlogo");
    _gameTitle = assets->get<scene2::SceneNode>("menu_title");
    _gamePlanet = assets->get<scene2::SceneNode>("menu_world");
    /** Set game title/planet position to one from end of loading scene */
    const float rend = dimen.getIWidth()/ 4.0f;
    _gameTitle->setPositionX(rend);
    _gamePlanet->setPositionX(rend);

    /** Initialize main menu scene */
    _settingsbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsbutton"));
    _joinmenubutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joinbutton"));
    _newmenubutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_newbutton"));
    _tutorialmenubutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_tutorialbutton"));

    /** Initialize settings scene */
    _settingsbackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsback"));
    _settingstitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingstitle"));
    _settingsnamelabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsnamelabel"));
    _settingsnameinput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_settingsnameinput"));
    _settingsmusiclabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsmusiclabel"));
    _settingsmusicbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsmusicbutton"));
    _settingsvolumelabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsvolumelabel"));
    _settingsvolumeslider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("menu_settingsvolumeinput"));
    _settingsparallaxlabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settingsparallaxlabel"));
    _settingsparallaxbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settingsparallaxbutton"));
    _playerName = _settingsnameinput->getText();

    /** Initialize joingame scene then set invisible */
    _joingamebackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joingameback"));
    _joingametextinput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_joingameroomidinput"));
    _joingamejoinbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joingamejoinbutton"));
    _joinGame = _joingametextinput->getText();

    /** Initialize game lobby then set invisible */
    _gamelobbybackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_gamelobbyback"));
    _gamelobbyroomidlabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyroomidlabel"));
    _gamelobbyplayerlabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel1"));
    _gamelobbyplayerlabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel2"));
    _gamelobbyplayerlabel3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel3"));
    _gamelobbyplayerlabel4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel4"));
    _gamelobbyplayerlabel5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobbyplayerlabel5"));
    _gamelobbystartbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_gamelobbystartbutton"));
    
    /** Initialize tutorial page then set invisible */
    _tutorialbackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_tutorialback"));
    _tutorialsceneheader = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_tutorialtitle"));

    // TODO: integrate network manager to game lobby

    /** Button listener function for menu scene buttons. */
    const auto menuButtonHandler = [=](const std::string& name, bool down) {
        if (!down) {
            // Main Menu buttons
            if (name == "joinbutton")
                _status = MenuStatus::MainToJoin;
            else if (name == "settingsbutton")
                _status = MenuStatus::MainToSetting;
            else if (name == "newbutton") {
                _status = MenuStatus::MainToLobby;
                _joinGame = "";
            }
            else if (name == "tutorialbutton")
                _status = MenuStatus::MainToTutorial;
            // Join Game buttons
            else if (name == "joingameback")
                _status = MenuStatus::JoinToMain;
            else if (name == "joingamejoinbutton")
                _status = MenuStatus::JoinToLobby;
            // Game Lobby buttons
            else if (name == "gamelobbyback")
                _status = MenuStatus::LobbyToMain;
            else if (name == "gamelobbystartbutton")
                _status = MenuStatus::LobbyToGame;
            // Settings butons
            else if (name == "settingsback")
                _status = MenuStatus::SettingToMain;
            else if (name == "settingsmusicbutton") {
                CULog("Music toggle button pressed in settings");
                // TODO: Toggle game music off/on
                _musicOn = !_musicOn;
            }
            else if (name == "settingsparallaxbutton") {
                CULog("Parallax effect toggle button pressed");
                // TODO: Toggle game's parallax effect
                _parallaxOn = !_parallaxOn;
            }
            // Tutorial buttons
            else if (name == "tutorialback")
                _status = MenuStatus::TutorialToMain;
            // Invalid buttons
            else {
                CULog("Menu scene button not found! Please add update the menuButtonHandler in MenuScene's init method.");
            }
        }
    };

    /** Add listeners to menu scenes */
    _settingsbutton->addListener(menuButtonHandler);
    _joinmenubutton->addListener(menuButtonHandler);
    _newmenubutton->addListener(menuButtonHandler);
    _tutorialmenubutton->addListener(menuButtonHandler);
    _settingsbackbutton->addListener(menuButtonHandler);
    _settingsmusicbutton->addListener(menuButtonHandler);
    _settingsparallaxbutton->addListener(menuButtonHandler);
    _joingamebackbutton->addListener(menuButtonHandler);
    _joingamejoinbutton->addListener(menuButtonHandler);
    _gamelobbybackbutton->addListener(menuButtonHandler);
    _gamelobbystartbutton->addListener(menuButtonHandler);
    _tutorialbackbutton->addListener(menuButtonHandler);

    _settingsvolumeslider->addListener([=](const std::string& name, float value) {
        if (value != _volume) {
            // TODO: Update global game volume
            _volume = value;
        }
        });
    _settingsnameinput->addTypeListener([=](const std::string& name, const std::string& value) {
        // Handle size limit on settings name textfield
        if (value.length() > 12) {
            _settingsnameinput->setText(value.substr(0, 12));
        }
        });
    _settingsnameinput->addExitListener([=](const std::string& name, const std::string& value) {
        // Set player name input
        _playerName = value;
        CULog("Player Name set to %s", value.c_str());
        });

    _joingametextinput->addTypeListener([=](const std::string& name, const std::string& value) {
        // Handle room id textfield with size limit
        if (value.length() > 5) {
            _joingametextinput->setText(value.substr(value.length() - 5, 5));
        }
        });
    _joingametextinput->addExitListener([=](const std::string& name, const std::string& value) {
        // Set game room id input 
        std::string val;
        for (auto c : value) {
            if (std::isdigit(c)) {
                val += c;
            }
        }
        while (val.length() < 5) {
            val = "0" + val;
        }
        _joingametextinput->setText(val);
        _joinGame = val;
        CULog("Room id set to %s", val.c_str());
        });

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
        _settingsbutton->deactivate();
        _joinmenubutton->deactivate();
        _newmenubutton->deactivate();
        _tutorialmenubutton->deactivate();
        _joingamebackbutton->deactivate();
        _joingametextinput->deactivate();
        _joingamejoinbutton->deactivate();
        _gamelobbybackbutton->deactivate();
        _gamelobbystartbutton->deactivate();
        _tutorialbackbutton->deactivate();
        _settingsbackbutton->deactivate();
        _settingsnameinput->deactivate();
        _settingsmusicbutton->deactivate();
        _settingsvolumeslider->deactivate();
        _settingsparallaxbutton->deactivate();
    }
    _settingsbutton = nullptr;
    _settingsbackbutton = nullptr;
    _settingstitle = nullptr;
    _settingsnamelabel = nullptr;
    _settingsnameinput = nullptr;
    _settingsmusiclabel = nullptr;
    _settingsmusicbutton = nullptr;
    _settingsvolumelabel = nullptr;
    _settingsvolumeslider = nullptr;
    _settingsparallaxlabel = nullptr;
    _settingsparallaxbutton = nullptr;

    _assets = nullptr;
    _teamlogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _joinmenubutton = nullptr;
    _newmenubutton = nullptr;
    _tutorialmenubutton = nullptr;

    _joinmenubutton = nullptr;
    _newmenubutton = nullptr;
    _tutorialmenubutton = nullptr;

    _joingamebackbutton = nullptr;
    _joingametextinput = nullptr;
    _joingamejoinbutton = nullptr;

    _gamelobbybackbutton = nullptr;
    _gamelobbyroomidlabel = nullptr;
    _gamelobbyplayerlabel1 = nullptr;
    _gamelobbyplayerlabel2 = nullptr;
    _gamelobbyplayerlabel3 = nullptr;
    _gamelobbyplayerlabel4 = nullptr;
    _gamelobbyplayerlabel5 = nullptr;
    _gamelobbystartbutton = nullptr;

    _tutorialbackbutton = nullptr;
    _tutorialsceneheader = nullptr;

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
void MenuScene::update(float progress) {
    switch (_status) {
    case MenuStatus::MainMenu:
        if (isActive() && !_isLoaded) {
            _menuSceneInputHelper(true, _tutorialmenubutton, _settingsbutton, _joinmenubutton, _newmenubutton);
            _isLoaded = true;
            Scene2::reset();
        }
        break;
    case MenuStatus::MainToSetting:
        _menuSceneInputHelper(false, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _menuSceneInputHelper(true, _settingsbackbutton, _settingsnameinput, _settingsmusicbutton, _settingsvolumeslider, _settingsparallaxbutton);
        _settingstitle->setVisible(true);
        _settingsnamelabel->setVisible(true);
        _settingsmusiclabel->setVisible(true);
        _settingsvolumelabel->setVisible(true);
        _settingsparallaxlabel->setVisible(true);
        _status = MenuStatus::Setting;
        break;
    case MenuStatus::MainToJoin:
        _menuSceneInputHelper(false, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _menuSceneInputHelper(true, _joingamebackbutton, _joingametextinput, _joingamejoinbutton);
        _status = MenuStatus::JoinRoom;
        break;
    case MenuStatus::MainToLobby:
        _menuSceneInputHelper(false, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _menuSceneInputHelper(true, _gamelobbybackbutton, _gamelobbystartbutton);
        _gamelobbyroomidlabel->setVisible(true);
        _gamelobbyplayerlabel1->setVisible(true);
        _gamelobbyplayerlabel2->setVisible(true);
        _gamelobbyplayerlabel3->setVisible(true);
        _gamelobbyplayerlabel4->setVisible(true);
        _gamelobbyplayerlabel5->setVisible(true);
        _status = MenuStatus::GameLobby;
        break;
    case MenuStatus::MainToTutorial:
        _menuSceneInputHelper(false, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _menuSceneInputHelper(true, _tutorialbackbutton);
        _tutorialsceneheader->setVisible(true);
        _status = MenuStatus::Tutorial;
        break;
    case MenuStatus::Setting:
        break;
    case MenuStatus::JoinRoom:
        break;
    case MenuStatus::Tutorial:
        break;
    case MenuStatus::GameLobby:
        _gamelobbyplayerlabel1->setText(_playerName);
        break;
    case MenuStatus::JoinToLobby:
        _menuSceneInputHelper(false, _joingamebackbutton, _joingamejoinbutton, _joingametextinput);
        _menuSceneInputHelper(true, _gamelobbybackbutton, _gamelobbystartbutton);
        _gamelobbyroomidlabel->setVisible(true);
        _gamelobbyplayerlabel1->setVisible(true);
        _gamelobbyplayerlabel2->setVisible(true);
        _gamelobbyplayerlabel3->setVisible(true);
        _gamelobbyplayerlabel4->setVisible(true);
        _gamelobbyplayerlabel5->setVisible(true);
        _status = MenuStatus::GameLobby;
        break;
    case MenuStatus::SettingToMain:
        _menuSceneInputHelper(false, _settingsbackbutton, _settingsnameinput, _settingsmusicbutton, _settingsvolumeslider, _settingsparallaxbutton);
        _settingstitle->setVisible(false);
        _settingsnamelabel->setVisible(false);
        _settingsmusiclabel->setVisible(false);
        _settingsvolumelabel->setVisible(false);
        _settingsparallaxlabel->setVisible(false);
        _menuSceneInputHelper(true, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::JoinToMain:
        _menuSceneInputHelper(false, _joingamebackbutton, _joingametextinput, _joingamejoinbutton);
        _menuSceneInputHelper(true, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::LobbyToMain:
        _menuSceneInputHelper(false, _gamelobbybackbutton, _gamelobbystartbutton);
        _gamelobbyroomidlabel->setVisible(false);
        _gamelobbyplayerlabel1->setVisible(false);
        _gamelobbyplayerlabel2->setVisible(false);
        _gamelobbyplayerlabel3->setVisible(false);
        _gamelobbyplayerlabel4->setVisible(false);
        _gamelobbyplayerlabel5->setVisible(false);
        _menuSceneInputHelper(true, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::TutorialToMain:
        _menuSceneInputHelper(false, _tutorialbackbutton);
        _tutorialsceneheader->setVisible(false);
        _menuSceneInputHelper(true, _joinmenubutton, _newmenubutton, _tutorialmenubutton, _settingsbutton);
        _status = MenuStatus::MainMenu;
        break;
    case MenuStatus::LobbyToGame:
        _menuSceneInputHelper(false, _gamelobbybackbutton, _gamelobbystartbutton);
        _gamelobbyroomidlabel->setVisible(false);
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
    return _settingsbutton != nullptr && _settingsbutton->isVisible();
}
