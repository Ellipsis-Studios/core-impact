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

#include  "CIMenuState.h"

/**
 * Model class representing the Game Lobby menu scene.
 *
 * The scene contains a roomid label and 5 player name labels along with
 * start game button to trigger gameplay start. Back button is not included
 * in this class (in parent MenuScene class).
 *
 * The screen is set-up and put on-display on states transitioning into
 * GameLobby state. Similarly, screen is taken-down and hidden from display
 * on states transitioning out of GameLobby state. The player name labels
 * are updated to match the current game room player names in GameLobby state.
 */
class LobbyMenu {
private:
    /** Menu state for the upcoming frame */
    MenuState _nextState;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the game lobby's label for game room id */
    std::shared_ptr<cugl::scene2::Label> _lobbyRoomLabel;
    /** References to the game lobby's labels for player name */
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel1; // top
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel2; // middle left
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel3; // middle right
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel4; // bottom left
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel5; // bottom right
    /** Reference to game lobby's button to start gameplay */
    std::shared_ptr<cugl::scene2::Button> _gameStartBtn;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game lobby with default values.
     */
    LobbyMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    ~LobbyMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    void dispose();

    /**
     * Initializes a new game lobby menu with the state pointer.
     *
     * @param assets        The (loaded) assets for this game lobby menu
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Returns a newly allocated game lobby menu.
     *
     * @param assets        The (loaded) assets for this game lobby menu
     *
     * @return a newly allocated game lobby menu
     */
    static std::shared_ptr<LobbyMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<LobbyMenu> result = std::make_shared<LobbyMenu>();
        return (result->init(assets) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the game lobby menu is currently active and visible.
     *
     * @param value     Whether the game lobby menu is currently active and visible
     */
    void setDisplay(bool value);

    /**
     * The method called to update the game lobby menu.
     *
     * This method handles transitions into and out of game lobby menu along
     * with any updates within the game lobby menu scene.
     */
    void update(MenuState& state, string& joingame, string& playername, std::vector<string>& othernames);

    /**
     * Returns the root scene node for the game lobby menu layer.
     *
     * @return a shared pointer to game lobby menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_LOBBY_MENU_H__ */