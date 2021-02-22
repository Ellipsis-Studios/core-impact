R"(////////// SHADER BEGIN /////////
//  SpriteShader.vert
//  Cornell University Game Library (CUGL)
//
//  This is a full-power SpriteBatch vertex shader for both OpenGL and OpenGL ES.
//  It supports textures which can be tinted per vertex. It also supports gradients
//  (which can be used simulataneously with textures, but not with colors), as
//  well as a scissor mask.  Gradients use the color inputs as their texture
//  coordinates. Finally, there is support for very simple blur effects, which
//  are used for font labels.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 3/20/20
#ifdef CUGLES
// This one line is all the difference
precision highp float;  // highp required for gradient precision
#endif

// Bit vector for texturing, gradients, and scissoring
uniform int  uType;
// Blur offset for simple kernel blur
uniform vec2 uBlur;

// The texture for sampling
uniform sampler2D uTexture;

// The output color
out vec4 frag_color;

// The inputs from the vertex shader
in vec2 outPosition;
in vec4 outColor;
in vec2 outTexCoord;

// The stroke+gradient uniform block
layout (std140) uniform uContext
{
    mat3 scMatrix;      // 48
    vec2 scExtent;      //  8
    vec2 scScale;       //  8
    mat3 gdMatrix;      // 48
    vec4 gdInner;       // 16
    vec4 gdOuter;       // 16
    vec2 gdExtent;      //  8
    float gdRadius;     //  4
    float gdFeathr;     //  4
};

/**
 * Returns an interpolation value for a box gradient
 *
 * The value returned is the mix parameter for the inner and
 * outer color.
 *
 * Adapted from nanovg by Mikko Mononen (memon@inside.org)
 *
 * pt:      The (transformed) point to test
 * ext:     The gradient extent
 * radius:  The gradient radius
 * feather: The gradient feather amount
 */
float boxgradient(vec2 pt, vec2 ext, float radius, float feather) {
    vec2 ext2 = ext - vec2(radius,radius);
    vec2 dst = abs(pt) - ext2;
    float m = min(max(dst.x,dst.y),0.0) + length(max(dst,0.0)) - radius;
    return clamp((m + feather*0.5) / feather, 0.0, 1.0);
}

/**
 * Returns an alpha value for scissoring
 *
 * A pixel with value 0 is dropped, while one with value 1 is kept.
 * The scale value sets the 0 to 1 transition (which should be quick).
 *
 * Adapted from nanovg by Mikko Mononen (memon@inside.org)
 *
 * pt:  The point to test
 */
float scissormask(vec2 pt) {
    vec2 sc = (abs((scMatrix * vec3(pt,1.0)).xy) - scExtent);
    sc = vec2(0.5,0.5) - sc * scScale;
    return clamp(sc.x,0.0,1.0) * clamp(sc.y,0.0,1.0);
}

/**
 * Returns the result of a simple kernel blur
 *
 * This function blurs the texture with a simple 9-element
 * kernel.  It is not meant to provide a full-feature
 * Gaussian blur. It is a high performance blur for simple
 * effects (like font blurring).
 *
 * The idea for this function was taken from
 *     https://learnopengl.com/Advanced-OpenGL/Framebuffers
 *
 * coord: The texture coordinate to blur
 */
vec4 blursample(vec2 coord) {
    // Sample offsets
    vec2 off[9] = vec2[](
        vec2(-uBlur.x,  uBlur.y), // top-left
        vec2(    0.0f,  uBlur.y), // top-center
        vec2( uBlur.x,  uBlur.y), // top-right
        vec2(-uBlur.x,     0.0f), // center-left
        vec2(    0.0f,     0.0f), // center-center
        vec2( uBlur.x,     0.0f), // center-right
        vec2(-uBlur.x, -uBlur.y), // bottom-left
        vec2(    0.0f, -uBlur.y), // bottom-center
        vec2( uBlur.x, -uBlur.y)  // bottom-right
    );
    
    // Blur kernel
    float kernel[9] = float[](
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    );
    
    // Sample from the texture and average
    vec4 result = vec4(0.0);
    for(int ii = 0; ii < 9; ii++) {
        result += texture(uTexture, coord + off[ii])*kernel[ii];
    }

    return result;
}

/**
 * Performs the main fragment shading.
 */
void main(void) {
    vec4 result;
    float fType = float(uType);
    if (mod(fType, 4.0) >= 2.0) {
        // Apply a gradient color
        mat3  cmatrix = gdMatrix;
        vec2  cextent = gdExtent;
        float cfeathr = gdFeathr;
        if (outColor.z != 0.0) {
            // Modulation of start point (linear gradient only)
            cmatrix = mat3(cmatrix[0],cmatrix[1],
                           cmatrix[2].x+(cmatrix[0].x+cmatrix[1].x)*(-outColor.z),
                           cmatrix[2].y+(cmatrix[0].y+cmatrix[1].y)*(-outColor.z),
                           1.0);
        }
        if (outColor.w != 0.0) {
            // Modulation of end point (linear gradient only)
            cextent = vec2(cextent.x,cextent.y+outColor.w*0.5);
            cfeathr = cfeathr+outColor.w;
        }
        vec2 pt = (cmatrix * vec3(outColor.xy,1.0)).xy;
        float d = boxgradient(pt,cextent,gdRadius,cfeathr);
        result = mix(gdInner,gdOuter,d);
    } else {
        // Use a solid color
        result = outColor;
    }
    
    if (mod(fType, 2.0) == 1.0) {
        // Include texture (tinted by color or gradient)
        if (uType >= 8) {
            result *= blursample(outTexCoord);
        } else {
            result *= texture(uTexture, outTexCoord);
        }
    }
    
    if (mod(fType, 8.0) >= 4.0) {
        // Apply scissor mask
        result *= scissormask(outPosition);
    }

    frag_color = result;
}
/////////// SHADER END //////////)"

