//
//  CUComplexExtruder.h
//  Cornell University Game Library (CUGL)
//
//  This module is a factory for extruding a path polgyon into a stroke with
//  width. It has support for joints and end caps.
//
//  This version of the extruder is built on top of the famous Clipper library:
//
//      http://www.angusj.com/delphi/clipper.php
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
#ifndef __CU_COMPLEX_EXTRUDER_H__
#define __CU_COMPLEX_EXTRUDER_H__

#include <clipper/clipper.hpp>
#include <cugl/math/CUPoly2.h>
#include <cugl/math/CUVec2.h>
#include <cugl/math/polygon/CUPolyEnums.h>
#include <vector>

namespace cugl {
 
/**
 * This class is a factory for extruding wireframe paths into a solid path.
 *
 * An extrusion of a path is a second polygon that follows the path of
 * the first one, but gives it width.  Hence it takes a path and turns it
 * into a solid shape. This is more complicated than simply triangulating
 * the original polygon.  The new polygon has more vertices, depending on
 * the choice of joint (shape at the corners) and cap (shape at the end).
 *
 * This class is significantly more accurate than {@link SimpleExtruder}.
 * The extruded shape has no overlapping triangles and is safe to use with
 * transparency. However, this comes at massive cost in speed. Even a
 * simple line can take a full millisecond to compute, and more complicated
 * paths will significantly affect frame rate. If you need to extrude a
 * path at framerate, you should use {@link SimpleExtruder} instead, and
 * pre-render to a texture if you need transparency.
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
 */
class ComplexExtruder {
#pragma mark Values
private:
    /** The set of vertices to use in the calculation */
    std::vector<std::vector<Vec2>> _input;
    /** Whether the path is closed */
    std::vector<bool> _closed;

    /** The extrusion joint settings */
    ClipperLib::JoinType _joint;
    /** The extrusion end cap settings */
    ClipperLib::EndType  _endcap;
    /** The resolution tolerance of this algorithm */
    Uint32 _resolution;

    /** The output results */
    Poly2 _output;
    /** Whether or not the calculation has been run */
    bool _calculated;
    
#pragma mark -
#pragma mark Constructors
public:
    /**
     * Creates an extruder with no vertex data.
     */
    ComplexExtruder();
    
    /**
     * Creates an extruder with the given vertex data.
     *
     * The vertex data is copied.  The extruder does not retain any references
     * to the original data.
     *
     * @param points    The vertices to extrude
     * @param closed    Whether the path is closed
     */
    ComplexExtruder(const std::vector<Vec2>& points, bool closed);
    
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
    ComplexExtruder(const Poly2& poly);
    
    /**
     * Deletes this extruder, releasing all resources.
     */
    ~ComplexExtruder() {}
    
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
    void setJoint(poly2::Joint joint);
    
    /**
     * Returns the joint value for the extrusion.
     *
     * The joint type determines how the extrusion joins the extruded
     * line segments together.  See {@link poly2::Joint} for the
     * description of the types.
     *
     * @return the joint value for the extrusion.
     */
    poly2::Joint getJoint() const;

    /**
     * Sets the end cap value for the extrusion.
     *
     * The end cap type determines how the extrusion draws the ends of
     * the line segments at the start and end of the path. See
     * {@link poly2::EndCap} for the description of the types.
     *
     * @param endcap    The extrusion end cap type
     */
    void setEndCap(poly2::EndCap endcap);
    
    /**
     * Returns the end cap value for the extrusion.
     *
     * The end cap type determines how the extrusion draws the ends of
     * the line segments at the start and end of the path. See
     * {@link poly2::EndCap} for the description of the types.
     *
     * @return the end cap value for the extrusion.
     */
    poly2::EndCap getEndCap() const;
    
    /**
     * Sets the subdivision resolution for the Clipper library.
     *
     * Clipper is not only accurate, it is also computationally stable.
     * However, it achieves this stable by only using integer coordinates.
     * This class supports float coordinates, but it does it by scaling
     * the points to fit on an integer grid.
     *
     * The resolution is the scaling factor before rounding the points
     * to the nearest integer. It is effectively the same as specifying
     * the number of integer subdivisions supported. For example, if the
     * resolution is 8 (the default), then every point will be rounded
     * to the nearest 1/8 value.
     *
     * @param resolution    The subdivision resolution
     */
    void setResolution(Uint32 resolution) {
         _resolution = resolution;
    }
    
    /**
     * Returns the subdivision resolution for the Clipper library.
     *
     * Clipper is not only accurate, it is also computationally stable.
     * However, it achieves this stable by only using integer coordinates.
     * This class supports float coordinates, but it does it by scaling
     * the points to fit on an integer grid.
     *
     * The resolution is the scaling factor before rounding the points
     * to the nearest integer. It is effectively the same as specifying
     * the number of integer subdivisions supported. For example, if the
     * resolution is 8 (the default), then every point will be rounded
     * to the nearest 1/8 value.
     *
     * @return the subdivision resolution for the Clipper library.
     */
    Uint32 getResolution() const {
         return _resolution;
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
     * the first one, but gives it width.  Hence it takes a path and turns it
     * into a solid shape. This is more complicated than simply triangulating
     * the original polygon.  The new polygon has more vertices, depending on
     * the choice of joint (shape at the corners) and cap (shape at the end).
     *
     * This method uses the Clipper library to perform the extrusion. While
     * accurate and preferred for static shapes, it is not ideal to call
     * this method at framerate.
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
     * the indices defining the extrusion path. The extruder does not maintain
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
     * Processes a single node of a Clipper PolyTree
     *
     * This method is used to extract the data from the Clipper solution
     * and convert it to a cugl Poly2 object. This is a recursive method
     * and assumes that the PolyNode is a outer polygon and not a hole.
     *
     * @param node  The PolyNode to accumulate
     */
    void processNode(const ClipperLib::PolyNode* node);

};
}

#endif /* __CU_PATH_EXTRUDER_H__ */
