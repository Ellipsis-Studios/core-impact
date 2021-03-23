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
        // Send the dot to the player assigned to that corner. If client rather than host and the cornerId matches playerId, send to host.
        int dstPlayerId = (key == playerId) ? 0 : key;
        
        // send stardust sent
        for (size_t jj = 0; jj < val.size(); jj++) {
            int stardustColor = val[jj]->getColor();
            
            // TODO: only send speed of stardust
            float xVel = val[jj]->getVelocity().x;
            float yVel = val[jj]->getVelocity().y;
            
            CULog("SENT> SRC[%i], DST[%i], CLR[%i], VEL[%f,%f]", playerId, dstPlayerId, stardustColor, xVel, yVel);
            
            data.push_back(NetworkUtils::MessageType::StardustSent);
            NetworkUtils::encodeInt(playerId, data);
            NetworkUtils::encodeInt(dstPlayerId, data);
            NetworkUtils::encodeInt(stardustColor, data);
            NetworkUtils::encodeFloat(xVel, data);
            NetworkUtils::encodeFloat(yVel, data);
            NetworkUtils::encodeInt(_timestamp, data);
            _timestamp++;
            
             _conn->send(data);
            data.clear();
        }
    }
        
        // send planet update on last game update to process
//        if (ii == gameUpdatesToProcess.size() - 1) {
//            int planetColor = cugl::marshall(gameUpdate->getPlanet()->getColor());
//            int planetSize = cugl::marshall(gameUpdate->getPlanet()->getMass());
//            int planetCurrLayerSize = cugl::marshall(gameUpdate->getPlanet()->getCurrLayerProgress());
//            int planetStardustToNextLockIn = cugl::marshall(gameUpdate->getPlanet()->getLayerLockinTotal());
//
//            data.push_back(PLANET_UPDATE);
//            encodeInt(playerId, data);
//            encodeInt(planetColor, data);
//            encodeInt(planetSize, data);
//            encodeInt(planetCurrLayerSize, data);
//            encodeInt(planetStardustToNextLockIn, data);
//            encodeInt(cugl::marshall(_timestamp), data);
//            _timestamp++;
//
//            // TODO: uncomment when cugl-net set up
//            // conn->send(data);
//            data.clear();
//        }
        
        // TODO: send win game message
//    }
    
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
        //CULog("RECEIVED MESSAGE");
        //CULog("%u", message.size());
        if (!recv.empty()) {
            //uint8_t message_type = cugl::marshall(recv[0]);
            uint8_t message_type = recv[0];
            //CULog("MESSAGE TYPE");
            //CULog("%u", message_type);

            if (message_type == NetworkUtils::MessageType::StardustSent) {
                int srcPlayer = NetworkUtils::decodeInt(recv[1], recv[2], recv[3], recv[4]);
                int dstPlayer = NetworkUtils::decodeInt(recv[5], recv[6], recv[7], recv[8]);
                int stardustColor = NetworkUtils::decodeInt(recv[9], recv[10], recv[11], recv[12]);
                float xVel = NetworkUtils::decodeFloat(recv[13], recv[14], recv[15], recv[16]);
                float yVel = NetworkUtils::decodeFloat(recv[17], recv[18], recv[19], recv[20]);
                int timestamp = NetworkUtils::decodeInt(recv[21], recv[22], recv[23], recv[24]);

                CULog("RCVD> SRC[%i], DST[%i], CLR[%i], VEL[%f,%f]", srcPlayer, dstPlayer, stardustColor, xVel, yVel);

                std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(0, 0), cugl::Vec2(xVel, yVel), static_cast<CIColor::Value>(stardustColor));
                std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = { { dstPlayer, std::vector<std::shared_ptr<StardustModel>> { stardust } } };
                std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, nullptr, timestamp);
                _gameUpdateManager->addGameUpdate(gameUpdate);
            }
            else if (message_type == NetworkUtils::MessageType::PlanetUpdate) {
                CULog("RECEIVED PLANET UPDATE MESSAGE");

                // TODO: no need to process bytes now as opponent planet is not drawn on screen
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
