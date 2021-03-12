//
//  CIColor.cpp
//  CoreImpact
//
//  This class represents the colors that are used by stardust and the planet.
//  It maps from a custom enum to cugl Color4 colors.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIColor.h"

/** Map from color enum values to cugl Color4 values */
const cugl::Color4 CIColor::_color4Map[] = {
   cugl::Color4::RED,
   cugl::Color4::BLUE,
   cugl::Color4::GREEN,
   cugl::Color4::CYAN,
   cugl::Color4::MAGENTA,
   cugl::Color4::YELLOW,
};
