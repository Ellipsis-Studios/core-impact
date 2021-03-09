//
//  CIDotModel.cpp
//  Core Impact
//
//  This model class represents all the information for a single dot.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIDotModel.h"

#pragma mark Properties

/**
 * Sets the position of this dot.
 *
 * This is location of the center pixel of the dot on the screen.
 *
 * @param value the position of this dot
 */
void DotModel::setPosition(cugl::Vec2 value) {
    _position = value;
}

#pragma mark Constructors

DotModel::DotModel() {
    _position.set(0,0);
    _color = CIColor::magenta;
}

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
bool DotModel::init(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c) {
    _position = position;
    _color = c;
    _mass = 1;
    _radius = 1;
    _velocity = velocity;
    return true;
}

/**
 * Disposes the dot in queue, releasing all resources.
 */
void DotModel::dispose() {
    _mass = 0;
}

#pragma mark Movement

/**
 * Moves the dot one animation frame
 */
void DotModel::update(float timestep) {
    _position += _velocity;
}

/**
 * Flags the dot for deletion.
 *
 * This just sets the mass of the dot to be negative.
 * That way it is removed soon after during the collection phase.
 */
void DotModel::destroy() {
    _mass = -1;
}
