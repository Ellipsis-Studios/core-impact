//
//  CIMenuScene.h
//  CoreImpact
//
//  Created by Richard Yoon on 3/27/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_MENU_SCENE_H__
#define __CI_MENU_SCENE_H__
#include <cugl/cugl.h>

/**
 * This class is the menu screens before the actual game play. 
 * 
 * There are 5 total menu scenes (Main Menu, Settings, Join Game, Game Lobby, and Tutorial).
 * The MenuStatus value shows which scene to display. In addition to the 5 pages, MenuStatus 
 * also handles transition from/to each of the pages. Page transitions work by clearing out 
 * the previous screen's assets before activating and displaying the new screen's assets. 
 * 
 * Many of the scenes possess inputs to configure the gameplay setting by the user. Game Lobby 
 * is always the last scene before launching gameplay. Clicking the start game button in Game
 * Lobby will inform the application root to switch to the gameplay mode. 
 */
class MenuScene : public cugl::Scene2 {
protected:
    /** The asset manager for menu screens. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // NO CONTROLLER 

    // VIEW 
    /** The team logo */
    std::shared_ptr<cugl::scene2::SceneNode> _teamLogo;
    /** Background game title */
    std::shared_ptr<cugl::scene2::SceneNode> _gameTitle;
    /** Background game planet */
    std::shared_ptr<cugl::scene2::SceneNode> _gamePlanet;

    // MAIN MENU 
    /** Button to switch to Settings screen */
    std::shared_ptr<cugl::scene2::Button> _settingsBtn;
    /** Button to switch to Join Game screen */
    std::shared_ptr<cugl::scene2::Button> _joinBtn;
    /** Button to create a new game/room as host (Game Lobby) */
    std::shared_ptr<cugl::scene2::Button> _newBtn;
    /** Button to switch to Tutorial screen */
    std::shared_ptr<cugl::scene2::Button> _tutorialBtn;

    /** Back button to return to main menu */
    std::shared_ptr<cugl::scene2::Button> _backBtn;

    // SETTINGS         
    /** Settings screen title */
    std::shared_ptr<cugl::scene2::Label> _settingsTitle;
    /** Label for player name input */
    std::shared_ptr<cugl::scene2::Label> _pnameLabel;
    /** Label for music toggle button */
    std::shared_ptr<cugl::scene2::Label> _musicLabel;
    /** Label for volume slider */
    std::shared_ptr<cugl::scene2::Label> _volumeLabel;
    /** Label for parallax toggle button */
    std::shared_ptr<cugl::scene2::Label> _parallaxLabel;

    /** Player Name input */
    std::shared_ptr<cugl::scene2::TextField> _pnameInput;
    /** Button to toggle music */
    std::shared_ptr<cugl::scene2::Button> _musicBtn;
    /** Slider for gameplay volume */
    std::shared_ptr<cugl::scene2::Slider> _volumeSlider;
    /** Button to toggle parallax effect */
    std::shared_ptr<cugl::scene2::Button> _parallaxBtn;

    // JOIN GAME  
    /** Game room id input */
    std::shared_ptr<cugl::scene2::TextField> _roomIdInput;
    /** Button to join game with specified id */
    std::shared_ptr<cugl::scene2::Button> _roomJoinBtn;

    // GAME LOBBY
    /** Label for Room Id in Game Lobby */
    std::shared_ptr<cugl::scene2::Label> _lobbyRoomLabel;
    /** Each label for a user in the game */
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel1; // top
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel2; // middle left
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel3; // middle right
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel4; // bottom left
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel5; // bottom right
    /** Button to trigger start of a game with the current room */
    std::shared_ptr<cugl::scene2::Button> _gameStartBtn;

    // TUTORIAL 
    /** Label for Tutorial page title */
    std::shared_ptr<cugl::scene2::Label> _tutorialTitle;

    // MODEL
    /** Value for the player name */
    string _playerName;
    /** Value for the game audio volume */
    float _volume;
    /** Whether game music is turned on/off */
    bool _musicOn;
    /** Whether game parallax effect is turned on/off */
    bool _parallaxOn;
    /** Whether menu has been initialized previously */
    bool _isLoaded;
    /** Stores the game code for joining as client*/
    string _joinGame;

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
     * TutorialToMain         - When user presses back button from tutorial.
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

    MenuStatus _status;

private:
#pragma mark -
#pragma mark Helpers

    /**
     * Helper to set up/take down menu sub-scenes assets.
     *
     * @param value           Whether to set up/take down
     * @param t, ...args        List of shared pointer to input assets
     */
    template <typename T>
    const void _menuSceneInputHelper(bool value, const std::shared_ptr<T>& t) {
        t->setVisible(value);
        if (value) {
            t->activate();
        }
        else {
            t->deactivate();
        }
    }

    template<typename T, typename... Args>
    const void _menuSceneInputHelper(bool value, const std::shared_ptr<T>& t, Args... args) {
        _menuSceneInputHelper(value, t);
        _menuSceneInputHelper(value, args...);
    }

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new menu mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::Scene2(), _volume(0.0f), _musicOn(true), _parallaxOn(true), _isLoaded(false), _status(MenuStatus::MainMenu) {}

    /**
    * Disposes of all (non-static) resources allocated to this mode.
    *
    * This method is different from dispose() in that it ALSO shuts off any
    * static resources, like the input controller.
    */
    ~MenuScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes the controller contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @p6aram timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

    /**
     * Returns true if loading is complete, but game has not launched yet
     *
     * @return true if loading is complete, but game has not launched yet
     */
    bool isPending() const;

    /**
     * Returns the room id value of game to join.
     *
     * @return string room id value of game to join.
     */
    string getJoinGameId() {
        return _joinGame;
    }
};

#endif /* __CI_MENU_SCENE_H__ */
