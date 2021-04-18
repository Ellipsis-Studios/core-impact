//
//  CISettingsMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_SETTINGS_MENU_H__
#define __CI_SETTINGS_MENU_H__
#include <cugl/cugl.h>

#include  "CIMenuState.h"
#include "CIPlayerSettings.h"
#include "CIGameConstants.h"

/**
 * Model class representing a Settings menu scene.
 *
 * The scene contains inputs for player name, volume,
 * music and parallax effect toggles to edit the gameplay
 * settings. All these values are not located inside this class
 * (in parent MenuScene class) while this class only contains
 * pointers.
 */
class SettingsMenu {
private:
    /** Menu state for the upcoming frame */
    MenuState _nextState;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the settings page title label */
    std::shared_ptr<cugl::scene2::Label> _settingsTitle;
    /** Reference to the player name input label */
    std::shared_ptr<cugl::scene2::Label> _pnameLabel;
    /** Reference to the music toggle button label */
    std::shared_ptr<cugl::scene2::Label> _musicLabel;
    /** Reference to the volume slider label */
    std::shared_ptr<cugl::scene2::Label> _volumeLabel;
    /** Reference to the parallax toggle button label */
    std::shared_ptr<cugl::scene2::Label> _parallaxLabel;
    /** Reference to player name input */
    std::shared_ptr<cugl::scene2::TextField> _pnameInput;
    /** Reference to music toggle button */
    std::shared_ptr<cugl::scene2::Button> _musicBtn;
    /** Reference to volume slider */
    std::shared_ptr<cugl::scene2::Slider> _volumeSlider;
    /** Reference to parallax toggle button */
    std::shared_ptr<cugl::scene2::Button> _parallaxBtn;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new settings menu with default values.
     */
    SettingsMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    ~SettingsMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    void dispose();

    /**
     * Initializes a new settings menu with the state pointer.
     *
     * @param assets        The (loaded) assets for this settings menu
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Returns a newly allocated settings menu.
     *
     * @param assets        The (loaded) assets for this settings menu
     *
     * @return a newly allocated settings menu
     */
    static std::shared_ptr<SettingsMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<SettingsMenu> result = std::make_shared<SettingsMenu>();
        return (result->init(assets) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the settings menu is currently active and visible.
     *
     * @param onDisplay     Whether the settings menu is currently active and visible
     */
    void setDisplay(bool onDisplay);

    /**
     * The method called to update the settings menu.
     *
     * This method handles transitions into and out of settings menu along
     * with any updates within the settings menu scene.
     */
    void update(MenuState& state, const std::shared_ptr<PlayerSettings>& playerSettings);

    /**
     * Returns the root scene node for the settings menu layer.
     *
     * @return a shared pointer to settings menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_SETTINGS_MENU_H__ */
