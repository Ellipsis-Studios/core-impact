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
#define SPF .033 //seconds per frame

void PlanetNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
    AnimationNode::draw(batch,transform, tint);
}

void PlanetNode::update(float timestep, bool isLockingIn, int numLayers, bool canLockIn) {
    _timeElapsed += timestep;
    if (_timeElapsed > SPF) {
        _timeElapsed = 0;
        
        unsigned int coreFrame = getFrame();
        coreFrame = (coreFrame == CORE_END) ? CORE_START : coreFrame + 1;
        setFrame(coreFrame);
        
        for (int ii = 0; ii < _layers->size(); ii++) {
            LayerNode* node = &_layerNodes[ii];
            if (node != nullptr && node->innerRing) {
                bool isLockedIn = _layers->at(ii).isLockedIn;
                bool locking = (ii == numLayers-1) && isLockingIn;
                bool lockingAvaliable = (ii == numLayers-1) && canLockIn;
                advanceInnerLayerFrame(node);
                advanceOuterLayerFrame(node, isLockedIn, locking, lockingAvaliable);
            }
        }
        
        for (int ii = 0; ii < _progressNodes.size(); ii++) {
            if (_progressNodes[ii] != nullptr) {
                _progressNodes[ii]->update(timestep);
            }
        }
    }
}

void PlanetNode::advanceInnerLayerFrame(LayerNode* node){
    unsigned int innerFrame = node->innerRing->getFrame();
    innerFrame = (innerFrame == INNER_RING_END) ? INNER_RING_START : innerFrame + 1;
    node->innerRing->setFrame(innerFrame);
}

void PlanetNode::advanceOuterLayerFrame(LayerNode* node, bool isLockedIn, bool isLockingIn, bool canLockIn){
    unsigned int outerFrame = node->outerRing->getFrame();
    if (isLockingIn){
        outerFrame = (outerFrame < OUTER_RING_LOCKIN_START or outerFrame > OUTER_RING_LOCKIN_END) ?  OUTER_RING_LOCKIN_START : outerFrame += 1;
    } else if (canLockIn){
        outerFrame = (outerFrame < OUTER_RING_PULSE_START or outerFrame > OUTER_RING_PULSE_END) ?  OUTER_RING_PULSE_START : outerFrame += 1;
    } else if (isLockedIn){
        outerFrame = (outerFrame >= OUTER_RING_LOCK_END) ? OUTER_RING_LOCK_START : outerFrame + 1;
    } else {
        outerFrame = (outerFrame >= OUTER_RING_UNLOCK_END) ? OUTER_RING_UNLOCK_START : outerFrame + 1;
    }
    node->outerRing->setFrame(outerFrame);
}

void PlanetNode::setLayers(std::vector<PlanetLayer>* layers) {
    _layers = layers;
    setColor(CIColor::getColor4(layers->at(0).layerColor));
    
    if (_layerNodes.size() != layers->size()) {
        _layerNodes.resize(layers->size());
    }
    if (_progressNodes.size() != layers->size()) {
        _progressNodes.resize(layers->size());
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
                    prev->outerRing->setScale(PLANET_OUTER_RING_SCALE*_layerScale*LOCK_IN_SCALE_DOWN/_coreScale);
                }
                
                node->innerRing = cugl::scene2::AnimationNode::alloc(_ringTexture, INNER_RING_ROWS, INNER_RING_COLS);
                node->outerRing = cugl::scene2::AnimationNode::alloc(_unlockedTexture, OUTER_RING_ROWS, OUTER_RING_COLS);
                
                node->innerRing->setAnchor(cugl::Vec2::ANCHOR_CENTER);
                node->outerRing->setAnchor(cugl::Vec2::ANCHOR_CENTER);
                cugl::Vec2 pos = cugl::Vec2(getTexture()->getSize()) * 0.5f / CORE_COLS;
                node->innerRing->setPosition(pos);
                node->outerRing->setPosition(pos);
                node->innerRing->setRelativeColor(false);
                node->outerRing->setRelativeColor(false);
                node->innerRing->setScale(_layerScale/_coreScale);
                node->outerRing->setScale(PLANET_OUTER_RING_SCALE * _layerScale/_coreScale);
                
                addChild(node->innerRing);
                addChild(node->outerRing);
            }
            node->innerRing->setColor(CIColor::getColor4(layers->at(ii).layerColor));
            node->outerRing->setColor(CIColor::getColor4(layers->at(ii).layerColor));
        }
        
        if (_taperedArcTexture != nullptr && getScene() != nullptr) {
            if (_progressNodes[ii] == nullptr) {
                _progressNodes[ii] = PlanetProgressNode::alloc(nullptr, nullptr, _taperedArcTexture);
                getScene()->addChild(_progressNodes[ii]);
            }
            _progressNodes[ii]->setLayer(layers->at(ii));
        }
    }
}
