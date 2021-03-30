//
//  CIOpponentPlanet.h
//  CoreImpact
//
//  This class represents all the information for an opponent's planet.
//  It makes use of the PlanetModel class, but in a degenerate case where
//  all information is maintainted in one layer
//
//  Created by Brandon Stein on 3/29/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_OPPONENT_PLANET_H__
#define __CI_OPPONENT_PLANET_H__

#include "CIPlanetModel.h"

class OpponentPlanet : public PlanetModel {

public:
#pragma mark Constructors
    /**
     * Returns a newly allocated opponent planet with the given color
     *
     * This method does NOT create a scene graph node for this planet.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The initial color code of the planet
     *
     * @return a newly allocated opponent planet at the given location with the given color.
     */
    static std::shared_ptr<OpponentPlanet> alloc(float x, float y, CIColor::Value c) {
        std::shared_ptr<OpponentPlanet> result = std::make_shared<OpponentPlanet>();
        return (result->init(x, y, c, 1) ? result : nullptr);
    }
    
#pragma mark Interactions
    /**
     * Decreases the size of the current layer
     */
    void decreaseLayerSize() override;
    
    /**
     * Increases the size of the current layer
     */
    void increaseLayerSize() override;
    
    /**
     * Sets the mass of the planet
     *
     * @param mass The new mass of this planet
     */
    void setMass(float mass);
    
};

#endif /* __CI_OPPONENT_PLANET_H__ */
