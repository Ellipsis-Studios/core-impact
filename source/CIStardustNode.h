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
     * Disposes the Stardust node, releasing all resources.
     */
    virtual void dispose() override;
    
    /** 
     * Returns the newly allocated Stardust Node
     *
     * @param texture   The pointer to the shared stardust texture
     * @param queue     The pointer to the stardust queue 
     * @param head      The pointer to the stardust queue head
     * @param tail      The pointer to the stardust queue tail
     * @param size      The pointer to the stardust queue size 
     *
     * @return a newly allocated Stardust Node
     */
    static std::shared_ptr<StardustNode> alloc(const std::shared_ptr<cugl::Texture>& texture, 
        std::vector<StardustModel>* queue, int* head, int* tail, int* size) {
        std::shared_ptr<StardustNode> node = std::make_shared<StardustNode>();
        return (node->init(texture, queue, head, tail, size) ? node : nullptr);
    }

    /** Initializes a new stardust node with the pointers.
     *
     * @param texture   The pointer to the shared stardust texture
     * @param queue     The pointer to the stardust queue
     * @param head      The pointer to the stardust queue head
     * @param tail      The pointer to the stardust queue tail
     * @param size      The pointer to the stardust queue size
     *
     * @return bool true if new node initialized successfully else false 
     */
    bool init(const std::shared_ptr<cugl::Texture>& texture, 
        std::vector<StardustModel>* queue, int* head, int* tail, int* size) {
        _texture = texture;
        _queue = queue;
        _qhead = head;
        _qtail = tail;
        _qsize = size;

        return SceneNode::init();
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
    const std::shared_ptr<cugl::Texture> getTexture() const {
        return _texture;
    }
};

#endif /* __CI_STARDUST_NODE_H__ */
