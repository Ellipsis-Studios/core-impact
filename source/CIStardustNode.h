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

    /** Pointer to stardust queue */
    std::vector<StardustModel>* _queue;
    /** Pointer to Stardust queue head */
    int* _qhead;
    /** Pointer to Stardust queue tail */
    int* _qtail;
    /** Pointer to Stardust queue size */
    int* _qsize;

public:
    /** 
     * Creates a stardust node with default values.
     */
    StardustNode() : SceneNode(), _qhead(), _qtail(), _qsize() {}

    /**
     * Disposes the stardust node, releasing all resources.
     */
    ~StardustNode() { dispose(); }
    
    /** 
     * Returns the newly allocated Stardust Node
     *
     * @return a newly allocated Stardust Node
     */
    static std::shared_ptr<StardustNode> alloc() {
        std::shared_ptr<StardustNode> node = std::make_shared<StardustNode>();
        return (node->init() ? node : nullptr);
    }
    
    /**
     * Initializes the Stardust Node by setting the queue-related 
     * pointer values.
     *
     * @param queue     Pointer to Stardust Queue's queue
     * @param head      Pointer to Stardust Queue's qhead
     * @param tail      Pointer to Stardust Queue's qtail
     * @param size      Pointer to Stardust Queue's qsize
     */
    void initialize(std::vector<StardustModel>* queue, int* head, int* tail, int* size) {
        _queue = queue;
        _qhead = head;
        _qtail = tail;
        _qsize = size;
    }
    /** 
     * Draws the stardusts in the queue to the game scene.
     */
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;

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
};

#endif /* __CI_STARDUST_NODE_H__ */
