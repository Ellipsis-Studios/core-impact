//
//  CISettingsMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CISettingsMenu.h"

using namespace cugl;

#define SCENE_SIZE  1024

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
        _parallaxBtn->deactivate();
    }
    _settingsTitle = nullptr;
    _pnameLabel = nullptr;
    _musicLabel = nullptr;
    _volumeLabel = nullptr;
    _parallaxLabel = nullptr;
    
    _pnameInput = nullptr;
    _musicBtn = nullptr;
    _volumeSlider = nullptr;
    _parallaxBtn = nullptr;
    
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
bool SettingsMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("settings");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _settingsTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_title"));
    _pnameLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_namelabel"));
    _musicLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_musiclabel"));
    _volumeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_volumelabel"));
    _parallaxLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_parallaxlabel"));
    
    // Music toggle button
    _musicBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_musicinput"));
    _musicBtn->setToggle(true);

    // Parallax toggle button
    _parallaxBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_parallaxinput"));
    _parallaxBtn->setToggle(true);

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
    
    // TEMP
    auto _volumeKnob = _volumeSlider->getKnob();
    _volumeKnob->setColor(Color4::WHITE);
    
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
        _parallaxLabel->setVisible(onDisplay);
        _parallaxBtn->setVisible(onDisplay);
        _layer->setVisible(onDisplay);
        
        if (onDisplay) {
            _pnameInput->activate();
            _musicBtn->activate();
            _volumeSlider->activate();
            _parallaxBtn->activate();
        } else {
            _pnameInput->deactivate();
            _musicBtn->deactivate();
            _volumeSlider->deactivate();
            _parallaxBtn->deactivate();
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
void SettingsMenu::update(MenuState& state, string& playername, float& volume, bool& musicon, bool& parallaxon) {
    if (_layer == nullptr) {
        return;
    }
    // handle Settings menu
    switch (state) 
    {
        case MenuState::MainToSetting:
            // handle transitioning into Settings 
            setDisplay(true);
            _pnameInput->setText(playername);
            _volumeSlider->setValue(volume);
            _musicBtn->setDown(!musicon);
            _parallaxBtn->setDown(!parallaxon);

            state = MenuState::Setting;
            _nextState = MenuState::Setting;
            break;
        case MenuState::Setting:
            // handle updating asset visuals and transitioning out of Settings
            playername = _pnameInput->getText();
            volume = _volumeSlider->getValue();
            musicon = !_musicBtn->isDown();
            parallaxon = !_parallaxBtn->isDown();
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
