//
//  CIPauseMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PAUSE_MENU_H__
#define __CI_PAUSE_MENU_H__
#include <cugl/cugl.h>

#include  "CIMenuState.h"
#include "CIPlayerSettings.h"
#include "CIGameConstants.h"
#include "CINetworkMessageManager.h"

/**
 * Model class representing a Pause menu scene.
 *
 * The scene contains inputs for player name, volume,
 * and music effect toggles to edit the gameplay
 * settings. All these values are not located inside this class
 * (in parent MenuScene class) while this class only contains
 * pointers.
 */
class PauseMenu {
private:

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the settings page title label */
    std::shared_ptr<cugl::scene2::Label> _settingsTitle;
    /** Reference to the music toggle button label */
    std::shared_ptr<cugl::scene2::Label> _musicLabel;
    /** Reference to the volume slider label */
    std::shared_ptr<cugl::scene2::Label> _volumeLabel;
    /** Reference to music toggle button */
    std::shared_ptr<cugl::scene2::Button> _musicBtn;
    /** Reference to volume slider */
    std::shared_ptr<cugl::scene2::Slider> _volumeSlider;
    /** Reference to the resume button in pause menu */
    std::shared_ptr<cugl::scene2::Button> _resumeBtn;
    /** Reference to the exit game button in pause menu */
    std::shared_ptr<cugl::scene2::Button> _exitBtn;
    
    /** Boolean setting request to exit game */
    bool _exitGame;
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new settings menu with default values.
     */
    PauseMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    ~PauseMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    void dispose();

    /**
     * Initializes a new pause menu with the state pointer.
     *
     * @param assets            The (loaded) assets for this pause menu
     * @param playerSettings    The player's saved settings value
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, 
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager, 
        const std::shared_ptr<PlayerSettings>& playerSettings);

    /**
     * Returns a newly allocated pause menu.
     *
     * @param assets            The (loaded) assets for this pause menu
     * @param playerSettings    The player's saved settings value
     *
     * @return a newly allocated settings menu
     */
    static std::shared_ptr<PauseMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager, 
        const std::shared_ptr<PlayerSettings>& playerSettings) {
        std::shared_ptr<PauseMenu> result = std::make_shared<PauseMenu>();
        return (result->init(assets, networkMessageManager, playerSettings) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the pause menu is currently active and visible.
     *
     * @param onDisplay     Whether the settings menu is currently active and visible
     */
    void setDisplay(bool onDisplay);

    /**
     * The method called to update the pause menu.
     *
     * This method handles transitions into and out of pause menu along
     * with any updates within the pause menu scene.
     * 
     * @param playerSettings    The player's saved settings value
     */
    void update(const std::shared_ptr<PlayerSettings>& playerSettings);

    /**
     * Returns the root scene node for the pause menu layer.
     *
     * @return a shared pointer to settings menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

    /**
     * Gets whether exitGame has been requested.
     */
    bool getExitGame() const {
        return _exitGame;
    }
};

#endif /* __CI_PAUSE_MENU_H__ */
