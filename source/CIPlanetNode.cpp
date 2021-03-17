//
//  CIPlanetNode.cpp
//  CoreImpact
//
//  This class is used to draw the planet to the screen.
//  The root is used for the texture of the core, and all other layers
//  are added as children of this node
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
