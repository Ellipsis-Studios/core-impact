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
#include "CINameMenu.h"
#include "CIJoinMenu.h"
#include "CILobbyMenu.h"
#include "CIPopupMenu.h"
#include "CIGameSettingsMenu.h"
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
    std::shared_ptr<NameMenu> _namemenu;
    std::shared_ptr<JoinMenu> _join;
    std::shared_ptr<LobbyMenu> _lobby;
    std::shared_ptr<GameSettingsMenu> _gsettingsmenu;
    std::shared_ptr<PopupMenu> _popupMenu;

    // Player settings (preserved over game reset)
    std::shared_ptr<PlayerSettings> _playerSettings;

    // Game settings
    std::shared_ptr<GameSettings> _gameSettings;

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
     * @param assets                The (loaded) assets for this game mode
     * @param networkMessageManager The network message manager for the game
     * @param gameSettings          The settings for the current game
     * @param playerSettings        The player's saved settings value
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
        const std::shared_ptr<GameSettings>& gameSettings,
        const std::shared_ptr<PlayerSettings>& playerSettings);

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
     * The method called to get the state of the menu.
     */
    MenuState getState() const {
        return _state;
    }

    /**
     * Returns the room id value of game to join.
     *
     * @return string room id value of game to join.
     */
    const string getJoinGameId() const {
        return (_gameSettings == nullptr) ? CONSTANTS::DEFAULT_GAME_ID : _gameSettings->getGameId();
    }

    /**
     * Returns the player name set in settings
     *
     * @return string player name set in settings
     */
    const string getPlayerName() const {
        return (_playerSettings == nullptr) ? CONSTANTS::DEFAULT_PLAYER_NAME : _playerSettings->getPlayerName();
    }

    /**
     * Returns the volume set in settings
     *
     * @return float volume value set in settings
     */
    const float getVolume() const {
        return (_playerSettings == nullptr) ? CONSTANTS::DEFAULT_VOLUME : _playerSettings->getVolume();
    }

    /**
     * Returns the music toggle value set in settings.
     * Whether music is on/off.
     *
     * @return bool music toggle value set in settings
     */
    const bool isMusicOn() const {
        return (_playerSettings == nullptr) ? CONSTANTS::DEFAULT_MUSIC_ON : _playerSettings->getMusicOn();
    }

    /**
     * Returns the player settings value. The player settings value contains
     * 3 fields (PlayerName, Volume, MusicOn).
     *
     * The method is used to retrieve and save the player settings.
     *
     * @param playerSettings reference to the json value for player settings from App
     */
    void appendPlayerSettings(std::shared_ptr<cugl::JsonValue>& playerSettings) const {
        playerSettings->appendValue("PlayerName", getPlayerName());
        playerSettings->appendValue("Volume", getVolume());
        playerSettings->appendValue("MusicOn", isMusicOn());
    }

    /**
     * Returns the spawn rate for new stardusts set in game lobby.
     *
     * @return float stardust spawn rate (multiplicative)
     */
    const float getSpawnRate() const {
        return (_gameSettings == nullptr) ? CONSTANTS::DEFAULT_SPAWN_RATE : _gameSettings->getSpawnRate();
    }

    /**
     * Returns the strength of planet's gravity set in game lobby.
     *
     * @return float stardust spawn rate (multiplicative)
     */
    const float getGravStrength() const {
        return (_gameSettings == nullptr) ? CONSTANTS::DEFAULT_GRAV_STRENGTH : _gameSettings->getGravStrength();
    }

    /**
     * Returns the number of stardust colors set in game lobby.
     *
     * @return uint8_t number of stardust colors supported
     */
    const uint8_t getColorCount() const {
        return (_gameSettings == nullptr) ? CONSTANTS::DEFAULT_COLOR_COUNT : _gameSettings->getColorCount();
    }

    /**
     * Returns the game win planet condition value set in game lobby.
     *
     * @return uint16_t win planet condition value (mass/num of layeres)
     */
    const uint16_t getPlanetLayerSize() const {
        return (_gameSettings == nullptr) ? CONSTANTS::DEFAULT_LAYER_SIZE : _gameSettings->getPlanetStardustPerLayer();
    }

};

#endif /* __CI_MENU_SCENE_H__ */
