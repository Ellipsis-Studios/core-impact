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
#include "CIOpponentNode.h"
#include "CIColor.h"

#define INITIAL_PLANET_MASS            25
#define PLANET_MASS_DELTA              10

/**
 * Sets the textures for this opponent planet.
 *
 * @param texture  The texture of the progress bar
 * @param bounds    The size of the game screen
 */
void OpponentPlanet::setTextures(const std::shared_ptr<cugl::Texture>& texture, const std::shared_ptr<cugl::Texture>& fogTexture, cugl::Size bounds) {
    _opponentNode = OpponentNode::alloc(texture, bounds.width/2, bounds.height/2);
    _opponentNode->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
    _opponentNode->setPosition(_position);
    _opponentNode->setLocation(_location);
    _opponentNode->setProgress(_mass / (_layerLockinTotal * _winPlanetLayers * PLANET_MASS_DELTA + INITIAL_PLANET_MASS), getColor());
    _opponentNode->setFogTexture(fogTexture);
}

/**
 * Set the player name associated with this opponent planet
 *
 * @param name The name to display
 * @param font The font to use for the name
 */
void OpponentPlanet::setName(std::string name, std::shared_ptr<cugl::Font> font) {
    if (_opponentNode != nullptr) {
        _opponentNode->setName(name, font);
    }
}

/**
 * Starts the animation of the progress bar flashing
 */
void OpponentPlanet::startHitAnimation() {
    if (_opponentNode != nullptr) {
        _opponentNode->startHitAnimation();
    }
}

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
    if (_opponentNode != nullptr) {
        _opponentNode->setProgress(mass / (_layerLockinTotal * _winPlanetLayers * PLANET_MASS_DELTA + INITIAL_PLANET_MASS), getColor());
    }
}

/**
 * Updates the animations for this opponent planet.
 *
 * @param timestep the amount of time since the last animation frame
 */
void OpponentPlanet::update(float timestep) {
    _opponentNode->update(timestep);
}
