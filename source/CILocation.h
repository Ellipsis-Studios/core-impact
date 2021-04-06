//
//  CILocation.h
//  CoreImpact
//
// Enum representing screen locations
//
//  Created by Brandon Stein on 4/3/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_LOCATION_H__
#define __CI_LOCATION_H__

class CILocation {
public:
    enum Value {
        ON_SCREEN = 0,
        TOP_LEFT = 1,
        TOP_RIGHT = 2,
        BOTTOM_LEFT = 3,
        BOTTOM_RIGHT = 4,
    };
    
    /**
     * Returns the position from the bounds corresponding to a given Location
     */
    static cugl::Vec2 getPositionOfLocation(Value location, cugl::Size bounds) {
        switch (location) {
            case Value::TOP_LEFT:
                return cugl::Vec2(0, bounds.height);
                break;
            case Value::TOP_RIGHT:
                return cugl::Vec2(bounds);
                break;
            case Value::BOTTOM_LEFT:
            case Value::ON_SCREEN: //this case should not occur
                return cugl::Vec2(0, 0);
                break;
            case Value::BOTTOM_RIGHT:
                return cugl::Vec2(bounds.width, 0);
                break;
        }
    }
};

#endif /* __CI_LOCATION_H__ */
