//
//  CIStardustNode.cpp
//  CoreImpact
//
//  This node class is used to draw all of the stardust to the screen.
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIStardustNode.h"
#include "CIColor.h"

/**
 * Disposes the Stardust node, releasing all resources.
 */
void StardustNode::dispose() {
    _queue->clear();
    _queue = NULL;
    _qhead = NULL;
    _qtail = NULL;
    _qsize = NULL;
    _texture = nullptr;
}

/** 
 * Draws the stardusts in queue to the game scene.
 */
void StardustNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    if (_texture == nullptr) {
        return;
    }
    
    // Get stardust texture origin
    cugl::Vec2 origin = _texture->getSize()/2;
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    
    // Step through each active stardust in the queue.
    for (size_t ii = 0; ii < *_qsize; ii++) {
        // Find the position of this stardust.
        size_t idx = ((*_qhead + ii) % _queue->size());

        if (_queue->at(idx).getMass() > 0) {
            cugl::Mat4 stardustTransform;
            stardustTransform.scale(_queue->at(idx).getRadius());
            stardustTransform.translate(_queue->at(idx).getPosition().x, 
                                        _queue->at(idx).getPosition().y, 0);
            stardustTransform.multiply(transform);
            
            // Handle stardust color 
            const cugl::Color4f stardustColor = CIColor::getColor4(_queue->at(idx).getColor());
            switch (_queue->at(idx).getStardustType()) {
                case StardustModel::Type::METEOR:
                    // TODO: draw meteor texture
                    CULog("DRAWING METEOR");
                    batch->draw(_texture, CIColor::getColor4(CIColor::Value::grey), origin, stardustTransform);
                    break;
                default:
                    batch->draw(_texture, stardustColor, origin, stardustTransform);
            }
        }
    }
    batch->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
