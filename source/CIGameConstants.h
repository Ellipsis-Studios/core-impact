//
//  CIGameConstants.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/17/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_GAME_CONSTANTS_H__ 
#define __CI_GAME_CONSTANTS_H__

#include <array>
//#include <string>


/** 
 * Namespace of constants used in the game.
 */
namespace constants {
#pragma mark Scenes 
    /** Scene width for all the scenes */
    inline constexpr int SCENE_WIDTH = 1024;
    /** Scene height for all the scenes */
    inline constexpr int SCENE_HEIGHT = 576;

    // Player Settings 
    namespace playersettings {
        /** Default values for Player Setting */
        inline constexpr auto  DEFAULT_PLAYER_NAME = "Player Name";
        inline constexpr float DEFAULT_VOLUME = 0.5f;
        inline constexpr bool  DEFAULT_MUSIC_ON = true;
        inline constexpr bool  DEFAULT_PARALLAX_ON = true;
    }

    // Game Settings
    namespace gamesettings {
        /** Default values for Game Setting */
        inline constexpr auto  DEFAULT_GAME_ID = "";
        inline constexpr float DEFAULT_SPAWN_RATE = 1.0f;
        inline constexpr float DEFAULT_GRAV_STRENGTH = 1.0f;
        inline constexpr int   DEFAULT_COLOR_COUNT = 4;
        inline constexpr int   DEFAULT_WIN_MASS = 200;
    }

    // GameScene 
    namespace gamescene {
        /** Max values for initializing stardusts and planet layers */
        inline constexpr int MAX_STARDUSTS = 512;
        inline constexpr int MAX_PLANET_LAYERS = 3;

        /** Base values for the game scene */
        inline constexpr int   BASE_SPAWN_RATE = 40; 
        inline constexpr float BASE_PARALLAX_AMT = 0.1f;
    }

}

#endif /* __CI_GAME_CONSTANTS_H__ */