//
//  CIGameUpdateManager.h
//  CoreImpact
//
//  The game update manager class is used for updates sent and received from other players.
//  This class will be stored in the game scene and will send updates to other players
//  when needed and will process updates from other players.
//
//  Created by William Long on 3/15/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_GAME_UPDATE_MANAGER_H__
#define __CI_GAME_UPDATE_MANAGER_H__

#include <vector>
#include <cugl/cugl.h>

#include "CIGameUpdate.h"
#include "CIPlanetModel.h"
#include "CIStardustQueue.h"
#include "CIOpponentPlanet.h"

class GameUpdateManager {
private:
    /** The last game update sent to other players */
    std::shared_ptr<GameUpdate> _prev_game_update_sent;
    
    /** The mass of the planet from the previous game update sent */
    float _prev_planet_mass;
    
    std::shared_ptr<GameUpdate> _game_update_to_send;
    
    /** Vector of game updates to process */
    std::vector<std::shared_ptr<GameUpdate>> _game_updates_to_process;
    
    /** The player id. Initialized to -1 before a player id is assigned. */
    int _player_id;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game update manager
     */
    GameUpdateManager() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this game update manager.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameUpdateManager() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this game update manager.
     */
    void dispose();

    /**
     * Initializes the game update manager
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @return true if the game update manager is initialized properly, false otherwise.
     */
    bool init();
    
    /**
     * Returns a newly allocated game update manager.
     *
     * @return a newly allocated game update manager.
     */
    static std::shared_ptr<GameUpdateManager> alloc() {
        std::shared_ptr<GameUpdateManager> result = std::make_shared<GameUpdateManager>();
        return (result->init() ? result : nullptr);
    }
    
#pragma mark Properties
    /**
     * Returns the game updates to process vector.
     *
     * @return the vector of game updates to process
     */
    std::vector<std::shared_ptr<GameUpdate>> getGameUpdatesToProcess() {
        return _game_updates_to_process;
    }
    
    /**
     * Clears the game updates to process. This method should only be called once all the game updates have been sent to other players.
     */
    void clearGameUpdatesToProcess() {
        _game_updates_to_process.clear();
    }
    
    void addGameUpdate(std::shared_ptr<GameUpdate> gameUpdate) {
        _game_updates_to_process.push_back(gameUpdate);
    }
    
    std::shared_ptr<GameUpdate> getGameUpdateToSend() {
        return _game_update_to_send;
    }
    
    void clearGameUpdateToSend() {
        _game_update_to_send = nullptr;
    }
    
    int getPlayerId() {
        return _player_id;
    }
    
    void setPlayerId(int playerId) {
        _player_id = playerId;
    }

#pragma mark Interactions
    /**
     * Sends a game update to other players if the game state has changed.
     *
     * @param planet                     A reference to the player's planet
     * @param stardustQueue     A reference to the player's stardust queue
     * @param bounds                    The bounds of the screen
     */
    void sendUpdate(const std::shared_ptr<PlanetModel> planet, const std::shared_ptr<StardustQueue> stardustQueue, cugl::Size bounds);
    
    /**
     * Processes current game updates from other players if there are any.
     *
     * @param stardustQueue     A reference to the player's stardust queue
     * @param opponentPlanets A vector containing the planets of the other players
     * @param bounds                    The bounds of the screen
     */
    void processGameUpdate(std::shared_ptr<StardustQueue> stardustQueue, std::vector<std::shared_ptr<OpponentPlanet>> opponentPlanets, cugl::Size bounds);
};

#endif /* __CI_GAME_UPDATE_MANAGER_H__ */
