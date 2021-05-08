//
//  CIPlanetProgressNode.cpp
//  CoreImpact
//
//  Created by William Long on 4/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetProgressNode.h"

#define SPF .01 //seconds per frame

/**
 * Disposes the planet progress node, releasing all resources.
 */
void PlanetProgressNode::dispose() {
    _planetLayer = {};
    _progressTexture = nullptr;
    _powerupTextures.clear();
    _timeElapsed = 0;
    _currFrame = 0;
    _opacities.clear();
    _opacitiesIndex = 0;
}

/**
 * Draws the planet progress to the game scene.
 */
void PlanetProgressNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                              const cugl::Mat4& transform, cugl::Color4 tint) {
    float height = _progressTexture->getHeight() / LOCKIN_ROWS;
    float width = _progressTexture->getWidth() / LOCKIN_COLS;
    cugl::Mat4 progressTransform;
    progressTransform.translate(width * 5.0f, -height * 1.3f, 0);
    progressTransform.translate(_layerNum * width, 0, 0);
    progressTransform.multiply(transform);
    CIColor::Value c = (_planetLayer.layerSize == 0) ? CIColor::getNoneColor() : _planetLayer.layerColor;
    cugl::Color4 color = CIColor::getColor4(c);
    color.a = 255 * _opacities[_opacitiesIndex];
    AnimationNode::draw(batch, progressTransform, color);
    
    if (_currFrame != 0 && c < 4) {
        float powerupHeight = _powerupTextures.at(c)->getHeight();
        float powerupWidth = _powerupTextures.at(c)->getWidth();
        cugl::Mat4 powerupTransform;
        cugl::Vec2 powerupOrigin = cugl::Vec2(0,0);
        powerupTransform.translate(width * 5.0f, -height * 1.3f, 0);
        powerupTransform.translate(_layerNum * width, 0, 0);
        powerupTransform.translate(powerupWidth * 0.5f, powerupHeight * 0.5f, 0);
        powerupTransform.multiply(transform);
        
        if (_currFrame < PROGRESS_ARC_END){
            color.a = 128;
        }
        
        batch->draw(_powerupTextures.at(c), color, powerupOrigin, powerupTransform);
    }
}

/**
 * Updates the frame of the planet progress animation
 */
void PlanetProgressNode::update(float timestep, int lockinLayerSize) {
    if (_planetLayer.isLockedIn) {
        _opacitiesIndex = 0;
        setFrame(PROGRESS_ARC_END);
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
        // layer can be locked in
        if (_planetLayer.layerSize > (lockinLayerSize - 1)) {
            if (frame != PROGRESS_ARC_END) {
                setFrame(frame + 1);
            }
            
            if (_opacitiesIndex == _opacities.size() - 1) {
                _opacitiesIndex = 0;
            } else {
                _opacitiesIndex++;
            }
            return;
        }
        
        _opacitiesIndex = 0;
        
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
