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
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE / dimen.width;
    }
    else {
        dimen *= SCENE_SIZE / dimen.height;
    }
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
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
    float rend = dimen.getIWidth()/ 4.0f;
    _gameTitle->setPositionX(rend);
    _gamePlanet->setPositionX(rend);

    /** Initialize main menu scene */
    _mainmenu = assets->get<scene2::SceneNode>("menu_mainmenu");
    _settingsbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_mainmenu_settingsbutton"));
    _joinmenubutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_mainmenu_joinbutton"));
    _newmenubutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_mainmenu_newbutton"));
    _tutorialmenubutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_mainmenu_tutorialbutton"));
    
    /** Initialize settings scene */
    _settings = assets->get<scene2::SceneNode>("menu_settings");
    _settingsbackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settings_back"));
    _settingstitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settings_title"));
    _settingsnamelabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settings_namelabel"));
    _settingsnameinput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_settings_nameinput"));
    _settingsmusiclabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settings_musiclabel"));
    _settingsmusicbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settings_musicinput"));
    _settingsvolumelabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settings_volumelabel"));
    _settingsvolumeslider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("menu_settings_volumeinput"));
    _settingsparallaxlabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_settings_parallaxlabel"));
    _settingsparallaxbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settings_parallaxinput"));
    
    /** Set settings scene to invisible */
    _settingstitle->setVisible(false);
    _settingsnamelabel->setVisible(false);
    _settingsnameinput->setVisible(false);
    _settingsmusiclabel->setVisible(false);
    _settingsmusicbutton->setVisible(false);
    _settingsvolumelabel->setVisible(false);
    _settingsvolumeslider->setVisible(false);
    _settingsparallaxlabel->setVisible(false);
    _settingsparallaxbutton->setVisible(false);
    _settings->setVisible(false);

    /** Initialize joingame scene then set invisible */
    _joingame = assets->get<scene2::SceneNode>("menu_joingame");
    _joingamebackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joingame_back"));
    _joingametextinput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_joingame_roomidinput"));
    _joingamejoinbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joingame_joinbutton"));
    _joingame->setVisible(false);

    /** Initialize game lobby then set invisible */
    _gamelobby = assets->get<scene2::SceneNode>("menu_gamelobby");
    _gamelobbybackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_gamelobby_back"));
    _gamelobbyroomidlabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobby_roomidlabel"));
    _gamelobbyplayerlabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobby_playerlabel1"));
    _gamelobbyplayerlabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobby_playerlabel2"));
    _gamelobbyplayerlabel3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobby_playerlabel3"));
    _gamelobbyplayerlabel4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobby_playerlabel4"));
    _gamelobbyplayerlabel5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_gamelobby_playerlabel5"));
    _gamelobbystartbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_gamelobby_startbutton"));
    _gamelobby->setVisible(false);
    // TODO: integrate network manager to game lobby
    
    /** Initialize tutorial page then set invisible */
    _tutorial = assets->get<scene2::SceneNode>("menu_tutorial");
    _tutorialbackbutton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_tutorial_back"));
    _tutorial->setVisible(false);

    /** Add listeners to main menu buttons */
   _settingsbutton->addListener([=](const std::string& name, bool down) {
        if (!down) {
            /** Removes other buttons' listeners */
            _joinmenubutton->clearListeners();
            _newmenubutton->clearListeners();
            _tutorialmenubutton->clearListeners();
            _mainmenu->setVisible(false);

            /** Transtition to Settings */
            _status = MenuStatus::MainToSetting;
        }
   });

   _joinmenubutton->addListener([=](const std::string& name, bool down) {
        if (!down) {
            /** Removes other buttons' listeners */
            _settingsbutton->clearListeners();
            _newmenubutton->clearListeners();
            _tutorialmenubutton->clearListeners();
            _mainmenu->setVisible(false);

            /** Transtition to Join Game */
            _status = MenuStatus::MainToJoin;
        }
   });

    _newmenubutton->addListener([=](const std::string& name, bool down) {
        if (!down) {
            /** Removes other buttons' listeners */
            _settingsbutton->clearListeners();
            _joinmenubutton->clearListeners();
            _tutorialmenubutton->clearListeners();
            _mainmenu->setVisible(false);

            /** Transtition to Game Lobby */
            _status = MenuStatus::MainToLobby;
        }
    });

    _tutorialmenubutton->addListener([=](const std::string& name, bool down) {
        if (!down) {
            /** Removes other buttons' listeners */
            _settingsbutton->clearListeners();
            _joinmenubutton->clearListeners();
            _newmenubutton->clearListeners();
            _mainmenu->setVisible(false);
            
            /** Transition to Tutorial */
            _status = MenuStatus::MainToTutorial;
        }
    });

    if (_active) {
        _settingsbutton->activate();
        _joinmenubutton->activate();
        _newmenubutton->activate();
        _tutorialmenubutton->activate();

    }
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
    _settings = nullptr;

    _assets = nullptr;
    _teamlogo = nullptr;
    _gameTitle = nullptr;
    _gamePlanet = nullptr;
    _mainmenu = nullptr;
    _joinmenubutton = nullptr;
    _newmenubutton = nullptr;
    _tutorialmenubutton = nullptr;

    _mainmenu = nullptr;
    _joinmenubutton = nullptr;
    _newmenubutton = nullptr;
    _tutorialmenubutton = nullptr;

    _joingame = nullptr;
    _joingamebackbutton = nullptr;
    _joingametextinput = nullptr;
    _joingamejoinbutton = nullptr;

    _gamelobby = nullptr;
    _gamelobbybackbutton = nullptr;
    _gamelobbyroomidlabel = nullptr;
    _gamelobbyplayerlabel1 = nullptr;
    _gamelobbyplayerlabel2 = nullptr;
    _gamelobbyplayerlabel3 = nullptr;
    _gamelobbyplayerlabel4 = nullptr;
    _gamelobbyplayerlabel5 = nullptr;
    _gamelobbystartbutton = nullptr;

    _tutorial = nullptr;
    _tutorialbackbutton = nullptr;

    _isLoaded = false;
}

/**
 * Resets the resources allocated to this mode to default.
 */
void MenuScene::reset() {
    if (isActive() && !_isLoaded) {
        _gameTitle->setVisible(true);
        _gamePlanet->setVisible(true);
        _teamlogo->setVisible(true);
        
        if (!_joinmenubutton->isActive()) {
            _joinmenubutton->activate();
        }
        if (!_newmenubutton->isActive()) {
            _newmenubutton->activate();
        }
        if (!_tutorialmenubutton->isActive()) {
            _tutorialmenubutton->activate();
        }
        _joinmenubutton->setVisible(true);
        _newmenubutton->setVisible(true);
        _tutorialmenubutton->setVisible(true);
        _mainmenu->setVisible(true);

        _isLoaded = true;

        Scene2::reset();
    }
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
    case MainMenu:
        break;
    case MainToSetting:
    {
        /** Clears/Deactivates previous scene's resources */
        _settingsbutton->clearListeners();
        _settingsbutton->deactivate();
        _joinmenubutton->deactivate();
        _newmenubutton->deactivate();
        _tutorialmenubutton->deactivate();

        /** Removes current scene's input listeners then set new listener */
        if (_settingsbackbutton->hasListener()) {
            _settingsbackbutton->clearListeners(); // prevent running out of listeners
        }
        _settingsbackbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Handle returning to main menu */
                _settingsnameinput->clearExitListeners();
                _settingsmusicbutton->clearListeners();
                _settingsvolumeslider->clearListeners();

                _settingsvolumeslider->setVisible(false);
                _settingsvolumelabel->setVisible(false);
                _settingsnameinput->setVisible(false);
                _settingsmusicbutton->setVisible(false);
                _settingsvolumelabel->setVisible(false);
                _settingsvolumeslider->setVisible(false);
                _settingsparallaxlabel->setVisible(false);
                _settingsparallaxbutton->setVisible(false);
                _settings->setVisible(false);

                _status = MenuStatus::SettingToMain;
            }
        });

        if (_settingsnameinput->hasExitListener()) {
            _settingsnameinput->clearExitListeners();
        }
        _settingsnameinput->addExitListener([=](const std::string& name, const std::string& value) {
            CULog("Player Name inputted %s", value.c_str());
            // TODO: Set player name
        });

        if (_settingsmusicbutton->hasListener()) {
            _settingsmusicbutton->clearListeners();
        }
        _settingsmusicbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                CULog("Music toggle button pressed in settings");
                // TODO: Toggle game music off/on
            }
        });

        if (_settingsvolumeslider->hasListener()) {
            _settingsvolumeslider->clearListeners();
        }
        _settingsvolumeslider->addListener([=](const std::string& name, float value) {
            if (value != _volume) {
                // TODO: Update global game volume
                _volume = value;
            }
        });

        if (_settingsparallaxbutton->hasListener()) {
            _settingsparallaxbutton->clearListeners();
        }
        _settingsparallaxbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                CULog("Parallax effect toggle button pressed");
                // TODO: Toggle game's parallax effect
            }
        });

        _settingstitle->setVisible(true);
        _settingsnamelabel->setVisible(true);
        _settingsmusiclabel->setVisible(true);
        _settingsvolumelabel->setVisible(true);
        _settingsparallaxlabel->setVisible(true);

        _settingsbackbutton->activate();
        _settingsbackbutton->setVisible(true);
        _settingsnameinput->activate();
        _settingsnameinput->setVisible(true);
        _settingsmusicbutton->activate();
        _settingsmusicbutton->setVisible(true);
        _settingsvolumeslider->activate();
        _settingsvolumeslider->setVisible(true);
        _settingsparallaxbutton->activate();
        _settingsparallaxbutton->setVisible(true);
        _settings->setVisible(true);
        _status = MenuStatus::Setting;
        break;
    }
    case MainToJoin:
    {
        /** Handle transition from main menu to join game scenes */
        _joinmenubutton->clearListeners();
        _joinmenubutton->setVisible(false);
        _settingsbutton->deactivate();
        _joinmenubutton->deactivate();
        _newmenubutton->deactivate();
        _tutorialmenubutton->deactivate();

        if (_joingamebackbutton->hasListener()) {
            _joingamebackbutton->clearListeners();
        }
        _joingamebackbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger return to main menu */
                _joingamejoinbutton->clearListeners();
                _joingametextinput->clearExitListeners();
                _joingame->setVisible(false);
                _status = MenuStatus::JoinToMain;
            }
        });

        if (_joingametextinput->hasExitListener()) {
            _joingametextinput->clearExitListeners();
        }
        _joingametextinput->addExitListener([=](const std::string& name, const std::string& value) {
            /** Handles room id input by user */
            CULog("Room id inputted %s", value.c_str());
            _joinGame = value;
        });

        if (_joingamejoinbutton->hasListener()) {
            _joingamejoinbutton->clearListeners();
        }
        _joingamejoinbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger join game lobby */
                _joingamebackbutton->clearListeners();
                _joingametextinput->clearExitListeners();
                _joingame->setVisible(false);
                _status = MenuStatus::JoinToLobby;
            }
        });

        _joingamebackbutton->activate();
        _joingamebackbutton->setVisible(true);
        _joingametextinput->activate();
        _joingametextinput->setVisible(true);
        _joingamejoinbutton->activate();
        _joingamejoinbutton->setVisible(true);
        _joingame->setVisible(true);
        _status = MenuStatus::JoinRoom;
        break;
    }
    case MainToLobby:
    {
        /** Handle transition from main menu to game lobby */
        _newmenubutton->clearListeners();
        _settingsbutton->deactivate();
        _joinmenubutton->deactivate();
        _newmenubutton->deactivate();
        _tutorialmenubutton->deactivate();

        if (_gamelobbybackbutton->hasListener()) {
            _gamelobbybackbutton->clearListeners();
        }
        _gamelobbybackbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger return to main menu */
                _gamelobbystartbutton->clearListeners();
                _gamelobby->setVisible(false);
                _status = MenuStatus::LobbyToMain;
            }
            });
        _gamelobbybackbutton->activate();
        _gamelobbybackbutton->setVisible(true);

        if (_gamelobbystartbutton->hasListener()) {
            _gamelobbystartbutton->clearListeners();
        }
        _gamelobbystartbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger create new game room */
                _gamelobbybackbutton->clearListeners();
                _gamelobby->setVisible(false);
                _status = MenuStatus::LobbyToGame;
            }
        });
        _gamelobbystartbutton->activate();
        _gamelobbystartbutton->setVisible(true);
        
        _gamelobby->setVisible(true);
        _status = MenuStatus::GameLobby;
        break;
    }
    case MainToTutorial:
    {
        /** Handle transition from main menu to tutorial */
        _tutorialmenubutton->clearListeners();
        _settingsbutton->deactivate();
        _joinmenubutton->deactivate();
        _newmenubutton->deactivate();
        _tutorialmenubutton->deactivate();

        if (_tutorialbackbutton->hasListener()) {
            _tutorialbackbutton->clearListeners();
        }
        _tutorialbackbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger return to main menu */
                _tutorial->setVisible(false);
                _status = MenuStatus::TutorialToMain;
            }
        });
        _tutorialbackbutton->activate();
        _tutorialbackbutton->setVisible(true);

        _tutorial->setVisible(true);
        _status = MenuStatus::Tutorial;
        break;
    }
    case Setting:
        break;
    case JoinRoom:
        break;
    case Tutorial:
        break;
    case GameLobby:
        // TODO: Integrate network manager to create/join games with game lobby labels updating
        break;
    case JoinToLobby:
    {
        /** Handle transition from Join Game to Game Lobby */
        _joingamejoinbutton->clearListeners();
        _joingamebackbutton->setVisible(false);
        _joingametextinput->setVisible(false);
        _joingamejoinbutton->setVisible(false);
        _joingamebackbutton->deactivate();
        _joingametextinput->deactivate();
        _joingamejoinbutton->deactivate();

        if (_gamelobbybackbutton->hasListener()) {
            _gamelobbybackbutton->clearListeners();
        }
        _gamelobbybackbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger return to main menu */
                _gamelobbystartbutton->clearListeners();
                _gamelobby->setVisible(false);
                _status = MenuStatus::LobbyToMain;
            }
        });
        _gamelobbybackbutton->activate();
        _gamelobbybackbutton->setVisible(true);

        if (_gamelobbystartbutton->hasListener()) {
            _gamelobbystartbutton->clearListeners();
        }
        _gamelobbystartbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger launching game from lobby */
                _gamelobbybackbutton->clearListeners();
                _gamelobby->setVisible(false);
                _status = MenuStatus::LobbyToGame;
            }
        });
        _gamelobbystartbutton->activate();
        _gamelobbystartbutton->setVisible(true);
        
        _gamelobby->setVisible(true);
        _status = MenuStatus::GameLobby;
        break;
    }
    case SettingToMain:
    {
        /** Handles transition from settings back to main menu */
        _settingsbackbutton->setVisible(false);
        _settingstitle->setVisible(false);
        _settingsbackbutton->deactivate();

        _settingsbutton->activate();
        _joinmenubutton->activate();
        _newmenubutton->activate();
        _tutorialmenubutton->activate();

        if (_settingsbutton->hasListener()) {
            _settingsbutton->clearListeners();
        }
        _settingsbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to settings */
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToSetting;
            }
        });

        if (_joinmenubutton->hasListener()) {
            _joinmenubutton->clearListeners();
        }
        _joinmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to join game */
                _settingsbutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToJoin;
            }
        });

        if (_newmenubutton->hasListener()) {
            _newmenubutton->clearListeners();
        }
        _newmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to game lobby */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToLobby;
            }
        });

        if (_tutorialmenubutton->hasListener()) {
            _tutorialmenubutton->clearListeners();
        }
        _tutorialmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to tutorial */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToTutorial;
            }
        });

        _mainmenu->setVisible(true);
        _status = MenuStatus::MainMenu;
        break;
    }
    case JoinToMain:
    {
        /** Handle transition from join game back to main menu */
        _joingamebackbutton->clearListeners();
        _joingamebackbutton->deactivate();
        _joingametextinput->deactivate();
        _joingamejoinbutton->deactivate();

        _settingsbutton->activate();
        _joinmenubutton->activate();
        _newmenubutton->activate();
        _tutorialmenubutton->activate();

        if (_settingsbutton->hasListener()) {
            _settingsbutton->clearListeners();
        }
        _settingsbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to settings */
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToSetting;
            }
        });

        if (_joinmenubutton->hasListener()) {
            _joinmenubutton->clearListeners();
        }
        _joinmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to join game */
                _settingsbutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToJoin;
            }
        });

        if (_newmenubutton->hasListener()) {
            _newmenubutton->clearListeners();
        }
        _newmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to game lobby */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToLobby;
            }
        });

        if (_tutorialmenubutton->hasListener()) {
            _tutorialmenubutton->clearListeners();
        }
        _tutorialmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to tutorial */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToTutorial;
            }
        });

        _settingsbutton->setVisible(true);
        _joinmenubutton->setVisible(true);
        _newmenubutton->setVisible(true);
        _tutorialmenubutton->setVisible(true);
        _mainmenu->setVisible(true);
        _status = MenuStatus::MainMenu;
        break;
    }
    case LobbyToMain:
    {
        /** Handle transition from game lobby back to main menu */
        _gamelobbybackbutton->clearListeners();
        _gamelobbybackbutton->deactivate();
        _gamelobbystartbutton->deactivate();

        _settingsbutton->activate();
        _joinmenubutton->activate();
        _newmenubutton->activate();
        _tutorialmenubutton->activate();

        if (_settingsbutton->hasListener()) {
            _settingsbutton->clearListeners();
        }
        _settingsbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to settings */
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToSetting;
            }
        });

        if (_joinmenubutton->hasListener()) {
            _joinmenubutton->clearListeners();
        }
        _joinmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to join game */
                _settingsbutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToJoin;
            }
        });

        if (_newmenubutton->hasListener()) {
            _newmenubutton->clearListeners();
        }
        _newmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to game lobby */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToLobby;
            }
        });

        if (_tutorialmenubutton->hasListener()) {
            _tutorialmenubutton->clearListeners();
        }
        _tutorialmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to tutorial */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToTutorial;
            }
        });

        _settingsbutton->setVisible(true);
        _joinmenubutton->setVisible(true);
        _newmenubutton->setVisible(true);
        _tutorialmenubutton->setVisible(true);
        _mainmenu->setVisible(true);
        _status = MenuStatus::MainMenu;
        break;
    }
    case TutorialToMain:
    {
        /** Handle transition from tutorial back to main menu */
        _tutorialbackbutton->deactivate();

        _settingsbutton->activate();
        _joinmenubutton->activate();
        _newmenubutton->activate();
        _tutorialmenubutton->activate();

        if (_settingsbutton->hasListener()) {
            _settingsbutton->clearListeners();
        }
        _settingsbutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to settings */
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToSetting;
            }
        });

        if (_joinmenubutton->hasListener()) {
            _joinmenubutton->clearListeners();
        }
        _joinmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to join game */
                _settingsbutton->clearListeners();
                _newmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToJoin;
            }
        });

        if (_newmenubutton->hasListener()) {
            _newmenubutton->clearListeners();
        }
        _newmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to game lobby */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _tutorialmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToLobby;
            }
        });

        if (_tutorialmenubutton->hasListener()) {
            _tutorialmenubutton->clearListeners();
        }
        _tutorialmenubutton->addListener([=](const std::string& name, bool down) {
            if (!down) {
                /** Trigger transition from main menu to tutorial */
                _settingsbutton->clearListeners();
                _joinmenubutton->clearListeners();
                _newmenubutton->clearListeners();
                _mainmenu->setVisible(false);
                _status = MenuStatus::MainToTutorial;
            }
        });

        _mainmenu->setVisible(true);
        _status = MenuStatus::MainMenu;
        break;
    }
    case LobbyToGame:
        /** Handle transition from game lobby to game */
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
