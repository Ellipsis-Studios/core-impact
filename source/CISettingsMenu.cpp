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

    _assets = nullptr;
}

bool SettingsMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _assets = assets;
    _layer = assets->get<scene2::SceneNode>("settings");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _settingsTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_title"));
    _pnameLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_namelabel"));
    _musicLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_musiclabel"));
    _volumeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_volumelabel"));
    _parallaxLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("settings_parallaxlabel"));
    
    _pnameInput = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("settings_nameinput"));
    _musicBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_musicinput"));
    _volumeSlider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("settings_volumeinput"));
    _parallaxBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_parallaxinput"));
    _playerName = _pnameInput->getText();
    _volume = _volumeSlider->getValue();
    /** Volume slider listeners */
    _volumeSlider->addListener([=](const std::string& name, float value) {
        if (value != _volume) {
            // TODO: Update global game volume
            _volume = value;
        }
        });
    /** Player name input listeners */
    _pnameInput->addTypeListener([=](const std::string& name, const std::string& value) {
        // Handle size limit on settings name textfield
        if (value.length() > 12) {
            _pnameInput->setText(value.substr(0, 12));
        }
        });
    _pnameInput->addExitListener([=](const std::string& name, const std::string& value) {
        // Set player name input
        _playerName = value;
        CULog("Player Name set to %s", value.c_str());
        });
    /** Set both toggle buttons. */
    _musicBtn->setToggle(true);
    _parallaxBtn->setToggle(true);
    
    return true;
}

void SettingsMenu::setDisplay(bool value) {
    if (_layer != nullptr) {
        _settingsTitle->setVisible(value);
        _pnameLabel->setVisible(value);
        _musicLabel->setVisible(value);
        _volumeLabel->setVisible(value);
        _parallaxLabel->setVisible(value);
        
        _pnameInput->setVisible(value);
        _musicBtn->setVisible(value);
        _volumeSlider->setVisible(value);
        _parallaxBtn->setVisible(value);
        
        _layer->setVisible(value);
        
        if (value) {
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
