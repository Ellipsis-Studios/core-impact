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
#include "CILocation.h"

#define  FRAMES_UNTIL_TIMEOUT   600     // 10 seconds
#define  FRAMES_UNTIL_PING      120     // 2 seconds

/**
 * Disposes of all (non-static) resources allocated to this network message manager.
 */
void NetworkMessageManager::dispose() {
    _conn = nullptr;
    _gameUpdateManager = nullptr;
    _timestamp = 0;
    _winnerPlayerId = -1;
    _framesSinceLastMessage.clear();
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
bool NetworkMessageManager::init(const shared_ptr<GameSettings>& gameSettings) {
    _gameSettings = gameSettings;
    reset();
    _framesSinceLastMessage.resize(5);
    return true;
}

/**
 * Resets the network message manager.
 */
void NetworkMessageManager::reset() {
    _conn = nullptr;
    _gameUpdateManager = nullptr;
    _gameState = GameState::OnMenuScreen;
    _timestamp = 0;
    _winnerPlayerId = -1;
    _playerMap.clear();
    _roomId = "00000";
    _playerName = "Waiting...";
    _gameSettings->reset();
    _framesSinceLastMessage.clear();
}

/**
 * Sends messages from the game update manager to other players over the network.
 */
void NetworkMessageManager::sendMessages() {
    if (_conn == nullptr || !_conn->getPlayerID().has_value())
        return;

    // vector containing data bytes for each message
    std::vector<uint8_t> data;
    const int playerId = _conn->getPlayerID().value();

    switch (_gameState)
    {
        case GameState::JoiningGameAsHost:
        case GameState::JoiningGameAsNonHost:
        {
            NetworkUtils::encodeInt(NetworkUtils::MessageType::NameSent, data);
            NetworkUtils::encodeString(_playerName, data);
            NetworkUtils::encodeInt(playerId, data);
            NetworkUtils::encodeInt(_timestamp, data);
            _timestamp++;
            _conn->send(data);
            data.clear();
            CULog("SENT PLAYER NAME MESSAGE> PLAYERNAME[%s], PLAYER[%i]", _playerName.c_str(), playerId);
            _gameState = GameState::NameSent;
            _playerMap[playerId] = std::make_pair(_playerName, (playerId == 0));
            return;
        }
        case GameState::SettingSent:
        {
            NetworkUtils::encodeInt(NetworkUtils::MessageType::UpdateSetting, data);
            NetworkUtils::encodeFloat(_gameSettings->getSpawnRate(), data);
            NetworkUtils::encodeFloat(_gameSettings->getGravStrength(), data);
            NetworkUtils::encodeInt(_gameSettings->getColorCount(), data);
            NetworkUtils::encodeInt(_gameSettings->getPlanetStardustPerLayer(), data);
            NetworkUtils::encodeInt(_timestamp, data);
            _timestamp++;
            _conn->send(data);
            data.clear();
            CULog("SENT UPDATE SETTING MESSAGE> SPAWNRATE[%f], GRAVSTRENGTH[%f], COLORCOUNT[%i], STARDUSTPERLAYER[%i]",
                _gameSettings->getSpawnRate(), _gameSettings->getGravStrength(), _gameSettings->getColorCount(), _gameSettings->getPlanetStardustPerLayer());

            _gameState = GameState::NameSent;
            return;
        }
        case GameState::GameStarted:
        {
            if (playerId > 0) { // non-host
                NetworkUtils::encodeInt(NetworkUtils::MessageType::ReadyGame, data);
                NetworkUtils::encodeString(_playerName, data);
                NetworkUtils::encodeInt(playerId, data);
                NetworkUtils::encodeInt(_timestamp, data);
                _timestamp++;
                _conn->send(data);
                data.clear();
                CULog("SENT NONHOST READY SIGNAL> PLAYERNAME[%s], PLAYER[%i]", _playerName.c_str(), playerId);
                _gameState = GameState::NameSent;
                _playerMap[playerId] = std::make_pair(_playerName, true);
                return;
            }
            else if (playerId == 0) {
                NetworkUtils::encodeInt(NetworkUtils::MessageType::StartGame, data);
                NetworkUtils::encodeFloat(_gameSettings->getSpawnRate(), data);
                NetworkUtils::encodeFloat(_gameSettings->getGravStrength(), data);
                NetworkUtils::encodeInt(_gameSettings->getColorCount(), data);
                NetworkUtils::encodeInt(_gameSettings->getPlanetStardustPerLayer(), data);
                NetworkUtils::encodeInt(_timestamp, data);
                _timestamp++;
                _conn->send(data);
                data.clear();
                CULog("SENT START GAME MESSAGE> SPAWNRATE[%f], GRAVSTRENGTH[%f], COLORCOUNT[%i], STARDUSTPERLAYER[%i]",
                    _gameSettings->getSpawnRate(), _gameSettings->getGravStrength(), _gameSettings->getColorCount(), _gameSettings->getPlanetStardustPerLayer());
                _gameState = GameState::GameInProgress;
                _playerMap[playerId] = std::make_pair(_playerName, true);

                /* TODO: uncomment when startGame works correctly
                if (getPlayerId() == 0) {
                    _conn->startGame();
                }
                */
                return;
            }
        }
        default:
        {
            if (_gameUpdateManager == nullptr) {
                return;
            }

            std::shared_ptr<GameUpdate> gameUpdate = _gameUpdateManager->getGameUpdateToSend();
            if (gameUpdate == nullptr) {
                if (_framesSinceLastMessage[playerId] >= FRAMES_UNTIL_PING) {
                    NetworkUtils::encodeInt(NetworkUtils::MessageType::Ping, data);
                    NetworkUtils::encodeInt(playerId, data);
                    NetworkUtils::encodeInt(_timestamp, data);
                    _timestamp++;

                    _conn->send(data);
                    data.clear();
                    CULog("SENT Ping> SRC[%i], TS[%i]", playerId, _timestamp);
                    
                    _framesSinceLastMessage[playerId] = 0;
                }
                return;
            }
            
            _framesSinceLastMessage[getPlayerId()] = 0;

            for (auto const& [key, val] : gameUpdate->getStardustSent()) {
                int dstPlayerId = key;

                // send stardust sent
                for (size_t jj = 0; jj < val.size(); jj++) {
                    if (val[jj]->getStardustType() != StardustModel::Type::NORMAL) {
                        int powerup = val[jj]->getStardustType();
                        int stardustColor = val[jj]->getColor();

                        NetworkUtils::encodeInt(NetworkUtils::MessageType::PowerupApplied, data);
                        NetworkUtils::encodeInt(playerId, data);
                        NetworkUtils::encodeInt(powerup, data);
                        NetworkUtils::encodeInt(stardustColor, data);
                        NetworkUtils::encodeInt(_timestamp, data);
                        _timestamp++;

                        _conn->send(data);
                        data.clear();
                        CULog("SENT Powerup> SRC[%i], POWERUP[%i], CLR[%i], TS[%i]", playerId, powerup, stardustColor, _timestamp);
                    }
                    else if (val[jj]->getStardustLocation() == CILocation::Value::ON_SCREEN) {
                        NetworkUtils::encodeInt(NetworkUtils::MessageType::StardustHit, data);
                        NetworkUtils::encodeInt(playerId, data);
                        NetworkUtils::encodeInt(dstPlayerId, data);
                        NetworkUtils::encodeInt(_timestamp, data);
                        _timestamp++;

                        _conn->send(data);
                        data.clear();
                        CULog("SENT Stardust Hit> SRC[%i], DST[%i], TS[%i]", playerId, dstPlayerId, _timestamp);
                    }
                    else {
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
            }

            // send planet update
            int planetColor = gameUpdate->getPlanet()->getColor();
            float planetSize = gameUpdate->getPlanet()->getMass();

            NetworkUtils::encodeInt(NetworkUtils::MessageType::PlanetUpdate, data);
            NetworkUtils::encodeInt(playerId, data);
            NetworkUtils::encodeInt(planetColor, data);
            NetworkUtils::encodeFloat(planetSize, data);
            NetworkUtils::encodeInt(_timestamp, data);
            _timestamp++;

            _conn->send(data);
            data.clear();
            CULog("SENT PU> SRC[%i], CLR[%i], SIZE[%f]", playerId, planetColor, planetSize);

            if (gameUpdate->getPlanet()->isWinner()) {
                if (playerId == 0) {
                    // if we are the host and win first then we immediately send the won game message
                    _winnerPlayerId = playerId;

                    NetworkUtils::encodeInt(NetworkUtils::MessageType::WonGame, data);
                    NetworkUtils::encodeInt(playerId, data);
                    NetworkUtils::encodeInt(_timestamp, data);
                    _timestamp++;
                    _conn->send(data);
                    data.clear();
                    CULog("SENT WON GAME MESSAGE> PLAYER[%i]", playerId);
                }
                else {
                    NetworkUtils::encodeInt(NetworkUtils::MessageType::AttemptToWin, data);
                    NetworkUtils::encodeInt(playerId, data);
                    NetworkUtils::encodeInt(_timestamp, data);
                    _timestamp++;
                    _conn->send(data);
                    data.clear();
                    CULog("SENT ATTEMPT TO WIN MESSAGE> SRC[%i]", playerId);
                }
            }

            // clear game update to send now that we have sent update.
            _gameUpdateManager->clearGameUpdateToSend();
            break;
        }
    }
}

/**
 * Receives messages sent over the network and adds them to the queue in game update manager.
 */
void NetworkMessageManager::receiveMessages() {
    if (_conn == nullptr || _gameSettings == nullptr) {
        return;
    }

    // Check game room members
    if (getPlayerId() == 0) {
        std::vector<int> eraseId;

        for (const auto& p : _playerMap) {
            if (p.first > 0) {
                if (!_conn->isPlayerActive(p.first)) {
                    // remove id from map
                    eraseId.emplace_back(p.first);
                    // send disconnect signal 
                    std::vector<uint8_t> data;
                    NetworkUtils::encodeInt(NetworkUtils::MessageType::DisconnectGame, data);
                    NetworkUtils::encodeInt(p.first, data);
                    NetworkUtils::encodeInt(_timestamp, data);
                    _timestamp++;
                    _conn->send(data);
                    data.clear();
                    CULog("SENT DISCONNECT PLAYER MESSAGE> PLAYER[%i]", p.first);
                }
            }
        }
        for (int id : eraseId) {
            _playerMap.erase(id);
        }
    }
    
    _conn->receive([this](const std::vector<uint8_t>& recv) {
        if (recv.empty()) {
            return;
        }

        const int message_type = NetworkUtils::decodeInt(recv[0], recv[1], recv[2], recv[3]);

        if (_gameUpdateManager == nullptr && !isLobbyMessage(message_type)) {
            return;
        }

        switch (message_type)
        {
            case NetworkUtils::MessageType::Ping:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int timestamp = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);

                CULog("RCVD Ping> SRC[%i], TS[%i]", srcPlayer, timestamp);
                
                _framesSinceLastMessage[srcPlayer] = 0;
                break;
            }
            case NetworkUtils::MessageType::DisconnectGame:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int timestamp = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);

                CULog("RCVD PLAYER DISCONNECT> SRC[%i], TS[%i]", srcPlayer, timestamp);

                _playerMap.erase(srcPlayer);
                break;
            }
            case NetworkUtils::MessageType::StardustSent:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int dstPlayer = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);
                int stardustColor = NetworkUtils::decodeInt(recv[12], recv[13], recv[14], recv[15]);
                float xVel = NetworkUtils::decodeFloat(recv[16], recv[17], recv[18], recv[19]);
                float yVel = NetworkUtils::decodeFloat(recv[20], recv[21], recv[22], recv[23]);
                int timestamp = NetworkUtils::decodeInt(recv[24], recv[25], recv[26], recv[27]);

                CULog("RCVD SU> SRC[%i], DST[%i], CLR[%i], VEL[%f,%f]", srcPlayer, dstPlayer, stardustColor, xVel, yVel);
                _framesSinceLastMessage[srcPlayer] = 0;

                std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(0, 0), cugl::Vec2(xVel, yVel), static_cast<CIColor::Value>(stardustColor));
                std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = { { dstPlayer, std::vector<std::shared_ptr<StardustModel>> { stardust } } };
                std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, nullptr, timestamp);
                _gameUpdateManager->addGameUpdate(gameUpdate);
                break;
            }
            case NetworkUtils::MessageType::PlanetUpdate:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int planetColor = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);
                float planetSize = NetworkUtils::decodeFloat(recv[12], recv[13], recv[14], recv[15]);
                int timestamp = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);

                CULog("RCVD PU> SRC[%i], CLR[%i], SIZE[%f]", srcPlayer, planetColor, planetSize);
                _framesSinceLastMessage[srcPlayer] = 0;

                CILocation::Value corner = NetworkUtils::getLocation(getPlayerId(), srcPlayer);
                std::shared_ptr<OpponentPlanet> planet = OpponentPlanet::alloc(0, 0, CIColor::Value(planetColor), corner);
                planet->setMass(planetSize);
                std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = {};
                std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, planet, timestamp);
                _gameUpdateManager->addGameUpdate(gameUpdate);
                break;
            }
            case NetworkUtils::MessageType::AttemptToWin:
            {
                // only respond to attempt to win message if we are a host
                if (_conn->getPlayerID().value_or(-1) == 0) {
                    int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                    int timestamp = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);

                    CULog("RCVD Attempt To Win> SRC[%i], TS[%i]", srcPlayer, timestamp);
                    _framesSinceLastMessage[srcPlayer] = 0;

                    if (_winnerPlayerId == -1) {
                        _winnerPlayerId = srcPlayer;

                        std::vector<uint8_t> data;
                        NetworkUtils::encodeInt(NetworkUtils::MessageType::WonGame, data);
                        NetworkUtils::encodeInt(srcPlayer, data);
                        NetworkUtils::encodeInt(_timestamp, data);
                        _timestamp++;
                        _conn->send(data);
                        data.clear();
                        CULog("SENT WON GAME MESSAGE> PLAYER[%i]", srcPlayer);
                    }
                }
                break;
            }
            case NetworkUtils::MessageType::WonGame:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int timestamp = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);

                CULog("RCVD GAME WON> SRC[%i], TS[%i]", srcPlayer, timestamp);
                _framesSinceLastMessage[srcPlayer] = 0;

                if (_winnerPlayerId == -1) {
                    _winnerPlayerId = srcPlayer;
                }
                break;
            }
            case NetworkUtils::MessageType::ReadyGame:
            {
                string player_name = NetworkUtils::decodeString(recv[4], recv[5], recv[6], recv[7], recv[8], recv[9], recv[10], recv[11], recv[12], recv[13], recv[14], recv[15]);
                player_name.erase(player_name.begin(), std::find_if(player_name.begin(), player_name.end(), [](unsigned char ch) {
                        return ch != '\0';
                    }));
                player_name.erase(std::find_if(player_name.rbegin(), player_name.rend(), [](unsigned char ch) {
                    return ch != '\0';
                    }).base(), player_name.end());
                int srcPlayer = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);
                int timestamp = NetworkUtils::decodeInt(recv[20], recv[21], recv[22], recv[23]);
                CULog("RCVD NON-HOST READY MESSAGE> PLAYERNAME[%s], PLAYER[%i], TS[%i]", player_name.c_str(), srcPlayer, timestamp);

                _playerMap[srcPlayer] = std::make_pair(player_name, true);
                break;
            }
            case NetworkUtils::MessageType::StartGame:
            {
                float spawnRate = NetworkUtils::decodeFloat(recv[4], recv[5], recv[6], recv[7]);
                float gravStrength = NetworkUtils::decodeFloat(recv[8], recv[9], recv[10], recv[11]);
                int colorCount = NetworkUtils::decodeInt(recv[12], recv[13], recv[14], recv[15]);
                int layerSize = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);
                int timestamp = NetworkUtils::decodeInt(recv[20], recv[21], recv[22], recv[23]);

                CULog("RCVD START GAME MESSAGE> SPAWNRATE[%f], GRAVSTRENGTH[%f], COLORCOUNT[%i], PLANETMASS[%i], TS[%i]", spawnRate, gravStrength, colorCount, layerSize, timestamp);

                if (_gameSettings == nullptr) {
                    _gameSettings = GameSettings::alloc();
                }

                _gameSettings->setSpawnRate(spawnRate);
                _gameSettings->setGravStrength(gravStrength);
                _gameSettings->setColorCount(colorCount);
                _gameSettings->setPlanetStardustPerLayer(layerSize);

                _gameState = GameState::GameInProgress;
                break;
            }
            case NetworkUtils::MessageType::UpdateSetting:
            {
                float spawnRate = NetworkUtils::decodeFloat(recv[4], recv[5], recv[6], recv[7]);
                float gravStrength = NetworkUtils::decodeFloat(recv[8], recv[9], recv[10], recv[11]);
                int colorCount = NetworkUtils::decodeInt(recv[12], recv[13], recv[14], recv[15]);
                int layerSize = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);
                int timestamp = NetworkUtils::decodeInt(recv[20], recv[21], recv[22], recv[23]);

                CULog("RCVD UPDATE GAMESETTINGS MESSAGE> SPAWNRATE[%f], GRAVSTRENGTH[%f], COLORCOUNT[%i], PLANETMASS[%i], TS[%i]", spawnRate, gravStrength, colorCount, layerSize, timestamp);

                _gameSettings->setSpawnRate(spawnRate);
                _gameSettings->setGravStrength(gravStrength);
                _gameSettings->setColorCount(colorCount);
                _gameSettings->setPlanetStardustPerLayer(layerSize);
                break;
            }
            case NetworkUtils::MessageType::NameSent:
            {
                string player_name = NetworkUtils::decodeString(recv[4], recv[5], recv[6], recv[7], recv[8], recv[9], recv[10], recv[11], recv[12], recv[13], recv[14], recv[15]);
                player_name.erase(player_name.begin(), std::find_if(player_name.begin(), player_name.end(), [](unsigned char ch) {
                        return ch != '\0';
                    }));
                player_name.erase(std::find_if(player_name.rbegin(), player_name.rend(), [](unsigned char ch) {
                    return ch != '\0';
                    }).base(), player_name.end());
                int playerId = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);
                int timestamp = NetworkUtils::decodeInt(recv[20], recv[21], recv[22], recv[23]);

                CULog("RCVD PLAYERNAME> PLAYERNAME[%s], PLAYER[%i], TS[%i]", player_name.c_str(), playerId, timestamp);
                
                if (_playerMap.count(playerId) == 0)
                    _playerMap[playerId] = std::make_pair(player_name, (playerId == 0));

                std::vector<uint8_t> data;
                NetworkUtils::encodeInt(NetworkUtils::MessageType::NameReceivedResponse, data);
                NetworkUtils::encodeString(_playerName, data);
                NetworkUtils::encodeInt(getPlayerId(), data);
                
                int ready = _playerMap[getPlayerId()].second ? 1 : 0;
                NetworkUtils::encodeInt(ready, data);
                if (getPlayerId() == 0) {
                    NetworkUtils::encodeFloat(_gameSettings->getSpawnRate(), data);
                    NetworkUtils::encodeFloat(_gameSettings->getGravStrength(), data);
                    NetworkUtils::encodeInt(_gameSettings->getColorCount(), data);
                    NetworkUtils::encodeInt(_gameSettings->getPlanetStardustPerLayer(), data);
                }
                NetworkUtils::encodeInt(_timestamp, data);
                _timestamp++;
                _conn->send(data);
                data.clear();
                CULog("SENT PLAYER NAME MESSAGE> PLAYERNAME[%s], PLAYER[%i]", _playerName.c_str(), getPlayerId());
                break;
            }
            case NetworkUtils::MessageType::NameReceivedResponse:
            {
                string player_name = NetworkUtils::decodeString(recv[4], recv[5], recv[6], recv[7], recv[8], recv[9], recv[10], recv[11], recv[12], recv[13], recv[14], recv[15]);
                player_name.erase(player_name.begin(), std::find_if(player_name.begin(), player_name.end(), [](unsigned char ch) {
                        return ch != '\0';
                    }));
                player_name.erase(std::find_if(player_name.rbegin(), player_name.rend(), [](unsigned char ch) {
                    return ch != '\0';
                    }).base(), player_name.end());
                int playerId = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);
                bool ready = NetworkUtils::decodeInt(recv[20], recv[21], recv[22], recv[23]) == 1;
                
                if (_playerMap.count(playerId) == 0)
                    _playerMap[playerId] = std::make_pair(player_name, ready);

                if (playerId != 0) {
                    int timestamp = NetworkUtils::decodeInt(recv[24], recv[25], recv[26], recv[27]);

                    CULog("RCVD PLAYERNAME> PLAYERNAME[%s], PLAYER[%i], TS[%i]", player_name.c_str(), playerId, timestamp);
                }
                else {
                    float spawnRate = NetworkUtils::decodeFloat(recv[24], recv[25], recv[26], recv[27]);
                    float gravStrength = NetworkUtils::decodeFloat(recv[28], recv[29], recv[30], recv[31]);
                    int colorCount = NetworkUtils::decodeInt(recv[32], recv[33], recv[34], recv[35]);
                    int layerSize = NetworkUtils::decodeInt(recv[36], recv[37], recv[38], recv[39]);
                    int timestamp = NetworkUtils::decodeInt(recv[40], recv[41], recv[42], recv[43]);

                    CULog("RCVD RESPONSE PLAYERNAME> PLAYERNAME[%s], PLAYER[%i], TS[%i]", player_name.c_str(), playerId, timestamp);
                    CULog("RCVD UPDATE GAMESETTINGS MESSAGE> SPAWNRATE[%f], GRAVSTRENGTH[%f], COLORCOUNT[%i], PLANETMASS[%i], TS[%i]", spawnRate, gravStrength, colorCount, layerSize, timestamp);

                    _gameSettings->setSpawnRate(spawnRate);
                    _gameSettings->setGravStrength(gravStrength);
                    _gameSettings->setColorCount(colorCount);
                    _gameSettings->setPlanetStardustPerLayer(layerSize);
                }
                break;
            }
            case NetworkUtils::MessageType::StardustHit:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int dstPlayer = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);
                int timestamp = NetworkUtils::decodeInt(recv[12], recv[13], recv[14], recv[15]);

                CULog("RCVD Stardust Hit> SRC[%i], DST[%i], TS[%i]", srcPlayer, dstPlayer, timestamp);
                _framesSinceLastMessage[srcPlayer] = 0;

                if (dstPlayer == getPlayerId()) {
                    // put a grey stardust on the queue to indicate it is a reward stardust
                    std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(0, 0), cugl::Vec2(0, 0), CIColor::getNoneColor());
                    std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = { { dstPlayer, std::vector<std::shared_ptr<StardustModel>> { stardust } } };
                    std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, nullptr, timestamp);
                    _gameUpdateManager->addGameUpdate(gameUpdate);
                }
                break;
            }
            case NetworkUtils::MessageType::PowerupApplied:
            {
                int srcPlayer = NetworkUtils::decodeInt(recv[4], recv[5], recv[6], recv[7]);
                int powerup = NetworkUtils::decodeInt(recv[8], recv[9], recv[10], recv[11]);
                int stardustColor = NetworkUtils::decodeInt(recv[12], recv[13], recv[14], recv[15]);
                int timestamp = NetworkUtils::decodeInt(recv[16], recv[17], recv[18], recv[19]);

                CULog("RCVD Powerup Applied> SRC[%i], POWERUP[%i], CLR[%i], TS[%i]", srcPlayer, powerup, stardustColor, timestamp);
                _framesSinceLastMessage[srcPlayer] = 0;

                std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(0, 0), cugl::Vec2(0, 0), CIColor::Value(stardustColor));
                stardust->setStardustType(StardustModel::Type(powerup));
                stardust->setPreviousOwner(srcPlayer);
                std::map<int, std::vector<std::shared_ptr<StardustModel>>> map = { { getPlayerId(), std::vector<std::shared_ptr<StardustModel>> { stardust } } };
                std::shared_ptr<GameUpdate> gameUpdate = GameUpdate::alloc(_conn->getRoomID(), srcPlayer, map, nullptr, timestamp);
                _gameUpdateManager->addGameUpdate(gameUpdate);
                break;
            }
            default:
            {
                CULog("WRONG MESSAGE TYPE");
                break;
            }
        }
        });
    
    if (_gameState == GameState::GameInProgress) {
        int minFrame = FRAMES_UNTIL_TIMEOUT;
        for (auto ii = 0; ii < _framesSinceLastMessage.size(); ii++) {
            _framesSinceLastMessage[ii]++;
            if (ii != getPlayerId())
                minFrame = min(minFrame, _framesSinceLastMessage[ii]);
            
            if (_framesSinceLastMessage[ii] == FRAMES_UNTIL_TIMEOUT) {
                CULog("Player %i has disconnected.", ii);
                if (ii == 0) {
                    _winnerPlayerId = -2;
                }
            }
        }
        
        if (minFrame == FRAMES_UNTIL_TIMEOUT) {
            CULog("Player Has Disconnected from the game.");
        }
    }
}

/**
 * Creates a game instance with this player as the host.
 */
void NetworkMessageManager::createGame() {
    _conn = std::make_shared<cugl::CUNetworkConnection>(NetworkUtils::getConnectionConfig());
    _gameState = GameState::JoiningGameAsHost;
    _playerMap.clear();
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
    _playerMap.clear();
    CULog("CONNECTING AS NON HOST");
}
