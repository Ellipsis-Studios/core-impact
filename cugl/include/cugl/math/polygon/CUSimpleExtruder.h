//
//  CUSimpleExtruder.h
//  Cornell University Game Library (CUGL)
//
//  This module is a factory for extruding a path polgyon into a stroke with
//  width. It has support for joints and end caps.
//
//  The code in this factory is ported from the Kivy implementation of Line in
//  package kivy.vertex_instructions. We believe that this port is acceptable
//  within the scope of the Kivy license. There are no specific credits in that
//  file, so there is no one specific to credit. However, thanks to the Kivy
//  team for doing the heavy lifting on this method.
//
//  Because they did all the hard work, we will recommend their picture of how
//  joints and end caps work:
//
//      http://kivy.org/docs/_images/line-instruction.png
//
//  Since math objects are intended to be on the stack, we do not provide
//  any shared pointer support in this class.
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
//  Version: 1/22/21
//
#ifndef __CU_SIMPLE_EXTRUDER_H__
#define __CU_SIMPLE_EXTRUDER_H__

#include <cugl/math/CUPoly2.h>
#include <cugl/math/CUVec2.h>
#include <cugl/math/polygon/CUPolyEnums.h>
#include <vector>

namespace cugl {

// Forward reference to the opaque data class.
class KivyData;
    
/**
 * This class is a factory for extruding wireframe paths into a solid path.
 *
 * An extrusion of a path is a second polygon that follows the path of
 * the first one, but gives it width. Hence it takes a path and turns it
 * into a solid shape. This is more complicated than simply triangulating
 * the original polygon. The new polygon has more vertices, depending on
 * the choice of joint (shape at the corners) and cap (shape at the end).
 *
 * This class is significantly faster than {@link ComplexExtruder}, and
 * can be reasonably used at framerate. However, this speed comes at
 * significant cost in flexibility. In particular, the Poly2 created has
 * overlapping triangles (as the algorithm makes no effort to detecting
 * crossing or overlaps). While this is fine if the polygon is drawn with
 * a solid color, it will not look correct if the polygon is drawn with
 * any transparency at all. For an accurate extrusion when transparency
 * is necessary, you should use {@link ComplexExtruder} instead.
 *
 * On the other hand, you can get around this problem by drawing the
 * polygon with a solid (non-transparent) color to a texture, and then
 * applying transparency to the texture.  This is the preferred way to
 * handle transparency if you need extrusion at framerate (such as
 * when you are drawing the path of a finger).
 *
 * As with all factories, the methods are broken up into three phases:
 * initialization, calculation, and materialization.  To use the factory,
 * you first set the data (in this case a set of vertices or another Poly2)
 * with the initialization methods.  You then call the calculation method.
 * Finally, you use the materialization methods to access the data in several
 * different ways.
 *
 * This division allows us to support multithreaded calculation if the data
 * generation takes too long.  However, note that this factory is not thread
 * safe in that you cannot access data while it is still in mid-calculation.
 *
 * CREDITS: This code is ported from the Kivy implementation of Line in package
 * kivy.vertex_instructions.  We believe that this port is acceptable within
 * the scope of the Kivy license.  There are no specific credits in that file,
 * so there is no one specific to credit.  However, thanks to the Kivy team for
 * doing the heavy lifting on this method.
 *
 * Because they did all the hard work, we will recommend their picture of how
 * joints and end caps work:
 *
 *      http://kivy.org/docs/_images/line-instruction.png
 *
 */
class SimpleExtruder {
#pragma mark Values
private:
    /** The set of vertices to use in the calculation */
    std::vector<std::vector<Vec2>> _input;
    /** Whether the path is closed */
    std::vector<bool> _closed;
    
    /** The extrusion joint settings */
    poly2::Joint  _joint;
    /** The extrusion end cap settings */
    poly2::EndCap _endcap;
    /** Active cap (depends on closure) */
    poly2::EndCap _truecap;
    /** The rounded joint/cap precision */
    Uint32 _precision;

    /** The output results of extruded vertices */
    std::vector<Vec2> _outverts;
    /** The output results of extruded indices */
    std::vector<Uint32> _outindx;
    /** Whether or not the calculation has been run */
    bool _calculated;
    
#pragma mark -
#pragma mark Constructors
public:
    /**
     * Creates an extruder with no vertex data.
     */
    SimpleExtruder();
    
    /**
     * Creates an extruder with the given vertex data.
     *
     * The vertex data is copied.  The extruder does not retain any references
     * to the original data.
     *
     * @param points    The vertices to extrude
     * @param closed    Whether the path is closed
     */
    SimpleExtruder(const std::vector<Vec2>& points, bool closed);

    /**
     * Creates an extruder with the given vertex data.
     *
     * The polygon must have geometry `IMPLICIT` or `PATH`. If it is `IMPLICIT`,
     * it assumes the polygon is closed.  Otherwise, it uses the indices to
     * define the path.  However, the path must be continuous. If the method
     * detects a discontinuity in the path, it will only use the first component.
     * Unconnected components should be extruded separately.
     *
     * The vertex data is copied.  The extruder does not retain any references
     * to the original data.
     *
     * @param poly    The vertices to extrude
     */
    SimpleExtruder(const Poly2& poly);
    
    /**
     * Deletes this extruder, releasing all resources.
     */
    ~SimpleExtruder() {}
    
#pragma mark -
#pragma mark Initialization
    /**
     * Sets the vertex data for this extruder.
     *
     * The vertex data is copied.  The extruder does not retain any references
     * to the original data.
     *
     * This method resets all interal data.  You will need to reperform the
     * calculation before accessing data.
     *
     * @param points    The vertices to extruder
     * @param closed    Whether the path is closed
     */
    void set(const std::vector<Vec2>& points, bool closed) {
        reset();
        _input.push_back(points);
        _closed.push_back(closed);
    }

    /**
     * Sets the vertex data for this extruder.
     *
     * The polygon must have geometry `IMPLICIT` or `PATH`. If it is `IMPLICIT`,
     * it assumes the polygon is closed.  Otherwise, it uses the indices to
     * define the path.  However, the path must be continuous. If the method
     * detects a discontinuity in the path, it will only use the first component.
     * Unconnected components should be extruded separately.
     *
     * The vertex data is copied.  The extruder does not retain any references
     * to the original data. The method assumes the polygon is closed if the
     * number of indices is twice the number of vertices.
     *
     * This method resets all interal data.  You will need to reperform the
     * calculation before accessing data.
     *
     * @param poly    The vertices to extrude
     */
    void set(const Poly2& poly);
    
    /**
     * Sets the joint value for the extrusion.
     *
     * The joint type determines how the extrusion joins the extruded
     * line segments together.  See {@link poly2::Joint} for the
     * description of the types.
     *
     * @param joint     The extrusion joint type
     */
    void setJoint(poly2::Joint joint) {
        _joint = joint;
    }
    
     /**
      * Returns the joint value for the extrusion.
      *
      * The joint type determines how the extrusion joins the extruded
      * line segments together.  See {@link poly2::Joint} for the
      * description of the types.
      *
      * @return the joint value for the extrusion.
      */
     poly2::Joint getJoint() const {
         return _joint;
     }

    /**
     * Sets the end cap value for the extrusion.
     *
     * The end cap type determines how the extrusion draws the ends of
     * the line segments at the start and end of the path. See
     * {@link poly2::EndCap} for the description of the types.
     *
     * @param endcap    The extrusion end cap type
     */
    void setEndCap(poly2::EndCap endcap) {
        _endcap = endcap;
    }
    
    /**
     * Returns the end cap value for the extrusion.
     *
     * The end cap type determines how the extrusion draws the ends of
     * the line segments at the start and end of the path. See
     * {@link poly2::EndCap} for the description of the types.
     *
     * @return the end cap value for the extrusion.
     */
    poly2::EndCap getEndCap() const {
        return _endcap;
    }
    
    /**
     * Sets the precision for rounded caps and joints.
     *
     * Rounded joints and caps are created as a polygon fan, just as
     * all rounded shapes are. The precision is the number of
     * polygons in the fan for each joint and each end cap. A lower
     * number will result in less smooth joints/caps and is particular
     * visible in strokes of large width. By default this value is 10.
     *
     * @param precision The precision for rounded caps and joints.
     */
    void setPrecision(Uint32 precision) {
         _precision = precision;
     }
    
    /**
     * Returns the precision for rounded caps and joints.
     *
     * Rounded joints and caps are created as a polygon fan, just as
     * all rounded shapes are. The precision is the number of
     * polygons in the fan for each joint and each end cap. A lower
     * number will result in less smooth joints/caps and is particular
     * visible in strokes of large width. By default this value is 10.
     *
     * @return the precision for rounded caps and joints.
     */
    Uint32 getPrecision() const {
         return _precision;
     }
    
#pragma mark -
#pragma mark Calculation
    /**
     * Clears all computed data, but still maintains the settings.
     *
     * This method preserves all initial vertex data, as well as the joint,
     * cap, and precision settings.
     */
    void reset();
    
    /**
     * Clears all internal data, including initial vertex data.
     *
     * When this method is called, you will need to set a new vertices before
     * calling {@link #calculate}.  However, the joint, cap, and precision
     * settings are preserved.
     */
    void clear();
    
    /**
     * Performs a extrusion of the current vertex data.
     *
     * An extrusion of a polygon is a second polygon that follows the path of
     * the first one, but gives it width. Hence it takes a path and turns it
     * into a solid shape. This is more complicated than simply triangulating
     * the original polygon.  The new polygon has more vertices, depending on
     * the choice of joint (shape at the corners) and cap (shape at the end).
     *
     * CREDITS: This code is ported from the Kivy implementation of Line in 
     * package kivy.vertex_instructions.  We believe that this port is
     * acceptable within the scope of the Kivy license.  There are no specific 
     * credits in that file, so there is no one specific to credit.  However, 
     * thanks to the Kivy team for doing the heavy lifting on this method.
     *
     * Because they did all the hard work, we will plug their picture of how
     * joints and end caps work:
     *
     *      http://kivy.org/docs/_images/line-instruction.png
     *
     * @param stroke    The stroke width of the extrusion
     */
    void calculate(float stroke);
    
#pragma mark -
#pragma mark Materialization
    /**
     * Returns a polygon representing the path extrusion.
     *
     * The polygon contains the a completely new set of vertices together with 
     * the indices defining the extrusion path.  The extruder does not maintain
     * references to this polygon and it is safe to modify it.
     *
     * If the calculation is not yet performed, this method will return the
     * empty polygon.
     *
     * @return a polygon representing the path extrusion.
     */
    Poly2 getPolygon();
    
    /**
     * Stores the path extrusion in the given buffer.
     *
     * This method will add both the new vertices, and the corresponding
     * indices to the new buffer.  If the buffer is not empty, the indices
     * will be adjusted accordingly. You should clear the buffer first if
     * you do not want to preserve the original data.
     *
     * If the calculation is not yet performed, this method will do nothing.
     *
     * @param buffer    The buffer to store the extruded polygon
     *
     * @return a reference to the buffer for chaining.
     */
    Poly2* getPolygon(Poly2* buffer);
    
#pragma mark -
#pragma mark Internal Data Generation
private:
    /**
     * Computes the number of vertices and indices necessary for the extrusion.
     *
     * The number of vertices is put in vcount, while the number of indices
     * is put in icount.  The method returns the total number of points 
     * generating the extruded path, which may be more than the input size when 
     * the path is closed.
     *
     * @param insize    The input size
     * @param vcount    Pointer to store the number of vertices needed.
     * @param icount    Pointer to store the number of vertices needed.
     */
    void computeSize(Uint32 insize, Uint32* vcount, Uint32* icount) const;
    
    /**
     * Creates the extruded line segment from a to b.
     *
     * The new vertices are appended to _outvert, while the new _indices are 
     * appended to _outindx.
     *
     * @param a     The start of the line segment
     * @param b     The end of the line segment.
     * @param data  The data necessary to run the Kivy algorithm.
     */
    void makeSegment(const Vec2 a, const Vec2 b, KivyData* data);

    /**
     * Creates a joint immediately before point a.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param a         The generating point after the joint.
     * @param data      The data necessary to run the Kivy algorithm.
     *
     * @return true if a joint was successfully created.
     */
    bool makeJoint(const Vec2 a, KivyData* data);

    /**
     * Creates a mitre joint immediately before point a.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param a         The generating point after the joint.
     * @param jangle    The joint angle
     * @param data      The data necessary to run the Kivy algorithm.
     *
     * @return true if a joint was successfully created.
     */
    bool makeMitreJoint(const Vec2 a, float jangle, KivyData* data);

    /**
     * Creates a bevel joint immediately before point a.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param a         The generating point after the joint.
     * @param jangle    The joint angle
     * @param data      The data necessary to run the Kivy algorithm.
     *
     * @return true if a joint was successfully created.
     */
    bool makeBevelJoint(const Vec2 a, float jangle, KivyData* data);

    /**
     * Creates a round joint immediately before point a.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param a         The generating point after the joint.
     * @param jangle    The joint angle
     * @param data      The data necessary to run the Kivy algorithm.
     *
     * @return true if a joint was successfully created.
     */
    bool makeRoundJoint(const Vec2 a, float jangle, KivyData* data);
    
    /**
     * Creates the caps on the two ends of the open path.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param count     The number of generating points in the path.
     * @param input		The current input segment
     * @param data      The data necessary to run the Kivy algorithm.
     */
    void makeCaps(int count, std::vector<Vec2>* input, KivyData* data);
    
    /**
     * Creates square caps on the two ends of the open path.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param count     The number of generating points in the path.
     * @param input		The current input segment
     * @param data      The data necessary to run the Kivy algorithm.
     */
    void makeSquareCaps(int count, std::vector<Vec2>* input, KivyData* data);

    /**
     * Creates round caps on the two ends of the open path.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param count     The number of generating points in the path.
     * @param input		The current input segment
     * @param data      The data necessary to run the Kivy algorithm.
     */
    void makeRoundCaps(int count, std::vector<Vec2>* input, KivyData* data);
    
    /**
     * Creates the final joint at the end of a closed path.
     *
     * The new vertices are appended to _outvert, while the new _indices are
     * appended to _outindx.
     *
     * @param input		The current input segment
     * @param data      The data necessary to run the Kivy algorithm.
     *
     * @return true if a joint was successfully created.
     */
    bool makeLastJoint(std::vector<Vec2>* input, KivyData* data);
};
}

#endif /* __CU_SIMPLE_EXTRUDER_H__ */
