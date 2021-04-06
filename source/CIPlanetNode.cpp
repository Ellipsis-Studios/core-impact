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

#define SPF .033 //seconds per frame

void PlanetNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
                      const cugl::Mat4& transform, cugl::Color4 tint) {
  AnimationNode::draw(batch,transform, tint);
}

void PlanetNode::update(float timestep) {
  _timeElapsed += timestep;
  if (_timeElapsed > SPF) {
    _timeElapsed = 0;
    
    unsigned int coreFrame = getFrame();
    coreFrame = (coreFrame == CORE_END) ? CORE_START : coreFrame + 1;
    setFrame(coreFrame);
    
    for (int ii = 0; ii < _layers->size(); ii++) {
      LayerNode* node = &_layerNodes[ii];
      if (node != nullptr && node->innerRing) {
        advanceFrame(node);
      }
    }
  }
}

void PlanetNode::advanceFrame(LayerNode* node) {
  // Our animation depends on the current frame
  unsigned int frame = node->innerRing->getFrame();
  frame = (frame == INNER_RING_END) ? INNER_RING_START : frame + 1;
  node->innerRing->setFrame(frame);
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
        node->innerRing = cugl::scene2::AnimationNode::alloc(_ringTexture, INNER_RING_ROWS, INNER_RING_COLS);
        node->outerRing = cugl::scene2::PolygonNode::allocWithTexture(_unlockedTexture);
        
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
      if (layers->at(ii).isLockedIn) {
        node->outerRing->setTexture(_lockedTexture);
      }
    }
  }
}
