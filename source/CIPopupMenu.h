//
//  CIPopupMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 5/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_POPUP_MENU_H__
#define __CI_POPUP_MENU_H__
#include <cugl/cugl.h>

#include "CIMenuState.h"
#include "CIGameConstants.h"
#include "CINetworkMessageManager.h"
#include "CIPlayerSettings.h"
#include "CIGameSettings.h"

/**
 * Model class representing the popup menu scene.
 */
class PopupMenu {
private:
	std::shared_ptr<NetworkMessageManager> _networkMessageManager;
	std::shared_ptr<PlayerSettings> _playerSettings;
	std::shared_ptr<GameSettings> _gameSettings;

	std::shared_ptr<cugl::scene2::SceneNode> _layer;

	std::shared_ptr<cugl::scene2::SceneNode> _window;
	std::shared_ptr<cugl::scene2::NinePatch> _windowBackground;
	std::shared_ptr<cugl::scene2::Label> _windowLabel;
	std::shared_ptr<cugl::scene2::SceneNode> _windowCircle;

	std::shared_ptr<cugl::scene2::SceneNode> _error;
	std::shared_ptr<cugl::scene2::NinePatch> _errorBackground;
	std::shared_ptr<cugl::scene2::Label> _errorLabel1;
	std::shared_ptr<cugl::scene2::Label> _errorLabel2;

	float _timer;

public:
#pragma mark -
#pragma mark Constructors
	/**
	 * Creates a new popup menu
	 */
	PopupMenu() : _timer(1.0f) {}

	~PopupMenu() { dispose(); }

	void dispose();

	bool init(const std::shared_ptr<cugl::AssetManager>& assets,
		const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
		const std::shared_ptr<GameSettings>& gameSettings,
		const std::shared_ptr<PlayerSettings>& playerSettings);

	static std::shared_ptr<PopupMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets,
		const std::shared_ptr<NetworkMessageManager>& networkMessageManager,
		const std::shared_ptr<GameSettings>& gameSettings,
		const std::shared_ptr<PlayerSettings>& playerSettings) {
		std::shared_ptr<PopupMenu> result = std::make_shared<PopupMenu>();
		return (result->init(assets, networkMessageManager, gameSettings, playerSettings) ? result : nullptr);
	}

#pragma mark -
#pragma mark Menu Monitoring
	/**
	 * Sets whether the game lobby menu is currently active and visible.
	 *
	 * @param onDisplay     Whether the game lobby menu is currently active and visible
	 */
	void setDisplay(bool onDisplay);

	void update(MenuState& state, float timestep);

	const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
		return _layer;
	}
};

#endif /* __CI_POPUP_MENU_H__ */
