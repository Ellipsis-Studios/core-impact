//
//  CIMainMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_MAIN_MENU_H__
#define __CI_MAIN_MENU_H__
#include <cugl/cugl.h>

#include  "CIMenuState.h"

/**
 * This class is the Main Menu screen within the Menu Scenes.
 *
 * The scene contains buttons to trigger switch to other menu scenes (Settings,
 * Join Game, Game Lobby, Tutorial). Back button is not included in this
 * class (in parent MenuScene class).
 *
 * The screen is set-up and put on-display on states transitioning into MainMenu
 * state. Similarly, screen is taken-down and hidden from display on states
 * transitioning out of MainMenu state.
 */
class MainMenu {
private:
    /** Menu state for the upcoming frame */
    MenuState _nextState;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the settings button in main menu */
    std::shared_ptr<cugl::scene2::Button> _settingsBtn;
    /** Reference to the join button in main menu */
    std::shared_ptr<cugl::scene2::Button> _joinBtn;
    /** Reference to the new button in main menu */
    std::shared_ptr<cugl::scene2::Button> _newBtn;
    /** Reference to the tutorial button in main menu */
    std::shared_ptr<cugl::scene2::Button> _tutorialBtn;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new main menu with default values.
     */
    MainMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    ~MainMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    void dispose();

    /**
     * Initializes a new main menu with the state pointer.
     *
     * @param assets    The (loaded) assets for this main menu
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);


    /**
     * Returns a newly allocated Main Menu.
     *
     * @param assets    The (loaded) assets for this main menu
     *
     * @return a newly allocated main menu
     */
    static std::shared_ptr<MainMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<MainMenu> result = std::make_shared<MainMenu>();
        return (result->init(assets) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the main menu is currently active and visible.
     *
     * @param onDisplay     Whether the main menu is currently active and visible
     */
    void setDisplay(bool onDisplay);

    /**
     * The method called to update the main menu.
     *
     * This method handles transitions into and out of main menu along
     * with any updates within the main menu scene.
     */
    void update(MenuState& state);

    /**
     * Returns the root scene node for the main menu layer.
     *
     * @return a shared pointer to main menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_MAIN_MENU_H__ */
