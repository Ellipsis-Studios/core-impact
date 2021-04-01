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

class MenuScene : public cugl::Scene2 {
protected:
    std::shared_ptr<cugl::AssetManager> _assets;

    /** Team logo */
    std::shared_ptr<cugl::scene2::SceneNode> _teamlogo;
    /** Game Title */
    std::shared_ptr<cugl::scene2::SceneNode> _gameTitle;
    /** Game Planet */
    std::shared_ptr<cugl::scene2::SceneNode> _gamePlanet;

    /** Main Menu */
    std::shared_ptr<cugl::scene2::Button> _settingsbutton;
    std::shared_ptr<cugl::scene2::Button> _joinmenubutton;
    std::shared_ptr<cugl::scene2::Button> _newmenubutton;
    std::shared_ptr<cugl::scene2::Button> _tutorialmenubutton;

    /** Settings */
    std::shared_ptr<cugl::scene2::Button> _settingsbackbutton;
    std::shared_ptr<cugl::scene2::Label> _settingstitle;
    std::shared_ptr<cugl::scene2::Label> _settingsnamelabel;
    std::shared_ptr<cugl::scene2::TextField> _settingsnameinput;
    std::shared_ptr<cugl::scene2::Label> _settingsmusiclabel;
    std::shared_ptr<cugl::scene2::Button> _settingsmusicbutton;
    std::shared_ptr<cugl::scene2::Label> _settingsvolumelabel;
    std::shared_ptr<cugl::scene2::Slider> _settingsvolumeslider;
    std::shared_ptr<cugl::scene2::Label> _settingsparallaxlabel;
    std::shared_ptr<cugl::scene2::Button> _settingsparallaxbutton;

    /** Join Game */
    std::shared_ptr<cugl::scene2::Button> _joingamebackbutton;
    std::shared_ptr<cugl::scene2::TextField> _joingametextinput;
    std::shared_ptr<cugl::scene2::Button> _joingamejoinbutton;

    /** Game Lobby */
    std::shared_ptr<cugl::scene2::Button> _gamelobbybackbutton;
    std::shared_ptr<cugl::scene2::Label> _gamelobbyroomidlabel;
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel1;
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel2;
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel3;
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel4;
    std::shared_ptr<cugl::scene2::Label> _gamelobbyplayerlabel5;
    std::shared_ptr<cugl::scene2::Button> _gamelobbystartbutton;

    /** Tutorial */
    std::shared_ptr<cugl::scene2::Button> _tutorialbackbutton;
    std::shared_ptr<cugl::scene2::Label> _tutorialsceneheader;

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
        SettingToMain, // go back to main menu from setting
        JoinToMain, // go back to main menu from join room
        LobbyToMain, // go back to main menu from lobby
        TutorialToMain, // go back to main menu from tutorial
        Tutorial, // tutorial page
        LobbyToGame // transition to a new game (set loading scene inactive)
    };

    MenuStatus _status;

    /** Stores the game code for joining as client*/
    string _joinGame;

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
     * @param timestep  The amount of time (in seconds) since the last frame
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
