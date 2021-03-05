//
//  CIPlanetModel.h
//  CoreImpact
//
//  Created by William Long on 3/4/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLANET_MODEL_H__
#define __CI_PLANET_MODEL_H__
#include <cugl/cugl.h>
#include "CIColor.h"

class PlanetModel {
private:
    /** Current color code of the outermost layer of this planet */
    CIColor::Value _color;
    
    std::vector<CIColor::Value> _prevLayerColors;
    
    /** The number of layers this planet has */
    int _numLayers;
    /** The number of dots the player has put in for the current layer */
    int _currLayerProgress;
    /** The total number of dots the player need to put in to lock in the current layer  */
    int _layerLockinTotal;
    
    /** Radius of the planet in pixels */
    float _radius;
    /** Mass/weight of the planet. Used in collisions and physics. */
    float _mass;
    /** Position of the planet in world space */
    cugl::Vec2 _position;

public:
#pragma mark Properties
    /**
     * Returns the color of this planet's current layer
     *
     * @return the color of the planet
     */
    const CIColor::Value getColor() const {
        return _color;
    }

    /**
     * Returns the number of layers this planet has
     *
     * @return the numer of layers of this planet
     */
    const int getNumLayers() const {
        return _numLayers;
    }
    
    /**
     * Returns the total number of dots added to the current layer
     *
     * @return the numer of dots added to the current layer
     */
    const int getCurrLayerProgress() const {
        return _currLayerProgress;
    }
    
    /**
     * Returns the total number of dots need to lock in the current layer
     *
     * @return the numer of dots needed to lock in the current layer
     */
    const int getLayerLockinTotal() const {
        return _layerLockinTotal;
    }
    
    /**
     * Returns the radius of the planet
     *
     * @return the radisu of the planet
     */
    const float getRadius() const {
        return _radius;
    }
    
    /**
     * Returns the mass of the planet
     *
     * @return the mass of the planet
     */
    const float getMass() const {
        return _mass;
    }

    const cugl::Vec2 getPosition() const {
        return _position;
    }

#pragma mark Constructors
    /**
     * Creates a new planet at the center of the screen
     */
    PlanetModel() {}

    /**
     * Destroys this planet, releasing all resources.
     */
    ~PlanetModel() { dispose(); }

    /**
     * Disposes all resources and assets of this planet
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a planet may not be used until it is initialized again.
     */
    void dispose();
        
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
    bool init(float x, float y, CIColor::Value c, int maxLayers);
        
    /**
     * Returns a newly allocated planet with the given color
     *
     * This method does NOT create a scene graph node for this dot.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The initial color code of the planet
     * @param maxLayers The maximum number of layers the planet can have
     *
     * @return a newly allocated dot at the given location with the given color.
     */
    static std::shared_ptr<PlanetModel> alloc(float x, float y, CIColor::Value c, int maxLayers) {
        std::shared_ptr<PlanetModel> result = std::make_shared<PlanetModel>();
        return (result->init(x, y, c, maxLayers) ? result : nullptr);
    }

#pragma mark Interactions
    /**
     * Decreases the size of the current layer
     */
    void decreaseLayerSize();
    
    /**
     * Increases the size of the current layer
     */
    void increaseLayerSize();
    
    /**
     * Locks in the current layer and adds a new layer if the current layer is able to be locked in.
     *
     * @return true if the layer lock in was successful
     */
    bool lockInLayer();
    
};


#endif /* __CI_PLANET_MODEL_H__ */
