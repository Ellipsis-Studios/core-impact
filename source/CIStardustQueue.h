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
	///** Graphic asset representing a single stardust. */
	std::shared_ptr<cugl::Texture> _texture;

	/** Pointer to StardustNode */
	std::shared_ptr<StardustNode> _stardustNode;

	/** Index of head element in the queue */
	int _qhead;
	/** Index of tail element in the queue */
	int _qtail;
	/** Number of elements currently in the queue */
	int _qsize;

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
	 *
	 *  @return true if initialization is successful
	 */
	bool init(size_t max);

	/**
	 *  Returns a newly allocated (empty) StardustQueue
	 *
	 *  @param max  The maximum number of stardust to support
	 *
	 *  @return a newly allocated (empty) StardustQueue
	 */
	static std::shared_ptr<StardustQueue> alloc(size_t max) {
		std::shared_ptr<StardustQueue> result = std::make_shared<StardustQueue>();
		return (result->init(max) ? result : nullptr);
	}

	/**
	 * Returns the image for a single stardust; reused by all stardust.
	 *
	 * This value should be loaded by the GameMode and set there. However, we
	 * have to be prepared for this to be null at all times
	 *
	 * @return the image for a single stardust; reused by all stardust.
	 */
	const std::shared_ptr<cugl::Texture>& getTexture() const {
		return _texture;
	}

	/**
	 * Sets the image for a single stardust; reused by all stardust.
	 *
	 * This value should be loaded by the GameMode and set there. However, we
	 * have to be prepared for this to be null at all times
	 *
	 * @param value the image for a single stardust; reused by all stardust.
	 */
	void setTexture(const std::shared_ptr<cugl::Texture>& value) {
		_texture = value;
		_stardustNode->setTexture(value);
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
	void addStardust(const cugl::Size bounds);

	/**
	 * Returns the number of active stardust
	 *
	 * @return the number of active stardust
	 */
	size_t size() const {
		return _qsize;
	}

	/**
	 * Returns the stardust node pointer.
	 *
	 * @return shared pointer to StardustNode
	 */
	const std::shared_ptr<StardustNode>& getStardustNode() const {
		return _stardustNode;
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
		return _stardustNode->getQueue();
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
	 * Moves all the stardust in the active queue.
	 *
	 * Each stardust is advanced according to its velocity. Stardusts which are too old
	 * are deleted.  This method does not bounce off walls.  We moved all collisions
	 * to the collision controller where they belong.
	 */
	void update();
};

#endif /* __CI_STARDUST_QUEUE_H__ */
