//
//  CIStardustNode.cpp
//  CoreImpact
//
//  This node class is used to draw all of the stardust to the screen.
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIStardustNode.h"
#include "CIColor.h"

void StardustNode::dispose() {
	_queue = nullptr;
	_texture = nullptr;

	_qhead = 0;
	_qtail = -1;
	_qsize = 0;
}

void StardustNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
	const cugl::Mat4& transform, cugl::Color4 tint) {
	if (_texture == nullptr) {
		return;
	}

	// Get stardust texture origin
	cugl::Vec2 origin = _texture->getSize() / 2;
	batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending

	// Step through each active stardust in the queue.
	for (size_t ii = 0; ii < _qsize; ii++) {
		// Find the position of this stardust.
		size_t idx = ((_qhead + ii) % _queue->size());

		if (_queue->at(idx).getMass() > 0) {
			cugl::Mat4 stardustTransform;
			stardustTransform.scale(_queue->at(idx).getRadius());
			stardustTransform.translate(_queue->at(idx).getPosition().x, _queue->at(idx).getPosition().y, 0);
			stardustTransform.multiply(transform);

			// Use this information to draw.
			const cugl::Color4f stardustColor = CIColor::getColor4(_queue->at(idx).getColor());
			batch->draw(_texture, stardustColor, origin, stardustTransform);
		}
	}
	batch->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

StardustModel* StardustNode::get(size_t pos) {
	size_t idx = ((_qhead + pos) % _queue->size());
	if (_queue->at(idx).getMass() > 0) {
		return &_queue->at(idx);
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
 * @param bounds the bounds of the game screen
 */
void StardustNode::addStardust(const cugl::Size bounds) {
	// Check if any room in queue.
	// If maximum is reached, remove the oldest stardust.
	if (_qsize == _queue->size()) {
		_qhead = ((_qhead + 1) % _queue->size());
		_qsize--;
	}

	// Add a new stardust at the end.
	// Already declared, so just initialize.
	int posX = ((rand() % 2 == 0) ? bounds.width + 20 : -20) + (rand() % 20 - 10);
	int posY = ((rand() % 2 == 0) ? bounds.height + 20 : -20) + (rand() % 20 - 10);
	cugl::Vec2 pos = cugl::Vec2(posX, posY);
	cugl::Vec2 dir = cugl::Vec2(bounds.width / 2, bounds.height / 2) - pos;
	dir.normalize();
	dir.x *= (rand() % 3) + 2;
	dir.y *= (rand() % 3) + 2;
	_qtail = ((_qtail + 1) % _queue->size());

	// TODO: adding dot queues based on an upcoming algorithm
	auto randColor = CIColor::getRandomColor();
	_queue->at(_qtail).init(pos, dir, randColor);
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
void StardustNode::update() {
	// First, delete all old stardust.
	// INVARIANT: Stardusts are in queue in decending age order.
	// That means we just remove the head until the stardusts are young enough.
	while (_qsize > 0 && _queue->at(_qhead).getMass() <= 0) {
		// As stardusts are predeclared, all we have to do is move head forward.
		_qhead = ((_qhead + 1) % _queue->size());
		_qsize--;
	}

	// Now, step through each active stardust in the queue.
	for (size_t ii = 0; ii < _qsize; ii++) {
		// Find the position of this stardust.
		size_t idx = ((_qhead + ii) % _queue->size());

		// Move the stardust according to velocity.
		_queue->at(idx).update();
	}
}