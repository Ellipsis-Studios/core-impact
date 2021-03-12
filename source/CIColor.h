//
//  CIColor.h
//  CoreImpact
//
//  This class represents the colors that are used by stardust and the planet.
//  It maps from a custom enum to cugl Color4 colors.
//
//  Created by Brandon Stein on 3/1/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_COLOR_H__
#define __CI_COLOR_H__
#include <cugl/cugl.h>

class CIColor {
public:
    /**
     * Enum representing the different colors stardust or planet layers can have
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
    static cugl::Color4 getColor4(Value v) {
        cugl::Color4 color;
        switch(v)
        {
            case red:
                color = cugl::Color4::RED;
                break;
            case green:
                color = cugl::Color4::GREEN;
                break;
            case blue:
                color = cugl::Color4::BLUE;
                break;
            case cyan:
                color = cugl::Color4::CYAN;
                break;
            case magenta:
                color = cugl::Color4::MAGENTA;
                break;
            case yellow:
                color = cugl::Color4::YELLOW;
                break;
        }
        
        return color;
    }

    /**
     * Get the Color4 represented by a particular CIColor value
     */
    static cugl::Color4f getColor4f(Value v) {
        cugl::Color4f color;
        switch (v)
        {
        case red:
            color = cugl::Color4f::RED;
            break;
        case green:
            color = cugl::Color4f::GREEN;
            break;
        case blue:
            color = cugl::Color4f::BLUE;
            break;
        case cyan:
            color = cugl::Color4f::CYAN;
            break;
        case magenta:
            color = cugl::Color4f::MAGENTA;
            break;
        case yellow:
            color = cugl::Color4f::YELLOW;
            break;
        }

        return color;
    }


    
    /**
     * Get random CIColor Value.
     */
    static Value getRandomColor() {
        return Value(rand() % 6);
    }
};

#endif /* __CI_COLOR_H__ */
