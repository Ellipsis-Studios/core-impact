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
#include "CICollisionController.h"

/** Impulse for giving collisions a slight bounce. */
#define COLLISION_COEFF     0.1f
/** The standard mass for a photon. */
#define PHOTON_MASS         5.0f

using namespace cugl;

/**
 *  Handles collisions between a planet and stardust.
 *
 *  Increases or decreases layer planet size depending on the color of the stardust
 *
 *  @param planet     The planet in the candidate collision
 *  @param dots       The stardust in the candidate collision
 */
void collisions::checkForCollision(const std::shared_ptr<PlanetModel>& planet, const std::shared_ptr<DotsQueue>& dots) {
    // Get the photon size from the texture
    auto texture = dots->getTexture();
    float dradius = 0;
    if (texture != nullptr) {
        dradius = std::max(texture->getWidth(),texture->getHeight())/2.0f;
    }
    
    for(size_t ii = 0; ii < dots->size(); ii++) {
        // This returns a reference
        DotModel* dot = dots->get(ii);
        // We add a layer due to own colored dot
        if (dot != nullptr) {
            Vec2 norm = planet->getPosition()-dot->getPosition();
            float distance = norm.length();
//            float impactDistance = (planet->getRadius() + dradius);
            float impactDistance = 32+dradius;
            norm.normalize();

            // If this normal is too small, there was a collision
            if (distance < impactDistance) {
                if (dot->getColor() == planet->getColor()) {
                    planet->increaseLayerSize();
                }
                else {
                    planet->decreaseLayerSize();
                }

                // Destroy the photon
                dot->destroy();
            } else {
                float force = 9.81f * dot->getMass() * planet->getMass() / (distance * distance);
                dot->setVelocity(((force / dot->getMass()) * 1.0f)*norm + dot->getVelocity());
            }
        }
    }

}

/*
void collisions::checkForCollision(const std::shared_ptr<DotsQueue>& dots1, const std::shared_ptr<DotsQueue>& dots2) {
    // Get the photon size from the texture
    auto texture = photons->getTexture();
    float pradius = 0;
    if (texture != nullptr) {
        pradius = std::max(texture->getWidth(), texture->getHeight()) / 2.0f;
    }

    for (size_t ii = 0; ii < photons->size(); ii++) {
        // This returns a reference
        PhotonQueue::Photon* photon = photons->get(ii);
        // We are immune to our own photons
        if (photon != nullptr && photon->ship != ship->getSID()) {
            Vec2 norm = ship->getPosition() - photon->pos;
            float distance = norm.length();
            float impactDistance = (ship->getRadius() + pradius * photon->scale);
            norm.normalize();

            // If this normal is too small, there was a collision
            if (distance < impactDistance) {
                // "Roll back" time so that the ships are barely touching (e.g. point of impact).
                Vec2 temp = norm * ((impactDistance - distance) / 2);
                ship->setPosition(ship->getPosition() + temp);

                // Now it is time for Newton's Law of Impact.
                // Convert the two velocities into a single reference frame
                Vec2 vel = ship->getVelocity() - photon->vel;

                // Compute the impulse (see Essential Math for Game Programmers)
                float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
                    (norm.dot(norm) * (1 / ship->getMass() + 1 / PHOTON_MASS));

                // Change velocity of the two ships using this impulse
                temp = norm * (impulse / ship->getMass());
                ship->setVelocity(ship->getVelocity() + temp);

                // Destroy the photon
                photon->destroy();
            }
        }
    }

}
*/

void collisions::checkForCollision(cugl::Vec2 inputPos, const std::shared_ptr<DotsQueue>& dots) {
    // Get the photon size from the texture
    auto texture = dots->getTexture();
    float dradius = 0;
    if (texture != nullptr) {
        dradius = std::max(texture->getWidth(), texture->getHeight()) / 2.0f;
    }

    for (size_t ii = 0; ii < dots->size(); ii++) {
        // This returns a reference
        DotModel* dot = dots->get(ii);
        // We add a layer due to own colored dot
        if (dot != nullptr) {
            Vec2 dotPos = dot->getPosition();
            Vec2 norm = inputPos - dotPos;
            float distance = norm.length();
            //float impactDistance = (planet->getRadius() + dradius*dot->scale);
            float impactDistance = dradius;
            norm.normalize();

            // If this normal is too small, there was a collision
            if (distance < impactDistance) {
//                cout << "Collision!\n";
                dot->setVelocity(Vec2::ZERO);
            }
            else if (distance < impactDistance * 2) {
                dot->setVelocity(norm * 10);
            }
        }
    }
}

/**
 * Nudge the ship to ensure it does not do out of view.
 *
 * This code bounces the ship off walls.  You will replace it as part of
 * the lab.
 *
 * @param ship      They player's ship which may have collided
 * @param bounds    The rectangular bounds of the playing field
 */
/*
void collisions::checkInBounds(const std::shared_ptr<PlanetModel>& planet, const Rect bounds) {
    // UNLIKE Java, these are values, not references
    Vec2 vel = ship->getVelocity();
    Vec2 pos = ship->getPosition();
    
    //Ensure the ship doesn't go out of view. Bounce off walls.
    if (pos.x <= bounds.origin.x) {
        vel.x = -vel.x;
        pos.x = bounds.origin.x;
        ship->setVelocity(vel);
        ship->setPosition(pos);
    } else if (pos.x >= bounds.size.width+bounds.origin.x) {
        vel.x = -vel.x;
        pos.x = bounds.size.width+bounds.origin.x-1.0f;
        ship->setVelocity(vel);
        ship->setPosition(pos);
    }

    if (pos.y <= bounds.origin.y) {
        vel.y = -vel.y;
        pos.y = bounds.origin.y;
        ship->setVelocity(vel);
        ship->setPosition(pos);
    } else if (pos.y >= bounds.size.height+bounds.origin.y) {
        vel.y = -vel.y;
        pos.y = bounds.size.height+bounds.origin.y-1.0f;
        ship->setVelocity(vel);
        ship->setPosition(pos);
    }

}
*/

/**
 * Nudge the photons to ensure they do not do out of view.
 *
 * This code bounces the photons off walls.  You will replace it as part of
 * the lab.
 *
 * @param photons   They photon queue
 * @param bounds    The rectangular bounds of the playing field
 */

void collisions::checkInBounds(const std::shared_ptr<DotsQueue>& dots, const cugl::Size bounds) {
    for (size_t ii = 0; ii < dots->size(); ii++) {
        // This returns a reference
        DotModel* dot = dots->get(ii);
        // We add a layer due to own colored dot
        if (dot != nullptr) {
            Vec2 dotPos = dot->getPosition();
            Vec2 center = Vec2(bounds.width/2, bounds.height/2);
            Vec2 longest = Vec2(bounds.width, bounds.height) - center;
            Vec2 distance = (dotPos - center);
            if (distance.length() > longest.length() + 50){
                dot->destroy();
            }
        }
    }
}

