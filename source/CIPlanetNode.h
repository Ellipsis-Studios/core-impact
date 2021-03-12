//
//  CIPlanetNode.h
//  CoreImpact
//
//  Created by William Long on 3/8/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLANET_NODE_H__
#define __CI_PLANET_NODE_H__
#include <cugl/cugl.h>

class PlanetNode : public cugl::scene2::PolygonNode {
public:
    PlanetNode() : PolygonNode() {}
    
    ~PlanetNode() { dispose(); }
    
    static std::shared_ptr<PlanetNode> alloc(const std::shared_ptr<cugl::Texture>& texture) {
        std::shared_ptr<PlanetNode> node = std::make_shared<PlanetNode>();
        return (node->initWithTexture(texture) ? node : nullptr);
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    void setRadius(float r) {
        _radius = r;
        setScale(_radius * 0.005f);
    }

private:
    /** Radius of the planet in pixels */
    float _radius;
};

#endif /* __CI_PLANET_NODE_H__ */
