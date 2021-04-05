//
//  CIJoinMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIJoinMenu.h"

using namespace cugl;

#define SCENE_SIZE  1024

void JoinMenu::dispose() {
    if (_roomIdInput != nullptr && _roomIdInput->isActive()) {
        _roomIdInput->deactivate();
        _roomJoinBtn->deactivate();
    }    
    _roomJoinBtn = nullptr;
    _roomIdInput = nullptr;
    _layer = nullptr;
    _assets = nullptr;
}

bool JoinMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    }
    
    _assets = assets;
    _layer = assets->get<scene2::SceneNode>("join");
    _layer->setContentSize(dimen);
    _layer->doLayout();
    
    _roomIdInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("join_roomidinput"));
    _roomJoinBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_joingamebutton"));
    
    _joinGame = _roomIdInput->getText();
    _pressedJoin = false;
    
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
    
    _roomJoinBtn->addListener([=](const std::string& name, bool down) {
        if (!down) {
            _pressedJoin = true;
        }
    });
    
    return true;
}

void JoinMenu::setDisplay(bool value) {
    if (_layer != nullptr) {
        _roomJoinBtn->setVisible(value);
        _roomIdInput->setVisible(value);
        _layer->setVisible(value);
        
        if (value) {
            _roomJoinBtn->activate();
            _roomIdInput->activate();
        } else {
            _roomJoinBtn->deactivate();
            _roomIdInput->deactivate();
        }
    }
}
