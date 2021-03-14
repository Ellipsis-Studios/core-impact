//
//  CIGameUpdate.h
//  CoreImpact
//
//  This class is used to store information associated with game updates between players.
//
//  Created by William Long on 3/13/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef CIGameUpdate_h
#define CIGameUpdate_h

#include <map>
#include <string>
#include <vector>
#include "CIStardustModel.h"
#include "CIPlanetModel.h"


class GameUpdate {
private:
    /** The id of the game currently being played on this device */
    std::string _game_id;
    
    /** The id of the player who sent this update */
    int _player_id;
    
    /** Map of player id to stardust sent to other players */
    std::map<int, std::vector<StardustModel>> _stardust_sent;
    
    /** The planet of the player sending the update */
    PlanetModel _planet;
    
    /** The timestamp associated with this  */
    int _timestamp;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game update
     */
    GameUpdate() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this game update.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameUpdate() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this game update.
     */
    void dispose();
    
    /**
     * Initializes the game update
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param gameId                 The game id of the current game
     * @param playerId            The id of the player who sent the game update
     * @param stardustSent   Map of player id to stardust
     * @param planet                The planet of the player that sent the message
     * @param timestamp         The timestamp of the game update
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(std::string gameId, int playerId, std::map<int, std::vector<StardustModel>> stardustSent, PlanetModel planet, int timestamp);
    
    /**
     * Returns a newly allocated game update.
     *
     * This method does NOT create a scene graph node for this stardust.  You
     * must call setTextures for that.
     *
     * @param gameId                The game id of the current game
     * @param playerId            The id of the player who sent the game update
     * @param stardustSent   Map of player id to stardust
     * @param planet                The planet of the player that sent the message
     * @param timestamp         The timestamp of the game update
     *
     * @return a newly allocated game update.
     */
    static std::shared_ptr<GameUpdate> alloc(std::string gameId, int playerId, std::map<int, std::vector<StardustModel>> stardustSent, PlanetModel planet, int timestamp) {
        std::shared_ptr<GameUpdate> result = std::make_shared<GameUpdate>();
        return (result->init(gameId, playerId, stardustSent, planet, timestamp) ? result : nullptr);
    }
    
#pragma mark Properties
    /**
     * Returns the game id associated with this game update.
     */
    std::string getGameId() {
        return _game_id;
    }
    
    /**
     * Returns the player id who sent this game update.
     */
    int getPlayerId() {
        return _player_id;
    }
    
    /**
     * Return the map of player ids to stardust.
     */
    std::map<int, std::vector<StardustModel>> getStardustSent() {
        return _stardust_sent;
    }
    
    void setStardustSent(std::map<int, std::vector<StardustModel>> stardustSent) {
        _stardust_sent = stardustSent;
    }
    
    /**
     * Returns the planet of the player who sent the game update.
     */
    PlanetModel getPlanet() {
        return _planet;
    }
    
    /**
     * Sets the planet associated with this game update.
     */
    void setPlanet(PlanetModel planet) {
        _planet = planet;
    }
    
    /**
     * Returns the timestamp associated with this game update.
     */
    int getTimestamp() {
        return _timestamp;
    }
    
    /**
     * Returns whether the player sending the game update has won.
     */
    bool didPlayerWin() {
        // TODO: need to implement win condition
        return false;
    }
    
};

#endif /* CIGameUpdate_h */
