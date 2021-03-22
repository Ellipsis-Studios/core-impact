//
//  CIGameState.h
//  CoreImpact
//
//  Created by William Long on 3/20/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_GAME_STATE_H__
#define __CI_GAME_STATE_H__

/**
 * The state the game is currently in.
 */
enum GameState {
    OnMenuScreen,
    JoiningGameAsHost,
    JoiningGameAsNonHost,
    GameStarted,
    DisconnectedFromGame,
    ReconnectingToGame,
    GameOver
};

#endif /* __CI_GAME_STATE_H__ */
