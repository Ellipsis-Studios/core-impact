//
//  CITutorialMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CITutorialMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors
/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void TutorialMenu::dispose() {
    _layer = nullptr;
    _tutorialTitle = nullptr;
    _nextState = MenuState::Tutorial;
}

/**
 * Initializes a new tutorial menu with the state pointer.
 *
 * @param assets    The (loaded) assets for this tutorial menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool TutorialMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("tutorial");
    _layer->setContentSize(dimen);
    _layer->doLayout();
    _tutorialTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("tutorial_title"));
    
    _nextState = MenuState::Tutorial;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the settings menu is currently active and visible.
 *
 * @param onDisplay     Whether the settings menu is currently active and visible
 */
void TutorialMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _tutorialTitle->setVisible(onDisplay);
        _layer->setVisible(onDisplay);
    }
}

/**
 * The method called to update this menu.
 *
 * The menu screen is only visible during the Tutorial state. Transition
 * states into Tutorial state puts the screen on display. The screen
 * is taken down once menu state exits Tutorial.
 */
void TutorialMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle Tutorial menu
    switch (state) 
    {
        case MenuState::MainToTutorial:
            // handle transitioning into Tutorial
            setDisplay(true);
            state = MenuState::Tutorial;
            _nextState = MenuState::Tutorial;
            break;
        case MenuState::Tutorial:
            // handle transitioning out of Tutorial
            state = _nextState;
            break;
        default:
            // hide menu screen
            if (_layer != nullptr && _layer->isVisible()) {
                setDisplay(false);
            }
            break;
    }
}
