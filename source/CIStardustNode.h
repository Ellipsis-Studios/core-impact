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
	std::vector<StardustModel> _queue;

	/** Index of head element in the queue */
	int _qhead;
	/** Index of tail element in the queue */
	int _qtail;
	/** Number of elements currently in the queue */
	int _qsize;

public:
	/**
	 * Creates a stardust node with the default values.
	 *
	 * To properly initialize the node, you should call the init
	 * method.
	 */
	StardustNode() : SceneNode() {}

	/**
	 * Disposes the stardust node, releasing all resources.
	 */
	~StardustNode() { dispose(); }

	/**
	 *  Returns a newly allocated (empty) StardustNode
	 *
	 *  @param max  The maximum number of stardust to support
	 *
	 *  @return a newly allocated (empty) StardustNode
	 */
	static std::shared_ptr<StardustNode> alloc() {
		std::shared_ptr<StardustNode> node = std::make_shared<StardustNode>();
		return (node->init() ? node : nullptr);
	}

	/**
	 * Draws the stardusts to the game scene
	 */
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
	 * Returns the queue of stardust
	 *
	 * @return the queue of stardust
	 */
	std::vector<StardustModel> getQueue() const {
		return _queue;
	}

	/**
	 * Resizes the queue of stardust
	 */
	void resizeQueue(size_t max) {
		_queue.resize(max);
	}

	/**
	 * Returns the size of the queue of stardust
	 */
	size_t getQueueSize() const {
		return _queue.size();
	}

	/**
	 * Sets the _qhead value
	 */
	void setQHead(int ind) {
		_qhead = ind;
	}

	/**
	 * Sets the _qtail value
	 */
	void setQTail(int ind) {
		_qtail = ind;
	}

	/**
	 * Sets the _qsize value
	 */
	void setQSize(int s) {
		_qsize = s;
	}

	/**
	 * Clears the queue of stardust
	 */
	void clearQueue() {
		_queue.clear();
	}

	/**
	 * Returns the image for a single stardust; reused by all stardust.
	 *
	 * This value should be loaded by the GameMode and set there. However, we
	 * have to be prepared for this to be null at all times
	 *
	 * @return the image for a single stardust; reused by all stardust.
	 */
	std::shared_ptr<cugl::Texture> getTexture() const {
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
	void setTexture(const std::shared_ptr<cugl::Texture>& texture) {
		_texture = texture;
	}

	/**
	 * Clears out the memory for the texture.
	 */
	void clearTexture() {
		_texture = nullptr;
	}

	/**
	 * Initializes a StardustModel at the given index in queue with the provided values.
	 */
	void initNewStardust(cugl::Vec2 position, cugl::Vec2 velocity, CIColor::Value c) {
		_queue[_qtail].init(position, velocity, c);
	}

	/**
	 * Returns the mass of the model at the given index in queue.
	 */
	float getModelMass(size_t ind) const {
		return _queue[ind].getMass();
	}

	/**
	 * Steps through active stardust in queue and updates them.
	 *
	 * For use only in StardustQueue's update()
	 */
	void updateQueueModels(int& qsize, int& qhead) {
		// Step through each active stardust in the queue.
		for (size_t ii = 0; ii < qsize; ii++) {
			// Find the position of this stardust.
			size_t idx = ((qhead + ii) % _queue.size());
			// Move the stardust according to velocity.
			_queue[idx].update();
		}
	}
};

#endif /* __CI_STARDUST_NODE_H__ */
