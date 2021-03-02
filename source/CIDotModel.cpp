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
bool DotModel::init(float x, float y, CIColor::Value c) {
    _position.set(x,y);
    _color = c;
    return true;
}

#pragma mark Movement

/**
 * Moves the dot one animation frame
 */
void DotModel::update() {
    _position += _velocity;
}
