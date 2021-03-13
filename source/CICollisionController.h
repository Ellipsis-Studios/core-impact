//
//  CICollisionController.h
//  CoreImpact
//
//  Unless you are making a point-and-click adventure game, every single
//  game is going to need some sort of collision detection.  In a later
//  lab, we will see how to do this with a physics engine. For now, we use
//  custom physics.
//
//  You might ask why we need this file when we have Box2d. That is because
//  we are trying to make this code as close to that of 3152 as possible. At
//  this point in the semester of 3152, we had not covered Box2d.
//
//  However, you will notice that this is NOT A CLASS.  The collision
//  controller in 3152 did not have any state to speak of (it had some cache
//  objects that are completely unnecessary in C++.  So we can just do this
//  as a collection of functions.  But if you do that, we recommend that
//  you put the functions together in a namespace, like we have done here.
//
//  Author: Walker M. White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#ifndef __CI_COLLISION_CONTROLLER_H__
#define __CI_COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>
#include "CIPlanetModel.h"
#include "CIStardustModel.h"
#include "CIStardustQueue.h"

/**
 * Namespace of functions implementing simple game physics.
 *
 * This is the simplest of physics engines. In reality, you will
 * probably use Box2d just like you did in 3152.
 */
namespace collisions {

/**
 *  Handles collisions between a planet and stardust.
 *
 *  Increases or decreases layer planet size depending on the color of the stardust
 *
 *  @param planet     The planet in the candidate collision
 *  @param queue       The stardust queue
 */
void checkForCollision(const std::shared_ptr<PlanetModel>& planet, const std::shared_ptr<StardustQueue>& queue);

/**
 *  Handles collisions between stardusts, causing them to bounce off one another.
 *
 *  This method updates the velocities of both stardusts: the collider and the
 *  collidee. Therefore, you should only call this method for one of the
 *  stardusts, not both. Otherwise, you are processing the same collisions twice.
 *
 *  @param queue    The stardust queue
 */
void checkForCollision(const std::shared_ptr<StardustQueue>& queue);

/**
 *  Handles collisions between an input and stardust.
 *
 *  Moves the stardust to follow the input
 *
 *  @param inputPos     The input position of the finger
 *  @param queue		The stardust queue
 */
void checkForCollision(cugl::Vec2 inputPos, const std::shared_ptr<StardustQueue>& queue);

/**
 * Destroy any stardust that leaves the bounds
 *
 * @param queue   The stardust queue
 * @param bounds    The rectangular bounds of the playing field
 */
void checkInBounds(const std::shared_ptr<StardustQueue>& queue, const cugl::Size bounds);
}

#endif /* __GL_COLLISION_CONTROLLER_H__ */
