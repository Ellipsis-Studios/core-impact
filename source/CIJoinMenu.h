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

#include "CIMenuState.h"
#include "CIGameSettings.h"
#include "CIGameConstants.h"

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
    /** Reference to the game settings */
    std::shared_ptr<GameSettings> _gameSettings;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    
    // 5 digit room id 
    std::vector<string> _roomIds;
    /** Room Ids Index */
    int _roomInd;

    /** Reference to the 1st room id label node */
    std::shared_ptr<cugl::scene2::SceneNode> _roomIdLabelNode1;
    /** Reference to the 2nd room id label node */
    std::shared_ptr<cugl::scene2::SceneNode> _roomIdLabelNode2;
    /** Reference to the 3rd room id label node */
    std::shared_ptr<cugl::scene2::SceneNode> _roomIdLabelNode3;
    /** Reference to the 4th room id label node */
    std::shared_ptr<cugl::scene2::SceneNode> _roomIdLabelNode4;
    /** Reference to the 5th room id label node */
    std::shared_ptr<cugl::scene2::SceneNode> _roomIdLabelNode5;

    /** Reference to the 1st room id label */
    std::shared_ptr<cugl::scene2::Label> _roomIdLabel1;
    /** Reference to the 2nd room id label */
    std::shared_ptr<cugl::scene2::Label> _roomIdLabel2;
    /** Reference to the 3rd room id label */
    std::shared_ptr<cugl::scene2::Label> _roomIdLabel3;
    /** Reference to the 4th room id label */
    std::shared_ptr<cugl::scene2::Label> _roomIdLabel4;
    /** Reference to the 5th room id label */
    std::shared_ptr<cugl::scene2::Label> _roomIdLabel5;

    // number pad 
    /** Reference to the number pad 1 */
    std::shared_ptr<cugl::scene2::Button> _numberpad1;
    /** Reference to the number pad 2 */
    std::shared_ptr<cugl::scene2::Button> _numberpad2;
    /** Reference to the number pad 3 */
    std::shared_ptr<cugl::scene2::Button> _numberpad3;
    /** Reference to the number pad 4 */
    std::shared_ptr<cugl::scene2::Button> _numberpad4;
    /** Reference to the number pad 5 */
    std::shared_ptr<cugl::scene2::Button> _numberpad5;
    /** Reference to the number pad 6 */
    std::shared_ptr<cugl::scene2::Button> _numberpad6;
    /** Reference to the number pad 7 */
    std::shared_ptr<cugl::scene2::Button> _numberpad7;
    /** Reference to the number pad 8 */
    std::shared_ptr<cugl::scene2::Button> _numberpad8;
    /** Reference to the number pad 9 */
    std::shared_ptr<cugl::scene2::Button> _numberpad9;
    /** Reference to the number pad delete */
    std::shared_ptr<cugl::scene2::Button> _numberpaddel;
    /** Reference to the number pad 0 */
    std::shared_ptr<cugl::scene2::Button> _numberpad0;
    /** Reference to the number pad join */
    std::shared_ptr<cugl::scene2::Button> _numberpadjoin;

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
     * @param playerSettings    The player's saved settings value
     * @param gameSettings      The settings for the current game
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<GameSettings>& gameSettings);

    /**
     * Returns a newly allocated Join Game menu.
     *
     * @param assets    The (loaded) assets for this join game menu
     * @param playerSettings    The player's saved settings value
     * @param gameSettings      The settings for the current game
     *
     * @return a newly allocated join game menu
     */
    static std::shared_ptr<JoinMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<GameSettings>& gameSettings) {
        std::shared_ptr<JoinMenu> result = std::make_shared<JoinMenu>();
        return (result->init(assets, gameSettings) ? result : nullptr);
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
    void update(MenuState& state);

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
