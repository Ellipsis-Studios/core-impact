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

// TODO: turn message types into a class/enum
#define STARDUST_SENT   1
#define PLANET_UPDATE   2

// TODO: change when match making is done
#define GAME_ID         "test game id"

/** The precision to multiply floating point numbers by */
constexpr float FLOAT_PRECISION = 10.0f;

/** One byte */
constexpr uint16_t ONE_BYTE = 256;

/** IP of the NAT punchthrough server */
constexpr auto SERVER_ADDRESS = "34.74.68.73";
/** Port of the NAT punchthrough server */
constexpr uint16_t SERVER_PORT = 61111;

struct cugl::CUNetworkConnection::ConnectionConfig CONNECTION_CONFIG = {
    SERVER_ADDRESS,
    SERVER_PORT,
    4,
    0
};

// TODO: put into a class
float decodeFloat(uint8_t m1, uint8_t m2) {
    return static_cast<float>(m1 + ONE_BYTE * m2) / FLOAT_PRECISION;
}

void encodeFloat(float f, std::vector<uint8_t>& out) {
    auto ff = static_cast<uint16_t>(f * FLOAT_PRECISION);
    out.push_back(static_cast<uint8_t>(ff % ONE_BYTE));
    out.push_back(static_cast<uint8_t>(ff / ONE_BYTE));
}

// Maybe wrong, took from stack overflow
int decodeInt(uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4) {
    int x = 0;
    x <<= 8;
    x |= i1;
    x <<= 8;
    x |= i2;
    x <<= 8;
    x |= i3;
    x <<= 8;
    x |= i4;
    return x;
}

// Maybe wrong, took from stack overflow
void encodeInt(int x, std::vector<uint8_t>& out) {
    for (size_t i = 0; i < sizeof(x); ++i) {
        out.push_back(x & 0xFF);
        x >>= 8;
    }
}

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
    _gameUpdateManager = GameUpdateManager::alloc();
    _timestamp = 0;
    return true;
}

/**
 * Sends messages from the game update manager to other players over the network.
 */
void NetworkMessageManager::sendMessages() {
    std::vector<std::shared_ptr<GameUpdate>> gameUpdatesToProcess = _gameUpdateManager->getGameUpdatesToProcess();
    if (gameUpdatesToProcess.empty()) {
        return;
    }
    
    CULog("%u", gameUpdatesToProcess.size());
    
    // vector containing data bytes for each message
    std::vector<uint8_t> data;
    
    for (size_t ii = 0; ii < gameUpdatesToProcess.size(); ii++) {
        std::shared_ptr<GameUpdate> gameUpdate = gameUpdatesToProcess[ii];
        
        int playerId = cugl::marshall(gameUpdate->getPlayerId());
        for (auto const& [key, val] : gameUpdate->getStardustSent()) {
            int dstPlayerId = cugl::marshall(key);
            
            // send stardust sent
            for (size_t jj = 0; jj < val.size(); jj++) {
                int stardustColor = cugl::marshall(val[jj]->getColor());
                
                // TODO: only send speed of stardust
                float xVel = cugl::marshall(val[jj]->getVelocity().x);
                float yVel = cugl::marshall(val[jj]->getVelocity().y);
                
                data.push_back(STARDUST_SENT);
                encodeInt(playerId, data);
                encodeInt(dstPlayerId, data);
                encodeInt(stardustColor, data);
                encodeFloat(xVel, data);
                encodeFloat(yVel, data);
                encodeInt(cugl::marshall(_timestamp), data);
                _timestamp++;
                
                // TODO: uncomment when cugl-net set up
                // conn->send(data);
                data.clear();
            }
        }
        
        // send planet update on last game update to process
        if (ii == gameUpdatesToProcess.size() - 1) {
            int planetColor = cugl::marshall(gameUpdate->getPlanet()->getColor());
            int planetSize = cugl::marshall(gameUpdate->getPlanet()->getMass());
            int planetCurrLayerSize = cugl::marshall(gameUpdate->getPlanet()->getCurrLayerProgress());
            int planetStardustToNextLockIn = cugl::marshall(gameUpdate->getPlanet()->getLayerLockinTotal());
            
            data.push_back(PLANET_UPDATE);
            encodeInt(playerId, data);
            encodeInt(planetColor, data);
            encodeInt(planetSize, data);
            encodeInt(planetCurrLayerSize, data);
            encodeInt(planetStardustToNextLockIn, data);
            encodeInt(cugl::marshall(_timestamp), data);
            _timestamp++;
            
            // TODO: uncomment when cugl-net set up
            // conn->send(data);
            data.clear();
        }
        
        // TODO: send win game message
    }
    
    // clear game updates to process now that we have processed them all
    _gameUpdateManager->clearGameUpdatesToProcess();
}

/**
 * Receives messages sent over the network and adds them to the queue in game update manager.
 */
void NetworkMessageManager::receiveMessages() {
    CULog("RECEIVED NETWORK MESSAGE");
    
    // TODO: call conn->receive instead of dunmby message
    std::vector<uint8_t> recv{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    uint8_t message_type = cugl::marshall(recv[0]);
    if (message_type == STARDUST_SENT) {
        int srcPlayer = cugl::marshall(decodeInt(recv[1], recv[2], recv[3], recv[4]));
        int dstPlayer = cugl::marshall(decodeInt(recv[5], recv[6], recv[7], recv[8]));
        int stardustColor = cugl::marshall(decodeInt(recv[9], recv[10], recv[11], recv[12]));
        float xVel = cugl::marshall(decodeFloat(recv[13], recv[14]));
        float yVel = cugl::marshall(decodeFloat(recv[15], recv[16]));
        int timestamp = cugl::marshall(decodeInt(recv[17], recv[18], recv[19], recv[20]));
        
        std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(0, 0), cugl::Vec2(xVel, yVel), static_cast<CIColor::Value>( stardustColor));
        std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = {{ dstPlayer, std::vector<std::shared_ptr<StardustModel>> { stardust } }};
        std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(GAME_ID, srcPlayer, map, nullptr, timestamp);
        _gameUpdateManager->addGameUpdate(gameUpdate);
    } else if (message_type == PLANET_UPDATE) {
        CULog("RECEIVED PLANET UPDATE MESSAGE");
        
        // TODO: no need to process bytes now as opponent planet is not drawn on screen
    }
    return;
}

/**
 * Creates a game instance with this player as the host.
 */
void NetworkMessageManager::createGame() {
    _conn = std::make_shared<cugl::CUNetworkConnection>(CONNECTION_CONFIG);
    _gameState = GameState::JoiningGameAsHost;
    CULog("CONNECTING AS HOST");
}

/**
 * Joins the game instance with room id roomID
 *
 * @param roomID the roomId of the game to be joined
 */
void NetworkMessageManager::joinGame(std::string roomID) {
    _conn = std::make_shared<cugl::CUNetworkConnection>(CONNECTION_CONFIG, roomID);
    _gameState = GameState::JoiningGameAsNonHost;
    CULog("CONNECTING AS NON HOST");
}

void NetworkMessageManager::receive() {
    _conn->receive([this](const std::vector<uint8_t>& message) {
        CULog("%u", message.size());
    });
}
