//
//  CIStardustNode.h
//  CoreImpact
//
//  This node class is used to draw all of the stardust to the screen.
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_STARDUST_NODE_H__
#define __CI_STARDUST_NODE_H__
#include <cugl/cugl.h>
#include "CIStardustModel.h"

class StardustNode : public cugl::scene2::SceneNode {
private:
	/** Graphic asset representing a single stardust. */
	std::shared_ptr<cugl::Texture> _texture;

	// QUEUE DATA STRUCTURES
	/** Vector implementation of a circular queue. */
	std::vector<StardustModel>* _queue;

	/** Index of head element in the queue */
	int _qhead;
	/** Index of tail element in the queue */
	int _qtail;
	/** Number of elements currently in the queue */
	size_t _qsize;

public:
	/**
	 * Creates a stardust node with the default values.
	 *
	 * To properly initialize the node, you should call the init
	 * method.
	 */
	StardustNode() : SceneNode() {
		_qhead = 0;
		_qtail = -1;
		_qsize = 0;
	}

	/**
	 * Disposes the stardust node, releasing all resources.
	 */
	~StardustNode() { dispose(); }

	/**
	 * Disposes the stardust node, releasing all resources.
	 */
	void dispose();

	/**
	 *  Returns a newly allocated (empty) StardustNode
	 *
	 *  @param max  The maximum number of stardust to support
	 *
	 *  @return a newly allocated (empty) StardustNode
	 */
	static std::shared_ptr<StardustNode> alloc() { // std::vector<StardustModel>* q) {
		std::shared_ptr<StardustNode> node = std::make_shared<StardustNode>();
		return (node->init() ? node : nullptr);
	}

	/**
	 *  Initialies a new (empty) StardustNode.
	 *
	 *  @param max  The maximum number of stardust to support
	 *
	 *  @return true if initialization is successful
	 */
	bool init() { //std::vector<StardustModel>* q) {
		//_queue = q;
		_qhead = 0;
		_qtail = -1;
		_qsize = 0;
		return true;
	}

	void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
		const cugl::Mat4& transform, cugl::Color4 tint) override;

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
	 * Sets the queue size of stardust
	 *
	 * @param integer value for queue size
	 */
	void setSize(size_t qsize) {
		_qsize = qsize;
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
	 * Sets the queue head value
	 *
	 * @param the index of the first stardust
	 */
	void setHeadIndex(int ind) {
		_qhead = ind;
	}

	/**
	 * Returns the index of the last stardust
	 *
	 * @return the index of the last stardust
	 */
	int tailIndex() const {
		return _qtail;
	}

	/**
	 * Sets the queue tail value
	 *
	 * @param the index of the last stardust
	 */
	void setTailIndex(int ind) {
		_qtail = ind;
	}

	void setQueue(std::vector<StardustModel>* q) {
		_queue = q;
	}

	///**
	// * Returns the queue of stardust
	// *
	// * @return the queue of stardust
	// */
	//std::vector<StardustModel>* getQueue() const {
	//	return _queue;
	//}

	/**
	 * Moves all the stardust in the active queue.
	 *
	 * Each stardust is advanced according to its velocity. Stardusts which are too old
	 * are deleted.  This method does not bounce off walls.  We moved all collisions
	 * to the collision controller where they belong.
	 */
	void update();
};

#endif /* __CI_STARDUST_NODE_H__ */
