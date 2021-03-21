//
//  CIGameUpdateManager.cpp
//  CoreImpact
//
//  The game update manager class is used for updates sent and received from other players.
//  This class will be stored in the game scene and will send updates to other players
//  when needed and will process updates from other players.
//
//  Created by William Long on 3/15/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIGameUpdateManager.h"
#include <map>
#include <cugl/cugl.h>

/** Max unprocessed updates at a given time */
#define MAX_PENDING_UPDATES     25

/** The timestamp of the first update sent */
#define INITIAL_TIMESTAMP       0

// TODO: REMOVE WHEN MATCHMAKING IMPLEMENTED
/** HARDCORD GAME ID FOR NOW. REMOVE THIS WHEN MATCH MAKING IS IMPLEMENTED */
#define GAME_ID                 "TEST"
/** HARDCORD PLAYER ID FOR NOW. REMOVE THIS WHEN MATCH MAKING IS IMPLEMENTED */
#define PLAYER_ID               0
/** HARDCODE PLAYER POSITIONS FOR NOW */
#define TOP_LEFT_PLAYER         1
#define TOP_RIGHT_PLAYER        2
#define BOTTOM_LEFT_PLAYER      3
#define BOTTOM_RIGHT_PLAYER     4


/**
 * Disposes of all (non-static) resources allocated to this game update manager.
 */
void GameUpdateManager::dispose() {
    _prev_game_update_sent = nullptr;
    _game_updates_to_process.clear();
}

/**
 * Initializes the game update manager
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @return true if the game update manager is initialized properly, false otherwise.
 */
bool GameUpdateManager::init() {
    _game_updates_to_process.resize(MAX_PENDING_UPDATES);
    return true;
}

#pragma mark Interactions
/**
 * Sends a game update to other players if the game state has changed.
 *
 * @param planet                     A reference to the player's planet
 * @param stardustQueue     A reference to the player's stardust queue
 * @param bounds                    The bounds of the screen
 */
void GameUpdateManager::sendUpdate(const std::shared_ptr<PlanetModel> planet, const std::shared_ptr<StardustQueue> stardustQueue, cugl::Size bounds) {
    std::map<int, std::vector<std::shared_ptr<StardustModel>>> stardustToSend;
    
    int playerIdToSend = 0;
    
    std::vector<std::shared_ptr<StardustModel>> stardustToSendQueue = stardustQueue->getSendQueue();
    for (size_t ii = 0; ii < stardustToSendQueue.size(); ii++) {
        std::shared_ptr<StardustModel> stardust = stardustToSendQueue[ii];

        // only send stardust that is off the screen currently
        if (stardust != nullptr && stardust->getStardustLocation() != StardustModel::Location::ON_SCREEN) {
            if (stardust->getStardustLocation() == StardustModel::Location::TOP_LEFT) {
                playerIdToSend = TOP_LEFT_PLAYER;
            } else if (stardust->getStardustLocation() == StardustModel::Location::TOP_RIGHT) {
                playerIdToSend = TOP_RIGHT_PLAYER;
            } else if (stardust->getStardustLocation() == StardustModel::Location::BOTTOM_LEFT) {
                playerIdToSend = BOTTOM_LEFT_PLAYER;
            } else if (stardust->getStardustLocation() == StardustModel::Location::BOTTOM_RIGHT) {
                playerIdToSend = BOTTOM_RIGHT_PLAYER;
            }
            
            if (stardustToSend.count(playerIdToSend) > 0) {
                std::vector<std::shared_ptr<StardustModel>> stardustVector = stardustToSend[playerIdToSend];
                stardustVector.emplace_back(stardust);
            } else {
                std::vector<std::shared_ptr<StardustModel>> stardustVector;
                stardustVector.emplace_back(stardust);
                stardustToSend[playerIdToSend] = stardustVector;
            }
        }
    }
    
    // clear the send queue
    stardustQueue->clearSendQueue();
    
    // always send a game update initially
    if (_prev_game_update_sent == nullptr) {
        std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(GAME_ID, PLAYER_ID, stardustToSend, planet, 0);
        _prev_game_update_sent = gameUpdate;
        _game_update_to_send = gameUpdate;
        return;
    }
    
    // do not send any update if the planet does not have a new layer and there is no stardust to send
    if (stardustToSend.empty() && planet->getNumLayers() == _prev_game_update_sent->getPlanet()->getNumLayers()) {
        return;
    }

    std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(GAME_ID, PLAYER_ID, stardustToSend, planet, _prev_game_update_sent->getTimestamp() + 1);
    _prev_game_update_sent = gameUpdate;
    _game_update_to_send = gameUpdate;
}

/**
 * Processes current game updates from other players if there are any.
 *
 * @param stardustQueue     A reference to the player's stardust queue
 * @param bounds                    The bounds of the screen
 */
void GameUpdateManager::processGameUpdate(std::shared_ptr<StardustQueue> stardustQueue, cugl::Size bounds) {
    if (_game_updates_to_process.empty()) {
        return;
    }
    
    for (size_t ii = 0; ii < _game_updates_to_process.size(); ii++) {
        std::shared_ptr<GameUpdate> gameUpdate = _game_updates_to_process[ii];
        std::map<int, std::vector<std::shared_ptr<StardustModel>>> stardustSent = gameUpdate->getStardustSent();
        
        if (stardustSent.count(PLAYER_ID) > 0) {
            std::vector<std::shared_ptr<StardustModel>> stardustVector = stardustSent[PLAYER_ID];
            for (size_t jj = 0; jj < stardustVector.size(); jj++) {
                std::shared_ptr<StardustModel> stardust = stardustVector[jj];
                
                // adjust stardust position and velocity based on location of player who sent stardust
                if (gameUpdate->getPlayerId() == TOP_LEFT_PLAYER) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x < 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y > 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = bounds.width - 20 + (rand() % 20 - 10);
                    int posY = bounds.width + 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                } else if (gameUpdate->getPlayerId() == TOP_RIGHT_PLAYER) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x > 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y > 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = bounds.width + 20 + (rand() % 20 - 10);
                    int posY = bounds.width + 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                } else if (gameUpdate->getPlayerId() == BOTTOM_LEFT_PLAYER) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x < 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y < 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = bounds.width - 20 + (rand() % 20 - 10);
                    int posY = bounds.width - 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                } else if (gameUpdate->getPlayerId() == BOTTOM_RIGHT_PLAYER) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x > 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y < 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = bounds.width + 20 + (rand() % 20 - 10);
                    int posY = bounds.width - 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                }
                
                stardust->setPreviousOwner(gameUpdate->getPlayerId());
                stardustQueue->addStardust(stardust);
            }
        }
    }
    
    // TODO: update other player's planet displays
    
    _game_updates_to_process.clear();
}


