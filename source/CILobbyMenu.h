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

#include "CIMenuState.h"
#include "CIPlayerSettings.h"
#include "CIGameSettings.h"
#include "CIGameConstants.h"
#include "CINetworkMessageManager.h"

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
    /** Reference to the player settings */
    std::shared_ptr<PlayerSettings> _playerSettings;
    /** Reference to the game settings */
    std::shared_ptr<GameSettings> _gameSettings;

    /** The network message manager for managing connections to other players */
    std::shared_ptr<NetworkMessageManager> _networkMessageManager;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the title label */
    std::shared_ptr<cugl::scene2::Label> _title;
    /** Reference to the game lobby's label for game room id */
    std::shared_ptr<cugl::scene2::Label> _lobbyRoomLabel;
    /** References to the game lobby's player names */
    std::vector<std::shared_ptr<cugl::scene2::NinePatch>> _gameLobbyPlayerNames;
    /** References to the game lobby's player name labels */
    std::vector<std::shared_ptr<cugl::scene2::Label>> _gameLobbyPlayerLabels;

    /** Reference to game lobby's button for game settings */
    std::shared_ptr<cugl::scene2::Button> _gameSettingsBtn;
    /** Reference to game lobby's button to start gameplay */
    std::shared_ptr<cugl::scene2::Button> _gameStartBtn;
    /** Reference to game lobby's ready button for non-hosts  */
    std::shared_ptr<cugl::scene2::Button> _gameReadyBtn;

    /** Whether the game is ready to start by the host */
    bool _isReadyToStart;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game lobby with default values.
     */
    LobbyMenu() : _nextState(MenuState::GameLobby), _isReadyToStart(false) {}

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
     * @param assets                The (loaded) assets for this game lobby menu
     * @param networkMessageManager The network message manager for the game
     * @param gameSettings          The settings for the current game
     * @param playerSettings        The player's saved settings value
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
        const std::shared_ptr<GameSettings>& gameSettings,
        const std::shared_ptr<PlayerSettings>& playerSettings);

    /**
     * Returns a newly allocated game lobby menu.
     *
     * @param assets        The (loaded) assets for this game lobby menu
     * @param networkMessageManager The network message manager for the game
     * @param gameSettings          The settings for the current game
     * @param playerSettings        The player's saved settings value
     *
     * @return a newly allocated game lobby menu
     */
    static std::shared_ptr<LobbyMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
        const std::shared_ptr<GameSettings>& gameSettings,
        const std::shared_ptr<PlayerSettings>& playerSettings) {
        std::shared_ptr<LobbyMenu> result = std::make_shared<LobbyMenu>();
        return (result->init(assets, networkMessageManager, gameSettings, playerSettings) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the game lobby menu is currently active and visible.
     *
     * @param onDisplay     Whether the game lobby menu is currently active and visible
     */
    void setDisplay(bool onDisplay);

    /**
     * Sets the other players labels
     */
    void setOtherPlayerLabels(vector<string> othernames) {
        // update other players' labels
        for (int ii = 0; ii < othernames.size(); ii++) {
            _gameLobbyPlayerLabels[ii + 1]->setText(othernames[ii]);
        }
    }

    /**
     * Sets all the players labels
     */
    void setPlayerLabels(vector<string> playernames) {
        for (int ii = 0; ii < playernames.size(); ii++) {
            _gameLobbyPlayerLabels[ii]->setText(playernames[ii]);
        }
    }

    /**
     * The method called to update the game lobby menu.
     *
     * This method handles transitions into and out of game lobby menu along
     * with any updates within the game lobby menu scene.
     */
    void update(MenuState& state);

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
