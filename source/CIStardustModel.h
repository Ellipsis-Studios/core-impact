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

class StardustModel {
private:
    /** Color code of this stardust */
    CIColor::Value _color;
    /** Player ID of the player that last interacted with it */
    int _player;
    /** Radius of the stardust in pixels */
    float _radius;
    /** Mass/weight of the stardust. Used in collisions and physics. */
    float _mass;

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
    void setVelocity(cugl::Vec2 value) {
        _velocity = value;
    }
    
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
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The color code of the stardust
     * @param ang The initial angle of rotation
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
