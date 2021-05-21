//
//  CINetworkMessageManager.h
//  CoreImpact
//
// This class will be used for send a receiving messages on the network.
//
//  Created by William Long on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_NETWORK_MESSAGE_MANAGER_H__
#define __CI_NETWORK_MESSAGE_MANAGER_H__

#include <cugl/cugl.h>
#include <string>
#include "CIGameUpdateManager.h"
#include "CIGameState.h"
#include "CINetworkUtils.h"
#include "CIGameSettings.h"

class NetworkMessageManager {
private:
    /** The network connection. Used for sending and receiving messages. */
    std::shared_ptr<cugl::CUNetworkConnection> _conn;

    /** The state the game currently is in. */
    GameState _gameState;

    /** Pointer to the game update manager class */
    std::shared_ptr<GameUpdateManager> _gameUpdateManager;

    /** The timestamp of the next message to send. */
    int _timestamp;

    /** The id of the player who was won the game. -1 if the game is still ongoing. */
    int _winnerPlayerId;

    /* string containing player name */
    string _playerName;

    /* string containing room ID for non-hosts */
    string _roomId;

    /** Reference to the game settings */
    std::shared_ptr<GameSettings> _gameSettings;

    /** map from player id to player name and whether player is ready */
    std::map<int, std::pair<string, bool>> _playerMap;
    
    /** The number of frames that have gone by since the last message received from each player */
    std::vector<int> _framesSinceLastMessage;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new network message manager
     */
    NetworkMessageManager() {}

    /**
     * Disposes of all (non-static) resources allocated to this network message manager.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~NetworkMessageManager() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this network message manager.
     */
    void dispose();

    /**
     * Initializes the network message manager
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @return true if the game update manager is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<GameSettings>& gameSettings);

    /**
     * Returns a newly allocated network message manager.
     *
     * @return a newly allocated game update manager.
     */
    static std::shared_ptr<NetworkMessageManager> alloc(const std::shared_ptr<GameSettings>& gameSettings) {
        std::shared_ptr<NetworkMessageManager> result = std::make_shared<NetworkMessageManager>();
        return (result->init(gameSettings) ? result : nullptr);
    }

    /**
     * Resets the network message manager.
     */
    void reset();

#pragma mark Properties
    void setGameUpdateManager(std::shared_ptr<GameUpdateManager> gameUpdateManager) {
        _gameUpdateManager = gameUpdateManager;
    }

    /** 
     * Returns whether current player is the host.
     */
    const bool isPlayerHost() const {
        return (getPlayerId() == 0);
    }
    
    /**
     * Returns the names of the players in the game excluding the current player.
     *
     * @return vector of other player names
     */
    std::vector<string> getOtherNames() const {
        std::vector<string> opponentNames{ "", "", "", "" };
        const int pid = getPlayerId();
        int pind = 0;
        for (const auto& p : _playerMap) {
            if (p.first != pid) {
                if (pind < 4) {
                    opponentNames[pind] = get<0>(p.second);
                    pind++;
                }
                else {
                    break;
                }
            }
        }
        return opponentNames;
    }

    /**
     * Returns the player mapping.
     *
     * @return the current player mapping
     */
    std::map<int, std::pair<string, bool>> getPlayerMap() const {
        return _playerMap;
    }

    /**
     * Sets the player name.
     */
    void setPlayerName(string playerName) {
        _playerName = playerName;
    }

    /**
     * Returns the game state
     *
     * @return the current game state
     */
    GameState getGameState() const {
        return _gameState;
    }

    /**
     * Sets the game state
     */
    void setGameState(GameState gameState) {
        _gameState = gameState;
    }

    /**
     * Returns the room id of the current game. Returns an empty string if not connected to a game yet.
     *
     * @return the room id of the current game
     */
    std::string getRoomId() const {
        if (_conn == nullptr) {
            return "";
        }

        // if player is non host, return saved room ID
        if (getPlayerId() > 0) {
            return _roomId;
        }

        return _conn->getRoomID();
    }

    void setRoomID(string roomID) {
        _roomId = roomID;
    }

    int getPlayerId() const {
        if (_conn == nullptr) {
            return -1;
        }
        else if (!_conn->getPlayerID().has_value()) {
            return -1;
        }
        else {
            return _conn->getPlayerID().value();
        }
    }

    int getWinnerPlayerId() const {
        return _winnerPlayerId;
    }

    /**
     * Returns the number of player connected to the current game.
     *
     * @return number of current player
     */
    int getPlayerCount() const {
        if (_conn == nullptr) {
            return 0;
        }
        return _conn->getNumPlayers();
    }

    /**
     * Returns whether player with given id is connected
     *
     * @return whether player is connected
     */
    bool isActivePlayer(int playerId) const {
        if (_conn == nullptr) {
            return false;
        }
        return _conn->isPlayerActive(playerId);
    }

    /**
     * Returns the current connection status value.
     *
     * @return current connection status
     */
    cugl::CUNetworkConnection::NetStatus getNetworkStatus() const {
        if (_conn == nullptr) {
            return cugl::CUNetworkConnection::NetStatus::GenericError;
        }
        return _conn->getStatus();
    }

#pragma mark Interactions
    /**
     * Sends messages from the game update manager to other players over the network.
     */
    void sendMessages();

    /**
     * Receives messages sent over the network and adds them to the queue in game update manager.
     */
    void receiveMessages();

    /**
     * Creates a game instance with this player as the host.
     */
    void createGame();

    /**
     * Joins the game instance with room id roomID
     *
     * @param roomID the roomId of the game to be joined
     */
    void joinGame(std::string roomID);

private:
    bool isLobbyMessage(int messageType) {
        return messageType == NetworkUtils::MessageType::StartGame
            || messageType == NetworkUtils::MessageType::NameSent
            || messageType == NetworkUtils::MessageType::NameReceivedResponse
            || messageType == NetworkUtils::MessageType::UpdateSetting
            || messageType == NetworkUtils::MessageType::ReadyGame
            || messageType == NetworkUtils::MessageType::DisconnectGame;
    }

};

#endif /* __CI_NETWORK_MESSAGE_MANAGER_H__ */
