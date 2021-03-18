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
        orange, 
        yellow,
        turquoise,
        cyan, 
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
            case orange:
                color = cugl::Color4(252.0f, 68.0f, 34.0f);
                break;
            case yellow:
                color = cugl::Color4(253.0f, 142.0f, 50.0f);
                break;
            case turquoise:
                color = cugl::Color4(0.0f, 181.0f, 137.0f);
                break;
            case cyan:
                color = cugl::Color4(33.0f, 189.0f, 202.0f);
                break;
            case blue:
                color = cugl::Color4(70.0f, 87.0f, 206.0f);
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
        case orange:
            color = "orange";
            break;
        case yellow:
            color = "yellow";
            break;
        case turquoise:
            color = "turquoise";
            break;
        case cyan:
            color = "cyan";
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
        std::uniform_int_distribution<int> dist(0, 4); // pick out of first 5 colors
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
