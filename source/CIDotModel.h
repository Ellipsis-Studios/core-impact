//
//  CIDotModel.h
//  Core Impact
//
//  This model class represents all the information for a single dot.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef CIDotModel_hpp
#define CIDotModel_hpp
#include "CIColor.h"

class DotModel {
private:
    /** Color code of this dot */
    CIColor::Value _color;
    /** Position of the ship in world space */
    cugl::Vec2 _position;
    /** Current dot velocity */
    cugl::Vec2 _velocity;
    
    /** Radius of the dot in pixels */
    float _radius;
    /** Mass/weight of the dot. Used in collisions and physics. */
    float _mass;


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
    bool init(float x, float y, CIColor::Value c);
        
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
    static std::shared_ptr<DotModel> alloc(float x, float y, CIColor::Value c) {
        std::shared_ptr<DotModel> result = std::make_shared<DotModel>();
        return (result->init(x,y,c) ? result : nullptr);
    }

#pragma mark Movement
    /**
     * Moves the dot one animation frame
     */
    void update();
    
};

#endif /* CIDotModel_hpp */
