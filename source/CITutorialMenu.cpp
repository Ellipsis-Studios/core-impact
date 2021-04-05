//
//  CITutorialMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CITutorialMenu.h"

using namespace cugl;

#define SCENE_SIZE  1024

void TutorialMenu::dispose() {
    _layer = nullptr;
    _tutorialTitle = nullptr;
    _assets = nullptr;
}

bool TutorialMenu::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= SCENE_SIZE / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _assets = assets;
    _layer = assets->get<scene2::SceneNode>("tutorial");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _tutorialTitle = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("tutorial_title"));

    return true;
}
