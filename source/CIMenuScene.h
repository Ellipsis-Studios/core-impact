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

#include  "CIMenuState.h"
#include "CIMainMenu.h"
#include "CISettingsMenu.h"
#include "CIJoinMenu.h"
#include "CILobbyMenu.h"
#include "CITutorialMenu.h"

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

    // VIEW 
    /** The team logo */
    std::shared_ptr<cugl::scene2::SceneNode> _teamLogo;
    /** Background game title */
    std::shared_ptr<cugl::scene2::SceneNode> _gameTitle;
    /** Background game planet */
    std::shared_ptr<cugl::scene2::SceneNode> _gamePlanet;

    /** Back button to return to main menu */
    std::shared_ptr<cugl::scene2::Button> _backBtn;

    // References to menu screens 
    std::shared_ptr<MainMenu> _mainmenu;
    std::shared_ptr<SettingsMenu> _settings;
    std::shared_ptr<JoinMenu> _join;
    std::shared_ptr<LobbyMenu> _lobby;
    std::shared_ptr<TutorialMenu> _tutorial;

    // Preserved over game reset
    /** Value for the player name */
    string _playerName;
    /** Value for the game audio volume */
    float _volume;
    /** Whether game music is turned on/off */
    bool _musicOn;
    /** Whether game parallax effect is turned on/off */
    bool _parallaxOn;

    // Preserved until reset
    /** Stores the game code for joining as client*/
    string _joinGame;
    /** Value for other players' names */
    vector<string> _otherNames;

    // Menu scene state value
    MenuState _state;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new menu mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::Scene2(), _volume(0.0f), _musicOn(true), _parallaxOn(true) {}

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

    /**
     * Initializes the controller contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets        The (loaded) assets for this game mode
     * @param playerName    The player name value
     * @param volume        The game volume setting value
     * @param musicOn       The musicOn setting value
     * @param parallaxOn    The parallax effect setting value
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        string playerName, float volume, bool musicOn, bool parallaxOn);

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
     * Returns the room id value of game to join.
     *
     * @return string room id value of game to join.
     */
    string getJoinGameId() const {
        return _joinGame;
    }

    /**
     * Returns the player name set in settings
     *
     * @return string player name set in settings
     */
    string getPlayerName() const {
        return _playerName;
    }

    /**
     * Returns the volume set in settings
     *
     * @return float volume value set in settings
     */
    float getVolume() const {
        return _volume;
    }

    /**
     * Returns the music toggle value set in settings.
     * Whether music is on/off.
     *
     * @return bool music toggle value set in settings
     */
    bool isMusicOn() const {
        return _musicOn;
    }

    /**
     * Returns the parallax toggle value set in settings.
     * Whether parallax effect is on/off.
     *
     * @return bool parallax toggle value set in settings
     */
    bool isParallaxOn() const {
        return _parallaxOn;
    }
};

#endif /* __CI_MENU_SCENE_H__ */
