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

void PlanetNode::update(float timestep, bool isLockingIn, int numLayers) {
  _timeElapsed += timestep;
  CULog("Is Locking in:  %i", isLockingIn);
  if (_timeElapsed > SPF) {
    _timeElapsed = 0;
    
    unsigned int coreFrame = getFrame();
    coreFrame = (coreFrame == CORE_END) ? CORE_START : coreFrame + 1;
    setFrame(coreFrame);
    
    for (int ii = 0; ii < _layers->size(); ii++) {
      LayerNode* node = &_layerNodes[ii];
      if (node != nullptr && node->innerRing) {
        bool isLockedIn = _layers->at(ii).isLockedIn;
        advanceInnerLayerFrame(node);
        advanceOuterLayerFrame(node, isLockedIn, isLockingIn, ii, numLayers);
      }
    }
  }
}

void PlanetNode::advanceInnerLayerFrame(LayerNode* node){
  unsigned int innerFrame = node->innerRing->getFrame();
  innerFrame = (innerFrame == INNER_RING_END) ? INNER_RING_START : innerFrame + 1;
  node->innerRing->setFrame(innerFrame);
}

void PlanetNode::advanceOuterLayerFrame(LayerNode* node, bool isLockedIn, bool isLockingIn, int ii, int numLayers){
  unsigned int outerFrame = node->outerRing->getFrame();
  CULog("Layer Index: %i", ii);
  if (isLockingIn and ii == numLayers-1){
    CULog("locking in animation");
    if (outerFrame < OUTER_RING_LOCKIN_START or outerFrame > OUTER_RING_LOCKIN_END){
      outerFrame = OUTER_RING_LOCKIN_START;
    } else{
      outerFrame += 1;
    }
  }else{
    if(isLockedIn){
      CULog("locked in");
      outerFrame = (outerFrame >= OUTER_RING_LOCK_END) ? OUTER_RING_LOCK_START : outerFrame + 1;
    }else{
      CULog("unlocked");
      outerFrame = (outerFrame >= OUTER_RING_UNLOCK_END) ? OUTER_RING_UNLOCK_START : outerFrame + 1;
    }
  }
  CULog("OUTER FRAME: %i", outerFrame);
  node->outerRing->setFrame(outerFrame);
}

//void PlanetNode::advanceFrame(LayerNode* node, bool isLockedIn, bool isLockingIn) {
//  // Our animation depends on the current frame
//  unsigned int innerFrame = node->innerRing->getFrame();
//  innerFrame = (innerFrame == INNER_RING_END) ? INNER_RING_START : innerFrame + 1;
//  node->innerRing->setFrame(innerFrame);
//}


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
        node->outerRing->setScale(_layerScale/_coreScale);
        
        addChild(node->innerRing);
        addChild(node->outerRing);
      }
      node->innerRing->setColor(CIColor::getColor4(layers->at(ii).layerColor));
      node->outerRing->setColor(CIColor::getColor4(layers->at(ii).layerColor));
//      if (layers->at(ii).isLockedIn) {
//        node->outerRing->setTexture(_lockedTexture);
//      }
    }
  }
}
