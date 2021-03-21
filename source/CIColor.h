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
#include <random>
#include <string>


class CIColor {
public:
    /**
     * Enum representing the different colors stardust or planet layers can have
     */
    enum Value {
        purple,
        red,
        orange,
        turquoise,
        green,
        blue, 
        grey, 
        lightgrey,
    };

    
    /**
     * Get the Color4 represented by a particular CIColor value
     * 
     * @param CIColor Value 
     * 
     * @return Color4 representation of the CIColor Value
     */
    static cugl::Color4 getColor4(Value v) {
        cugl::Color4 color;
        switch(v)
        {
            case purple:
                color = cugl::Color4(145.0f, 20.0f, 218.0f);
                break;
            case red:
                color = cugl::Color4(252.0f, 67.0f, 34.0f);
                break;
            case orange:
                color = cugl::Color4(211.0f, 132.0f, 19.0f);
                break;
            case turquoise:
                color = cugl::Color4(33.0f, 189.0f, 202.0f);
                break;
            case green:
                color = cugl::Color4(43.0f, 151.0f, 26.0f);
                break;
            case blue:
                color = cugl::Color4(33.0f, 49.0f, 165.0f);
                break;
            case grey:
                color = cugl::Color4(194.0f, 194.0f, 194.0f);
                break;
            case lightgrey:
                color = cugl::Color4(252.0f, 239.0f, 237.0f);
                break;
        }
        
        return color;
    }

    /**
     * Get the string representation of CIColor Value
     *
     * @param CIColor Value
     *
     * @return string representation of the CIColor Value
     */
    static string getString(Value v) {
        string color;
        switch (v)
        {
        case purple:
            color = "purple";
            break;
        case red:
            color = "red";
            break;
        case orange:
            color = "orange";
            break;
        case turquoise:
            color = "turquoise";
            break;
        case green:
            color = "green";
            break;
        case blue:
            color = "blue";
            break;
        case grey:
            color = "grey";
            break;
        case lightgrey:
            color = "lightgrey";
            break;
        }

        return color;
    }

    /** 
     * Returns a random CIColor Value for a stardust.
     * 
     * This method picks out of the first 5 CIColor values since they 
     * are valid for stardust color. 
     * 
     * @return CIColor Value for a stardust 
     */
    static Value getRandomColor() {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(0, 5); // pick out of first 5 colors
        int rand = dist(mt);
        return Value(rand);  
    }
    
    /**
     * Get the CIColor Value representing the lack of a color
     */
    static Value getNoneColor() {
        return Value::lightgrey;
    }
};

#endif /* __CI_COLOR_H__ */
