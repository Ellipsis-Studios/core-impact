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
#include "CIOpponentNode.h"
#include "CILocation.h"

class OpponentPlanet : public PlanetModel {
private:
    /** Scene graph node for the opponent planet */
    std::shared_ptr<OpponentNode> _opponentNode;
    /** The corner that this opponent planet is in */
    CILocation::Value _location;

public:
#pragma mark Properties
    /**
     * Sets the textures for this opponent planet.
     *
     * @param texture  The texture of the progress bar
     * @param bounds    The size of the game screen
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& texture, cugl::Size bounds);
    
    void setLocation(CILocation::Value location) {
        _location = location;
    }
    
    const CILocation::Value getLocation() const {
        return _location;
    }
    
    const std::shared_ptr<cugl::scene2::SceneNode> getOpponentNode() const {
        return _opponentNode;
    }
    
#pragma mark Constructors
    /**
     * Returns a newly allocated opponent planet with the given color
     *
     * This method does NOT create a scene graph node for this planet.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The initial color code of the opponent planet
     * @param location The location of the opponent planet
     *
     * @return a newly allocated opponent planet at the given location with the given color.
     */
    static std::shared_ptr<OpponentPlanet> alloc(float x, float y, CIColor::Value c, CILocation::Value location) {
        std::shared_ptr<OpponentPlanet> result = std::make_shared<OpponentPlanet>();
        result->setLocation(location);
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
