//
//  CITutorialMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_TUTORIAL_MENU_H__
#define __CI_TUTORIAL_MENU_H__
#include <cugl/cugl.h>

/**
 * This class is the Tutorial screen within the Menu scene.
 *
 * The screen will display a page title 'How to Play' along with
 * directions on how to play the game.
 */
class TutorialMenu {
private:
    /** The asset manager for menu screens. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // Tutorial Screen Layer
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Label for Tutorial screen title */
    std::shared_ptr<cugl::scene2::Label> _tutorialTitle;

public:
    /**
     * Creates a new tutorial Menu mode with the default values.
     */
    TutorialMenu() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    ~TutorialMenu() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    static std::shared_ptr<TutorialMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<TutorialMenu> result = std::make_shared<TutorialMenu>();
        return (result->init(assets) ? result : nullptr);
    }

    /**
     * Put-up/take-down the current mode to the overall display.
     *
     * Sets all assets' visible value and activates/deactivates all inputs in
     * the current mode.
     *
     * @param value Whether to put-up or take-down the current mode.
     */
    void setDisplay(bool value) {
        if (_layer != nullptr) {
            _tutorialTitle->setVisible(value);
            _layer->setVisible(value);
        }
    }

    /**
    * Returns the layer node of the current mode.
    *
    * @return layer The layer node of the current mode.
    */
    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }

};

#endif /* __CI_TUTORIAL_MENU_H__ */
