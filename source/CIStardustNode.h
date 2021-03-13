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

public:
    std::shared_ptr<cugl::Texture> _texture;
    std::vector<StardustModel> _queue;
    
    int _qhead;
    int _qtail;
    int _qsize;
    
    StardustNode() : SceneNode() {}

    ~StardustNode() { dispose(); }
    
    static std::shared_ptr<StardustNode> alloc() {
        std::shared_ptr<StardustNode> node = std::make_shared<StardustNode>();
        return (node->init() ? node : nullptr);
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    StardustModel* get(size_t pos);
};

#endif /* __CI_STARDUST_NODE_H__ */
