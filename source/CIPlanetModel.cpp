//
//  CIPlanetModel.cpp
//  CoreImpact
//
//  This model class represents all the information for the planet.
//
//  Created by William Long on 3/4/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetModel.h"
#include "CIPlanetNode.h"
#include "CIColor.h"

#define PLANET_RADIUS_DELTA          2
#define INITIAL_PLANET_RADIUS           32
#define PLANET_MASS_DELTA            10
#define INITIAL_PLANET_MASS             25
#define INIT_LAYER_LOCKIN_TOTAL         5
#define LAYER_LOCKIN_TOTAL_INCREASE     1

#pragma mark Properties
/**
 * Sets the textures for this planet.
 *
 * @param core          The texture of the core
 * @param ring          The texture of an innner ring
 * @param unlocked The texture on the outside of an unlocked ring
 * @param unlocked The texture on the outside of a locked ring
 */
void PlanetModel::setTextures(const std::shared_ptr<cugl::Texture>& core,
                 const std::shared_ptr<cugl::Texture>& ring,
                 const std::shared_ptr<cugl::Texture>& unlocked,
                 const std::shared_ptr<cugl::Texture>& locked) {
    _planetNode = PlanetNode::alloc(core, ring, unlocked, locked);
    _planetNode->setAnchor(cugl::Vec2::ANCHOR_CENTER);
    _planetNode->setLayers(&_layers);
    _planetNode->setPosition(_position);
    _planetNode->setRadius(_radius);
}
 

#pragma mark Constructors
/**
 * Disposes the planet, releasing all resources.
 */
void PlanetModel::dispose() {
    _layers.clear();
    _planetNode = nullptr;
}

/**
 * Initializes a new planet with the given color
 *
 * This method does NOT create a scene graph node for this planet.  You
 * must call setTexture for that.
 *
 * @param x The initial x-coordinate of the center
 * @param y The initial y-coordinate of the center
 * @param c The initial color code of the planet
 * @param maxLayers The maximum number of layers the planet can have
 *
 * @return true if the initialization was successful
 */
bool PlanetModel::init(float x, float y, CIColor::Value c, int maxLayers) {
    _position.set(x, y);
    _layers.resize(maxLayers);
    
    _numLayers = 1;
    _layers[_numLayers-1] = getNewLayer();
    
    _layerLockinTotal = INIT_LAYER_LOCKIN_TOTAL;
    
    _radius = INITIAL_PLANET_RADIUS;
    _mass = INITIAL_PLANET_MASS;
    return true;
}


#pragma mark Interactions

/**
 * Decreases the size of the current layer
 */
void PlanetModel::decreaseLayerSize() {
    PlanetLayer* currentLayer = &_layers[_numLayers-1];
    if (currentLayer->layerSize > 0) {
        currentLayer->layerSize--;
        if (currentLayer->layerSize == 0) {
            currentLayer->layerColor = CIColor::getNoneColor();
        }
        _radius -= PLANET_RADIUS_DELTA;
        _mass -= PLANET_MASS_DELTA;
        _planetNode->setRadius(_radius);
        _planetNode->setLayers(&_layers);
    }
}

/**
 * Increases the size of the current layer
 */
void PlanetModel::increaseLayerSize() {
    _layers[_numLayers-1].layerSize++;
    _radius += PLANET_RADIUS_DELTA;
    _mass += PLANET_MASS_DELTA;

    _planetNode->setRadius(_radius);
    _planetNode->setLayers(&_layers);
}

/**
 * Attempts to lock in the current layer and adds a new one
 */
bool PlanetModel::lockInLayer() {
    // do not lock in if there is not enough stardust to do a lock in or the planet already has the max number of layers
    if (getCurrLayerProgress() < _layerLockinTotal || _numLayers >= _layers.size()) {
        return false;
    }
    
    _numLayers++;
    _layerLockinTotal += LAYER_LOCKIN_TOTAL_INCREASE;
    _layers[_numLayers-1] = getNewLayer();
    return true;
}
