//
//  CIOpponentNode.cpp
//  CoreImpact
//
//  This class is used to draw the opponent's progress to the screen.
//
//  Created by Brandon Stein on 4/2/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include "CIOpponentNode.h"

#define SPF .033 //seconds per frame
#define FOG_SEC_ON_SCREEN  10
#define FOG_FRAMES  60

/**
 * Disposes the Stardust node, releasing all resources.
 */
void OpponentNode::dispose() {
    _texture = nullptr;
}

void OpponentNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    if (_texture == nullptr) {
        return;
    }
    
    float width = _texture->getWidth() / PROGRESS_COLS;
    float height = _texture->getHeight() / PROGRESS_ROWS;
    cugl::Vec2 reflection = getReflectFromLocation(_location);
    
    cugl::Mat4 horizontalBarTransform;
    horizontalBarTransform.scale((_progress * _maxwidth * reflection.x) / width, reflection.y, 1);
    horizontalBarTransform.translate(0, height * reflection.x / 2, 0);
    horizontalBarTransform.multiply(transform);
    AnimationNode::draw(batch, horizontalBarTransform, tint);
    
    cugl::Mat4 verticalBarTransform;
    verticalBarTransform.rotateZ(-M_PI / 2);
    verticalBarTransform.scale(reflection.x, (_progress * _maxheight * -reflection.y) / width, 1);
    verticalBarTransform.translate(height * reflection.y / 2, 0, 0);
    verticalBarTransform.multiply(transform);
    AnimationNode::draw(batch, verticalBarTransform, tint);
    
    if (_fogAnimationProgress > 0) {
        float fogProgress = _fogAnimationEasingFunction->evaluate((float) _fogAnimationProgress / FOG_FRAMES);
        cugl::Vec2 fogOrigin = _fogTexture->getSize() / 2;
        cugl::Mat4 fogTransform;
        fogTransform.rotateZ(getFogRotationFromLocation(_location));
        fogTransform.translate(fogOrigin.x * reflection.x * fogProgress * 2 - (fogOrigin.x * reflection.x), fogOrigin.y * reflection.y * fogProgress * 2 - (fogOrigin.y * reflection.y), 0);
        fogTransform.multiply(transform);
        batch->draw(_fogTexture, cugl::Color4::GRAY, fogOrigin, fogTransform);
    }
}

/**
 * Updates the animations for this opponent node.
 *
 * @param timestep The amount of time since the last animation frame
 */
void OpponentNode::update(float timestep) {
    _timeElapsed += timestep;
    if (_timeElapsed > SPF) {
        _timeElapsed = 0;
        if (getFrame() == PROGRESS_NORMAL_END || getFrame() == PROGRESS_FLASH_END) {
            setFrame(PROGRESS_NORMAL_START);
        } else {
            setFrame(getFrame() + 1);
        }
    }
    
    if (_fogOngoing) {
        // fog is fully on screen
        if (_fogAnimationProgress == FOG_FRAMES) {
            if (_fogTimeOnScreen >= FOG_SEC_ON_SCREEN) {
                _fogOngoing = false;
            } else {
                _fogTimeOnScreen += timestep;
            }
        } else {
            _fogAnimationProgress++;
        }
    } else {
        if (_fogAnimationProgress > 0) {
            _fogAnimationProgress--;
        }
    }
}
