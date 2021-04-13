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

#define SPF .1 //seconds per frame
#define GREYSCALE_TIME 5 // number of seconds for greyscale power up

/**
 * Disposes the Stardust node, releasing all resources.
 */
void StardustNode::dispose() {
    _queue->clear();
    _queue = NULL;
    _qhead = NULL;
    _qtail = NULL;
    _qsize = NULL;
    _timeElapsed = 0;
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
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    
    // Step through each active stardust in the queue.
    for (size_t ii = 0; ii < *_qsize; ii++) {
        // Find the position of this stardust.
        size_t idx = ((*_qhead + ii) % _queue->size());

        if (_queue->at(idx).getMass() > 0) {
            cugl::Mat4 stardustTransform;
            // this translation is what batch->draw does with the origin field
            stardustTransform.translate(-64,-64,0);
            stardustTransform.scale(_queue->at(idx).getRadius() / 3);
            stardustTransform.translate(_queue->at(idx).getPosition().x,
                                        _queue->at(idx).getPosition().y, 0);
            stardustTransform.multiply(transform);
            
            // draw stardust
            cugl::Color4f stardustColor = CIColor::getColor4(_queue->at(idx).getColor());
            if (_grayScaleTime > 0) {
                stardustColor = cugl::Color4::GRAY;
            }
            
            AnimationNode::draw(batch, stardustTransform, stardustColor);
            
            // draw tail
            cugl::Color4 tailColor = cugl::Color4(stardustColor);
            tailColor.a = 125;
            stardustTransform.translate(-_queue->at(idx).getVelocity().x * 2, -_queue->at(idx).getVelocity().y * 2, 0);
            AnimationNode::draw(batch, stardustTransform, tailColor);
        }
    }
    batch->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * Applies a greyscale to all stardust for a period of time.
 */
void StardustNode::applyGreyScale() {
    _grayScaleTime = GREYSCALE_TIME;
}

/**
 * Updates the frame of the stardust animation
 */
void StardustNode::update(float timestep) {
    _timeElapsed += timestep;
    if (_timeElapsed > SPF) {
      _timeElapsed = 0;
      
      unsigned int coreFrame = getFrame();
      coreFrame = (coreFrame == STARDUST_END) ? STARDUST_START : coreFrame + 1;
      setFrame(coreFrame);
    }
    
    if (_grayScaleTime > 0) {
        _grayScaleTime -= timestep;
    }
}
