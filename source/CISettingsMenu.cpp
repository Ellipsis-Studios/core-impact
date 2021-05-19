//
//  CISettingsMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CISettingsMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void SettingsMenu::dispose() {
    if (_pnameInput != nullptr && _pnameInput->isActive()) {
        _pnameInput->deactivate();
        _musicBtn->deactivate();
        _volumeSlider->deactivate();
    } else if (_pnameInput != nullptr) {
        _pnameInput->clearTypeListeners();
        _pnameInput->clearExitListeners();
        _musicBtn->clearListeners();
        _volumeSlider->clearListeners();
    }
    
    _settingsTitle = nullptr;
    _pnameLabel = nullptr;
    _musicLabel = nullptr;
    _volumeLabel = nullptr;
    
    _pnameInput = nullptr;
    _musicBtn = nullptr;
    _volumeSlider = nullptr;
    
    _layer = nullptr;
    _nextState = MenuState::Setting;
}

/**
 * Initializes a new settings menu with the state pointer.
 *
 * @param assets        The (loaded) assets for this settings menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool SettingsMenu::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<PlayerSettings>& playerSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("settings");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _playerSettings = playerSettings;

    _settingsTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_title"));
    _pnameLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_namelabel"));
    _musicLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_musiclabel"));
    _volumeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_volumelabel"));
    
    // Music toggle button
    _musicBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_musicinput"));
    _musicBtn->setToggle(true);

    // Player name input
    _pnameInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("settings_nameinput"));
    _pnameInput->addTypeListener([&](const std::string& name, const std::string& value) {
        // Handle size limit
        if (value.length() > 12) {
            _pnameInput->setText(value.substr(0, 12));
        }
        });

    // Volume slider 
    _volumeSlider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("settings_volumeinput"));
      
    _nextState = MenuState::Setting;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the settings menu is currently active and visible.
 *
 * @param onDisplay     Whether the settings menu is currently active and visible
 */
void SettingsMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _settingsTitle->setVisible(onDisplay);
        _pnameLabel->setVisible(onDisplay);
        _pnameInput->setVisible(onDisplay);
        _musicLabel->setVisible(onDisplay);
        _musicBtn->setVisible(onDisplay);
        _volumeLabel->setVisible(onDisplay);
        _volumeSlider->setVisible(onDisplay);
        _layer->setVisible(onDisplay);
        
        if (onDisplay) {
            _pnameInput->activate();
            _musicBtn->activate();
            _volumeSlider->activate();
        } else {
            _pnameInput->deactivate();
            _musicBtn->deactivate();
            _volumeSlider->deactivate();
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
void SettingsMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle Settings menu
    switch (state)
    {
        case MenuState::MainToSetting:
            // handle transitioning into Settings 
            setDisplay(true);
        
            _pnameInput->setText(_playerSettings->getPlayerName());
            _volumeSlider->setValue(_playerSettings->getVolume());
            _musicBtn->setDown(!_playerSettings->getMusicOn());

            state = MenuState::Setting;
            _nextState = MenuState::Setting;
            break;
        case MenuState::Setting:
            // handle updating asset visuals and transitioning out of Settings
            _playerSettings->setPlayerName(_pnameInput->getText());
            _playerSettings->setVolume(_volumeSlider->getValue());
            _playerSettings->setMusicOn(!_musicBtn->isDown());

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
