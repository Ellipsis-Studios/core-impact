//
//  CIDotsQueue.h
//
#ifndef __CI_DOTS_QUEUE_H__
#define __CI_DOTS_QUEUE_H__
#include <cugl/cugl.h>
#include "CIDotModel.h"

/**
 * Model class representing an "particle system" of dots.
 *
 * Note that the graphics resources in this class are static.  That
 * is because all photons share the same image file, and it would waste
 * memory to load the same image file for each dots.
 */
class DotsQueue {
private:
    /** Graphic asset representing a single dots. */
    std::shared_ptr<cugl::Texture> _texture;

    // QUEUE DATA STRUCTURES
    /** Vector implementation of a circular queue. */
    std::vector<DotModel> _queue;
    /** Index of head element in the queue */
    int _qhead;
    /** Index of tail element in the queue */
    int _qtail;
    /** Number of elements currently in the queue */
    int _qsize;

#pragma mark The Queue
public:
    /**
     * Creates a photon queue with the default values.
     *
     * To properly initialize the queue, you should call the init
     * method.
     */
    DotsQueue();
    
    /**
     * Disposes the photon queue, releasing all resources.
     */
    ~DotsQueue() { dispose(); }

    /**
     * Disposes the photon queue, releasing all resources.
     */
    void dispose();
    
    /**
     *  Initialies a new (empty) DotsQueue.
     *
     *  @param max  The maximum number of photons to support
     *
     *  @return true if initialization is successful
     */
    bool init(size_t max);

    /**
     *  Returns a newly allocated (empty) DotsQueue
     *
     *  @param max  The maximum number of photons to support
     *
     *  @return a newly allocated (empty) DotsQueue
     */
    static std::shared_ptr<DotsQueue> alloc(size_t max) {
        std::shared_ptr<DotsQueue> result = std::make_shared<DotsQueue>();
        return (result->init(max) ? result : nullptr);
    }

    /**
     * Returns the image for a single dot; reused by all dots.
     *
     * This value should be loaded by the GameMode and set there. However, we
     * have to be prepared for this to be null at all times
     *
     * @return the image for a single dot; reused by all dots.
     */
    const std::shared_ptr<cugl::Texture>& getTexture() const {
        return _texture;
    }

    /**
     * Sets the image for a single dot; reused by all dots.
     *
     * This value should be loaded by the GameMode and set there. However, we
     * have to be prepared for this to be null at all times
     *
     * @param value the image for a single dot; reused by all dots.
     */
    void setTexture(const std::shared_ptr<cugl::Texture>& value) {
        _texture = value;
    }

    /**
     * Adds a dot to the active queue.
     *
     * As all dots are predeclared, this involves moving the head and the tail,
     * and reseting the values of the object in place.  This is a simple implementation
     * of a memory pool. It works because we delete objects in the same order that
     * we allocate them.
     */
    void addDot(const cugl::Size bounds);
    
    /**
     * Returns the number of active dots
     *
     * @return the number of active dots
     */
    size_t size() const {
        return _qsize;
    }
    
    /**
     * Returns the index of the first dot
     *
     * @return the index of the first dot
     */
    int headIndex() const {
        return _qhead;
    }
    
    /**
     * Returns the queue
     *
     * @return the queue of dots
     */
    std::vector<DotModel> getQueue() const {
        return _queue;
    }
    
    /**
     * Returns the (reference to the) photon at the given position.
     *
     * If the position is not a valid dot, then the result is null.
     *
     * @param pos   The dot position in the queue
     *
     * @return the (reference to the) dot at the given position.
     */
    DotModel* get(size_t pos);
    
    /**
     * Moves all the dots in the active queue.
     *
     * Each photon is advanced according to its velocity. Dots which are too old
     * are deleted.  This method does not bounce off walls.  We moved all collisions
     * to the collision controller where they belong.
     */
    void update();
};

#endif /* __CI_DOTS_QUEUE_H__ */

