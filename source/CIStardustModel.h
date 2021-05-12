//
//  CIStardustModel.h
//  CoreImpact
//
//  This model class represents all the information for a single stardust.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_STARDUST_MODEL_H__
#define __CI_STARDUST_MODEL_H__
#include "CIColor.h"
#include "CILocation.h"

class StardustModel {
public:
    /**
     * Enum representing the types of stardust.
     */
    enum Type {
        NORMAL = 0,
        METEOR = 1,
        SHOOTING_STAR = 2,
        GRAYSCALE = 3,
        FOG = 4,
    };
private:
    /** Color code of this stardust */
    CIColor::Value _color;
    /** Player ID of the player that last interacted with it */
    int _player;
    /** Radius of the stardust in pixels */
    float _radius;
    /** Mass/weight of the stardust. Used in collisions and physics. */
    float _mass;
    /** Whether this stardust is being dragged */
    bool _isDragged;
    
    /** The location of the stardust */
    CILocation::Value _stardust_location;
    /** The player id of the last player to own this stardust. -1 if this stardust does not have a previous owner */
    int _previous_owner;
    /** The type of stardust this is. By default it will be a normal stardust. */
    Type _stardust_type;

protected:
    /** Position of the stardust in world space */
    cugl::Vec2 _position;
    /** Current stardust velocity */
    cugl::Vec2 _velocity;
    
public:
#pragma mark Properties
    /**
     * Returns the position of this stardust.
     *
     * This is location of the center pixel of the stardust on the screen.
     *
     * @return the position of this stardust
     */
    const cugl::Vec2& getPosition() const {
        return _position;
    }
    
    /**
     * Sets the position of this stardust.
     *
     * This is location of the center pixel of the stardust on the screen.
     *
     * @param value the position of this stardust
     */
    void setPosition(cugl::Vec2 value) {
        _position = value;
    }

    /**
     * Returns the velocity of this stardust.
     *
     * This value is necessary to control momementum in stardust movement.
     *
     * @return the velocity of this stardust
     */
    const cugl::Vec2& getVelocity() const {
        return _velocity;
    }

    /**
     * Sets the velocity of this stardust.
     *
     * This value is necessary to control momementum in stardust movement.
     *
     * @param value the velocity of this stardust
     */
    void setVelocity(cugl::Vec2 value);
    
    /**
     * Returns the mass of the stardust.
     *
     * This value is necessary to resolve collisions.
     *
     * @return the stardust's mass
     */
    float getMass() const {
        return _mass;
    }

    /**
     * Returns the radius of the stardust.
     *
     * This value is necessary to resolve collisions.
     *
     * @return the stardust's radius
     */
    float getRadius() {
        return _radius;
    }

    /**
     * Returns the color of the stardust
     *
     * @return the stardust's color
     */
    CIColor::Value getColor() {
        return _color;
    }
    
    /**
     * Returns the location of the stardust
     *
     * @return the stardust's location
     */
    CILocation::Value getStardustLocation() {
        return _stardust_location;
    }
    
    /**
     * Sets the location of the stardust
     *
     * @param location      the stardust's location
     */
    void setStardustLocation(CILocation::Value location) {
        _stardust_location = location;
    }
    
    /**
     * Returns the previous owner of this stardust
     *
     * @return the previous owner of this stardust
     */
    int getPreviousOwner() {
        return _previous_owner;
    }
    
    /**
     * Sets the previous owner of the stardust
     *
     * @param playerId      the playerId of the previous owner of this stardust
     */
    void setPreviousOwner(int playerId) {
        _previous_owner = playerId;
    }
    
    /**
     * Returns the type of this stardust
     *
     * @return the type of this stardust
     */
    Type getStardustType() {
        return _stardust_type;
    }
    
    /**
     * Sets the type of the stardust
     *
     * @param type      the type of the stardust
     */
    void setStardustType(Type type) {
        _stardust_type = type;
    }
    
    /**
     * Returns whether this stardust is being dragged
     *
     * @return whether this stardust is being dragged
     */
    bool isDragged() {
        return _isDragged;
    }
    
    /**
     * Sets whether this stardust is being dragged
     *
     * @param value  whether this stardust is being dragged
     */
    void setIsDragged(bool value) {
        _isDragged = value;
    }

#pragma mark Constructors
    /**
     * Creates a new stardust at the origin.
     */
    StardustModel();

    /**
     * Destroys this stardust, releasing all resources.
     */
    ~StardustModel() { dispose(); }

    /**
     * Disposes all resources and assets of this stardust
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a stardust may not be used until it is initialized again.
     */
    void dispose();
        
    /**
     * Initializes a new stardust at the given location
     *
     * This method does NOT create a scene graph node for this stardust.  You
     * must call setTexture for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The color code of the stardust
     *
     * @return true if the initialization was successful
     */
    bool init(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c);
        
    /**
     * Returns a newly allocated stardust at the given location
     *
     * This method does NOT create a scene graph node for this stardust.  You
     * must call setTextures for that.
     *
     * @param position The position of the stardust
     * @param velocity The velocity of the stardust
     * @param c The color code of the stardust
     *
     * @return a newly allocated stardust at the given location with the given color.
     */
    static std::shared_ptr<StardustModel> alloc(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c) {
        std::shared_ptr<StardustModel> result = std::make_shared<StardustModel>();
        return (result->init(position, velocity, c) ? result : nullptr);
    }

    /**
    * Flags the stardust for deletion.
    *
    * This just sets the mass of the stardust to be negative.
    * That way it is removed soon after during the collection phase.
    */
    void destroy();
    
#pragma mark -
#pragma mark Movement
    /**
     * Updates the state of the model
     *
     * This method moves the stardust in accordance with the forces applied.
     *
     * @param timestep  Time elapsed since last called.
     */
    void update(float timestep=0.0f);
};

#endif /* __CI_STARDUST_MODEL_H__ */
