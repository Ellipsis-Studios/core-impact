//
//  CINetworkMessageManager.cpp
//  CoreImpact
//
// This class will be used for send a receiving messages on the network.
//
//  Created by William Long on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CINetworkMessageManager.h"
#include <vector>
#include <cugl/cugl.h>
#include "CIGameUpdate.h"
#include "CIStardustModel.h"
#include "CIOpponentPlanet.h"
#include "CINetworkUtils.h"

/**
 * Disposes of all (non-static) resources allocated to this network message manager.
 */
void NetworkMessageManager::dispose() {
    _conn = nullptr;
    _gameUpdateManager = nullptr;
    _timestamp = 0;
}

/**
 * Initializes the network message manager
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @return true if the game update manager is initialized properly, false otherwise.
 */
bool NetworkMessageManager::init() {
    _gameState = GameState::OnMenuScreen;
    _timestamp = 0;
    return true;
}

/**
 * Sends messages from the game update manager to other players over the network.
 */
void NetworkMessageManager::sendMessages() {
    if (_conn == nullptr || !_conn->getPlayerID().has_value())
        return;

    std::shared_ptr<GameUpdate> gameUpdate = _gameUpdateManager->getGameUpdateToSend();
    if (gameUpdate == nullptr) {
        return;
    }
    
    // vector containing data bytes for each message
    std::vector<uint8_t> data;
        
    int playerId = _conn->getPlayerID().value();
    for (auto const& [key, val] : gameUpdate->getStardustSent()) {
        int dstPlayerId = key;
        
        // send stardust sent
        for (size_t jj = 0; jj < val.size(); jj++) {
            int stardustColor = val[jj]->getColor();
            
            // TODO: only send speed of stardust
            float xVel = val[jj]->getVelocity().x;
            float yVel = val[jj]->getVelocity().y;
            
            NetworkUtils::encodeInt(NetworkUtils::MessageType::StardustSent, data);
            NetworkUtils::encodeInt(playerId, data);
            NetworkUtils::encodeInt(dstPlayerId, data);
            NetworkUtils::encodeInt(stardustColor, data);
            NetworkUtils::encodeFloat(xVel, data);
            NetworkUtils::encodeFloat(yVel, data);
            NetworkUtils::encodeInt(_timestamp, data);
            _timestamp++;
            
             _conn->send(data);
            data.clear();
            CULog("SENT SU> SRC[%i], DST[%i], CLR[%i], VEL[%f,%f]", playerId, dstPlayerId, stardustColor, xVel, yVel);
        }
    }
    
    // send planet update
    int planetColor = gameUpdate->getPlanet()->getColor();
    float planetSize = gameUpdate->getPlanet()->getMass();

    NetworkUtils::encodeInt(NetworkUtils::MessageType::PlanetUpdate, data);
    NetworkUtils::encodeInt(playerId, data);
    NetworkUtils::encodeInt(planetColor, data);
    NetworkUtils::encodeInt(planetSize, data);
    NetworkUtils::encodeInt(_timestamp, data);
    _timestamp++;

    _conn->send(data);
    data.clear();
    CULog("SENT PU> SRC[%i], CLR[%i], SIZE[%f]", playerId, planetColor, planetSize);

    // TODO: send win game message
    
    // clear game update to send now that we have sent update.
    _gameUpdateManager->clearGameUpdateToSend();
}

/**
 * Receives messages sent over the network and adds them to the queue in game update manager.
 */
void NetworkMessageManager::receiveMessages() {
    if (_conn == nullptr)
        return;

    _conn->receive([this](const std::vector<uint8_t>& recv) {
        if (!recv.empty()) {
            int message_type = NetworkUtils::decodeInt(recv[0], recv[1], recv[2], recv[3]);

            if (message_type == NetworkUtils::MessageType::StardustSent) {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int dstPlayer = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);
                int stardustColor = NetworkUtils::decodeInt(recv[12], recv[13], recv[14], recv[15]);
                float xVel = NetworkUtils::decodeFloat(recv[16], recv[17], recv[18], recv[19]);
                float yVel = NetworkUtils::decodeFloat(recv[20], recv[21], recv[22], recv[23]);
                int timestamp = NetworkUtils::decodeInt(recv[24], recv[25], recv[26], recv[27]);

                CULog("RCVD SU> SRC[%i], DST[%i], CLR[%i], VEL[%f,%f]", srcPlayer, dstPlayer, stardustColor, xVel, yVel);

                std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(0, 0), cugl::Vec2(xVel, yVel), static_cast<CIColor::Value>(stardustColor));
                std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = { { dstPlayer, std::vector<std::shared_ptr<StardustModel>> { stardust } } };
                std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, nullptr, timestamp);
                _gameUpdateManager->addGameUpdate(gameUpdate);
            }
            else if (message_type == NetworkUtils::MessageType::PlanetUpdate) {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int planetColor = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);
                float planetSize = NetworkUtils::decodeFloat(recv[12], recv[13], recv[14], recv[15]);
                int timestamp = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);
                
                CULog("RCVD PU> SRC[%i], CLR[%i], SIZE[%f]", srcPlayer, planetColor, planetSize);

                std::shared_ptr<OpponentPlanet> planet = OpponentPlanet::alloc(0, 0, static_cast<CIColor::Value>(planetColor));
                planet->setMass(planetSize);
                std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = {};
                std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, planet, timestamp);
                _gameUpdateManager->addGameUpdate(gameUpdate);
            }
            else {
                CULog("WRONG MESSAGE TYPE");
            }
        }
    });
}

/**
 * Creates a game instance with this player as the host.
 */
void NetworkMessageManager::createGame() {
    _conn = std::make_shared<cugl::CUNetworkConnection>(NetworkUtils::getConnectionConfig());
    _gameState = GameState::JoiningGameAsHost;
    CULog("CONNECTING AS HOST");
}

/**
 * Joins the game instance with room id roomID
 *
 * @param roomID the roomId of the game to be joined
 */
void NetworkMessageManager::joinGame(std::string roomID) {
    _conn = std::make_shared<cugl::CUNetworkConnection>(NetworkUtils::getConnectionConfig(), roomID);
    _gameState = GameState::JoiningGameAsNonHost;
    CULog("CONNECTING AS NON HOST");
}
