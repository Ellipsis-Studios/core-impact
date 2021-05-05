//
//  CIPlanetProgressNode.cpp
//  CoreImpact
//
//  Created by William Long on 4/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetProgressNode.h"

#define SPF .01 //seconds per frame

#define CANLOCKIN_END   155

/**
 * Disposes the planet progress node, releasing all resources.
 */
void PlanetProgressNode::dispose() {
    _planetLayer = {};
    _progressTexture = nullptr;
    _timeElapsed = 0;
    _currFrame = 0;
}

/**
 * Draws the planet progress to the game scene.
 */
void PlanetProgressNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                              const cugl::Mat4& transform, cugl::Color4 tint) {
    float height = _progressTexture->getHeight() / LOCKIN_ROWS;
    float width = _progressTexture->getWidth() / LOCKIN_COLS;
    cugl::Mat4 progressTransform;
    progressTransform.translate(width * 2.36f, -height * 2.75f, 0);
    progressTransform.translate(_layerNum * width, 0, 0);
    progressTransform.multiply(transform);
    CIColor::Value c = (_planetLayer.layerSize == 0) ? CIColor::getNoneColor() : _planetLayer.layerColor;
    AnimationNode::draw(batch, progressTransform, CIColor::getColor4(c));
}

/**
 * Updates the frame of the planet progress animation
 */
void PlanetProgressNode::update(float timestep, int lockinLayerSize) {
    if (_planetLayer.isLockedIn) {
        setFrame(CANLOCKIN_END);
        return;
    }
    
    if (_planetLayer.layerSize == 0) {
        _currFrame = LOCKIN_END;
    } else {
        _currFrame = round(((float) _planetLayer.layerSize / (float) lockinLayerSize) * PROGRESS_ARC_END);
    }
    
    _timeElapsed += timestep;
    if (_timeElapsed > SPF) {
        _timeElapsed = 0;
        unsigned int frame = getFrame();
        if (_planetLayer.layerSize > (lockinLayerSize - 1)) {
            if (frame == CANLOCKIN_END) {
                setFrame(PROGRESS_ARC_END);
            } else {
                setFrame(frame + 1);
            }
            return;
        }
        
        if (frame == _currFrame) {
            return;
        }
        
        if (_currFrame == LOCKIN_END) {
            if (frame == 0) {
                setFrame(LOCKIN_END);
            } else {
                setFrame(frame - 1);
            }
            return;
        }
        
        
        if (frame == LOCKIN_END) {
            setFrame(LOCKIN_START);
        } else if (frame > PROGRESS_ARC_END && _planetLayer.layerSize <= (lockinLayerSize - 1)) {
            setFrame(PROGRESS_ARC_END);
        } else if (_currFrame > frame) {
            setFrame(frame + 1);
        } else {
            setFrame(frame - 1);
        }
    }
}
