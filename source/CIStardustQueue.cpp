//
//  CIStardustQueue.cpp
//  CoreImpact
//
//  This class maintains all of the stardust present in the game.
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//
#include "CIStardustQueue.h"
#include "CIColor.h"

using namespace cugl;

#define STARDUST_BUFFER     64

#pragma mark The Queue
/**
 * Creates a stardust queue with the default values.
 *
 * To properly initialize the queue, you should call the init
 * method.
 */
StardustQueue::StardustQueue() :
_qhead(0),
_qtail(-1),
_qsize(0) {
}
    
/**
 * Disposes the stardust queue, releasing all resources.
 */
void StardustQueue::dispose() {
    _queue.clear();
    _qhead = 0;
    _qtail = -1;
    _qsize = 0;
    _stardustNode = nullptr;
    _stardust_to_send.clear();
    _stardust_powerups.clear();
}
    
/**
 *  Initialies a new (empty) StardustQueue
 *
 *  @param max  The maximum number of stardust to support
 *  @param texture The pointer to the shared stardust texture 
 *
 *  @return true if initialization is successful
 */
bool StardustQueue::init(size_t max, const std::shared_ptr<cugl::Texture>& texture) {
    _queue.resize(max);
    _stardustNode = StardustNode::alloc(texture, &_queue, &_qhead, &_qtail, &_qsize);
    return true;
}

/**
 * Adds a stardust to the active queue.
 *
 * As all stardusts are predeclared, this involves moving the head and the tail,
 * and reseting the values of the object in place.  This is a simple implementation
 * of a memory pool. It works because we delete objects in the same order that
 * we allocate them.
 *
 * @param c the color of the stardust to spawn
 * @param bounds the bounds of the game screen
 * @param corner the corner to spawn in
 * @param type the type of the stardust to add (defaults to normal)
 */
void StardustQueue::addStardust(CIColor::Value c, const Size bounds, const CILocation::Value corner, StardustModel::Type type) {
    // Add a new stardust at the end.
    // Already declared, so just initialize.
    int spawnCorner = (corner == 0) ? rand() % 4 : corner - 1;
    int posX = ((spawnCorner % 2 == 0) ? -20 : bounds.width + 20) + (rand() % 20 - 10);
    int posY = ((spawnCorner / 2 == 0) ? bounds.height + 20 : -20) + (rand() % 20 - 10);
    Vec2 pos = Vec2(posX, posY);
    Vec2 dir = Vec2(bounds.width/2, bounds.height/2) - pos;
    dir.normalize();
    dir.x *= (rand() % 3)+2;
    dir.y *= (rand() % 3)+2;

    std::shared_ptr<StardustModel> stardust = StardustModel::alloc(pos, dir, c);
    stardust->setStardustType(type);
    addStardust(stardust);
}

/**
 * Adds a stardust that will move fast and be aimed directly at the core.
 * This is the result of the shooting star powerup.
 *
 * @param c the color of the stardust to spawn
 * @param bounds the bounds of the game screen
 */
void StardustQueue::addShootingStardust(CIColor::Value c, const cugl::Size bounds) {
    int posX = ((rand()%2==0) ? bounds.width + 5 : -5) + (rand() % 20 - 10);
    int posY = ((rand()%2==0) ? bounds.height + 5 : -5) + (rand() % 20 - 10);
    Vec2 pos = Vec2(posX, posY);
    Vec2 dir = Vec2(bounds.width/2, bounds.height/2) - pos;
    dir.normalize();
    dir.x *= 10;
    dir.y *= 10;

    std::shared_ptr<StardustModel> stardust = StardustModel::alloc(pos, dir, c);
    addStardust(stardust);
}

/**
 * Adds a stardust to the active queue given a pointer to the stardust
 *
 * @param stardust the stardust to add to the queue
 */
void StardustQueue::addStardust(const std::shared_ptr<StardustModel> stardust) {
    // Check if any room in queue.
    // If maximum is reached, remove the oldest stardust.
    if (_qsize == _queue.size()) {
        // If not a particle, bump the oldest item in the queue
        if (stardust->isInteractable()){
            _qhead = ((_qhead + 1) % _queue.size());
            _qsize--;
        } else {
            // If it is a particle, ignore the spawning
            return;
        }
    }
    
    _qtail = ((_qtail + 1) % _queue.size());
    _queue[_qtail] = *stardust;
    _qsize++;
}

/**
 * Adds a blast of stardust particles to the active queue
 *
 * @param position The initial position of the particle
 * @param velocity The initial velocity of the particle
 * @param c1 The color code of the stardust
 * @param c2 The color code of the planet
 */

void StardustQueue::createStardustParticleBlast(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c1, CIColor::Value c2){
    int blastSize = min((int)(velocity.length() * 3 + 8), 32);
    for (int i=0;i<blastSize;i++){
        // If queue is close to full, ignore spawning
        if (_qsize > _queue.size() - STARDUST_BUFFER){
            break;
        }
        
        Vec2 particleVel = Vec2(velocity.x, velocity.y);
        particleVel.x += (rand() % 10)/2.0 - 2.5;
        particleVel.y += (rand() % 10)/2.0 - 2.5;
        float size = ((rand() % 6) + 7) / 50.0;
        float lifespan = ((rand() % 8) + 14);
        std::shared_ptr<StardustModel> particle = StardustModel::allocParticle(position, particleVel, ((rand() % 2 == 0) ? c1 : c2), size, lifespan);
        addStardust(particle);
    }
}

/**
 * Returns the (reference to the) stardust at the given position.
 *
 * If the position is not a valid stardust, then the result is null.
 *
 * @param pos   The stardust position in the queue
 *
 * @return the (reference to the) stardust at the given position.
 */
StardustModel* StardustQueue::get(size_t pos) {
    size_t idx = ((_qhead+pos) % _queue.size());
    if (_queue[idx].getMass() > 0) {
        return &_queue[idx];
    }
    return nullptr;
}

/**
 * Adds a stardust to the queue of stardust to send to other players
 *
 * @param stardust   The stardust that is to be sent to another player
 */
void StardustQueue::addToSendQueue(StardustModel* stardust) {
    _stardust_to_send.push_back(std::make_shared<StardustModel>(*stardust));
}

/**
 * Adds a stardust to the powerup queue.
 *
 * @param stardust the stardust to add to the powerup queue
 */
void StardustQueue::addToPowerupQueue(StardustModel* stardust) {
    _stardust_powerups.push_back(std::make_shared<StardustModel>(*stardust));
}

/**
 * Adds a powerup to the powerup queue.
 *
 * @param color the color of layer that was just locked in
 * @param id the source player of the powerup
 */
void StardustQueue::addToPowerupQueue(CIColor::Value color, int id) {
    // Favor variability in powerups
    CIColor::Value c = CIColor::getRandomColor();
    while (c == color){
        c = CIColor::getRandomColor();
    }
    std::shared_ptr<StardustModel> stardust = StardustModel::alloc(cugl::Vec2(), cugl::Vec2(), c);
    stardust->setPreviousOwner(id);
    switch (color) {
        case CIColor::Value::red:
            stardust->setStardustType(StardustModel::Type::METEOR);
            _stardust_powerups.push_back(stardust);
            break;
        case CIColor::Value::yellow:
            stardust->setStardustType(StardustModel::Type::SHOOTING_STAR);
            _stardust_powerups.push_back(stardust);
            break;
        case CIColor::Value::purple:
            stardust->setStardustType(StardustModel::Type::GRAYSCALE);
            _stardust_powerups.push_back(stardust);
            break;
        case CIColor::Value::turquoise:
            stardust->setStardustType(StardustModel::Type::FOG);
            _stardust_powerups.push_back(stardust);
            break;
        default:
            break;
    }
    
    if (stardust->getStardustType() != StardustModel::Type::NORMAL) {
        _stardust_to_send.push_back(stardust);
    }
    
}

/**
 * Moves all the stardust in the active queue.
 *
 * Each stardust is advanced according to its velocity. Stardusts which are too old
 * are deleted.  This method does not bounce off walls.  We moved all collisions
 * to the collision controller where they belong.
 *
 * @param timestep  How much time has passed since the last frame
 */
void StardustQueue::update(float timestep) {
    // First, delete all old stardust.
    // INVARIANT: Stardusts are in queue in decending age order.
    // That means we just remove the head until the stardusts are young enough.
    while (_qsize > 0 && _queue[_qhead].getMass() <= 0) {
        // As stardusts are predeclared, all we have to do is move head forward.
        _qhead = ((_qhead + 1) % _queue.size());
        _qsize--;
    }

    // Now, step through each active stardust in the queue.
    for (size_t ii = 0; ii < _qsize; ii++) {
        // Find the position of this stardust.
        size_t idx = ((_qhead+ii) % _queue.size());

        // Move the stardust according to velocity.
        _queue[idx].update(timestep);
    }
    
    _stardustNode->update(timestep);
}

/**
 * Returns the radius of a stardust. Returns 0 if the stardust texture has not been set yet.
 *
 * @return the radius of a stardust
 */
float StardustQueue::getStardustRadius() {
    if (_stardustNode == nullptr) {
        return 0;
    }
    
    float sdRadius = 0;
    auto texture = _stardustNode->getTexture();
    if (texture != nullptr) {
        sdRadius = std::max(texture->getWidth(), texture->getHeight()) / (2.0f * 13.0f);
    }
    return sdRadius / 3;
}
