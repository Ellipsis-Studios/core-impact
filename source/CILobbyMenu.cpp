//
//  CILobbyMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
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
    }
    _gameStartBtn = nullptr;
    _lobbyRoomLabel = nullptr;
    _gamelobbyplayerlabel1 = nullptr;
    _gamelobbyplayerlabel2 = nullptr;
    _gamelobbyplayerlabel3 = nullptr;
    _gamelobbyplayerlabel4 = nullptr;
    _gamelobbyplayerlabel5 = nullptr;
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
    _gamelobbyplayerlabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel1"));
    _gamelobbyplayerlabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel2"));
    _gamelobbyplayerlabel3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel3"));
    _gamelobbyplayerlabel4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel4"));
    _gamelobbyplayerlabel5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_playerlabel5"));
    
    _gameStartBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_startgamebutton"));
    _gameStartBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::LobbyToGame;
        }
        });

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
        _gameStartBtn->setVisible(onDisplay);
        _lobbyRoomLabel->setVisible(onDisplay);
        _gamelobbyplayerlabel1->setVisible(onDisplay);
        _gamelobbyplayerlabel2->setVisible(onDisplay);
        _gamelobbyplayerlabel3->setVisible(onDisplay);
        _gamelobbyplayerlabel4->setVisible(onDisplay);
        _gamelobbyplayerlabel5->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (onDisplay) {
            _gameStartBtn->activate();
        }
        else {
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
 * @param state       MenuScene's state value
 * @param joingame    Game lobby's room id value
 * @param playername  Player's name 
 * @param othernames  List of names of other players in current game lobby
 */
void LobbyMenu::update(MenuState& state, string& joingame, string& playername, 
    std::vector<string>& othernames) {
    if (_layer == nullptr) {
        return;
    }
    // handle GameLobby menu
    switch (state) 
    {
        case MenuState::MainToLobby:
            joingame = ""; // HOST
        case MenuState::JoinToLobby:
            // handle transitioning into GameLobby
            setDisplay(true);
            _lobbyRoomLabel->setText(joingame);
            _gamelobbyplayerlabel1->setText(playername);
            state = MenuState::GameLobby;
            _nextState = MenuState::GameLobby;
            break;
        case MenuState::GameLobby:
            // handle room player updates and triggering game start
            if (othernames.size() == 4) {
                // update other players' labels
                _gamelobbyplayerlabel2->setText(othernames.at(0));
                _gamelobbyplayerlabel3->setText(othernames.at(1));
                _gamelobbyplayerlabel4->setText(othernames.at(2));
                _gamelobbyplayerlabel5->setText(othernames.at(3));
            }
            state = _nextState;
            break;
        default:
            // hide menu screen
            if (_layer != nullptr && _layer->isVisible()) {
                setDisplay(false);
            }
            break;
    }
}
