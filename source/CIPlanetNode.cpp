//
//  CIPlanetNode.cpp
//  CoreImpact
//
//  Created by William Long on 3/8/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIPlanetNode.h"
#include <cugl/cugl.h>

void PlanetNode::draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
          const cugl::Mat4& transform, cugl::Color4 tint) {
    cugl::Mat4 scale = cugl::Mat4::createScale(_radius * 0.01f);
    CULog(transform.toString().c_str());
    PolygonNode::draw(batch,transform * scale, cugl::Color4::MAGENTA);
}
