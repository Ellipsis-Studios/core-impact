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
#include <map>
#include "CICollisionController.h"
#include "CILocation.h"

/** Impulse for giving collisions a slight bounce. */
#define COLLISION_COEFF     0.1f

using namespace cugl;

/**
 *  Handles collisions between a planet and stardust.
 *
 *  Increases or decreases layer planet size depending on the color of the stardust
 *
 *  @param planet     The planet in the candidate collision
 *  @param queue       The stardust queue
 *  @return true if any stardust collided with the planet
 */
bool collisions::checkForCollision(const std::shared_ptr<PlanetModel>& planet, const std::shared_ptr<StardustQueue>& queue, float timestep) {
    // Get the stardust size from the texture
    float sdRadius = queue->getStardustRadius();
    bool wasCollision = false;
    
    for(size_t ii = 0; ii < queue->size(); ii++) {
        // This returns a reference
        StardustModel* stardust = queue->get(ii);
        if (stardust != nullptr && stardust->isInteractable()) {
            Vec2 norm = planet->getPosition() - stardust->getPosition();
            float distance = norm.length();
            //TODO update with planet radius
            float impactDistance = planet->getRadius()+sdRadius;
            norm.normalize();

            // If this normal is too small, there was a collision
            if (distance < impactDistance) {
                // We add a layer due to own colored stardust
                if (planet->getColor() == CIColor::getNoneColor()) {
                    planet->setColor(stardust->getColor());
                    planet->increaseLayerSize();
                }
                else if (stardust->getColor() == planet->getColor()) {
                    planet->increaseLayerSize();
                }
                // We remove a layer due to different colored stardust
                else {
                    planet->decreaseLayerSize();
                    
                    // another player has hit this planet with a stardust they sent
                    if (stardust->getPreviousOwner() != -1) {
                        queue->addToSendQueue(stardust);
                    }
                }
                
                Vec2 vel = stardust->getVelocity();
                // Compute the impulse (see Essential Math for Game Programmers)
                float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
                    (norm.dot(norm) * (1 / stardust->getMass() + 1 / planet->getMass()));

                Vec2 temp = 1.4 * norm * (impulse/stardust->getMass());
                queue->createStardustParticleBlast(stardust->getPosition() + stardust->getVelocity(), (stardust->getVelocity()+temp) * 0.6, stardust->getColor(), planet->getColor());
                wasCollision = true;
                // Destroy the stardust
                stardust->destroy();
            } else {
                float force = timestep * 60 * 9.81f *
                    (stardust->getMass() * stardust->getMass()) *
                    planet->getMass() / (distance * distance);
                
                // handle game settings 
                force *= planet->getGravStrength();

                stardust->setVelocity(((force / stardust->getMass()) * 1.0f)*norm + stardust->getVelocity());
            }
        }
    }
    return wasCollision;
}

/**
 *  Handles collisions between stardusts, causing them to bounce off one another.
 *
 *  This method updates the velocities of both stardusts: the collider and the
 *  collidee. Therefore, you should only call this method for one of the
 *  stardusts, not both. Otherwise, you are processing the same collisions twice.
 *
 *  @param queue    The stardust queue
 *  @return true if there were any collisions outside of the cooldown period
 */
bool collisions::checkForCollisions(const std::shared_ptr<StardustQueue>& queue) {
    // Get the stardust size from the texture
    float sdRadius = queue->getStardustRadius();
    bool wasCollision = false;

    for (size_t ii = 0; ii < queue->size(); ii++) {
        // This returns a reference
        StardustModel* stardust1 = queue->get(ii);
        for (size_t jj = ii+1; jj < queue->size(); jj++) {
            StardustModel* stardust2 = queue->get(jj);
            if (stardust1 != nullptr && stardust2 != nullptr &&
                stardust1->isInteractable() && stardust2->isInteractable()) {
                Vec2 norm = stardust1->getPosition() - stardust2->getPosition();
                float distance = norm.length();
                float impactDistance = 1.8 * sdRadius;
                norm.normalize(); 

                // If this normal is too small, there was a collision
                if (distance < impactDistance) {
                    // "Roll back" time so that the stardusts are barely touching (e.g. point of impact).
                    Vec2 temp = norm * ((impactDistance - distance) / 2);
                    stardust1->setPosition(stardust1->getPosition()+temp);
                    stardust2->setPosition(stardust2->getPosition()-temp);

                    // Now it is time for Newton's Law of Impact.
                    // Convert the two velocities into a single reference frame
                    Vec2 vel = stardust1->getVelocity() - stardust2->getVelocity();

                    // Compute the impulse (see Essential Math for Game Programmers)
                    float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
                        (norm.dot(norm) * (1 / stardust1->getMass() + 1 / stardust2->getMass()));

                    temp = norm * (impulse/stardust1->getMass());
                    stardust1->setVelocity(stardust1->getVelocity()+temp);

                    temp = norm * (impulse/stardust2->getMass());
                    stardust2->setVelocity(stardust2->getVelocity()-temp);
                    
                    if (stardust1->getHitCooldown() == 0 && stardust2->getHitCooldown() == 0) {
                        queue->createStardustParticleBlast(stardust1->getPosition().getMidpoint(stardust2->getPosition()), stardust1->getVelocity().getMidpoint(stardust2->getVelocity()), stardust1->getColor(), stardust2->getColor());
                        wasCollision = true;
                        stardust1->triggerHit();
                        stardust2->triggerHit();
                    }
                }
            }
        }
    }
    return wasCollision;
}

/**
 *  Checks for a collision between a planet and the input position
 *
 *  @param planet     The planet in the candidate collision
 *  @param inputPos     The input position of the finger
 *  @return true if inputPos is inside the planet
 */
bool collisions::checkForCollision(const std::shared_ptr<PlanetModel>& planet, Vec2 inputPos) { 
    Vec2 norm = inputPos - planet->getPosition();
    float distance = norm.length();
    return distance <= planet->getRadius();
}

/**
 *  Checks for a collision between a planet and any touch on the screen. If there is a collision,
 *  it updates holdingPlanetTouchId to contain the touch id of the colliding touch.
 *
 *  @param planet The planet in the candidate collision
 *  @param touchInstances The locations of touches on the screen, mapped by touch id
 *  @param draggedStardust The stardusts that are being dragged, mapped by touch id
 *  @param holdingPlanetTouchId The touch id of the touch that was last holding on the planet
 *  @return true if any touch that is not holding a stardust is inside the planet
 */
bool collisions::checkForCollision(const std::shared_ptr<PlanetModel>& planet,
                                   std::map<Uint64, TouchInstance>* touchInstances,
                                   std::map<Uint64, StardustModel*>* draggedStardust,
                                   Uint64 &holdingPlanetTouchId) {
    // check if finger that was holding planet is still holding
    if (holdingPlanetTouchId != 0 && touchInstances->count(holdingPlanetTouchId) > 0) {
        if (draggedStardust->count(holdingPlanetTouchId) == 0 && checkForCollision(planet, touchInstances->find(holdingPlanetTouchId)->second.position)) {
            return true;
        } else {
            holdingPlanetTouchId = 0;
        }
    } else {
        holdingPlanetTouchId = 0;
    }
    
    // check other instances
    for (auto it = touchInstances->begin(); it != touchInstances->end(); it++) {
        if (draggedStardust->count(it->first) == 0 && checkForCollision(planet, it->second.position)) {
            holdingPlanetTouchId = it->first;
            return true;
        }
    }
    holdingPlanetTouchId = 0;
    
    return false;
}

/**
 * Finds the closest stardust that collides with the input position
 *
 * @param inputPos     The input position of the finger
 * @param queue        The stardust queue
 */
StardustModel* collisions::getNearestStardust(Vec2 inputPos, const std::shared_ptr<StardustQueue>& queue) {
    float sdRadius = queue->getStardustRadius();
    StardustModel* closest = NULL;
    float closestDistance = 0;

    for (size_t ii = 0; ii < queue->size(); ii++) {
        // This returns a reference
        StardustModel* stardust = queue->get(ii);
        if (stardust != nullptr && !stardust->isDragged() && stardust->isInteractable()) {
            Vec2 norm = inputPos - stardust->getPosition();
            float distance = norm.length();

            // check of inputPos is near stardust
            if (distance < sdRadius * 2) {
                // check if this is first one touching or closer than existing closest
                if (closest == NULL || distance < closestDistance) {
                    closest = stardust;
                    closestDistance = distance;
                }
            }
        }
    }
    return closest;
}

/**
 * Move the given stardust towards the input position
 *
 * @param inputPos The position of the current input
 * @param stardust The stardust to move
 * @param sdRaduis The radius of the stardust
 */
void collisions::moveDraggedStardust(Vec2 inputPos, StardustModel* stardust, float sdRadius) {
    Vec2 norm = inputPos - stardust->getPosition();
    float distance = norm.length();
    norm.normalize();
    if (distance < sdRadius) {
        stardust->setVelocity(Vec2::ZERO);
    } else {
        stardust->setVelocity(norm * sqrt(distance));
    }
}

/**
 * Destroy any stardust that leaves the bounds
 *
 * @param queue   The stardust queue
 * @param bounds    The rectangular bounds of the playing field
 */
void collisions::checkInBounds(const std::shared_ptr<StardustQueue>& queue, const Size bounds) {
    for (size_t ii = 0; ii < queue->size(); ii++) {
        // This returns a reference
        StardustModel* stardust = queue->get(ii);
        if (stardust != nullptr && stardust->isInteractable()) {
            Vec2 stardustPos = stardust->getPosition();
            Vec2 center = Vec2(bounds.width/2, bounds.height/2);
            Vec2 longest = Vec2(bounds.width, bounds.height) - center;
            Vec2 distance = (stardustPos - center);
            if (distance.length() > longest.length() + 50) {
                
                // set stardust's off screen location if it is not in bounds
                if (distance.x < 0 && distance.y < 0) {
                    stardust->setStardustLocation(CILocation::Value::BOTTOM_LEFT);
                } else if (distance.x > 0 && distance.y < 0) {
                    stardust->setStardustLocation(CILocation::Value::BOTTOM_RIGHT);
                } else if (distance.x < 0 && distance.y > 0) {
                    stardust->setStardustLocation(CILocation::Value::TOP_LEFT);
                } else if (distance.x > 0 && distance.y > 0) {
                    stardust->setStardustLocation(CILocation::Value::TOP_RIGHT);
                }
                stardust->destroy();
                queue->addToSendQueue(stardust);
            }
        }
    }
}

