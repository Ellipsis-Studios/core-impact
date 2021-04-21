//
//  CITutorialMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_TUTORIAL_MENU_H__
#define __CI_TUTORIAL_MENU_H__
#include <cugl/cugl.h>

#include  "CIMenuState.h"
#include "CIGameConstants.h"

/**
 * Model class representing the Tutorial menu scene.
 *
 * The screen will display a page title 'How to Play' along with
 * directions on how to play the game.
 */
class TutorialMenu {
private:
    /** Menu state for the upcoming frame */
    MenuState _nextState;

    // Asset references
    /** Reference to the node for the group representing this menu scene */
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Reference to the tutorial page title label */
    std::shared_ptr<cugl::scene2::Label> _tutorialTitle;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new tutorial menu with default values.
     */
    TutorialMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    ~TutorialMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes a new tutorial menu with the state pointer.
     *
     * @param assets    The (loaded) assets for this tutorial menu
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Returns a newly allocated tutorial menu.
     *
     * @param assets    The (loaded) assets for this tutorial menu
     *
     * @return a newly allocated tutorial menu
     */
    static std::shared_ptr<TutorialMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<TutorialMenu> result = std::make_shared<TutorialMenu>();
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
     * The method called to update the tutorial menu.
     *
     * This method handles transitions into and out of tutorial menu along
     * with any updates within the tutorial menu scene.
     */
    void update(MenuState& state);

    /**
     * Returns the root scene node for the tutorial menu layer.
     *
     * @return a shared pointer to tutorial menu layer root scene node
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_TUTORIAL_MENU_H__ */
