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
    void setTextures(const std::shared_ptr<cugl::Texture>& texture, const std::shared_ptr<cugl::Texture>& fogTexture, cugl::Size bounds);
    
    /**
     * Set the player name associated with this opponent planet
     *
     * @param name The name to display
     * @param font The font to use for the name
     */
    void setName(std::string name, std::shared_ptr<cugl::Font> font);
    
    /**
     * Set the location of this opponent planet
     *
     * @param location The location of this opponent planet
     */
    void setLocation(CILocation::Value location) {
        _location = location;
    }
    
    /**
     * Returns the location of this opponent planet
     *
     * @return The location of this opponent planet
     */
    const CILocation::Value getLocation() const {
        return _location;
    }
    
    /**
     * Get the OpponentNode associated with this opponent planet
     */
    const std::shared_ptr<OpponentNode> getOpponentNode() const {
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
        return (result->init(x, y, c) ? result : nullptr);
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
    
    /**
     * Updates the animations for this opponent planet.
     *
     * @param timestep the amount of time since the last animation frame
     */
    void update(float timestep) override;
    
};

#endif /* __CI_OPPONENT_PLANET_H__ */
