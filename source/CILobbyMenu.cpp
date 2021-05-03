//
//  CILobbyMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CILobbyMenu.h"
#include "CINetworkMessageManager.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void LobbyMenu::dispose() {
    if (_gameStartBtn != nullptr && _gameStartBtn->isActive()) {
        _gameStartBtn->deactivate();
        _spawnRateBtn->deactivate();
        _gravStrengthBtn->deactivate();
        _colorCountBtn->deactivate();
        _winMassBtn->deactivate();
    } else if (_gameStartBtn != nullptr) {
        _gameStartBtn->clearListeners();
        _spawnRateBtn->clearListeners();
        _gravStrengthBtn->clearListeners();
        _colorCountBtn->clearListeners();
        _winMassBtn->clearListeners();
    }
    
    _gameStartBtn = nullptr;
    _spawnRateBtn = nullptr;
    _gravStrengthBtn = nullptr;
    _colorCountBtn = nullptr;
    _winMassBtn = nullptr;

    _lobbyRoomLabel = nullptr;
    for (int ii = 0; ii < _gameLobbyPlayerLabels.size(); ii++) {
        _gameLobbyPlayerLabels[ii] = nullptr;
    }

    _gamelobbyplayerName1 = nullptr;
    _gamelobbyplayerName2 = nullptr;
    _gamelobbyplayerName3 = nullptr;
    _gamelobbyplayerName4 = nullptr;
    _gamelobbyplayerName5 = nullptr;
    _spawnRateLabel = nullptr;
    _gravStrengthLabel = nullptr;
    _colorCountLabel = nullptr;
    _winMassLabel = nullptr;

    _layer = nullptr;
    _nextState = MenuState::GameLobby;
    _currSpawn = _currGrav = _currWin = _currColor = 2;
    _isHost = false;
}

/**
 * Initializes a new game lobby menu with the state pointer.
 *
 * @param assets     The (loaded) assets for this game lobby menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool LobbyMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
    const std::shared_ptr<GameSettings>& gameSettings,
    const std::shared_ptr<PlayerSettings>& playerSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("lobby");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _playerSettings = playerSettings;
    _gameSettings = gameSettings;

    _networkMessageManager = networkMessageManager;

    _lobbyRoomLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_roomidlabel"));
    _gamelobbyplayerName1 = assets->get<scene2::SceneNode>("lobby_playerlabel1");
    _gamelobbyplayerName2 = assets->get<scene2::SceneNode>("lobby_playerlabel2");
    _gamelobbyplayerName3 = assets->get<scene2::SceneNode>("lobby_playerlabel3");
    _gamelobbyplayerName4 = assets->get<scene2::SceneNode>("lobby_playerlabel4");
    _gamelobbyplayerName5 = assets->get<scene2::SceneNode>("lobby_playerlabel5");
    _gameLobbyPlayerLabels.resize(5);
    _gameLobbyPlayerLabels[0] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel1_label"));
    _gameLobbyPlayerLabels[1] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel2_label"));
    _gameLobbyPlayerLabels[2] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel3_label"));
    _gameLobbyPlayerLabels[3] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel4_label"));
    _gameLobbyPlayerLabels[4] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel5_label"));

    // Game lobby settings 
    _spawnRateLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_spawnratebutton_up_label"));
    _gravStrengthLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_gravstrengthbutton_up_label"));
    _colorCountLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_colorcountbutton_up_label"));
    _winMassLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_wincondbutton_up_label"));

    _spawnRateBtnLabel = assets->get<scene2::SceneNode>("lobby_spawnratebuttonlabel");
    _gravStrengthBtnLabel = assets->get<scene2::SceneNode>("lobby_gravstrengthbuttonlabel");
    _colorCountBtnLabel = assets->get<scene2::SceneNode>("lobby_colorcountbuttonlabel");
    _winMassBtnLabel = assets->get<scene2::SceneNode>("lobby_wincondbuttonlabel");

    _currSpawn = _currGrav = _currWin = _currColor = 2;

    // Setting buttons 
    _spawnRateBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_spawnratebutton"));
    _gravStrengthBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_gravstrengthbutton"));
    _colorCountBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_colorcountbutton"));
    _winMassBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_wincondbutton"));

    _spawnRateBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currSpawn = (_currSpawn + 1) % 7;
            _gameSettings->setSpawnRate(_spawnRates[_currSpawn]);
        }
        });
    _gravStrengthBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currGrav = (_currGrav + 1) % 7;
            _gameSettings->setGravStrength(_gravStrengths[_currGrav]);
        }
        });
    _colorCountBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currColor = (_currColor + 1) % 5;
            _gameSettings->setColorCount(_colorCounts[_currColor]);
        }
        });
    _winMassBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currWin = (_currWin + 1) % 5;
            _gameSettings->setPlanetMassToWin(_winMass[_currWin]);
        }
        });

    _gameStartBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_startgamebutton"));
    _gameStartBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _networkMessageManager->setGameState(GameState::GameStarted);
            _networkMessageManager->setGameSettings(_gameSettings);
            _networkMessageManager->sendMessages();
            _nextState = MenuState::LobbyToGame;
        }
        return true;
        });

    _isHost = false;
    _nextState = MenuState::GameLobby;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the game lobby menu is currently active and visible.
 *
 * @param onDisplay     Whether the game lobby menu is currently active and visible
 */
void LobbyMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        if (!onDisplay || _nextState == MenuState::MainToLobby) {
            _spawnRateBtnLabel->setVisible(onDisplay);
            _gravStrengthBtnLabel->setVisible(onDisplay);
            _colorCountBtnLabel->setVisible(onDisplay);
            _winMassBtnLabel->setVisible(onDisplay);

            _spawnRateLabel->setVisible(onDisplay);
            _gravStrengthLabel->setVisible(onDisplay);
            _colorCountLabel->setVisible(onDisplay);
            _winMassLabel->setVisible(onDisplay);

            _spawnRateBtn->setVisible(onDisplay);
            _gravStrengthBtn->setVisible(onDisplay);
            _colorCountBtn->setVisible(onDisplay);
            _winMassBtn->setVisible(onDisplay);
        }
        _gamelobbyplayerName1->setVisible(onDisplay);
        _gamelobbyplayerName2->setVisible(onDisplay);
        _gamelobbyplayerName3->setVisible(onDisplay);
        _gamelobbyplayerName4->setVisible(onDisplay);
        _gamelobbyplayerName5->setVisible(onDisplay);
        _gameStartBtn->setVisible(onDisplay);
        _lobbyRoomLabel->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (!onDisplay) {
            _gameStartBtn->deactivate();
            _spawnRateBtn->deactivate();
            _gravStrengthBtn->deactivate();
            _colorCountBtn->deactivate();
            _winMassBtn->deactivate();
        } else {
            _gameStartBtn->activate();
            if (_nextState == MenuState::MainToLobby) {
                _spawnRateBtn->activate();
                _gravStrengthBtn->activate();
                _colorCountBtn->activate();
                _winMassBtn->activate();
            }
        }
    }
}

/**
 * The method called to update this menu.
 *
 * The menu screen is only visible during the GameLobby state. The
 * screen is put on display on states transitioning into GameLobby.
 * Screen is taken down once menu state exits the Lobby.
 *
 * @param state             MenuScene's state value
 * @param playerSettings    The player's saved settings value
 * @param gameSettings      The settings for the current game
 */
void LobbyMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle GameLobby menu
    switch (state)
    {
        case MenuState::MainToLobby:
        {
            // handle displaying for Host
            _networkMessageManager->createGame();
            _networkMessageManager->setPlayerName(_playerSettings->getPlayerName());
            _networkMessageManager->setOtherNames({ "", "", "", "" });
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();
            _gameSettings->setGameId(_networkMessageManager->getRoomId());
            setOtherPlayerLabels({ "N/A", "N/A", "N/A", "N/A" });

            _nextState = state; // prep to setDisplay
            setDisplay(true);

            _lobbyRoomLabel->setText(_gameSettings->getGameId());
            _gameLobbyPlayerLabels[0]->setText(_playerSettings->getPlayerName());

            // handle game lobby settings (only visible to the host)
            _spawnRateLabel->setText(cugl::strtool::to_string(_gameSettings->getSpawnRate(), 1) + "X");
            _gravStrengthLabel->setText(cugl::strtool::to_string(_gameSettings->getGravStrength(), 1) + "X");
            _colorCountLabel->setText(cugl::strtool::to_string(_gameSettings->getColorCount()));
            _winMassLabel->setText(cugl::strtool::to_string(_gameSettings->getPlanetMassToWin()));

            _isHost = true;
            state = MenuState::GameLobby;
            _nextState = MenuState::GameLobby;
            break;
        }
        case MenuState::JoinToLobby:
        {          
            // handle setting up the lobby
            _nextState = state; // prep to setDisplay
            setDisplay(true);
            // undo player name label offsets for clients
            const float clientOffset = _layer->getContentHeight() / 6.0f;
            _gamelobbyplayerName1->setPositionY(_gamelobbyplayerName1->getPositionY() - clientOffset);
            _gamelobbyplayerName2->setPositionY(_gamelobbyplayerName2->getPositionY() - clientOffset);
            _gamelobbyplayerName3->setPositionY(_gamelobbyplayerName3->getPositionY() - clientOffset);
            _gamelobbyplayerName4->setPositionY(_gamelobbyplayerName4->getPositionY() - clientOffset);
            _gamelobbyplayerName5->setPositionY(_gamelobbyplayerName5->getPositionY() - clientOffset);

            _networkMessageManager->setPlayerName(_playerSettings->getPlayerName());
            _networkMessageManager->setOtherNames({ "", "", "", "" });
            _networkMessageManager->joinGame(_gameSettings->getGameId());
            _networkMessageManager->setRoomID(_gameSettings->getGameId());
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();
            _lobbyRoomLabel->setText(_gameSettings->getGameId());
            _gameLobbyPlayerLabels[0]->setText(_playerSettings->getPlayerName());
            setOtherPlayerLabels({ "N/A", "N/A", "N/A", "N/A" });
            
            _isHost = false;
            state = MenuState::GameLobby;
            _nextState = MenuState::GameLobby;
            break;
        }
        case MenuState::GameLobby:
        {
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();
            _lobbyRoomLabel->setText(_networkMessageManager->getRoomId());
            vector<string> othernames = _networkMessageManager->getOtherNames();
            // update other players' labels
            for (int ii = 0; ii < othernames.size(); ii++) {
                if (othernames[ii] != "") {
                    _gameLobbyPlayerLabels[ii+1]->setText(othernames[ii]);
                }
            }
            // handle room player updates and triggering game start
            if (_networkMessageManager->getGameState() == GameState::GameInProgress) {
                _nextState = MenuState::LobbyToGame;
            }

            // Update game setting button labels for the Host
            _spawnRateLabel->setText(cugl::strtool::to_string(_gameSettings->getSpawnRate(), 1) + "X");
            _gravStrengthLabel->setText(cugl::strtool::to_string(_gameSettings->getGravStrength(), 1) + "X");
            _colorCountLabel->setText(cugl::strtool::to_string(_gameSettings->getColorCount()));
            _winMassLabel->setText(cugl::strtool::to_string(_gameSettings->getPlanetMassToWin()));

            state = _nextState;
            break;
        }
        default:
        {            
            // hide menu screen
            if (_layer != nullptr && _layer->isVisible()) {
                // undo player name labels offsets for clients
                if (!_isHost) {
                    const float clientOffset = _layer->getContentHeight() / 6.0f;
                    _gamelobbyplayerName1->setPositionY(_gamelobbyplayerName1->getPositionY() + clientOffset);
                    _gamelobbyplayerName2->setPositionY(_gamelobbyplayerName2->getPositionY() + clientOffset);
                    _gamelobbyplayerName3->setPositionY(_gamelobbyplayerName3->getPositionY() + clientOffset);
                    _gamelobbyplayerName4->setPositionY(_gamelobbyplayerName4->getPositionY() + clientOffset);
                    _gamelobbyplayerName5->setPositionY(_gamelobbyplayerName5->getPositionY() + clientOffset);
                }
                setDisplay(false);
                _nextState = MenuState::GameLobby;
            }
            break;
        }
    }
}
