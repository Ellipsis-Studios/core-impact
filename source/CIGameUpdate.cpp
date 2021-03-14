//
//  CIGameUpdate.cpp
//  CoreImpact
//
//  This class is used to store information associated with game updates between players.
//
//  Created by William Long on 3/13/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIGameUpdate.h"

void GameUpdate::dispose() {
    _game_id.clear();
    _stardust_sent.clear();
    _planet = nullptr;
}

bool GameUpdate::init(std::string gameId, int playerId, std::map<int, std::vector<StardustModel>> stardustSent, const std::shared_ptr<PlanetModel> planet, int timestamp) {
    _game_id = gameId;
    _player_id = playerId;
    _stardust_sent = stardustSent;
    _planet = planet;
    _timestamp = timestamp;
    return true;
}


