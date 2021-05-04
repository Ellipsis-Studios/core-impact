//
//  CIPauseMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright � 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPauseMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void PauseMenu::dispose() {
    if (_musicBtn != nullptr && _musicBtn->isVisible()) {
        _musicBtn->deactivate();
        _volumeSlider->deactivate();
        _parallaxBtn->deactivate();
        _resumeBtn->deactivate();
        _exitBtn->deactivate();
    }
    else if (_musicBtn != nullptr) {
        _musicBtn->clearListeners();
        _volumeSlider->clearListeners();
        _parallaxBtn->clearListeners();
        _resumeBtn->clearListeners();
        _exitBtn->clearListeners();
    }

    _settingsTitle = nullptr;
    _musicLabel = nullptr;
    _volumeLabel = nullptr;
    _parallaxLabel = nullptr;

    _musicBtn = nullptr;
    _volumeSlider = nullptr;
    _parallaxBtn = nullptr;

    _resumeBtn = nullptr;
    _exitBtn = nullptr;

    _layer = nullptr;
}

/**
 * Initializes a new pause menu with the state pointer.
 *
 * @param assets        The (loaded) assets for this pause menu
 *
 * @return true if initialization was successful, false otherwise
 */
bool PauseMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<NetworkMessageManager>& networkMessageManager, 
    const std::shared_ptr<PlayerSettings>& playerSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("pause");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _playerSettings = playerSettings;

    _settingsTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_title"));
    _musicLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_musiclabel"));
    _volumeLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_volumelabel"));
    _parallaxLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("pause_parallaxlabel"));

    // Music toggle button
    _musicBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("pause_musicinput"));
    _musicBtn->setToggle(true);
    _musicBtn->setDown(!_playerSettings->getMusicOn());

    // Parallax toggle button
    _parallaxBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("pause_parallaxinput"));
    _parallaxBtn->setToggle(true);
    _parallaxBtn->setDown(!_playerSettings->getParallaxOn());

    // Volume slider 
    _volumeSlider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("pause_volumeinput"));
    _volumeSlider->setValue(_playerSettings->getVolume());

    // Resume and exit game buttons
    _resumeBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("pause_resumebutton"));
    _exitBtn = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("pause_exitgamebutton"));

    _resumeBtn->addListener([&](const std::string& name, bool down) {
        if (!down) {
            networkMessageManager->setGameState(GameState::GameInProgress);
        }
        });

    _exitGame = false;
    _exitBtn->setDown(false);
    _exitBtn->addListener([&](const std::string& name, bool down) {
        if (down) {
            _exitGame = true;
        }
        });

    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the settings menu is currently active and visible.
 *
 * @param onDisplay     Whether the settings menu is currently active and visible
 */
void PauseMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _settingsTitle->setVisible(onDisplay);
        _musicLabel->setVisible(onDisplay);
        _musicBtn->setVisible(onDisplay);
        _volumeLabel->setVisible(onDisplay);
        _volumeSlider->setVisible(onDisplay);
        _parallaxLabel->setVisible(onDisplay);
        _parallaxBtn->setVisible(onDisplay);
        _resumeBtn->setVisible(onDisplay);
        _exitBtn->setVisible(onDisplay);
        _layer->setVisible(onDisplay);

        if (onDisplay) {
            _musicBtn->activate();
            _volumeSlider->activate();
            _parallaxBtn->activate();
            _resumeBtn->activate();
            _exitBtn->activate();
        }
        else {
            _musicBtn->deactivate();
            _volumeSlider->deactivate();
            _parallaxBtn->deactivate();
            _resumeBtn->deactivate();
            _exitBtn->deactivate();
        }
    }
}

/**
 * The method called to update this menu.
 */
void PauseMenu::update() {
    if (_layer == nullptr) {
        return;
    }

    _playerSettings->setVolume(_volumeSlider->getValue());
    _playerSettings->setMusicOn(!_musicBtn->isDown());
    _playerSettings->setParallaxOn(!_parallaxBtn->isDown());
    _volumeSlider->setValue(_playerSettings->getVolume());
    _musicBtn->setDown(!_playerSettings->getMusicOn());
    _parallaxBtn->setDown(!_playerSettings->getParallaxOn());
}