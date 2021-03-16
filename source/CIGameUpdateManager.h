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

class GameUpdateManager {
private:
    /** The last game update sent to other players */
    std::shared_ptr<GameUpdate> _prev_game_update_sent;
    
    /** Vector of game updates to process */
    std::vector<std::shared_ptr<GameUpdate>> _game_updates_to_process;
    
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
     * @param bounds                    The bounds of the screen
     */
    void processGameUpdate(std::shared_ptr<StardustQueue> stardustQueue, cugl::Size bounds);
};

#endif /* __CI_GAME_UPDATE_MANAGER_H__ */
