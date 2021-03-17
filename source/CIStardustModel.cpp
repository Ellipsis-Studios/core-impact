//
//  CIStardustModel.cpp
//  CoreImpact
//
//  This model class represents all the information for a single stardust.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIStardustModel.h"

/** Maximum speed of a stardust */
#define MAX_SPEED     10.0f

#pragma mark Properties

/**
 * Sets the velocity of this stardust.
 *
 * This value is necessary to control momementum in stardust movement.
 *
 * @param value the velocity of this stardust
 */
void StardustModel::setVelocity(cugl::Vec2 value) {
    _velocity = value;
    if (_velocity.length() > MAX_SPEED) {
        _velocity.scale(MAX_SPEED / _velocity.length());
    }
}

#pragma mark Constructors

/**
 * Creates a new stardust at the origin.
 */
StardustModel::StardustModel() {
    _position.set(0,0);
    _color = CIColor::blue;
}

/**
 * Destroys this stardust, releasing all resources.
 */
void StardustModel::dispose() {
    _mass = 0;
}

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
bool StardustModel::init(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c) {
    _position = position;
    _color = c;
    _mass = 1;
    _radius = 1;
    _velocity = velocity;
    _stardust_location = Location::ON_SCREEN;
    return true;
}

/**
* Flags the stardust for deletion.
*
* This just sets the mass of the stardust to be negative.
* That way it is removed soon after during the collection phase.
*/
void StardustModel::destroy() {
    _mass = -1;
}

#pragma mark Movement

/**
 * Updates the state of the model
 *
 * This method moves the stardust in accordance with the forces applied.
 *
 * @param timestep  Time elapsed since last called.
 */
void StardustModel::update(float timestep) {
    _position += _velocity;
}
