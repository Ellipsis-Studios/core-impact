//
//  CILobbyMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_LOBBY_MENU_H__
#define __CI_LOBBY_MENU_H__
#include <cugl/cugl.h>
#include  "CIMenuStatus.h"


class LobbyMenu {
private:
    /** The asset manager for menu screens. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // Game Lobby Screen Layer
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Label for Room Id in Game Lobby */
    std::shared_ptr<cugl::scene2::Label> _lobbyRoomLabel;
    /** Each label for a user in the game */
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel1; // top
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel2; // middle left
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel3; // middle right
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel4; // bottom left
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel5; // bottom right
    /** Button to trigger start of a game with the current room */
    std::shared_ptr<cugl::scene2::Button> _gameStartBtn;

    bool _pressedStart;

public:
#pragma mark -
#pragma mark Constructors
    LobbyMenu() : _pressedStart(false) {}

    ~LobbyMenu() { dispose(); }

    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    static std::shared_ptr<LobbyMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<LobbyMenu> result = std::make_shared<LobbyMenu>();
        return (result->init(assets) ? result : nullptr);
    }

    void setDisplay(bool value);

    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

    bool getPressedStart() {
        return _pressedStart;
    }

    void update(float timestep) {
        // TODO: update each players' name labels 
    }

    void resetPress() {
        _pressedStart = false;
    }

    void setRoomIdLabel(std::string s) {
        if (_lobbyRoomLabel != nullptr) {
            _lobbyRoomLabel->setText(s);
        }
    }

    void setPlayerLabel1(std::string s) {
        if (_gamelobbyplayerlabel1 != nullptr) {
            _gamelobbyplayerlabel1->setText(s);
        }
    }

};

#endif /* __CI_LOBBY_MENU_H__ */