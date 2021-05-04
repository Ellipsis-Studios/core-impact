//
//  CIMainMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIMainMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void MainMenu::dispose() {
    if (_settingsBtn != nullptr && _settingsBtn->isActive()) {
        _settingsBtn->deactivate();
        _joinBtn->deactivate();
        _newBtn->deactivate();
        _tutorialBtn->deactivate();
    } else if (_settingsBtn != nullptr) {
        _settingsBtn->clearListeners();
        _joinBtn->clearListeners();
        _newBtn->clearListeners();
        _tutorialBtn->clearListeners();
    }
    
    _settingsBtn = nullptr;
    _joinBtn = nullptr;
    _newBtn = nullptr;
    _tutorialBtn = nullptr;
    _layer = nullptr;

    _nextState = MenuState::MainMenu;
}

/**
 * Initializes a new main menu with the state pointer.
 *
 * @param assets    The (loaded) assets for this main menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool MainMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("mainmenu");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _settingsBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("mainmenu_settingsbutton"));
    _joinBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("mainmenu_joinbutton"));
    _newBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("mainmenu_newbutton"));
    _tutorialBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("mainmenu_tutorialbutton"));

    _settingsBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::MainToSetting;
        }
        });
    _joinBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::MainToJoin;
        }
        });
    _newBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::MainToLobby;
        }
        });
    _tutorialBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _nextState = MenuState::MainToTutorial;
        }
        });

    _nextState = MenuState::MainMenu;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the main menu is currently active and visible.
 *
 * @param onDisplay     Whether the main menu is currently active and visible
 */
void MainMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _settingsBtn->setVisible(onDisplay);
        _joinBtn->setVisible(onDisplay);
        _newBtn->setVisible(onDisplay);
        _tutorialBtn->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (onDisplay) {
            _settingsBtn->activate();
            _joinBtn->activate();
            _newBtn->activate();
            _tutorialBtn->activate();
        }
        else {
            _settingsBtn->deactivate();
            _joinBtn->deactivate();
            _newBtn->deactivate();
            _tutorialBtn->deactivate();
        }
    }
}

/**
 * The method called to update this menu.
 *
 * The menu screen is only visible during the MainMenu state. Transition
 * states into MainMenu state puts the screen on display. The screen
 * is taken down once menu state exits MainMenu.
 */
void MainMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle MainMenu
    switch (state) 
    {
        case MenuState::SettingToMain:
        case MenuState::JoinToMain:
        case MenuState::LobbyToMain:
            // handle transitioning into MainMenu
            setDisplay(true);
            state = MenuState::MainMenu;
            _nextState = MenuState::MainMenu;
            break;
        case MenuState::MainMenu:
            // handle transitioning out of MainMenu
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
