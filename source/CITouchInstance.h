//
//  CITouchInstance.h
//  CoreImpact
//
//  This struct represents a single touch or click on the screen
//
//  Created by Brandon Stein on 5/11/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_TOUCH_INSTANCE_H__
#define __CI_TOUCH_INSTANCE_H__
#include <cugl/cugl.h>

typedef struct TouchInstance {
    /** Whether this touch is still active */
    bool fingerDown;
    /** The touch position */
    cugl::Vec2 position;
    /** The touch velocity */
    cugl::Vec2 velocity;
    /** The current touch time */
    cugl::Timestamp timestamp;
    /** The touch id for future reference */
    Uint64 touchid;
} TouchInstance;


#endif /* __CI_TOUCH_INSTANCE_H__ */
