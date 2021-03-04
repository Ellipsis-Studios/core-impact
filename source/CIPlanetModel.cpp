//
//  CIPlanetModel.cpp
//  ShipDemo
//
//  Created by William Long on 3/4/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetModel.h"

#define PLANET_RADIUS_INCREASE          5
#define PLANET_MASS_INCREASE            10
#define INIT_LAYER_LOCKIN_TOTAL         5
#define LAYER_LOCKIN_TOTAL_INCREASE     1

#pragma mark Constructors
/**
 * Disposes the planet, releasing all resources.
 */
void PlanetModel::dispose() {
    _prevLayerColors.clear();
}

/**
 * Initializes a new planet with the given color
 *
 * This method does NOT create a scene graph node for this dot.  You
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
    _color = c;
    _prevLayerColors.resize(maxLayers);
    _layerLockinTotal = INIT_LAYER_LOCKIN_TOTAL;
    return true;
}


#pragma mark Interactions

/**
 * Decreases the size of the current layer
 */
void PlanetModel::decreaseLayerSize() {
    _currLayerProgress--;
    _radius -= PLANET_RADIUS_INCREASE;
    _mass -= PLANET_MASS_INCREASE;
}

/**
 * Increases the size of the current layer
 */
void PlanetModel::increaseLayerSize() {
    _currLayerProgress++;
    _radius += PLANET_RADIUS_INCREASE;
    _mass += PLANET_MASS_INCREASE;
}

/**
 * Attempts to lock in the current layer and adds a new one
 */
bool PlanetModel::lockInLayer() {
    // do not lock in if there is not enough dots to do a lock in or the planet already has the max number of layers
    if (_currLayerProgress < _layerLockinTotal || _numLayers >= _prevLayerColors.size()) {
        return false;
    }
    
    _numLayers++;
    _currLayerProgress = 0;
    _layerLockinTotal += LAYER_LOCKIN_TOTAL_INCREASE;
    
    _prevLayerColors[_numLayers-1] = _color;
    // TODO: add in logic to change color of the layer
    return true;
}
