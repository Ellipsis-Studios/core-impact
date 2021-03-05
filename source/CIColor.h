//
//  CIColor.h
//  Core Impact
//
//  This class represents the colors that are used by dots and the planet.
//  It maps from a custom enum to cugl Color4 colors.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_COLOR_H__
#define __CI_COLOR_H__
#include <cugl/cugl.h>

class CIColor {
private:
    /** Map from color enum values to cugl Color4 values */
    static const cugl::Color4 _color4Map[];
    
public:
    /**
     * Enum representing the different colors a dot or planet layer can have
     */
    enum Value {
        red,
        blue,
        green,
        cyan,
        magenta,
        yellow,
    };
    
    /**
     * Get the Color4 represented by a particular CIColor value
     */
    cugl::Color4 getColor4(Value v) {
        return _color4Map[v];
    }
};

#endif /* __CI_COLOR_H__ */
