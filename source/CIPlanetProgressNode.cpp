//
//  CIPlanetProgressNode.cpp
//  CoreImpact
//
//  Created by William Long on 4/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetProgressNode.h"

#define SPF .01 //seconds per frame

#define LOCKIN_ONE      12
#define LOCKIN_TWO      22
#define LOCKIN_THREE    33
#define LOCKIN_FOUR     46
#define LOCKIN_FIVE     59
#define CANLOCKIN_END   155

/**
 * Disposes the planet progress node, releasing all resources.
 */
void PlanetProgressNode::dispose() {
    _planetLayer = {};
    _canLockInLayerTexture = nullptr;
    _untaperedArcTexture = nullptr;
    _taperedArcTexture = nullptr;
    _timeElapsed = 0;
    _currFrame = 0;
}

/**
 * Draws the planet progress to the game scene.
 */
void PlanetProgressNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                              const cugl::Mat4& transform, cugl::Color4 tint) {
    float height = _canLockInLayerTexture->getHeight() / LOCKIN_ROWS;
    float width = _canLockInLayerTexture->getWidth() / LOCKIN_COLS;
    cugl::Mat4 progressTransform;
    progressTransform.translate(width, -height * 1.5f, 0);
    progressTransform.multiply(transform);
    AnimationNode::draw(batch, progressTransform, tint);
    
//    float width = _taperedArcTexture->getWidth() / PROGRESS_ARC_COLS;
//    float height = _taperedArcTexture->getHeight() / PROGRESS_ARC_ROWS;
//    float rotation = 0;
////    tint.a = 125;
//    int layerSize = 0;
//    while (layerSize < _planetLayer.layerSize) {
//        cugl::Mat4 progressTransform;
//        progressTransform.rotateZ(rotation);
////        progressTransform.translate(0, -height / 2,0);
//        if (layerSize == 1) {
//            progressTransform.translate(-width / 7.75f, height / 1.25f, 0);
//        } else if (layerSize == 2) {
//            progressTransform.translate(width / 1.7f, height * 1.165f, 0);
//        } else if (layerSize == 3) {
//            progressTransform.translate(width * 1.155f, height / 1.675f, 0);
//        } else if (layerSize == 4) {
//            progressTransform.translate(width / 1.26f, -height / 8.15f, 0);
//        }
//        progressTransform.multiply(transform);
//        AnimationNode::draw(batch, progressTransform, tint);
//        rotation -= 1.25664f;
//        layerSize++;
//    }
}

/**
 * Updates the frame of the planet progress animation
 */
void PlanetProgressNode::update(float timestep) {
//    _timeElapsed += timestep;
//    if (_timeElapsed > SPF) {
//        _timeElapsed = 0;
//
//        unsigned int frame = getFrame();
//        frame = (frame == PROGRESS_ARC_END) ? PROGRESS_ARC_START : frame + 1;
//        setFrame(frame);
//    }
    if (_planetLayer.layerSize == 0) {
        _currFrame = LOCKIN_END;
    } else if (_planetLayer.layerSize == 1) {
        _currFrame = LOCKIN_ONE;
    } else if (_planetLayer.layerSize == 2) {
        _currFrame = LOCKIN_TWO;
    } else if (_planetLayer.layerSize == 3) {
        _currFrame = LOCKIN_THREE;
    } else if (_planetLayer.layerSize == 4) {
        _currFrame = LOCKIN_FOUR;
    } else {
        _currFrame = LOCKIN_FIVE;
    }
    
    _timeElapsed += timestep;
    if (_timeElapsed > SPF) {
        _timeElapsed = 0;
        unsigned int frame = getFrame();
        if (_planetLayer.layerSize > 4) {
            if (frame == CANLOCKIN_END) {
                setFrame(LOCKIN_FIVE);
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
        } else if (frame > LOCKIN_FIVE && _planetLayer.layerSize <= 4) {
            setFrame(LOCKIN_FIVE);
        } else if (_currFrame > frame) {
            setFrame(frame + 1);
        } else {
            setFrame(frame - 1);
        }
    }
}
