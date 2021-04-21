//
//  CIPlanetProgressNode.cpp
//  CoreImpact
//
//  Created by William Long on 4/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetProgressNode.h"

#define SPF .033 //seconds per frame

/**
 * Disposes the planet progress node, releasing all resources.
 */
void PlanetProgressNode::dispose() {
    _planetLayer = {};
    _canLockInLayerTexture = nullptr;
    _untaperedArcTexture = nullptr;
    _taperedArcTexture = nullptr;
    _timeElapsed = 0;
}

/**
 * Draws the planet progress to the game scene.
 */
void PlanetProgressNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                              const cugl::Mat4& transform, cugl::Color4 tint) {
    float width = _taperedArcTexture->getWidth() / PROGRESS_ARC_COLS;
    float height = _taperedArcTexture->getHeight() / PROGRESS_ARC_ROWS;
    float rotation = 0;
    tint.a = 125;
//    progressTransform.rotateZ(-1.25664f);
//    int layerSize = _planetLayer.layerSize;
//    while (layerSize > 0) {
        cugl::Mat4 progressTransform;
//        progressTransform.rotateZ(rotation);
        progressTransform.translate(0, -height / 2,0);
        progressTransform.multiply(transform);
        AnimationNode::draw(batch, progressTransform, tint);
//        rotation += -1.25664f;
//        layerSize--;
//    }
}

/**
 * Updates the frame of the planet progress animation
 */
void PlanetProgressNode::update(float timestep) {
    _timeElapsed += timestep;
    if (_timeElapsed > SPF) {
        _timeElapsed = 0;
        
        unsigned int frame = getFrame();
        frame = (frame == PROGRESS_ARC_END) ? PROGRESS_ARC_START : frame + 1;
        setFrame(frame);
    }
}
