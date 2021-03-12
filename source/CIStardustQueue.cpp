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
}
    
/**
 *  Initialies a new (empty) StardustQueue
 *
 *  @param max  The maximum number of stardust to support
 *
 *  @return true if initialization is successful
 */
bool StardustQueue::init(size_t max) {
    _queue.resize(max);
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
 * @param bounds the bounds of the game screen
 */
void StardustQueue::addStardust(const Size bounds) {
    // Check if any room in queue.
    // If maximum is reached, remove the oldest stardust.
    if (_qsize == _queue.size()) {
        _qhead = ((_qhead + 1) % _queue.size());
        _qsize--;
    }

    // Add a new stardust at the end.
    // Already declared, so just initialize.
    int posX = ((rand()%2==0) ? bounds.width + 20 : -20) + (rand() % 20 - 10);
    int posY = ((rand()%2==0) ? bounds.height + 20 : -20) + (rand() % 20 - 10);
    Vec2 pos = Vec2(posX, posY);
    Vec2 dir = Vec2(bounds.width/2, bounds.height/2) - pos;
    dir.normalize();
    dir.x *= (rand() % 3)+2;
    dir.y *= (rand() % 3)+2;
    
    // Random Color    
    _qtail = ((_qtail + 1) % _queue.size());
    _queue[_qtail].init(pos, dir, CIColor::getRandomColor());
    _qsize++;
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
 * Moves all the stardust in the active queue.
 *
 * Each stardust is advanced according to its velocity. Stardusts which are too old
 * are deleted.  This method does not bounce off walls.  We moved all collisions
 * to the collision controller where they belong.
 */
void StardustQueue::update() {
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
        _queue[idx].update();
    }
}
