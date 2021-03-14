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

void StardustNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    if (_stardustQueue->getTexture() == nullptr) {
        return;
    }
    
    // Get stardust texture origin
    cugl::Vec2 origin = _stardustQueue->getTexture()->getSize()/2;
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    
    // Step through each active stardust in the queue.
    for (size_t ii = 0; ii < _stardustQueue->size(); ii++) {
        // Find the position of this stardust.
        std::vector<StardustModel> queue = _stardustQueue->getQueue();
        size_t idx = ((_stardustQueue->headIndex() + ii) % queue.size());

        if (queue[idx].getMass() > 0) {
            cugl::Mat4 stardustTransform;
            stardustTransform.scale(queue[idx].getRadius());
            stardustTransform.translate(queue[idx].getPosition().x, queue[idx].getPosition().y, 0);
            stardustTransform.multiply(transform);
            
            // Handle stardust color 
            const cugl::Color4f stardustColor = CIColor::getColor4(queue[idx].getColor());
            batch->draw(_stardustQueue->getTexture(), stardustColor, origin, stardustTransform);    
        }
    }
    batch->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
