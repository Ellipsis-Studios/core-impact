//
//  CINameMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 5/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_NAME_MENU_H__
#define __CI_NAME_MENU_H__
#include <cugl/cugl.h>

#include "CIMenuState.h"
#include "CIPlayerSettings.h"
#include "CIGameConstants.h"

/**
 * Model class representing the Game Settings menu scene.
 */
class NameMenu {
private:
    MenuState _nextState;
    std::shared_ptr<PlayerSettings> _playerSettings;

    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    std::shared_ptr<cugl::scene2::Label> _title;
    std::shared_ptr<cugl::scene2::Label> _subTitle;

    std::shared_ptr<cugl::scene2::TextField> _nameInput;
    std::shared_ptr<cugl::scene2::Button> _submitBtn;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new settings menu with default values.
     */
    NameMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    ~NameMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this menu.
     */
    void dispose();

    /**
     * Initializes a new settings menu with the state pointer.
     *
     * @param assets            The (loaded) assets for this settings menu
     * @param playerSettings    The player's saved settings value
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<PlayerSettings>& playerSettings);

    /**
     * Returns a newly allocated settings menu.
     *
     * @param assets            The (loaded) assets for this settings menu
     * @param playerSettings    The player's saved settings value
     *
     * @return a newly allocated settings menu
     */
    static std::shared_ptr<NameMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<PlayerSettings>& playerSettings) {
        std::shared_ptr<NameMenu> result = std::make_shared<NameMenu>();
        return (result->init(assets, playerSettings) ? result : nullptr);
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
    void update(MenuState& state);

    /**
     * Returns the root scene node for the settings menu layer.
     *
     * @return a shared pointer to settings menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_NAME_MENU_H__ */
