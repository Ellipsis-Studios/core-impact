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
    
    /**Thie id of the player who was won the game. -1 if the game is still ongoing. */
    int _winner_player_id;
    
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
    bool init();

    /**
     * Returns a newly allocated network message manager.
     *
     * @return a newly allocated game update manager.
     */
    static std::shared_ptr<NetworkMessageManager> alloc() {
        std::shared_ptr<NetworkMessageManager> result = std::make_shared<NetworkMessageManager>();
        return (result->init() ? result : nullptr);
    }
    
#pragma mark Properties
    void setGameuUpdateManager(std::shared_ptr<GameUpdateManager> gameUpdateManager) {
        _gameUpdateManager = gameUpdateManager;
    }
    
    /**
     * Returns the game state
     *
     * @return the current game state
     */
    GameState getGameState() {
        return _gameState;
    }
    
    /**
     * Returns the room id of the current game. Returns an empty string if not connected to a game yet.
     *
     * @return the room id of the current game
     */
    std::string getRoomId() {
        if (_conn == nullptr) {
            return "";
        }
        
        return _conn->getRoomID();
    }
    
    int getPlayerId() {
        if (_conn == nullptr) {
            return -1;
        } else if (!_conn->getPlayerID().has_value()) {
            return -1;
        } else {
            return _conn->getPlayerID().value();
        }
    }
    
    int getWinnerPlayerId() {
        return _winner_player_id;
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
    
};

#endif /* __CI_NETWORK_MESSAGE_MANAGER_H__ */

