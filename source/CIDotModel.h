//
//  CIDotModel.h
//  CoreImpact
//
//  This model class represents all the information for a single dot.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_DOT_MODEL_H__
#define __CI_DOT_MODEL_H__
#include "CIColor.h"

class DotModel {
private:
    /** Color code of this dot */
    CIColor::Value _color;
    /** Player ID of the player that last interacted with it */
    int _player;
    /** Radius of the dot in pixels */
    float _radius;
    /** Mass/weight of the dot. Used in collisions and physics. */
    float _mass;

protected:
    /** Position of the dot in world space */
    cugl::Vec2 _position;
    /** Current dot velocity */
    cugl::Vec2 _velocity;
    
public:
#pragma mark Properties
    /**
     * Returns the position of this dot.
     *
     * This is location of the center pixel of the dot on the screen.
     *
     * @return the position of this dot
     */
    const cugl::Vec2& getPosition() const {
        return _position;
    }
    
    /**
     * Sets the position of this dot.
     *
     * This is location of the center pixel of the dot on the screen.
     *
     * @param value the position of this dot
     */
    void setPosition(cugl::Vec2 value);

    /**
     * Returns the velocity of this dot.
     *
     * This value is necessary to control momementum in dot movement.
     *
     * @return the velocity of this dot
     */
    const cugl::Vec2& getVelocity() const {
        return _velocity;
    }

    /**
     * Sets the velocity of this dot.
     *
     * This value is necessary to control momementum in dot movement.
     *
     * @param value the velocity of this dot
     */
    void setVelocity(cugl::Vec2 value) {
        _velocity = value;
    }
    
    /**
     * Returns the mass of the dot.
     *
     * This value is necessary to resolve collisions.
     *
     * @return the dot's mass
     */
    float getMass() const {
        return _mass;
    }

    /**
     * Returns the radius of the dot.
     *
     * This value is necessary to resolve collisions.
     *
     * @return the dot's radius
     */
    float getRadius() {
        return _radius;
    }

    CIColor::Value getColor() {
        return _color;
    }
    
#pragma mark Constructors
    /**
     * Creates a new dot at the origin.
     */
    DotModel();

    /**
     * Destroys this dot, releasing all resources.
     */
    ~DotModel() { dispose(); }

    /**
     * Disposes all resources and assets of this dot
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a dot may not be used until it is initialized again.
     */
    void dispose();
        
    /**
     * Initializes a new dot at the given location
     *
     * This method does NOT create a scene graph node for this dot.  You
     * must call setTexture for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The color code of the dot
     *
     * @return true if the initialization was successful
     */
    bool init(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c);
        
    /**
     * Returns a newly allocated dot at the given location
     *
     * This method does NOT create a scene graph node for this dot.  You
     * must call setTextures for that.
     *
     * @param x The initial x-coordinate of the center
     * @param y The initial y-coordinate of the center
     * @param c The color code of the dot
     * @param ang The initial angle of rotation
     *
     * @return a newly allocated dot at the given location with the given color.
     */
    static std::shared_ptr<DotModel> alloc(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c) {
        std::shared_ptr<DotModel> result = std::make_shared<DotModel>();
        return (result->init(position, velocity, c) ? result : nullptr);
    }

    /**
    * Flags the dot for deletion.
    *
    * This just sets the mass of the dot to be negative.
    * That way it is removed soon after during the collection phase.
    */
    void destroy();
    
#pragma mark -
#pragma mark Movement
    /**
     * Updates the state of the model
     *
     * This method moves the dot in accordance with the forces applied.
     *
     * @param timestep  Time elapsed since last called.
     */
    void update(float timestep=0.0f);
};

#endif /* __CI_DOT_MODEL_H__ */
