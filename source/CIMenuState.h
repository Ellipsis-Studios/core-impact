//
//  CIMenuState.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_MENU_STATE_H__
#define __CI_MENU_STATE_H__

/**
 * Enum class for the current state of the menu scene.
 */
enum class MenuState {
    /** Display menu scene */
    MainMenu,
    Setting,
    NameMenu,
    JoinRoom,
    GameLobby,
    GameSetting,

    CreatingGame,
    JoiningGame,

    /** Transition between menu scenes */
    LoadToMain,    // Starts menu scene. After initial loading or reset.
    MainToSetting,
    MainToTutorial,
    MainToJoin,
    MainToName,
    NameToJoin,
    NameToMain,
    MenuToCreate,
    MenuToJoin,

    MainToLobby,   // Player = HOST
    SettingToMain,
    JoinToMain,
    JoinToLobby,   // Player = CLIENT
    LobbyToMain,
    LobbyToGameSetting,
    GameSettingToLobby,
    LobbyToGame    // Ends menu scene. Triggers gameplay to begin. 
};

#endif /* __CI_MENU_STATE_H__ */
