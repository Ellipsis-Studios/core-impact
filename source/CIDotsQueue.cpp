//
//  CIDotsQueue.cpp
//
#include "CIDotsQueue.h"

using namespace cugl;

#pragma mark Photon Queue
/**
 * Creates a photon queue with the default values.
 *
 * To properly initialize the queue, you should call the init
 * method.
 */
DotsQueue::DotsQueue() :
_qhead(0),
_qtail(-1),
_qsize(0) {
}
    
/**
 * Disposes the photon queue, releasing all resources.
 */
void DotsQueue::dispose() {
    _queue.clear();
    _qhead = 0;
    _qtail = -1;
    _qsize = 0;
}
    
/**
 *  Initialies a new (empty) PhotonQueue
 *
 *  @param max  The maximum number of photons to support
 *
 *  @return true if initialization is successful
 */
bool DotsQueue::init(size_t max) {
    _queue.resize(max);
    return true;
}

/**
 * Adds a photon to the active queue.
 *
 * When adding a photon, we assume that it is fired from the given ship.  We
 * factor in the position, velocity and angle of the ship.
 *
 * As all Photons are predeclared, this involves moving the head and the tail,
 * and reseting the values of the object in place.  This is a simple implementation
 * of a memory pool. It works because we delete objects in the same order that
 * we allocate them.
 *
 * @param ship  The ship that fired.
 */
void DotsQueue::addDot() {
    // Determine direction and velocity of the photon.
    float rads = M_PI*0.0f/180.0f+M_PI_2;
    Vec2 dir(cosf(rads),sinf(rads));
    
    // Check if any room in queue.
    // If maximum is reached, remove the oldest photon.
    if (_qsize == _queue.size()) {
        _qhead = ((_qhead + 1) % _queue.size());
        _qsize--;
    }

    // Add a new photon at the end.
    // Already declared, so just initialize.
    _qtail = ((_qtail + 1) % _queue.size());
    _queue[_qtail].init((rand() % 500), (rand() % 500), CIColor::blue);
    _qsize++;
}
    
/**
 * Moves all the photons in the active queue.
 *
 * Each photon is advanced according to its velocity. Photons which are too old
 * are deleted.  This method does not bounce off walls.  We moved all collisions
 * to the collision controller where they belong.
 */
void DotsQueue::update() {
    // First, delete all old photons.
    // INVARIANT: Photons are in queue in decending age order.
    // That means we just remove the head until the photons are young enough.
    while (_qsize > 0 && _queue[_qhead].getMass() <= 0) {
        // As photons are predeclared, all we have to do is move head forward.
        _qhead = ((_qhead + 1) % _queue.size());
        _qsize--;
    }

    // Now, step through each active photon in the queue.
    for (size_t ii = 0; ii < _qsize; ii++) {
        // Find the position of this photon.
        size_t idx = ((_qhead+ii) % _queue.size());

        // Move the photon according to velocity.
        _queue[idx].update();
    }
}

/**
 * Returns the (reference to the) photon at the given position.
 *
 * If the position is not a valid photon, then the result is null.
 *
 * @param pos   The photon position in the queue
 *
 * @return the (reference to the) photon at the given position.
 */
DotModel* DotsQueue::get(size_t pos) {
    size_t idx = ((_qhead+pos) % _queue.size());
    if (_queue[idx].getMass() <= 0) {
        return &_queue[idx];
    }
    return nullptr;
}