//
//  CIOpponentPlanet.cpp
//  CoreImpact
//
//  This class represents all the information for an opponent's planet.
//  It makes use of the PlanetModel class, but in a degenerate case where
//  all information is maintainted in one layer
//
//  Created by Brandon Stein on 3/29/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIOpponentPlanet.h"

/**
 * Decreases the size of the current layer
 */
void OpponentPlanet::decreaseLayerSize() {
    PlanetLayer* currentLayer = &_layers[_numLayers-1];
    if (currentLayer->layerSize > 0) {
        currentLayer->layerSize--;
    }
}

/**
 * Increases the size of the current layer
 */
void OpponentPlanet::increaseLayerSize() {
    _layers[_numLayers-1].layerSize++;
}

/**
 * Sets the mass of the planet
 *
 * @param mass The new mass of this planet
 */
void OpponentPlanet::setMass(float mass) {
    _mass = mass;
}
