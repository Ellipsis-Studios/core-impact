//
//  CIPopupMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 5/5/21.
//  Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPopupMenu.h"

using namespace cugl;

void PopupMenu::dispose() {
    _window = nullptr;
    _error = nullptr;
    _layer = nullptr;
    _timer = 0.0f;
}

bool PopupMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
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

    _layer = assets->get<scene2::SceneNode>("popup");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _networkMessageManager = networkMessageManager;
    _playerSettings = playerSettings;
    _gameSettings = gameSettings;

    // TODO: fix background sizing
    _window = assets->get<scene2::SceneNode>("popup_subscreen");
    _windowBackground = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("popup_subscreen_patch"));
    _windowLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("popup_subscreen_label"));
    _windowCircle = assets->get<scene2::SceneNode>("popup_subscreen_circle");

    _error = assets->get<scene2::SceneNode>("popup_errorscreen");
    _errorBackground = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>("popup_errorscreen_patch"));
    _errorLabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("popup_errorscreen_label1"));
    _errorLabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("popup_errorscreen_label2"));

    _timer = 12.0f;
    return true;
}

void PopupMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _error->setVisible(false);
        _window->setVisible(onDisplay);
        _layer->setVisible(onDisplay);
    }
}

void PopupMenu::update(MenuState& state, float timestep) {
    if (_layer == nullptr) {
        return;
    }
    // handle MainMenu
    switch (state)
    {
        case MenuState::MenuToCreate:
        {
            _networkMessageManager->createGame();
            _networkMessageManager->setPlayerName(_playerSettings->getPlayerName());
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();

            _timer = 12.0f;
            setDisplay(true);
            _window->setVisible(true);
            _windowLabel->setText("Creating game...");

            _error->setVisible(false);
            _errorLabel1->setText("Unable to create game.");
            _errorLabel2->setText("Please check your internet.");

            state = MenuState::CreatingGame;
            break;
        }
        case MenuState::MenuToJoin:
        {
            _networkMessageManager->setPlayerName(_playerSettings->getPlayerName());
            _networkMessageManager->joinGame(_gameSettings->getGameId());
            _networkMessageManager->setRoomID(_gameSettings->getGameId());
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();

            _timer = 12.0f;
            setDisplay(true);
            _window->setVisible(true);
            _windowLabel->setText("Joining game...");

            _error->setVisible(false);
            _errorLabel1->setText("Unable to join game.");
            _errorLabel2->setText("Invalid game code.");

            state = MenuState::JoiningGame;
            break;
        }
        case MenuState::MenuToReconnect:
        {
            _networkMessageManager->setPlayerName(_playerSettings->getPlayerName());
            _networkMessageManager->joinGame(_gameSettings->getGameId());
            _networkMessageManager->setRoomID(_gameSettings->getGameId());
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();

            _timer = 12.0f;
            setDisplay(true);
            _window->setVisible(true);
            _windowLabel->setText("Reconnecting game...");

            _error->setVisible(false);
            _errorLabel1->setText("Unable to reconnect game.");
            _errorLabel2->setText("Connection to game lost.");

            state = MenuState::ReconnectingGame;
            break;
        }
        case MenuState::CreatingGame:
        {
            _timer -= timestep;
            _windowCircle->setAngle(_windowCircle->getAngle() + ((timestep / 0.5f) * 6.28319));
            if (_timer > 11.0f) {
                _networkMessageManager->sendMessages();
                _networkMessageManager->receiveMessages();
                break;
            } 
            else if (_timer > 2.5f) {
                _networkMessageManager->sendMessages();
                _networkMessageManager->receiveMessages();

                if (_networkMessageManager->getNetworkStatus() == CUNetworkConnection::NetStatus::Connected) {
                    CULog("Successfully created game.");
                    state = MenuState::MainToLobby;
                }
            }
            else if (_timer > 0.0f) {
                // display error 
                _window->setVisible(false);
                _error->setVisible(true);
            }
            else {
                // return to main menu
                state = MenuState::LoadToMain;
            }
            break;
        }
        case MenuState::JoiningGame:
        {
            _timer -= timestep;
            _windowCircle->setAngle(_windowCircle->getAngle() + ((timestep / 0.5f) * 6.28319));
            if (_timer > 11.0f) {
                _networkMessageManager->sendMessages();
                _networkMessageManager->receiveMessages();
                break;
            }
            else if (_timer > 2.5f) {
                _networkMessageManager->sendMessages();
                _networkMessageManager->receiveMessages();

                if (_networkMessageManager->getNetworkStatus() == CUNetworkConnection::NetStatus::RoomNotFound) {
                    CULog("Invalid room id.");
                    _timer = 2.4f; // skip to displaying error message
                    break;
                }
                else if (_networkMessageManager->getNetworkStatus() == CUNetworkConnection::NetStatus::Connected) {
                    CULog("Successfully joined game.");
                    state = MenuState::JoinToLobby;
                }
            }
            else if (_timer > 0.0f) {
                // display error message
                _window->setVisible(false);
                _error->setVisible(true);
            }
            else {
                // return to join menu
                state = MenuState::MainToJoin;
            }
            break;
        }
        case MenuState::ReconnectingGame:
        {
            _timer -= timestep;
            _windowCircle->setAngle(_windowCircle->getAngle() + ((timestep / 0.5f) * 6.28319));
            if (_timer > 11.0f) {
                _networkMessageManager->sendMessages();
                _networkMessageManager->receiveMessages();
                break;
            }
            else if (_timer > 2.5f) {
                _networkMessageManager->sendMessages();
                _networkMessageManager->receiveMessages();

                if (_networkMessageManager->getNetworkStatus() == CUNetworkConnection::NetStatus::Reconnecting) {
                    break;
                }
                else if (_networkMessageManager->getNetworkStatus() == CUNetworkConnection::NetStatus::Disconnected) {
                    _timer = 2.4f;
                    break;
                }
                else if (_networkMessageManager->getNetworkStatus() == CUNetworkConnection::NetStatus::Connected) {
                    CULog("Successfully reconnected game.");
                    if (_networkMessageManager->isPlayerHost()) {
                        state = MenuState::GameLobby;
                    }
                    else {
                        state = MenuState::JoinToLobby;
                    }
                }
            }
            else if (_timer > 0.0f) {
                // display error message
                _window->setVisible(false);
                _error->setVisible(true);
            }
            else {
                // return to join menu
                state = MenuState::LobbyToMain;
            }
            break;
        }
        default:
            if (_layer != nullptr && _layer->isVisible()) {
                setDisplay(false);
            }
            break;
    }
}
