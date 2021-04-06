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
    _assets = nullptr;
}

bool LobbyMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _assets = assets;
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


    _pressedStart = false;

    _gameStartBtn->addListener([=](const std::string& name, bool down) {
        if (!down) {
            _pressedStart = true;
        }
        });

    return true;
}

void LobbyMenu::setDisplay(bool value) {
    if (_layer != nullptr) {
        _gameStartBtn->setVisible(value);
        _lobbyRoomLabel->setVisible(value);
        _gamelobbyplayerlabel1->setVisible(value);
        _gamelobbyplayerlabel2->setVisible(value);
        _gamelobbyplayerlabel3->setVisible(value);
        _gamelobbyplayerlabel4->setVisible(value);
        _gamelobbyplayerlabel5->setVisible(value);
        _layer->setVisible(value);

        if (value) {
            _gameStartBtn->activate();
        }
        else {
            _gameStartBtn->deactivate();
        }
    }
}

