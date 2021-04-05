//
//  CIMenuStatus.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_MENU_STATUS_H__
#define __CI_MENU_STATUS_H__

/**
 * Enum class for the current state of the menu scene.
 *
 * MainMenu               - starting point. User starts here once Loading Scene completed.
 *                        - 4 buttons (Join Game, New Game, Tutorial + Settings)
 *                        - Can transition to MainToSettings, MainToJoin, MainToLobby + MainToTutorial
 * MainToSetting          - When user clicks on Settings button in MainMenu. Transtion to Settings
 * MainToJoin             - When user clicks on Join Game button in MainMenu. Transition to JoinRoom
 * MainToLobby            - When user clicks on New Game button in MainMenu. Transition to GameLobby
 * MainToTutorial         - When user clicks on How To Play button in MainMenu. Transition to Tutorial.
 * Settings               - Displaying settings page. User can return to MainMenu with Back button.
 *                        - 5 inputs (Name textfield, Music + Parallax Toggles, Volume Slider, Back button)
 * JoinRoom               - Displaying join game page. User can input room id and join a game or return to MainMenu.
 *                        - 3 inputs (Room Id textfield, Join button, Back button)
 * GameLobby              - Displaying game lobby page. User can start the game or return to MainMenu.
 *                        - 2 buttons (Start button, Back button)
 * Tutorial               - Displaying tutorial page. User can return to MainMenu.
 *                        - 1 button (Back button)
 * SettingToMain          - When user presses back button to return to main from settings.
 * JoinToMain             - When user presses back button from join game.
 * LobbyToMain            - When user presses back button from game lobby.
 * TutorialToMain **        - When user presses back button from tutorial.
 * LobbyToGame            - Ends menu scene and Triggers gameplay to begin.
 */
enum class MenuStatus {
    MainMenu, // main menu with 3 menu buttons + setting button added
    MainToSetting, // transition to setting
    MainToJoin, // transtition to join room
    MainToLobby, // transition to lobby (create new game)
    MainToTutorial, // transition to tutorial
    Setting, // setting page
    JoinRoom, // join room page (with room id input)
    JoinToLobby, // transition to lobby
    GameLobby, // game lobby
    Tutorial, // tutorial page
    SettingToMain, // go back to main menu from setting
    JoinToMain, // go back to main menu from join room
    LobbyToMain, // go back to main menu from lobby
    TutorialToMain, // go back to main menu from tutorial
    LobbyToGame // transition to a new game (set loading scene inactive)
};


#endif /* __CI_MENU_STATUS_H__ */
