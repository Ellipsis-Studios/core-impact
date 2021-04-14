//
//  CIStardustQueue.h
//  CoreImpact
//
//  This class maintains all of the stardust present in the game.
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//
#ifndef __CI_STARDUST_QUEUE_H__
#define __CI_STARDUST_QUEUE_H__
#include <cugl/cugl.h>
#include "CIStardustModel.h"
#include "CIStardustNode.h"


/**
 * Model class representing an "particle system" of stardust.
 *
 * Note that the graphics resources in this class are static.  That
 * is because all the stardust shares the same image file, and it would waste
 * memory to load the same image file for each stardust.
 */
class StardustQueue {
private:
    // QUEUE DATA STRUCTURES
    /** Vector implementation of a circular queue. */
    std::vector<StardustModel> _queue;
    /** Index of head element in the queue */
    int _qhead;
    /** Index of tail element in the queue */
    int _qtail;
    /** Number of elements currently in the queue */
    int _qsize;

    /** Reference to stardust node */
    std::shared_ptr<StardustNode> _stardustNode;
    
    /** stardust to be sent to other players in the game. */
    std::vector<std::shared_ptr<StardustModel>> _stardust_to_send;
    
    /** Special stardust that need to be applied to the game. */
    std::vector<std::shared_ptr<StardustModel>> _stardust_powerups;

#pragma mark The Queue
public:
    /**
     * Creates a stardust queue with the default values.
     *
     * To properly initialize the queue, you should call the init
     * method.
     */
    StardustQueue();
    
    /**
     * Disposes the stardust queue, releasing all resources.
     */
    ~StardustQueue() { dispose(); }

    /**
     * Disposes the stardust queue, releasing all resources.
     */
    void dispose();
    
    /**
     *  Initialies a new (empty) StardustQueue.
     *
     *  @param max  The maximum number of stardust to support
     *  @param texture The pointer to the shared stardust texture
     *
     *  @return true if initialization is successful
     */
    bool init(size_t max, const std::shared_ptr<cugl::Texture>& texture);

    /**
     *  Returns a newly allocated (empty) StardustQueue
     *
     *  @param max  The maximum number of stardust to support
     *  @param texture The pointer to the shared stardust texture
     *
     *  @return a newly allocated (empty) StardustQueue
     */
    static std::shared_ptr<StardustQueue> alloc(size_t max, const std::shared_ptr<cugl::Texture>& texture) {
        std::shared_ptr<StardustQueue> result = std::make_shared<StardustQueue>();
        return (result->init(max, texture) ? result : nullptr);
    }

    /**
     * Returns the image for a single stardust; reused by all stardust.
     *
     * This value should be loaded by the GameMode and set there. However, we
     * have to be prepared for this to be null at all times
     *
     * @return the image for a single stardust; reused by all stardust.
     */
    const std::shared_ptr<cugl::Texture> getTexture() const {
        if (_stardustNode != nullptr) {
            return _stardustNode->getTexture();
        }
        return nullptr;
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
     * @param type the type of the stardust to add (defaults to normal)
     */
    void addStardust(CIColor::Value c, const cugl::Size bounds, StardustModel::Type type = StardustModel::Type::NORMAL);
    
    /**
     * Adds a stardust that will move fast and be aimed directly at the core.
     * This is the result of the shooting star powerup.
     *
     * @param c the color of the stardust to spawn
     * @param bounds the bounds of the game screen
     */
    void addShootingStardust(CIColor::Value c, const cugl::Size bounds);
    
    /**
     * Adds a stardust to the active queue given a pointer to the stardust
     *
     * @param stardust the stardust to add to the queue
     */
    void addStardust(const std::shared_ptr<StardustModel> stardust);
    
    /**
     * Returns the number of active stardust
     *
     * @return the number of active stardust
     */
    size_t size() const {
        return _qsize;
    }
    
    /**
     * Returns the index of the first stardust
     *
     * @return the index of the first stardust
     */
    int headIndex() const {
        return _qhead;
    }
    
    /**
     * Returns the queue of stardust
     *
     * @return the queue of stardust
     */
    std::vector<StardustModel> getQueue() const {
        return _queue;
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
    StardustModel* get(size_t pos);

    /**
     * Returns the Stardust Node pointer
     * 
     * @return shared pointer to Stardust Node
     */
    const std::shared_ptr<StardustNode> getStardustNode() const {
        return _stardustNode;
    }

    /**
     * Adds a stardust to the queue of stardust to send to other players
     *
     * @param stardust   The stardust that is to be sent to another player
     */
    void addToSendQueue(StardustModel* stardust);
    
    /**
     * Returns the queue of stardust to send
     *
     * @return The queue of stardust to send
     */
    std::vector<std::shared_ptr<StardustModel>> getSendQueue() {
        return _stardust_to_send;
    }
    
    /**
     * Clears the stardust to send queue
     */
    void clearSendQueue() {
        _stardust_to_send.clear();
    }
    
    /**
     * Adds a stardust to the powerup queue.
     *
     * @param stardust the stardust to add to the powerup queue
     */
    void addToPowerupQueue(StardustModel* stardust);
    
    /**
     * Adds a powerup to the powerup queue.
     *
     * @param color the color of layer that was just locked in
     * @param addToSendQueue whether to add the stardust to the send queue
     */
    void addToPowerupQueue(CIColor::Value color, bool addToSendQueue);
    
    /**
     * Returns the powerup queue
     * @return the powerup queue
     */
    std::vector<std::shared_ptr<StardustModel>> getPowerupQueue() {
        return _stardust_powerups;
    }
    
    /**
     * Clears the powerup queue.
     */
    void clearPowerupQueue() {
        _stardust_powerups.clear();
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
    void update(float timestep);
    
    /**
     * Returns the radius of a stardust. Returns 0 if the stardust texture has not been set yet.
     *
     * @return the radius of a stardust
     */
    float getStardustRadius();
};

#endif /* __CI_STARDUST_QUEUE_H__ */

