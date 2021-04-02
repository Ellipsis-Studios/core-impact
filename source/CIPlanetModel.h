//
//  CIPlanetModel.h
//  CoreImpact
//
//  This model class represents all the information for the planet.
//
//  Created by William Long on 3/4/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLANET_MODEL_H__
#define __CI_PLANET_MODEL_H__
#include <cugl/cugl.h>
#include "CIColor.h"
#include "CIPlanetLayer.h"
#include "CIPlanetNode.h"

class PlanetModel {
private:
    /** The layers of this planet */
    std::vector<PlanetLayer> _layers;
    
    /** The number of layers this planet has */
    int _numLayers;
    /** The total amount of stardust the player need to put in to lock in the current layer  */
    int _layerLockinTotal;
    /** The amount of time for which this planet is being held down on to lock in
        This is equal to 0 if the planet is not currently being locked in */
    float _lockInProgress;
    
    /** Radius of the planet in pixels */
    float _radius;
    /** Mass/weight of the planet. Used in collisions and physics. */
    float _mass;
    /** Position of the planet in world space */
    cugl::Vec2 _position;
    
    /** Scene graph node for the planet */
    std::shared_ptr<PlanetNode> _planetNode;

public:
#pragma mark Properties
  
    void update(float timestep);
  
    /**
     * Returns the color of this planet's current layer
     *
     * @return the color of the planet
     */
    const CIColor::Value getColor() const {
        return _layers[_numLayers-1].layerColor;
    }
    
    /**
     * Sets the color of this planet's current layer
     *
     * @param color The new color of this planet's current layer
     */
    void setColor(CIColor::Value color) {
        _layers[_numLayers-1].layerColor = color;
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
     * Returns the total amount of stardust added to the current layer
     *
     * @return the amount of stardust added to the current layer
     */
    const int getCurrLayerProgress() const {
        return _layers[_numLayers-1].layerSize;
    }
    
    /**
     * Returns the total amount of stardust need to lock in the current layer
     *
     * @return the amount of stardust needed to lock in the current layer
     */
    const int getLayerLockinTotal() const {
        return _layerLockinTotal;
    }
    
    /**
     * Returns whether the planet is in the process of locking in
     *
     * @return true if the planet is currently locking in, false otherwise
     */
    const bool isLockingIn() const {
        return _lockInProgress > 0;
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

    /**
     * Returns the position of the planet
     *
     * @return the position of the planet
     **/
    const cugl::Vec2 getPosition() const {
        return _position;
    }
    
    const PlanetLayer getNewLayer() const {
        return {
            .layerSize =  0,
            .layerColor =  CIColor::getNoneColor(),
            .isActive = true,
            .isLockedIn = false
        };
    }
    
    /**
     * Sets the textures for this planet.
     *
     * @param core          The texture of the core
     * @param ring          The texture of an innner ring
     * @param unlocked The texture on the outside of an unlocked ring
     * @param unlocked The texture on the outside of a locked ring
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& core,
                     const std::shared_ptr<cugl::scene2::AnimationNode>& ring,
                     const std::shared_ptr<cugl::Texture>& unlocked,
                     const std::shared_ptr<cugl::Texture>& locked);
   
    const std::shared_ptr<cugl::scene2::SceneNode> getPlanetNode() const {
        return _planetNode;
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
    bool init(float x, float y, CIColor::Value c, int maxLayers);
        
    /**
     * Returns a newly allocated planet with the given color
     *
     * This method does NOT create a scene graph node for this planet.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The initial color code of the planet
     * @param maxLayers The maximum number of layers the planet can have
     *
     * @return a newly allocated planet at the given location with the given color.
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
     * Stops any current progress towards locking in a layer
     */
    void stopLockIn();
    
    /**
     * Locks in the current layer and adds a new layer if the current layer is able to be locked in.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     * @return true if the layer lock in was successful
     */
    bool lockInLayer(float timestep);
    
};


#endif /* __CI_PLANET_MODEL_H__ */
