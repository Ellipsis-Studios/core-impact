//
//  CINameMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 5/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CINameMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void NameMenu::dispose() {
    if (_nameInput != nullptr && _nameInput->isActive()) {
        _nameInput->deactivate();
        _submitBtn->deactivate();
    }
    else if (_nameInput != nullptr) {
        _nameInput->clearTypeListeners();
        _nameInput->clearExitListeners();
        _submitBtn->clearListeners();
    }

    _title = nullptr;
    _subTitle = nullptr;
    _nameInput = nullptr;
    _submitBtn = nullptr;
    _layer = nullptr;
    _nextState = MenuState::NameMenu;
}

/**
 * Initializes a new settings menu with the state pointer.
 *
 * @param assets        The (loaded) assets for this settings menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool NameMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<PlayerSettings>& playerSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("namemenu");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _playerSettings = playerSettings;

    _title = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("namemenu_title"));
    _subTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("namemenu_subtitle"));
    _nameInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("namemenu_nameinput"));
    _nameInput->addTypeListener([&](const std::string& name, const std::string& value) {
        // Handle size limit
        if (value.length() > 12) {
            _nameInput->setText(value.substr(0, 12));
        }
        });

    _submitBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("namemenu_submitbutton"));
    _submitBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            std::string pname = _nameInput->getText();
            if (_playerSettings != nullptr && !pname.empty()) {
                _playerSettings->setPlayerName(pname);
                _playerSettings->setIsNew(false);
            }
            _nextState = MenuState::NameToJoin;
        }
        });

    _nextState = MenuState::NameMenu;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the settings menu is currently active and visible.
 *
 * @param onDisplay     Whether the settings menu is currently active and visible
 */
void NameMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _title->setVisible(onDisplay);
        _subTitle->setVisible(onDisplay);
        _nameInput->setVisible(onDisplay);
        _submitBtn->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (onDisplay) {
            _nameInput->activate();
            _submitBtn->activate();
        }
        else {
            _nameInput->deactivate();
            _submitBtn->deactivate();
        }
    }
}

/**
 * The method called to update this menu.
 *
 * The menu screen is only visible during the Setting state. Transition
 * states into Setting state puts the screen on display. The screen
 * is taken down once menu state exits Setting.
 */
void NameMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle Settings menu
    switch (state)
    {
        case MenuState::MainToName:
            if (_playerSettings->getSkipNameMenu()) {
                // skip to join menu
                state = _nextState = MenuState::MainToJoin;
                break;
            }
            // handle transitioning into Settings 
            setDisplay(true);
            _nameInput->setText("");
            state = _nextState = MenuState::NameMenu;
            break;
        case MenuState::NameMenu:
            if (_nameInput->getText().size() > 0) {
                _submitBtn->setColor(cugl::Color4f::WHITE);
                _submitBtn->activate();
            }
            else {
                _submitBtn->setColor(cugl::Color4f::GRAY);
                _submitBtn->deactivate();
            }

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
