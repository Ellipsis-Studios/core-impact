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
#include <cmath>

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
    
    cugl::Vec2 origin = _texture->getSize()/2;
    cugl::Vec2 reflection = getReflectFromLocation(_location);
    
    cugl::Mat4 horizontalBarTransform;
    horizontalBarTransform.scale((_progress * _maxwidth * reflection.x) / (2 * origin.x), reflection.y, 1);
    horizontalBarTransform.translate((_progress * _maxwidth * reflection.x) / 2, 0, 0);
    horizontalBarTransform.multiply(transform);
    batch->draw(_texture, getColor(), origin, horizontalBarTransform);
    
    cugl::Mat4 verticalBarTransform;
    verticalBarTransform.rotateZ(-M_PI / 2);
    verticalBarTransform.scale(reflection.x, (_progress * _maxheight * -reflection.y) / (2 * origin.x), 1);
    verticalBarTransform.translate(0, (_progress * _maxheight * reflection.y) / 2, 0);
    verticalBarTransform.multiply(transform);
    batch->draw(_texture, getColor(), origin, verticalBarTransform);
}
