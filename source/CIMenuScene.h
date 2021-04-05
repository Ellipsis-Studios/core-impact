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
#include "CIMenuStatus.h"
#include "CITutorialMenu.h"
#include "CISettingsMenu.h"
#include "CIJoinMenu.h"


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
    std::shared_ptr<TutorialMenu> _tutorial;
    
    // SETTINGS
    std::shared_ptr<SettingsMenu> _settings;
    
    // JOIN GAME
    std::shared_ptr<JoinMenu> _join;
    
    // MODEL
    /** Whether menu has been initialized previously */
    bool _isLoaded;

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
    MenuScene() : cugl::Scene2(), _isLoaded(false), _status(MenuStatus::MainMenu) {}

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
        if (_join == nullptr) {
            return "";
        }
        return _join->getRoomId();
    }
};

#endif /* __CI_MENU_SCENE_H__ */
