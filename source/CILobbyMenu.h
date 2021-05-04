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
    /** Reference to the game lobby's label for game room id */
    std::shared_ptr<cugl::scene2::Label> _lobbyRoomLabel;
    /** References to the game lobby's labels for player name */
    std::shared_ptr<cugl::scene2::SceneNode> _gamelobbyplayerName1;
    std::shared_ptr<cugl::scene2::SceneNode> _gamelobbyplayerName2;
    std::shared_ptr<cugl::scene2::SceneNode> _gamelobbyplayerName3;
    std::shared_ptr<cugl::scene2::SceneNode> _gamelobbyplayerName4;
    std::shared_ptr<cugl::scene2::SceneNode> _gamelobbyplayerName5;
    std::vector<std::shared_ptr<cugl::scene2::Label>> _gameLobbyPlayerLabels;

    // game lobby settings
    /** Reference to stardust spawn rate button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _spawnRateBtn; // button
    std::shared_ptr<cugl::scene2::Label> _spawnRateLabel; // rate value label
    std::shared_ptr<cugl::scene2::SceneNode> _spawnRateBtnLabel; // button label
    const float _spawnRates[7] = { 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 5.0f, 9.9f }; // rate value list
    uint8_t _currSpawn; // index to cycle the value list
    
    /** Reference to planet gravity strength button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _gravStrengthBtn;
    std::shared_ptr<cugl::scene2::Label> _gravStrengthLabel;
    std::shared_ptr<cugl::scene2::SceneNode> _gravStrengthBtnLabel;
    const float _gravStrengths[7] = { 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 5.0f, 9.9f };
    uint8_t _currGrav;
    
    /** Reference to stardust color count button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _colorCountBtn;
    std::shared_ptr<cugl::scene2::Label> _colorCountLabel;
    std::shared_ptr<cugl::scene2::SceneNode> _colorCountBtnLabel;
    const uint8_t _colorCounts[5] = { 2, 3, 4, 5, 6 };
    uint8_t _currColor;
    
    /** Reference to win condition button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _winMassBtn;
    std::shared_ptr<cugl::scene2::Label> _winMassLabel;
    std::shared_ptr<cugl::scene2::SceneNode> _winMassBtnLabel;
    const uint16_t _winMass[5] = { 4, 5, 6, 7, 8 };
    uint8_t _currWin;

    /** Reference to game lobby's button to start gameplay */
    std::shared_ptr<cugl::scene2::Button> _gameStartBtn;
    
    /** Whether host or client */
    bool _isHost;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game lobby with default values.
     */
    LobbyMenu() : _nextState(MenuState::GameLobby), _currSpawn(2), _currGrav(2), _currWin(2), _currColor(2), _isHost(false){}

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

    void setOtherPlayerLabels(vector<string> othernames) {
        // update other players' labels
        for (int ii = 0; ii < othernames.size(); ii++) {
            _gameLobbyPlayerLabels[ii + 1]->setText(othernames[ii]);
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
