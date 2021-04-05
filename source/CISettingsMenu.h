//
//  CISettingsMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_SETTINGS_MENU_H__
#define __CI_SETTINGS_MENU_H__
#include <cugl/cugl.h>

class SettingsMenu {
private:
    /** The asset manager for menu screens. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // Settings Screen Layer
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Label for Settings screen title */
    std::shared_ptr<cugl::scene2::Label> _settingsTitle;
    /** Label for player name input */
    std::shared_ptr<cugl::scene2::Label> _pnameLabel;
    /** Label for music toggle button */
    std::shared_ptr<cugl::scene2::Label> _musicLabel;
    /** Label for volume slider */
    std::shared_ptr<cugl::scene2::Label> _volumeLabel;
    /** Label for parallax toggle button */
    std::shared_ptr<cugl::scene2::Label> _parallaxLabel;
    
    /** Player Name input */
    std::shared_ptr<cugl::scene2::TextField> _pnameInput;
    /** Button to toggle music */
    std::shared_ptr<cugl::scene2::Button> _musicBtn;
    /** Slider for gameplay volume */
    std::shared_ptr<cugl::scene2::Slider> _volumeSlider;
    /** Button to toggle parallax effect */
    std::shared_ptr<cugl::scene2::Button> _parallaxBtn;
    
    /** Value for the player name */
    string _playerName;
    /** Value for the game audio volume */
    float _volume;
    /** Whether game music is turned on/off */
    bool _musicOn;
    /** Whether game parallax effect is turned on/off */
    bool _parallaxOn;

public:
#pragma mark -
#pragma mark Constructors
    SettingsMenu() : _volume(0.0f), _musicOn(true), _parallaxOn(true) {}

    ~SettingsMenu() { dispose(); }

    void dispose();
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    static std::shared_ptr<SettingsMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<SettingsMenu> result = std::make_shared<SettingsMenu>();
        return (result->init(assets) ? result : nullptr);
    }
    
    void setDisplay(bool value);
    
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }
    
    std::string getPlayerName() {
        return _playerName;
    }
    
    float getVolume() {
        return _volume;
    }
    
    bool getMusicOn() {
        return _musicOn;
    }
    
    bool getParallaxOn() {
        return _parallaxOn;
    }
    
    void update(float timestep) {
        if (_musicBtn->isActive()) {
            _musicOn = !_musicBtn->isDown();
        }
        if (_parallaxBtn->isActive()) {
            _parallaxOn = !_parallaxBtn->isDown();
        }
    }
    
};

#endif /* __CI_SETTINGS_MENU_H__ */
