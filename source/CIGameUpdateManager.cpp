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
#include "CINetworkUtils.h"

/** Max unprocessed updates at a given time */
#define MAX_PENDING_UPDATES     25

/** The timestamp of the first update sent */
#define INITIAL_TIMESTAMP       0

// TODO: remove this when game id is stored programmatically
#define GAME_ID                 "test"


/**
 * Disposes of all (non-static) resources allocated to this game update manager.
 */
void GameUpdateManager::dispose() {
    _prev_game_update_sent = nullptr;
    _game_updates_to_process.clear();
    _prev_planet_mass = 0;
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
    _prev_planet_mass = 0;
    _player_id = -1;
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
    if (getPlayerId() < 0) {
        return;
    }
    
    std::map<int, std::vector<std::shared_ptr<StardustModel>>> stardustToSend;
    
    int playerIdToSend = 0;
    
    std::vector<std::shared_ptr<StardustModel>> stardustToSendQueue = stardustQueue->getSendQueue();
    for (size_t ii = 0; ii < stardustToSendQueue.size(); ii++) {
        std::shared_ptr<StardustModel> stardust = stardustToSendQueue[ii];

        if (stardust != nullptr) {
            playerIdToSend = stardust->getPreviousOwner();
            
            // send stardust to the correct corner if the stardust went off screen
            if (stardust->getStardustLocation() != StardustModel::Location::ON_SCREEN) {
                playerIdToSend = NetworkUtils::getOpponentPlayerID(getPlayerId(), stardust->getStardustLocation());
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
        std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(GAME_ID, getPlayerId(), stardustToSend, planet, 0);
        _prev_game_update_sent = gameUpdate;
        _game_update_to_send = gameUpdate;
        _prev_planet_mass = planet->getMass();
        return;
    }
    
    // do not send any update if the planet's mass has not changed and there is no stardust to send
    if (stardustToSend.empty() && planet->getMass() == _prev_planet_mass) {
        return;
    }

    std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(GAME_ID, getPlayerId(), stardustToSend, planet, _prev_game_update_sent->getTimestamp() + 1);
    _prev_game_update_sent = gameUpdate;
    _game_update_to_send = gameUpdate;
    _prev_planet_mass = planet->getMass();
}

/**
 * Processes current game updates from other players if there are any.
 *
 * @param stardustQueue     A reference to the player's stardust queue
 * @param planet                    A reference to the player's planet
 * @param bounds                    The bounds of the screen
 */
void GameUpdateManager::processGameUpdate(std::shared_ptr<StardustQueue> stardustQueue, std::shared_ptr<PlanetModel> planet, std::vector<std::shared_ptr<OpponentPlanet>> opponentPlanets, cugl::Size bounds) {
    if (_game_updates_to_process.empty() || getPlayerId() < 0) {
        return;
    }
    
    for (size_t ii = 0; ii < _game_updates_to_process.size(); ii++) {
        std::shared_ptr<GameUpdate> gameUpdate = _game_updates_to_process[ii];
        if (gameUpdate == nullptr) break;
        std::map<int, std::vector<std::shared_ptr<StardustModel>>> stardustSent = gameUpdate->getStardustSent();
        StardustModel::Location opponentLocation = NetworkUtils::getStardustLocation(getPlayerId(), gameUpdate->getPlayerId());
        if (stardustSent.count(getPlayerId()) > 0) {
            std::vector<std::shared_ptr<StardustModel>> stardustVector = stardustSent[getPlayerId()];
            for (size_t jj = 0; jj < stardustVector.size(); jj++) {
                std::shared_ptr<StardustModel> stardust = stardustVector[jj];
                
                // a powerup has been applied by another player
                if (stardust->getStardustType() != StardustModel::Type::NORMAL) {
                    CULog("APPLYING METEOR SHOWER");
                    stardust->setPreviousOwner(gameUpdate->getPlayerId());
                    stardustQueue->addToSendQueue(stardust.get());
                    break;
                }
                
                // this player hit another player with a stardust
                if (stardust->getColor() == CIColor::getNoneColor()) {
                    CIColor::Value c = planet->getColor() == CIColor::getNoneColor() ? CIColor::getRandomColor() : planet->getColor();
                    if (true) {
                        // meteor shower
                        stardustQueue->addStardust(CIColor::getTest(), bounds, StardustModel::Type::METEOR);
                        CULog("Meteor Shower");
                        break;
                    } else {
                        // add 3 stardust, one is guaranteed to be a helpful color, other 2 are random
                        stardustQueue->addStardust(c, bounds);
                        stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                        stardustQueue->addStardust(CIColor::getRandomColor(), bounds);
                        CULog("Return Blast");
                        break;
                    }
                }
                
//                CULog("New stardust from player %i", gameUpdate->getPlayerId());
                // adjust stardust position and velocity based on location of player who sent stardust
                if (opponentLocation == StardustModel::Location::TOP_LEFT) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x < 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y > 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = 0 - 20 + (rand() % 20 - 10);
                    int posY = bounds.height + 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                } else if (opponentLocation == StardustModel::Location::TOP_RIGHT) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x > 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y > 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = bounds.width + 20 + (rand() % 20 - 10);
                    int posY = bounds.height + 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                } else if (opponentLocation == StardustModel::Location::BOTTOM_LEFT) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x < 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y < 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = 0 - 20 + (rand() % 20 - 10);
                    int posY = 0 - 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                } else if (opponentLocation == StardustModel::Location::BOTTOM_RIGHT) {
                    cugl::Vec2 vel = stardust->getVelocity();
                    if (vel.x > 0) {
                        vel.x = -vel.x;
                    }
                    if (vel.y < 0) {
                        vel.y = -vel.y;
                    }
                    stardust->setVelocity(vel);
                    
                    int posX = bounds.width + 20 + (rand() % 20 - 10);
                    int posY = 0 - 20 + (rand() % 20 - 10);
                    stardust->setPosition(cugl::Vec2(posX, posY));
                }
//                CULog("at position (%f, %f)", stardust->getPosition().x, stardust->getPosition().y);
                stardust->setPreviousOwner(gameUpdate->getPlayerId());
                stardustQueue->addStardust(stardust);
            }
        }
        
        std::shared_ptr<OpponentPlanet> planet = std::static_pointer_cast<OpponentPlanet>(gameUpdate->getPlanet());
        if (planet == nullptr) {
            break;
        }
        
        int playerId = gameUpdate->getPlayerId();
        if (opponentPlanets[playerId] == nullptr) {
            opponentPlanets[playerId] = planet;
        } else {
            opponentPlanets[playerId]->setMass(planet->getMass());
            opponentPlanets[playerId]->setColor(planet->getColor());
        }
        
    }
    
    _game_updates_to_process.clear();
}


