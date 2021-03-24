//
//  CIPlanetNode.cpp
//  CoreImpact
//
//  This class is used to draw the planet to the screen.
//  The root is used for the texture of the core, and all other layers
//  are added as children of this node
//
//  Created by William Long on 3/8/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetNode.h"
#include <cugl/cugl.h>

#define LOCK_IN_SCALE_DOWN  .75

void PlanetNode::reset() {
    _layerNodes.clear();
}

void PlanetNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
          const cugl::Mat4& transform, cugl::Color4 tint) {
    PolygonNode::draw(batch,transform, tint);
}

void PlanetNode::setLayers(std::vector<PlanetLayer>* layers) {
    _layers = layers;
    setColor(CIColor::getColor4(layers->at(0).layerColor));
    
    if (_layerNodes.size() != layers->size()) {
        _layerNodes.resize(layers->size());
    }
    for (int ii = 0; ii < layers->size(); ii++) {
        LayerNode* node = &_layerNodes[ii];
        if (layers->at(ii).isActive) {
            if (node->innerRing == nullptr) {
                if (ii > 0) {
                    // decrease size of locked in layer slightly
                    if (ii == 1) {
                        _coreScale *= .8;
                        setScale(_coreScale);
                    }
                    LayerNode* prev = &_layerNodes[ii-1];
                    prev->innerRing->setScale(_layerScale*LOCK_IN_SCALE_DOWN/_coreScale);
                    prev->outerRing->setScale(_layerScale*LOCK_IN_SCALE_DOWN/_coreScale);
                }
                
                node->innerRing = cugl::scene2::PolygonNode::allocWithTexture(_ringTexture);
                node->outerRing = cugl::scene2::PolygonNode::allocWithTexture(_unlockedTexture);
                
                node->innerRing->setAnchor(cugl::Vec2::ANCHOR_CENTER);
                node->outerRing->setAnchor(cugl::Vec2::ANCHOR_CENTER);
                cugl::Vec2 pos = cugl::Vec2(getTexture()->getSize()) * 0.5f;
                node->innerRing->setPosition(pos);
                node->outerRing->setPosition(pos);
                node->innerRing->setRelativeColor(false);
                node->outerRing->setRelativeColor(false);
                node->innerRing->setScale(_layerScale/_coreScale);
                node->outerRing->setScale(_layerScale/_coreScale);
                
                addChild(node->innerRing);
                addChild(node->outerRing);
            }
            node->innerRing->setColor(CIColor::getColor4(layers->at(ii).layerColor));
            node->outerRing->setColor(CIColor::getColor4(layers->at(ii).layerColor));
            if (layers->at(ii).isLockedIn) {
                node->outerRing->setTexture(_lockedTexture);
            }
        }
    }
}
