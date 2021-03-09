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
    PolygonNode::draw(batch,transform, tint);
}
