//
//  CILobbyMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CILobbyMenu.h"

using namespace cugl;

#define SCENE_SIZE  1024

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
        _winCondBtn->deactivate();
    }
    _gameStartBtn = nullptr;
    _spawnRateBtn = nullptr;
    _gravStrengthBtn = nullptr;
    _colorCountBtn = nullptr;
    _winCondBtn = nullptr;

    _lobbyRoomLabel = nullptr;
    _gamelobbyplayerlabel1 = nullptr;
    _gamelobbyplayerlabel2 = nullptr;
    _gamelobbyplayerlabel3 = nullptr;
    _gamelobbyplayerlabel4 = nullptr;
    _gamelobbyplayerlabel5 = nullptr;
    _gamelobbyplayerName1 = nullptr;
    _gamelobbyplayerName2 = nullptr;
    _gamelobbyplayerName3 = nullptr;
    _gamelobbyplayerName4 = nullptr;
    _gamelobbyplayerName5 = nullptr;
    _spawnRateLabel = nullptr;
    _gravStrengthLabel = nullptr;
    _colorCountLabel = nullptr;
    _winCondLabel = nullptr;

    _layer = nullptr;
    _nextState = MenuState::GameLobby;
    _lobbySpawnRate = 1.0f;
    _lobbyGravityStrength = 1.0f;
    _lobbyColorCount = 6;
    _lobbyWinCond = 200;
    _isClient = false;
}

/**
 * Initializes a new game lobby menu with the state pointer.
 *
 * @param assets     The (loaded) assets for this game lobby menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool LobbyMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("lobby");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _lobbyRoomLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_roomidlabel"));
    _gamelobbyplayerName1 = assets->get<scene2::SceneNode>("lobby_playerlabel1");
    _gamelobbyplayerName2 = assets->get<scene2::SceneNode>("lobby_playerlabel2");
    _gamelobbyplayerName3 = assets->get<scene2::SceneNode>("lobby_playerlabel3");
    _gamelobbyplayerName4 = assets->get<scene2::SceneNode>("lobby_playerlabel4");
    _gamelobbyplayerName5 = assets->get<scene2::SceneNode>("lobby_playerlabel5");
    _gamelobbyplayerlabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel1_label"));
    _gamelobbyplayerlabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel2_label"));
    _gamelobbyplayerlabel3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel3_label"));
    _gamelobbyplayerlabel4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel4_label"));
    _gamelobbyplayerlabel5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel5_label"));

    // Game lobby settings 
    _spawnRateLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_spawnratebutton_up_label"));
    _gravStrengthLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_gravstrengthbutton_up_label"));
    _colorCountLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_colorcountbutton_up_label"));
    _winCondLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_wincondbutton_up_label"));

    _spawnRateBtnLabel = assets->get<scene2::SceneNode>("lobby_spawnratebuttonlabel");
    _gravStrengthBtnLabel = assets->get<scene2::SceneNode>("lobby_gravstrengthbuttonlabel");
    _colorCountBtnLabel = assets->get<scene2::SceneNode>("lobby_colorcountbuttonlabel");
    _winCondBtnLabel = assets->get<scene2::SceneNode>("lobby_wincondbuttonlabel");

    _currSpawn = _currGrav = _currWin = 2;
    _currColor = 4;

    _spawnRateBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_spawnratebutton"));
    _gravStrengthBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_gravstrengthbutton"));
    _colorCountBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_colorcountbutton"));
    _winCondBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_wincondbutton"));

    _spawnRateBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currSpawn = (_currSpawn + 1) % 7;
            _lobbySpawnRate = _spawnRates[_currSpawn];
            _spawnRateLabel->setText(cugl::strtool::to_string(_lobbySpawnRate, 1) + "X");
        }
        });
    _gravStrengthBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currGrav = (_currGrav + 1) % 7;
            _lobbyGravityStrength = _gravStrengths[_currGrav];
            _gravStrengthLabel->setText(cugl::strtool::to_string(_lobbyGravityStrength, 1) + "X");
        }
        });
    _colorCountBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currColor = (_currColor + 1) % 5;
            _lobbyColorCount = _colorCounts[_currColor];
            _colorCountLabel->setText(cugl::strtool::to_string(_lobbyColorCount));
        }
        });
    _winCondBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _currWin = (_currWin + 1) % 5;
            _lobbyWinCond = _winConds[_currWin];
            _winCondLabel->setText(cugl::strtool::to_string(_lobbyWinCond));
        }
        });

    _gameStartBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_startgamebutton"));
    _gameStartBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::LobbyToGame;
        }
        });

    _isClient = false;
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
        if (!(onDisplay && _isClient) && _spawnRateBtn->isVisible() != onDisplay) {
            _spawnRateLabel->setVisible(onDisplay);
            _gravStrengthLabel->setVisible(onDisplay);
            _colorCountLabel->setVisible(onDisplay);
            _winCondLabel->setVisible(onDisplay);
            _spawnRateBtn->setVisible(onDisplay);
            _gravStrengthBtn->setVisible(onDisplay);
            _colorCountBtn->setVisible(onDisplay);
            _winCondBtn->setVisible(onDisplay);

            _spawnRateBtnLabel->setVisible(onDisplay);
            _gravStrengthBtnLabel->setVisible(onDisplay);
            _colorCountBtnLabel->setVisible(onDisplay);
            _winCondBtnLabel->setVisible(onDisplay);

            //_gameStartBtn->setVisible(onDisplay);
            // TODO: Remove start button from client once networking is integrated
        }
        _gameStartBtn->setVisible(onDisplay); 

        _lobbyRoomLabel->setVisible(onDisplay);
        _gamelobbyplayerName1->setVisible(onDisplay);
        _gamelobbyplayerName2->setVisible(onDisplay);
        _gamelobbyplayerName3->setVisible(onDisplay);
        _gamelobbyplayerName4->setVisible(onDisplay);
        _gamelobbyplayerName5->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (onDisplay) {
            _gameStartBtn->activate();
            if (!(onDisplay && _isClient) && !_spawnRateBtn->isActive()) {
                _spawnRateBtn->activate();
                _gravStrengthBtn->activate();
                _colorCountBtn->activate();
                _winCondBtn->activate();
            }
        } else {
            _gameStartBtn->deactivate();
            _spawnRateBtn->deactivate();
            _gravStrengthBtn->deactivate();
            _colorCountBtn->deactivate();
            _winCondBtn->deactivate();
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
 * @param state         MenuScene's state value
 * @param joingame      Game lobby's room id value
 * @param playername    Player's name
 * @param othernames    List of names of other players in current game lobby
 * @param spawnRate     Stardust spawn rate factor
 * @param gravStrength  Planet gravity strength factor
 * @param colorCount    Available stardust color count
 * @param winCond       Win condition for planet (game length)
 */
void LobbyMenu::update(MenuState& state, string& joingame, string& playername, std::vector<string>& othernames,
    float& spawnRate, float& gravStrength, uint8_t& colorCount, uint16_t& winCond) {
    if (_layer == nullptr) {
        return;
    }
    // handle GameLobby menu
    switch (state)
    {
        case MenuState::MainToLobby:
        {
            // handle displaying for Host
            joingame = "";
            _isClient = false;
            setDisplay(true);
            _lobbyRoomLabel->setText(joingame);
            _gamelobbyplayerlabel1->setText(playername);
            state = _nextState = MenuState::GameLobby;

            // handle game lobby settings
            _currSpawn = _currGrav = _currWin = 2;
            _currColor = 4;
            // only visible for the host
            _lobbySpawnRate = spawnRate = _spawnRates[_currSpawn];
            _lobbyGravityStrength = gravStrength = _gravStrengths[_currGrav];
            _lobbyColorCount = colorCount = _colorCounts[_currColor];
            _lobbyWinCond = winCond = _winConds[_currWin];
            _spawnRateLabel->setText(cugl::strtool::to_string(_lobbySpawnRate, 1) + "X");
            _gravStrengthLabel->setText(cugl::strtool::to_string(_lobbyGravityStrength, 1) + "X");
            _colorCountLabel->setText(cugl::strtool::to_string(_lobbyColorCount));
            _winCondLabel->setText(cugl::strtool::to_string(_lobbyWinCond));
            
            break;
        }
        case MenuState::JoinToLobby:
        {
            // handle displaying for Clients
            _isClient = true;
            setDisplay(true);
            _lobbyRoomLabel->setText(joingame);
            _gamelobbyplayerlabel1->setText(playername);
            state = _nextState = MenuState::GameLobby;

            // offset player name labels to center for clients
            const float clientOffset = _layer->getContentHeight() / 6.0f;
            _gamelobbyplayerName1->setPositionY(_gamelobbyplayerName1->getPositionY() - clientOffset);
            _gamelobbyplayerName2->setPositionY(_gamelobbyplayerName2->getPositionY() - clientOffset);
            _gamelobbyplayerName3->setPositionY(_gamelobbyplayerName3->getPositionY() - clientOffset);
            _gamelobbyplayerName4->setPositionY(_gamelobbyplayerName4->getPositionY() - clientOffset);
            _gamelobbyplayerName5->setPositionY(_gamelobbyplayerName5->getPositionY() - clientOffset);

            break;
        }
        case MenuState::GameLobby:
        {
            // handle room player updates and triggering game start
            if (_lobbyRoomLabel->getText().empty()) {
                // TODO: update lobby room label using room id from network connection
            }
            if (othernames.size() == 4) {
                // update other players' labels
                _gamelobbyplayerlabel2->setText(othernames.at(0));
                _gamelobbyplayerlabel3->setText(othernames.at(1));
                _gamelobbyplayerlabel4->setText(othernames.at(2));
                _gamelobbyplayerlabel5->setText(othernames.at(3));
            }

            // handle game lobby settings
            if (!_isClient) {
                spawnRate = _lobbySpawnRate;
                gravStrength = _lobbyGravityStrength;
                colorCount = _lobbyColorCount;
                winCond = _lobbyWinCond;
            }
            state = _nextState;
            break;
        }
        default:
        {
            // undo player name labels offsets
            if (_isClient) {
                const float clientOffset = _layer->getContentHeight() / 6.0f;
                _gamelobbyplayerName1->setPositionY(_gamelobbyplayerName1->getPositionY() + clientOffset);
                _gamelobbyplayerName2->setPositionY(_gamelobbyplayerName2->getPositionY() + clientOffset);
                _gamelobbyplayerName3->setPositionY(_gamelobbyplayerName3->getPositionY() + clientOffset);
                _gamelobbyplayerName4->setPositionY(_gamelobbyplayerName4->getPositionY() + clientOffset);
                _gamelobbyplayerName5->setPositionY(_gamelobbyplayerName5->getPositionY() + clientOffset);
                
                _isClient = false;
            }

            // hide menu screen
            if (_layer != nullptr && _layer->isVisible()) {
                setDisplay(false);
            }
            break;
        }
    }
}
