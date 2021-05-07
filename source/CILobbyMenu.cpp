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
    if (_gameSettingsBtn != nullptr && _gameSettingsBtn->isActive()) {
        _gameStartBtn->deactivate();
        _gameSettingsBtn->deactivate();
        _gameReadyBtn->deactivate();
    }
    else if (_gameSettingsBtn != nullptr) {
        _gameStartBtn->clearListeners();
        _gameSettingsBtn->clearListeners();
        _gameReadyBtn->clearListeners();
    }

    _gameStartBtn = nullptr;
    _gameReadyBtn = nullptr;
    _gameSettingsBtn = nullptr;

    _lobbyRoomLabel = nullptr;
    for (int ii = 0; ii < _gameLobbyPlayerLabels.size(); ii++) {
        _gameLobbyPlayerLabels[ii] = nullptr;
    }
    for (int ii = 0; ii < _gameLobbyPlayerNames.size(); ii++) {
        _gameLobbyPlayerNames[ii] = nullptr;
    }

    _title = nullptr;
    _layer = nullptr;
    _nextState = MenuState::GameLobby;
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

    _title = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_title"));
    _lobbyRoomLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_roomidlabel"));

    _gameLobbyPlayerNames.resize(5);
    _gameLobbyPlayerNames[0] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lobby_playerlabel1_pnamelabelbackground"));
    _gameLobbyPlayerNames[1] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lobby_playerlabel2_pnamelabelbackground"));
    _gameLobbyPlayerNames[2] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lobby_playerlabel3_pnamelabelbackground"));
    _gameLobbyPlayerNames[3] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lobby_playerlabel4_pnamelabelbackground"));
    _gameLobbyPlayerNames[4] = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("lobby_playerlabel5_pnamelabelbackground"));

    _gameLobbyPlayerLabels.resize(5);
    _gameLobbyPlayerLabels[0] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel1_label"));
    _gameLobbyPlayerLabels[1] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel2_label"));
    _gameLobbyPlayerLabels[2] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel3_label"));
    _gameLobbyPlayerLabels[3] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel4_label"));
    _gameLobbyPlayerLabels[4] = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel5_label"));

    _gameStartBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_startgamebutton"));
    _gameStartBtn->setToggle(true);
    _gameStartBtn->setDown(true);

    _gameReadyBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_readygamebutton"));
    _gameReadyBtn->addListener([&](const std::string& name, bool down) {
        if (down) {
            _networkMessageManager->setGameState(GameState::GameStarted);
            _networkMessageManager->sendMessages();
        }
        });
    _gameReadyBtn->setToggle(true);

    _gameSettingsBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_gamesettingsbutton"));
    _gameSettingsBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::LobbyToGameSetting;
        }
        });

    _isReadyToStart = false;
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
        _gameSettingsBtn->setVisible(onDisplay);
        _lobbyRoomLabel->setVisible(onDisplay);
        _title->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        _gameLobbyPlayerNames[0]->setVisible(onDisplay);
        _gameLobbyPlayerNames[1]->setVisible(false);
        _gameLobbyPlayerNames[2]->setVisible(false);
        _gameLobbyPlayerNames[3]->setVisible(false);
        _gameLobbyPlayerNames[4]->setVisible(false);

        if (!onDisplay) {
            _gameStartBtn->setVisible(false);
            _gameReadyBtn->setVisible(false);
            _gameReadyBtn->deactivate();
            _gameStartBtn->deactivate();
            _gameSettingsBtn->deactivate();
        }
        else {
            _gameSettingsBtn->activate();
            _gameStartBtn->deactivate();
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
            _gameSettings->setGameId(_networkMessageManager->getRoomId());
            setPlayerLabels({ "N/A", "N/A", "N/A", "N/A", "N/A" });

            _lobbyRoomLabel->setText("Code: " + _gameSettings->getGameId());

            setDisplay(true);

            state = _nextState = MenuState::GameLobby;
            break;
        }
        case MenuState::JoinToLobby:
        {
            // handle setting up the lobby
            setPlayerLabels({ "N/A", "N/A", "N/A", "N/A", "N/A" });
            _lobbyRoomLabel->setText("Code: " + _gameSettings->getGameId());

            setDisplay(true);

            state = _nextState = MenuState::GameLobby;
            break;
        }
        case MenuState::GameSettingToLobby:
        {
            _gameReadyBtn->setDown(false);
            setDisplay(true);
            state = _nextState = MenuState::GameLobby;
            break;
        }
        case MenuState::GameLobby:
        {
            if (_gameSettingsBtn != nullptr && !_gameSettingsBtn->isActive()) {
                _gameSettingsBtn->activate();
                _gameStartBtn->deactivate();
            }

            if (_networkMessageManager->getPlayerId() == 0) {
                _gameStartBtn->setVisible(true);
                _gameReadyBtn->setVisible(false);
                _gameReadyBtn->deactivate();
            }
            else if (_networkMessageManager->getPlayerId() > 0) {
                _gameStartBtn->setVisible(false);
                _gameReadyBtn->setVisible(true);
                _gameReadyBtn->activate();
            }

            if (_isReadyToStart && _gameStartBtn->isActive() && _gameStartBtn->isDown()) {
                _networkMessageManager->setGameState(GameState::GameStarted);
                _networkMessageManager->sendMessages();
                _nextState = MenuState::LobbyToGame;
            }

            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();
            _lobbyRoomLabel->setText("Code: " + _networkMessageManager->getRoomId());

            bool isReady = true;
            int pindex = 0;
            for (const auto& p : _networkMessageManager->getPlayerMap()) {
                if (get<1>(p.second) == false || p.first < 0) { // ready 
                    isReady = false;
                }
                if (p.first == 0) {
                    _gameLobbyPlayerLabels[0]->setText(get<0>(p.second));
                    _gameLobbyPlayerNames[0]->setVisible(get<1>(p.second));
                    pindex++;
                }
                else if (pindex < 5) {
                    _gameLobbyPlayerLabels[pindex]->setText(get<0>(p.second));
                    _gameLobbyPlayerNames[pindex]->setVisible(get<1>(p.second));
                    pindex++;
                }
            }
            
            // handle disconnected players
            for (; pindex < 5; pindex++) {
                _gameLobbyPlayerLabels[pindex]->setText("N/A");
                _gameLobbyPlayerNames[pindex]->setVisible(false);
            }

            if (isReady && _networkMessageManager->getPlayerId() == 0) {
                _isReadyToStart = true;
                _gameStartBtn->setDown(false);
                _gameStartBtn->activate();
            }
            else {
                _isReadyToStart = false;
                _gameStartBtn->setDown(true);
                _gameStartBtn->deactivate();
            }

            if (_gameReadyBtn->isActive() && _gameReadyBtn->isDown()) { // handle non-host press
                _gameReadyBtn->deactivate();
            }

            // handle room player updates and triggering game start
            if (_networkMessageManager->getGameState() == GameState::GameInProgress) {
                _nextState = MenuState::LobbyToGame;
            }
            // handle room player reconnecting
            if (_networkMessageManager->getGameState() == GameState::ReconnectingToGame) {
                _nextState = MenuState::MenuToReconnect;
            }
            // handle room player disconnect
            if (_networkMessageManager->getGameState() == GameState::DisconnectedFromGame) {
                _nextState = MenuState::LobbyToGame;
            }

            state = _nextState;
            break;
        }
        case MenuState::ReconnectingGame:
        case MenuState::MenuToReconnect:
        {
            _gameReadyBtn->deactivate();
            _gameStartBtn->deactivate();
            _gameSettingsBtn->deactivate();
            break;
        }
        default:
        {
            // hide menu screen
            if (_layer != nullptr && _layer->isVisible()) {
                _isReadyToStart = false;
                _gameReadyBtn->setDown(false);
                setDisplay(false);
                _nextState = MenuState::GameLobby;
            }
            break;
        }
    }
}
