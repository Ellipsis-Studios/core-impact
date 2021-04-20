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

    // Player settings (preserved over game reset)
    /** Value for the player name */
    string _playerName;
    /** Value for the game audio volume */
    float _volume;
    /** Whether game music is turned on/off */
    bool _musicOn;
    /** Whether game parallax effect is turned on/off */
    bool _parallaxOn;

    /** Stores the game code for joining as client*/
    string _joinGame;

    // Game lobby settings
    /** Value for the rate of stardust spawning */
    float _spawnRate;
    /** Value for the strength of planet's gravity */
    float _gravStrength;
    /** Value for the number of stardust colors available */
    uint8_t _colorCount;
    /** Criteria for the planet to win the game (min planet mass to win) */
    // TODO: switch to the planet's layers instead of mass
    uint16_t _winCondition;

    // Menu scene state value
    MenuState _state;

    /** The network message manager for managing connections to other players */
    std::shared_ptr<NetworkMessageManager> _networkMessageManager;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new menu mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::Scene2(), _volume(0.5f), _musicOn(true), _parallaxOn(true),
        _spawnRate(1.0f), _gravStrength(1.0f), _colorCount(4), _winCondition(200) {}

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
     * @param assets            The (loaded) assets for this game mode
     * @param playerSettings    The player's saved settings value
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<cugl::JsonValue>& playerSettings, std::shared_ptr<NetworkMessageManager> networkMessageManager);

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
        string playerName, float volume, bool musicOn, bool parallaxOn, std::shared_ptr<NetworkMessageManager> networkMessageManager);

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     * @param networkMessageManager  The network message manager for managing connections to other players
     */
    void update(float timestep);

    /**
     * Returns the room id value of game to join.
     *
     * @return string room id value of game to join.
     */
    const string getJoinGameId() const {
        return _joinGame;
    }

    /**
     * Returns the player name set in settings
     *
     * @return string player name set in settings
     */
    const string getPlayerName() const {
        return _playerName;
    }

    /**
     * Returns the volume set in settings
     *
     * @return float volume value set in settings
     */
    const float getVolume() const {
        return _volume;
    }

    /**
     * Returns the music toggle value set in settings.
     * Whether music is on/off.
     *
     * @return bool music toggle value set in settings
     */
    const bool isMusicOn() const {
        return _musicOn;
    }

    /**
     * Returns the parallax toggle value set in settings.
     * Whether parallax effect is on/off.
     *
     * @return bool parallax toggle value set in settings
     */
    const bool isParallaxOn() const {
        return _parallaxOn;
    }

    /**
     * Returns the player settings value. The player settings value contains
     * 4 fields (PlayerName, Volume, MusicOn, ParallaxOn).
     *
     * The method is used to retrieve and save the player settings.
     *
     * @param playerSettings reference to the json value for player settings from App
     */
    void appendPlayerSettings(std::shared_ptr<cugl::JsonValue>& playerSettings) {
        playerSettings->appendValue("PlayerName", _playerName);
        playerSettings->appendValue("Volume", _volume);
        playerSettings->appendValue("MusicOn", _musicOn);
        playerSettings->appendValue("ParallaxOn", _parallaxOn);
    }

    /**
     * Returns the spawn rate for new stardusts set in game lobby.
     *
     * @return float stardust spawn rate (multiplicative)
     */
    const float getSpawnRate() const {
        return _spawnRate;
    }

    /**
     * Returns the strength of planet's gravity set in game lobby.
     *
     * @return float stardust spawn rate (multiplicative)
     */
    const float getGravStrength() const {
        return _gravStrength;
    }

    /**
     * Returns the number of stardust colors set in game lobby.
     *
     * @return uint8_t number of stardust colors supported
     */
    const uint8_t getColorCount() const {
        return _colorCount;
    }

    /**
     * Returns the game win planet condition value set in game lobby.
     *
     * @return uint16_t win planet condition value (mass/num of layeres)
     */
    const uint16_t getGameWinCondition() const {
        return _winCondition;
    }

};

#endif /* __CI_MENU_SCENE_H__ */
