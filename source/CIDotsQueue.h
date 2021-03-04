//
//  PhotonQueue.h
//
//  This class implements a "particle system" that manages the photons fired
//  by either ship in the game.  When a ship fires a photon, it adds it to this
//  particle system.  The particle system is responsible for moving (and drawing)
//  the photon particle.  It also keeps track of the age of the photon.  Photons
//  that are too old are deleted, so that they are not bouncing about the game
//  forever.
//
//  The PhotonQueue is exactly what it sounds like: a queue. In this implementation
//  we use the circular array implementation of a queue (which you may have learned
//  in CS 2110). If you notice, all the Photon objects are declared and initialized
//  in the constructor; we just reassign the fields
//
//  Author: Walker M. White
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 2/21/21
//
#ifndef __GL_PHOTON_QUEUE_H__
#define __GL_PHOTON_QUEUE_H__
#include <cugl/cugl.h>
#include <vector>
#include "CIDotModel.h"

/**
 * Model class representing an "particle system" of photons.
 *
 * Note that the graphics resources in this class are static.  That
 * is because all photons share the same image file, and it would waste
 * memory to load the same image file for each photon.
 */
class DotsQueue {
private:
    /** Graphic asset representing a single photon. */
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
     *  Initialies a new (empty) PhotonQueue.
     *
     *  @param max  The maximum number of photons to support
     *
     *  @return true if initialization is successful
     */
    bool init(size_t max);

    /**
     *  Returns a newly allocated (empty) PhotonQueue
     *
     *  @param max  The maximum number of photons to support
     *
     *  @return a newly allocated (empty) PhotonQueue
     */
    static std::shared_ptr<DotsQueue> alloc(size_t max) {
        std::shared_ptr<DotsQueue> result = std::make_shared<DotsQueue>();
        return (result->init(max) ? result : nullptr);
    }

    /**
     * Returns the image for a single photon; reused by all photons.
     *
     * This value should be loaded by the GameMode and set there. However, we
     * have to be prepared for this to be null at all times
     *
     * @return the image for a single photon; reused by all photons.
     */
    const std::shared_ptr<cugl::Texture>& getTexture() const {
        return _texture;
    }

    /**
     * Sets the image for a single photon; reused by all photons.
     *
     * This value should be loaded by the GameMode and set there. However, we
     * have to be prepared for this to be null at all times
     *
     * @param value the image for a single photon; reused by all photons.
     */
    void setTexture(const std::shared_ptr<cugl::Texture>& value) {
        _texture = value;
    }

    /**
     * Adds a photon to the active queue.
     *
     * As all Photons are predeclared, this involves moving the head and the tail,
     * and reseting the values of the object in place.  This is a simple implementation
     * of a memory pool. It works because we delete objects in the same order that
     * we allocate them.
     *
     * @param ship  The ship that fired.
     */
    void addDot();
    
    /**
     * Returns the number of active photons
     *
     * @return the number of active photons
     */
    size_t size() const {
        return _qsize;
    }
    
    /**
     * Returns the index of the first photon
     *
     * @return the index of the first photon
     */
    int headIndex() const {
        return _qhead;
    }
    
    /**
     * Returns the queue
     *
     * @return the queue of photons
     */
    std::vector<DotModel> getQueue() const {
        return _queue;
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
    DotModel* get(size_t pos);
    
    /**
     * Moves all the photons in the active queue.
     *
     * Each photon is advanced according to its velocity. Photons which are too old
     * are deleted.  This method does not bounce off walls.  We moved all collisions
     * to the collision controller where they belong.
     */
    void update();

    /**
     * Draws the photons to the drawing canvas.
     *
     * This method allows you to do old-style 3152 drawing instead.
     * This is the easiest way to set the blend mode.  Otherwise, if
     * you want to vary blend modes in a scene graph, you have to
     * define your own nodes.
     *
     * @param bath  The sprite batch
     */
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch);
};

#endif /* __GL_PHOTON_QUEUE_H__ */

