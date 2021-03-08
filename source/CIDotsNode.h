//
//  CIPhotonNode.h
//  CoreImpact
//
//  Created by Kevin Sun on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_DOTS_NODE_H__
#define __CI_DOTS_NODE_H__
#include <cugl/cugl.h>
#include "CIDotsQueue.h"

class DotsNode : public cugl::scene2::SceneNode {
private:
    /** Shared memory pool for photons. (MODEL CLASS) */
    std::shared_ptr<DotsQueue> _photonQueue;
public:
    DotsNode() : SceneNode() {}

    ~DotsNode() { dispose(); }
    
    static std::shared_ptr<DotsNode> alloc() {
        std::shared_ptr<DotsNode> node = std::make_shared<DotsNode>();
        return (node->init() ? node : nullptr);
    }
    
    void setDotsQueue(std::shared_ptr<DotsQueue> queue){
        _photonQueue = queue;
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
};

#endif /* __CI_DOTS_NODE_H__ */
