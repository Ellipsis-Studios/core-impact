//
//  CIPlanetLayer.h
//  CoreImpact
//
//  This struct represents a completed layer of the planet.
//  It is used by both the model and node class
//
//  Created by Brandon Stein on 3/15/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLANET_LAYER_H__
#define __CI_PLANET_LAYER_H__
#include "CIColor.h"

typedef struct PlanetLayer {
    /** The size of this layer */
    int layerSize;
    /** The color of this layer */
    CIColor::Value layerColor;
    /** Whether this layer is in use */
    bool isActive;
    /** Whether this layer is locked in */
    bool isLockedIn;
} PlanetLayer;

#endif /* __CI_PLANET_LAYER_H__ */
