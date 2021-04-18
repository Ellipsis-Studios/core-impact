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

#include "CIMenuState.h"
#include "CIMainMenu.h"
#include "CISettingsMenu.h"
#include "CIJoinMenu.h"
#include "CILobbyMenu.h"
#include "CITutorialMenu.h"
#include "CIGameSettings.h"
#include "CIPlayerSettings.h"
#include "CIGameConstants.h"


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
    std::shared_ptr<PlayerSettings> _playerSettings;

    // Game settings
    std::shared_ptr<GameSettings> _gameSettings;

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
    MenuScene() : cugl::Scene2() {}

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
     * @param gameSettings      The settings for the current game
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<PlayerSettings>& playerSettings,
        const std::shared_ptr<GameSettings>& gameSettings);

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
    const string getJoinGameId() const {
        if (_gameSettings == nullptr) {
            return constants::gamesettings::DEFAULT_GAME_ID;
        }
        return _gameSettings->getGameId();
    }

    /**
     * Returns the player name set in settings
     *
     * @return string player name set in settings
     */
    const string getPlayerName() const {
        if (_playerSettings == nullptr) {
            return constants::playersettings::DEFAULT_PLAYER_NAME;
        }
        return _playerSettings->getPlayerName();
    }

    /**
     * Returns the volume set in settings
     *
     * @return float volume value set in settings
     */
    const float getVolume() const {
        if (_playerSettings == nullptr) {
            return constants::playersettings::DEFAULT_VOLUME;
        }
        return _playerSettings->getVolume();
    }

    /**
     * Returns the music toggle value set in settings.
     * Whether music is on/off.
     *
     * @return bool music toggle value set in settings
     */
    const bool isMusicOn() const {
        if (_playerSettings == nullptr) {
            return constants::playersettings::DEFAULT_MUSIC_ON;
        }
        return _playerSettings->getMusicOn();
    }

    /**
     * Returns the parallax toggle value set in settings.
     * Whether parallax effect is on/off.
     *
     * @return bool parallax toggle value set in settings
     */
    const bool isParallaxOn() const {
        if (_playerSettings == nullptr) {
            return constants::playersettings::DEFAULT_PARALLAX_ON;
        }
        return _playerSettings->getParallaxOn();
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
        playerSettings->appendValue("PlayerName", getPlayerName());
        playerSettings->appendValue("Volume", getVolume());
        playerSettings->appendValue("MusicOn", isMusicOn());
        playerSettings->appendValue("ParallaxOn", isParallaxOn());
    }

    /**
     * Returns the list of other player names in the game lobby.
     *
     * @return vector<string> list of other player names
     */
    const vector<string>& getOtherPlayerNames() const {
        return _otherNames;
    }

    /**
     * Returns the spawn rate for new stardusts set in game lobby.
     *
     * @return float stardust spawn rate (multiplicative)
     */
    const float getSpawnRate() const {
        if (_gameSettings == nullptr) {
            return constants::gamesettings::DEFAULT_SPAWN_RATE;
        }
        return _gameSettings->getSpawnRate();
    }

    /**
     * Returns the strength of planet's gravity set in game lobby.
     *
     * @return float stardust spawn rate (multiplicative)
     */
    const float getGravStrength() const {
        if (_gameSettings == nullptr) {
            return constants::gamesettings::DEFAULT_GRAV_STRENGTH;
        }
        return _gameSettings->getGravStrength();
    }

    /**
     * Returns the number of stardust colors set in game lobby.
     *
     * @return uint8_t number of stardust colors supported
     */
    const uint8_t getColorCount() const {
        if (_gameSettings == nullptr) {
            return constants::gamesettings::DEFAULT_COLOR_COUNT;
        }
        return _gameSettings->getColorCount();
    }

    /**
     * Returns the planet mass required to win set in game lobby.
     *
     * @return uint16_t planet mass required to win
     */
    const uint16_t getPlanetMassToWinGame() const {
        if (_gameSettings == nullptr) {
            return constants::gamesettings::DEFAULT_WIN_MASS;
        }
        return _gameSettings->getPlanetMassToWin();
    }

};

#endif /* __CI_MENU_SCENE_H__ */
