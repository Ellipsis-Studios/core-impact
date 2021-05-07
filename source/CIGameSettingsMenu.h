//
//  CIGameSettingsMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 5/4/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_GAME_SETTINGS_MENU_H__
#define __CI_GAME_SETTINGS_MENU_H__
#include <cugl/cugl.h>

#include "CIMenuState.h"
#include "CIGameSettings.h"
#include "CIGameConstants.h"
#include "CINetworkMessageManager.h"

/**
 * Model class representing the Game Settings menu scene.
 */
class GameSettingsMenu {
private:
    MenuState _nextState;
    std::shared_ptr<GameSettings> _gameSettings;
    std::shared_ptr<NetworkMessageManager> _networkMessageManager;

    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    std::shared_ptr<cugl::scene2::Label> _title;
    std::shared_ptr<cugl::scene2::Label> _subTitle;

    // game lobby settings
    /** Reference to stardust spawn rate button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _spawnRateBtn; // button
    std::shared_ptr<cugl::scene2::Label> _spawnRateLabel; // rate value label
    std::shared_ptr<cugl::scene2::Label> _spawnRateBtnLabel; // button label
    const float _spawnRates[7] = { 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 5.0f, 9.9f }; // rate value list
    uint8_t _currSpawn; // index to cycle the value list

    /** Reference to planet gravity strength button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _gravStrengthBtn;
    std::shared_ptr<cugl::scene2::Label> _gravStrengthLabel;
    std::shared_ptr<cugl::scene2::Label> _gravStrengthBtnLabel;
    const float _gravStrengths[7] = { 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 5.0f, 9.9f };
    uint8_t _currGrav;

    /** Reference to layer size button + label + value list */
    std::shared_ptr<cugl::scene2::Button> _layerSizeBtn;
    std::shared_ptr<cugl::scene2::Label> _layerSizeLabel;
    std::shared_ptr<cugl::scene2::Label> _layerSizeBtnLabel;
    const uint16_t _layerSize[5] = { 4, 5, 6, 7, 8 };
    uint8_t _currWin;

    std::shared_ptr<cugl::scene2::Button> _applySettingsBtn;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game lobby with default values.
     */
    GameSettingsMenu() : _nextState(MenuState::GameSetting), _currSpawn(2), _currGrav(2), _currWin(2) {}

    ~GameSettingsMenu() { dispose(); }

    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
        const std::shared_ptr<GameSettings>& gameSettings);

    static std::shared_ptr<GameSettingsMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets,
        const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
        const std::shared_ptr<GameSettings>& gameSettings) {
        std::shared_ptr<GameSettingsMenu> result = std::make_shared<GameSettingsMenu>();
        return (result->init(assets, networkMessageManager, gameSettings) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    /**
     * Sets whether the game lobby menu is currently active and visible.
     *
     * @param onDisplay     Whether the game lobby menu is currently active and visible
     */
    void setDisplay(bool onDisplay);

    void update(MenuState& state);

    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_GAME_SETTINGS_MENU_H__ */
