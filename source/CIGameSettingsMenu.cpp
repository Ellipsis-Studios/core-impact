//
//  CIGameSettingsMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 5/4/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIGameSettingsMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

void GameSettingsMenu::dispose() {
    if (_applySettingsBtn != nullptr && _applySettingsBtn->isVisible()) {
        _applySettingsBtn->deactivate();
        _spawnRateBtn->deactivate();
        _gravStrengthBtn->deactivate();
        _layerSizeBtn->deactivate();
    }
    else if (_applySettingsBtn != nullptr) {
        _applySettingsBtn->clearListeners();
        _spawnRateBtn->clearListeners();
        _gravStrengthBtn->clearListeners();
        _layerSizeBtn->clearListeners();
    }

    _applySettingsBtn = nullptr;
    _spawnRateBtn = nullptr;
    _gravStrengthBtn = nullptr;
    _layerSizeBtn = nullptr;

    _spawnRateLabel = nullptr;
    _gravStrengthLabel = nullptr;
    _layerSizeLabel = nullptr;

    _subTitle = nullptr;
    _title = nullptr;
    _layer = nullptr;
    _nextState = MenuState::GameSetting;
    _currSpawn = _currGrav = 2;
    _currWin = 1;
}

bool GameSettingsMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
    const std::shared_ptr<GameSettings>& gameSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("gamesettings");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _gameSettings = gameSettings;

    _networkMessageManager = networkMessageManager;

    _title = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_title"));
    _subTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_subtitle"));

    // Game lobby settings
    _spawnRateLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_spawnratebutton_up_label"));
    _gravStrengthLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_gravstrengthbutton_up_label"));
    _layerSizeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_wincondbutton_up_label"));

    _spawnRateBtnLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_spawnratebuttonlabel"));
    _gravStrengthBtnLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_gravstrengthbuttonlabel"));
    _layerSizeBtnLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("gamesettings_wincondbuttonlabel"));

    _currSpawn = _currGrav = 2;
    _currWin = 1;

    // Setting buttons
    _spawnRateBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("gamesettings_spawnratebutton"));
    _gravStrengthBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("gamesettings_gravstrengthbutton"));
    _layerSizeBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("gamesettings_wincondbutton"));

    _spawnRateBtn->addListener([&](const std::string& name, bool down) {
        if (down) {
            _currSpawn = (_currSpawn + 1) % 7;
        }
        else {
            _gameSettings->setSpawnRate(_spawnRates[_currSpawn]);
        }
        });
    _gravStrengthBtn->addListener([&](const std::string& name, bool down) {
        if (down) {
            _currGrav = (_currGrav + 1) % 7;
        }
        else {
            _gameSettings->setGravStrength(_gravStrengths[_currGrav]);
        }
        });
    _layerSizeBtn->addListener([&](const std::string& name, bool down) {
        if (down) {
            _currWin = (_currWin + 1) % 5;
        }
        else {
            _gameSettings->setPlanetStardustPerLayer(_layerSize[_currWin]);
        }
        });

    _applySettingsBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("gamesettings_applybutton"));
    _applySettingsBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _networkMessageManager->setGameState(GameState::SettingSent);
            _networkMessageManager->sendMessages();
            _nextState = MenuState::GameSetting;
        }
        });

    _nextState = MenuState::GameSetting;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the game lobby menu is currently active and visible.
 *
 * @param onDisplay     Whether the game lobby menu is currently active and visible
 */
void GameSettingsMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _spawnRateBtnLabel->setVisible(onDisplay);
        _gravStrengthBtnLabel->setVisible(onDisplay);
        _layerSizeBtnLabel->setVisible(onDisplay);

        _spawnRateLabel->setVisible(onDisplay);
        _gravStrengthLabel->setVisible(onDisplay);
        _layerSizeLabel->setVisible(onDisplay);

        _spawnRateBtn->setVisible(onDisplay);
        _gravStrengthBtn->setVisible(onDisplay);
        _layerSizeBtn->setVisible(onDisplay);

        _title->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (!onDisplay) {
            _applySettingsBtn->deactivate();
            _spawnRateBtn->deactivate();
            _gravStrengthBtn->deactivate();
            _layerSizeBtn->deactivate();
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
void GameSettingsMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle GameLobby menu
    switch (state)
    {
        case MenuState::LobbyToGameSetting:
        {
            // handle displaying for Host
            _networkMessageManager->sendMessages();
            _networkMessageManager->receiveMessages();

            setDisplay(true);

            // handle game lobby settings
            _spawnRateLabel->setText(cugl::strtool::to_string(_gameSettings->getSpawnRate(), 1) + "X");
            _gravStrengthLabel->setText(cugl::strtool::to_string(_gameSettings->getGravStrength(), 1) + "X");
            _layerSizeLabel->setText(cugl::strtool::to_string(_gameSettings->getPlanetStardustPerLayer()));

            state = _nextState = MenuState::GameSetting;
            break;
        }
        case MenuState::GameSetting:
        {
            if (_networkMessageManager->getPlayerId() == 0) {
                _applySettingsBtn->setVisible(true);
                _subTitle->setVisible(false);

                _applySettingsBtn->activate();
                _spawnRateBtn->activate();
                _gravStrengthBtn->activate();
                _layerSizeBtn->activate();
            }
            else {
                _applySettingsBtn->setVisible(false);
                _subTitle->setVisible(true);

                _applySettingsBtn->deactivate();
                _spawnRateBtn->deactivate();
                _gravStrengthBtn->deactivate();
                _layerSizeBtn->deactivate();
            }

            if (!_networkMessageManager->getPlayerId() == 0) {
                _networkMessageManager->receiveMessages();
            }

            // Update game setting button labels for the Host
            _spawnRateLabel->setText(cugl::strtool::to_string(_gameSettings->getSpawnRate(), 1) + "X");
            _gravStrengthLabel->setText(cugl::strtool::to_string(_gameSettings->getGravStrength(), 1) + "X");
            _layerSizeLabel->setText(cugl::strtool::to_string(_gameSettings->getPlanetStardustPerLayer()));

            // handle room player updates and triggering game start
            if (_networkMessageManager->getGameState() == GameState::GameInProgress) {
                _nextState = MenuState::LobbyToGame;
            }

            state = _nextState;
            break;
        }
        default:
        {
            // hide menu screen
            if (_layer != nullptr && _layer->isVisible()) {
                setDisplay(false);
                _nextState = MenuState::GameSetting;
            }
            break;
        }
    }
}
