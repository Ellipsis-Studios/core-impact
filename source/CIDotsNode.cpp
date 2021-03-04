//
//  GLPhotonNode.cpp
//  GameLab
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIDotsNode.hpp"

void DotsNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    if (_photonQueue->getTexture() == nullptr) {
        return;
    }
    
    // Get photon texture origin
    cugl::Vec2 origin = _photonQueue->getTexture()->getSize()/2;
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    
    // Step through each active photon in the queue.
    for (size_t ii = 0; ii < _photonQueue->size(); ii++) {
        // Find the position of this photon.
        std::vector<DotModel> queue = _photonQueue->getQueue();
        size_t idx = ((_photonQueue->headIndex() + ii) % queue.size());

        // How big to make the photon.  Decreases with age.
        if (queue[idx].getMass() > 0) {
            cugl::Mat4 photonTransform;
            photonTransform.scale(queue[idx].getRadius());
            photonTransform.translate(queue[idx].getPosition().x, queue[idx].getPosition().y, 0);

            // Use this information to draw.
            batch->draw(_photonQueue->getTexture(), origin, photonTransform*transform);
        }
    }
    batch->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
