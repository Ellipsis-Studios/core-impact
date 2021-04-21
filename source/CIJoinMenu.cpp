//
//  CIJoinMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIJoinMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void JoinMenu::dispose() {
    if (_roomIdInput != nullptr && _roomIdInput->isActive()) {
        _roomIdInput->deactivate();
        _roomJoinBtn->deactivate();
    }    
    _roomJoinBtn = nullptr;
    _roomIdInput = nullptr;
    _layer = nullptr;

    _nextState = MenuState::JoinRoom;
}

/**
 * Initializes a new join game menu with the state pointer.
 *
 * @param assets    The (loaded) assets for this join game menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool JoinMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<GameSettings>& gameSettings) {    
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }
    
    _layer = assets->get<scene2::SceneNode>("join");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _gameSettings = gameSettings;
    
    // Room id Input 
    _roomIdInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("join_roomidinput"));
    _roomIdInput->addTypeListener([&](const std::string& name, const std::string& value) {
        // Handle room id size limit 
        if (value.length() > 5) {
            _roomIdInput->setText(value.substr(value.length() - 5, 5));
        }
        });
    _roomIdInput->addExitListener([&](const std::string& name, const std::string& value) {
        // Set game room id value
        if (_gameSettings != nullptr) {
            _gameSettings->setGameId(value);
        }
        CULog("Room id set to %s", value.c_str());
        });

    // Join game button 
    _roomJoinBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_joingamebutton"));
    _roomJoinBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::JoinToLobby;
        }
        });

    // TODO: Implement RoomIdInput with 5 digits with focus switching 

    _nextState = MenuState::JoinRoom;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the join game menu is currently active and visible.
 *
 * @param onDisplay     Whether the join game menu is currently active and visible
 */
void JoinMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _roomJoinBtn->setVisible(onDisplay);
        _roomIdInput->setVisible(onDisplay);
        _layer->setVisible(onDisplay);
        
        if (onDisplay) {
            _roomJoinBtn->activate();
            _roomIdInput->activate();
        } else {
            _roomJoinBtn->deactivate();
            _roomIdInput->deactivate();
        }
    }
}

/**
 * The method called to update this menu.
 *
 * The menu screen is only visible during the JoinRoom state. The
 * screen is put on display on states transitioning into JoinRoom.
 * Screen is taken down once menu state exits JoinRoom.
 */
void JoinMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle JoinRoom menu
    switch (state) 
    {
        case MenuState::MainToJoin:
            // handle transitioning into JoinRoom
            setDisplay(true);
            _gameSettings->setGameId("00000");
            _roomIdInput->setText(_gameSettings->getGameId());
            state = MenuState::JoinRoom;
            _nextState = MenuState::JoinRoom;
            break;
        case MenuState::JoinRoom:
            // handle roomid input and transition out of JoinRoom
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
