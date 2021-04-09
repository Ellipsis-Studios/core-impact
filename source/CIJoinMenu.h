//
//  CIJoinMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_JOIN_MENU_H__
#define __CI_JOIN_MENU_H__
#include <cugl/cugl.h>

#include  "CIMenuState.h"

/**
 * Model class representing the Join Game menu scene.
 *
 * The scene contains a game room id input to specify the room to join along
 * with the join game button to trigger switch to Game Lobby. Back button
 * is not included in this class (in parent MenuScene class).
 *
 * The screen is set-up and put on-display on states transitioning into JoinRoom
 * state. Similarly, screen is taken-down and hidden from display on states
 * transitioning out of JoinRoom state.
 */
class JoinMenu {
private:
    /** Menu state for the upcoming frame */
    MenuState _nextState;
    /** Join Game room id string */
    string _joinRoomId;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the join menu's input for game room id */
    std::shared_ptr<cugl::scene2::TextField> _roomIdInput;
    /** Reference to the join menu's button to join game room */
    std::shared_ptr<cugl::scene2::Button> _roomJoinBtn;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new join game-room with default values.
     */
    JoinMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    ~JoinMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    void dispose();

    /**
     * Initializes a new join game menu with the state pointer.
     *
     * @param assets    The (loaded) assets for this join game menu
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Returns a newly allocated Join Game menu.
     *
     * @param assets    The (loaded) assets for this join game menu
     *
     * @return a newly allocated join game menu
     */
    static std::shared_ptr<JoinMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<JoinMenu> result = std::make_shared<JoinMenu>();
        return (result->init(assets) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the join game menu is currently active and visible.
     *
     * @param onDisplay     Whether the join game menu is currently active and visible
     */
    void setDisplay(bool onDisplay);

    /**
     * The method called to update the join game menu.
     *
     * This method handles transitions into and out of join game menu along
     * with any updates within the join game menu scene.
     */
    void update(MenuState& state, string& joingame);

    /**
     * Returns the root scene node for the join game menu layer.
     *
     * @return a shared pointer to join game menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_JOIN_MENU_H__ */
