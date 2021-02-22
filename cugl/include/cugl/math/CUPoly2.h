//
//  CUPoly2.h
//  Cornell University Game Library (CUGL)
//
//  This module provides a class that represents a simple polygon.  The purpose
//  of this class is to separate the geometry (and math) of a polygon from the
//  rendering data of a pipeline. It is one of the most important classes for
//  2D game design in all of CUGL.
//
//  Polygons all have a corresponding geometry. If they are implicit, they
//  cannot be drawn, but can be used for geometric calculation.  Otherwise,
//  the polygon has a mesh defined by a set of vertices. This class is
//  intentionally (based on experience in previous semesters) lightweight.
//  There is no verification that indices are properly defined.  It is up to
//  the user to verify and specify the components. If you need help with
//  triangulation or path extrusion, use one the the related factory classes.
//
//  Because math objects are intended to be on the stack, we do not provide
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

#ifndef __CU_POLY2_H__
#define __CU_POLY2_H__

#include <vector>
#include <cugl/math/CUVec2.h>
#include <cugl/math/CURect.h>
#include <cugl/math/CUGeometry.h>

namespace cugl {

// Forward references
class Mat4;
class Affine2;
    
/**
 * Class to represent a simple polygon.
 *
 * This class is intended to represent any polygon (including non-convex polygons).
 * that does not have self-interections (as these can cause serious problems with
 * the mathematics).  Most polygons are simple, meaning that they have no holes.
 * However, this class does support complex polygons with holes, provided that
 * the polygon is not implicit and has an corresponding mesh.
 *
 * To define a mesh, the user should provide a set of indices which will be used
 * in rendering. These indices can either represent a triangulation of the polygon,
 * or they can represent a traversal (for a wireframe). The semantics of these
 * indices is provided by the associated {@link Geometry value}. This class performs
 * no verification. It will not check that a mesh is in proper form, nor will it
 * search for holes or self-intersections. These are the responsibility of the
 * programmer.
 *
 * Generating indices for a Poly2 can be nontrivial.  While this class has
 * standard constructors, allowing the programmer full control, most Poly2
 * objects are created through alternate means.  For simple shapes, like lines,
 * triangles, and ellipses, this class has several static constructors.
 *
 * For more complex shapes, we have several Poly2 factories.  These factories
 * allow for delegating index computation to a separate thread, if it takes
 * too long.  These factories are as follows:
 *
 * {@link SimpleTriangulator}: This is a simple earclipping-triangulator for
 * tesselating simple, solid polygons (e.g. no holes or self-intersections).
 *
 * {@link ComplexTriangulator}: This is a Delaunay Triangular that gives a
 * more uniform triangulation in accordance to the Vornoi diagram. It also
 * allows you to create 2d meshes while holes inside of them.
 *
 * {@link PolyFactory}: This is a tool is used to generate several basic 
 * path shapes, such as rounded rectangles or arcs.  It also allows you
 * construct wireframe traversals of existing polygons.
 *
 * {@link PolySplineFactory}: This is a tool is used to generate a Poly2
 * object from a Cubic Bezier curve.
 *
 * {@link SimpleExtruder}: This is a tool can take a path polygon and convert it
 * into a solid polygon.  This solid polygon is the same as the path, except 
 * that the path now has a width and a mitre at the joints. It is fast, but
 * has graphical limitations.
 *
 * {@link ComplexExtruder}: Like {@link SimpleExtruder}, this is a tool can
 * take a path polygon and convert it into a solid polygon. It is incredibly
 * versatile and works in all instances. However, it is extremely slow
 * (in the 10s of milliseconds) and is unsuitable for calcuations at framerate.
 */
class Poly2 {
#pragma mark Values
private:
    /** The vector of vertices in this polygon */
    std::vector<Vec2> _vertices;
    /** The vector of indices in the triangulation */
    std::vector<Uint32> _indices;
    /** The bounding box for this polygon */
    Rect _bounds;
    /** The index semantics */
    Geometry _geom;
    
#pragma mark -
#pragma mark Constructors
public:
    /**
     * Creates an empty polygon.
     *
     * The created polygon has no vertices and no triangulation.  The bounding 
     * box is trivial.
     */
    Poly2() : _geom(Geometry::IMPLICIT) { }
    
    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices and the geometry is `IMPLICIT`.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     */
    Poly2(const std::vector<Vec2>& vertices) { set(vertices); }
    
    /**
     * Creates a polygon with the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     * @param indices   The vector of indices for the rendering
     */
    Poly2(const std::vector<Vec2>& vertices, const std::vector<Uint32>& indices) {
        set(vertices, indices);
    }
    
    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices. 
     *
     * The float array should have an even number of elements.  The number of
     * vertices is half of the size of the array. For each value ii, 2*ii and
     * 2*ii+1 are the coordinates of a single vertex.
     *
     * The new polygon has no indices and the geometry is `IMPLICIT`.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     */
    Poly2(const std::vector<float>& vertices)    { set(vertices); }
    
    /**
     * Creates a polygon with the given vertices and indices.
     *
     * The float array should have an even number of elements.  The number of 
     * vertices is half of the size of the array. For each value ii, 2*ii and
     * 2*ii+1 are the coordinates of a single vertex.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     * @param indices   The vector of indices for the rendering
     */
    Poly2(const std::vector<float>& vertices, const std::vector<Uint32>& indices) {
        set(vertices, indices);
    }
    
    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices and the geometry is `IMPLICIT`.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     */
    Poly2(const float* vertices,  size_t vertsize) {
        set(vertices, vertsize);
    }
    
    /**
     * Creates a polygon with the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     */
    Poly2(const float* vertices, size_t vertsize, const Uint32* indices, size_t indxsize) {
        set(vertices, vertsize, indices, indxsize);
    }

    /**
     * Creates a copy of the given polygon.
     *
     * Both the vertices and the indices are copied.No references to the
     * original polygon are kept.
     *
     * @param poly  The polygon to copy
     */
    Poly2(const Poly2& poly) { set(poly); }

    /**
     * Creates a copy with the resource of the given polygon.
     *
     * @param poly  The polygon to take from
     */
    Poly2(Poly2&& poly) :
        _vertices(std::move(poly._vertices)), _indices(std::move(poly._indices)),
        _bounds(std::move(poly._bounds)), _geom(poly._geom) {}
    
    /**
     * Creates a polygon for the given rectangle.
     *
     * The polygon will have four vertices, one for each corner of the rectangle.
     * This optional argument (which is true by default) will initialize the
     * indices with a triangulation of the rectangle.  In other words, the geometry
     * will be `SOLID`. This is faster than using one of the more heavy-weight
     * triangulators.
     *
     * If solid is false, it will still generate indices, but will have `CLOSED`
     * geometry instead.
     *
     * @param rect  The rectangle to copy
     * @param solid Whether to treat this rectangle as a solid polygon
     */
    Poly2(const Rect rect, bool solid=true) { set(rect,solid); }
    
    /**
     * Deletes the given polygon, freeing all resources.
     */
    ~Poly2() { }

    
#pragma mark -
#pragma mark Setters
    /**
     * Sets this polygon to be a copy of the given one.
     *
     * All of the contents are copied, so that this polygon does not hold any
     * references to elements of the other polygon. This method returns
     * a reference to this polygon for chaining.
     *
     * @param other  The polygon to copy
     *
     * @return This polygon, returned for chaining
     */
    Poly2& operator= (const Poly2& other) { return set(other); }

    /**
     * Sets this polygon to be have the resources of the given one.
     *
     * @param other  The polygon to take from
     *
     * @return This polygon, returned for chaining
     */
    Poly2& operator=(Poly2&& other);
    
    /**
     * Sets this polygon to be a copy of the given rectangle.
     *
     * The polygon will have four vertices, one for each corner of the rectangle.
     * In addition, this assignment will initialize the indices with a simple 
     * triangulation of the rectangle. The geometry will be `SOLID`.
     *
     * @param rect  The rectangle to copy
     *
     * @return This polygon, returned for chaining
     */
    Poly2& operator=(const Rect rect) { return set(rect); }

    /**
     * Sets the polygon to have the given vertices
     *
     * The resulting polygon has no indices and the geometry is `IMPLICIT`.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<Vec2>& vertices);
    
    /**
     * Sets the polygon to have the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     * @param indices   The vector of indices for the rendering
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<Vec2>& vertices, const std::vector<Uint32>& indices);
    
    /**
     * Sets the polygon to have the given vertices
     *
     * The float array should have an even number of elements.  The number of
     * vertices is half of the size of the array. For each value ii, 2*ii and
     * 2*ii+1 are the coordinates of a single vertex.
     *
     * The resulting polygon has no indices and the geometry is `IMPLICIT`.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<float>& vertices);
    
    /**
     * Sets a polygon to have the given vertices and indices.
     *
     * The float array should have an even number of elements.  The number of
     * vertices is half of the size of the array. For each value ii, 2*ii and
     * 2*ii+1 are the coordinates of a single vertex.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     * @param indices   The vector of indices for the rendering
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<float>& vertices, const std::vector<Uint32>& indices);
    
    /**
     * Sets the polygon to have the given vertices.
     *
     * The resulting polygon has no indices and the geometry is `IMPLICIT`.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const Vec2* vertices, size_t vertsize);
    
    /**
     * Sets the polygon to have the given vertices.
     *
     * The float array should have an even number of elements.  The number of
     * vertices is half of the size of the array. For each value ii, 2*ii and
     * 2*ii+1 are the coordinates of a single vertex.
     *
     * The resulting polygon has no indices and the geometry is `IMPLICIT`.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as floats) in this polygon
     * @param vertsize  The number of elements to use from vertices
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const float* vertices,  size_t vertsize) {
        return set((Vec2*)vertices, vertsize/2);
    }
    
    /**
     * Sets the polygon to have the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const Vec2* vertices, size_t vertsize, const Uint32* indices, size_t indxsize);
    
    /**
     * Sets the polygon to have the given vertices and indices.
     *
     * The float array should have an even number of elements.  The number of
     * vertices is half of the size of the array. For each value ii, 2*ii and
     * 2*ii+1 are the coordinates of a single vertex.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as floats) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const float* vertices, size_t vertsize, const Uint32* indices, size_t indxsize) {
        return set((Vec2*)vertices, vertsize/2, indices, indxsize);
    }
    
    /**
     * Sets this polygon to be a copy of the given one.
     *
     * All of the contents are copied, so that this polygon does not hold any
     * references to elements of the other polygon. This method returns
     * a reference to this polygon for chaining.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param poly  The polygon to copy
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const Poly2& poly);
    
    /**
     * Sets the polygon to represent the given rectangle.
     *
     * The polygon will have four vertices, one for each corner of the rectangle.
     * This optional argument (which is true by default) will initialize the
     * indices with a triangulation of the rectangle.  In other words, the geometry
     * will be `SOLID`. This is faster than using one of the more heavy-weight
     * triangulators.
     *
     * If solid is false, it will still generate indices, but will have `CLOSED`
     * geometry instead.
     *
     * @param rect  The rectangle to copy
     * @param solid Whether to treat this rectangle as a solid polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const Rect rect, bool solid=true);

    /**
     * Sets the indices for this polygon to the ones given.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     * This method returns a reference to this polygon for chaining.
     *
     * @param indices   The vector of indices for the shape
     *
     * @return This polygon, returned for chaining
     */
    Poly2& setIndices(const std::vector<Uint32>& indices);
    
    /**
     * Sets the indices for this polygon to the ones given.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * That is, the indices should all be non-negative, and each value should be
     * less than the number of vertices.
     *
     * The provided array is copied.  The polygon does not retain a reference.
     *
     * The index geometry will be assigned via {@link Geometry#categorize}.
     * This method returns a reference to this polygon for chaining.
     *
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     *
     * @return This polygon, returned for chaining
     */
    Poly2& setIndices(const Uint32* indices, size_t indxsize);
    
    /**
     * Clears the contents of this polygon and sets the geometry to `IMPLICIT`
     *
     * @return This polygon, returned for chaining
     */
    Poly2& clear();

#pragma mark -
#pragma mark Polygon Attributes
    /**
     * Returns the number of vertices in a polygon.
     *
     * @return the number of vertices in a polygon.
     */
    size_t size() const { return _vertices.size(); }

    /**
     * Returns the number of indices in a polygon.
     *
     * @return the number of indices in a polygon.
     */
    size_t indexSize() const { return _indices.size(); }

    /**
     * Returns a reference to the attribute at the given index.
     *
     * This accessor will allow you to change the (singular) vertex. It is
     * intended to allow minor distortions to the polygon without changing
     * the underlying mesh.
     *
     * @param index  The attribute index
     *
     * @return a reference to the attribute at the given index.
     */
    Vec2& at(int index) { return _vertices.at(index); }

    /**
     * Returns the list of vertices
     *
     * This accessor will not permit any changes to the vertex array.  To change
     * the array, you must change the polygon via a set() method.
     *
     * @return a reference to the vertex array
     */
    const std::vector<Vec2>& vertices() const { return _vertices; }

    /**
     * Returns the list of vertices
     *
     * This accessor will not permit any changes to the vertex array.  To change
     * the array, you must change the polygon via a set() method.
     *
     * @return a reference to the vertex array
     */
    std::vector<Vec2>& vertices() { return _vertices; }

    /**
     * Returns a reference to list of indices.
     *
     * This accessor will not permit any changes to the index array.  To change
     * the array, you must change the polygon via a set() method.
     *
     * @return a reference to the vertex array
     */
    const std::vector<Uint32>& indices() const  { return _indices; }

    /**
     * Returns a reference to list of indices.
     *
     * This accessor will not permit any changes to the index array.  To change
     * the array, you must change the polygon via a set() method.
     *
     * This non-const version of the method is used by triangulators.
     *
     * @return a reference to the vertex array
     */
    std::vector<Uint32>& indices()  { return _indices; }

    /**
     * Returns the bounding box for the polygon
     *
     * The bounding box is the minimal rectangle that contains all of the vertices in
     * this polygon.  It is recomputed whenever the vertices are set.
     *
     * @return the bounding box for the polygon
     */
    const Rect getBounds() const { return _bounds; }
    
    /**
     * Returns the geometry of this polygon.
     *
     * The type determines the proper form of the indices.
     *
     * If the geometry is `SOLID`, the number of indices should be a multiple
     * of 3. Each triplet should define a triangle over the vertices.
     *
     * If the geometry is `PATH`, the number of indices should be a multiple
     * of 2.  Each pair should define a line segment over the vertices.
     *
     * If the polygon is `IMPLICIT`, the index list should be empty.
     *
     * @return the geometry of this polygon.
     */
    Geometry getGeometry() const { return _geom; }
    
    /**
     * Returns the geometry of this polygon.
     *
     * The type determines the proper form of the indices.
     *
     * If the geometry is `SOLID`, the number of indices should be a multiple
     * of 3. Each triplet should define a triangle over the vertices.
     *
     * If the geometry is `PATH`, the number of indices should be a multiple
     * of 2.  Each pair should define a line segment over the vertices.
     *
     * If the polygon is `IMPLICIT`, the index list should be empty.
     *
     * @param geom  The geometry of this polygon.
     */
    void setGeometry(Geometry geom) { _geom = geom; }
    
    
#pragma mark -
#pragma mark Operators
    /**
     * Uniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space. This
     * means that if the origin is not in the interior of this polygon, the 
     * polygon will be effectively translated by the scaling.
     *
     * @param scale The uniform scaling factor
     *
     * @return This polygon, scaled uniformly.
     */
    Poly2& operator*=(float scale);
    
    /**
     * Nonuniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space. This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * @param scale The non-uniform scaling factor
     *
     * @return This polygon, scaled non-uniformly.
     */
    Poly2& operator*=(const Vec2 scale);

    /**
     * Transforms all of the vertices of this polygon.
     *
     * @param transform The affine transform
     *
     * @return This polygon with the vertices transformed
     */
    Poly2& operator*=(const Affine2& transform);
    
    /**
     * Transforms all of the vertices of this polygon.
     *
     * The vertices are transformed as points. The z-value is 0.
     *
     * @param transform The transform matrix
     *
     * @return This polygon with the vertices transformed
     */
    Poly2& operator*=(const Mat4& transform);
    
    /**
     * Uniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space. This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * @param scale The inverse of the uniform scaling factor
     *
     * @return This polygon, scaled uniformly.
     */
    Poly2& operator/=(float scale);
    
    /**
     * Nonuniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space. This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * @param scale The inverse of the non-uniform scaling factor
     *
     * @return This polygon, scaled non-uniformly.
     */
    Poly2& operator/=(const Vec2 scale);
    
    /**
     * Uniformly translates all of the vertices of this polygon.
     *
     * @param offset The uniform translation amount
     *
     * @return This polygon, translated uniformly.
     */
    Poly2& operator+=(float offset);
    
    /**
     * Non-uniformly translates all of the vertices of this polygon.
     *
     * @param offset The non-uniform translation amount
     *
     * @return This polygon, translated non-uniformly.
     */
    Poly2& operator+=(const Vec2 offset);
    
    /**
     * Uniformly translates all of the vertices of this polygon.
     *
     * @param offset The inverse of the uniform translation amount
     *
     * @return This polygon, translated uniformly.
     */
    Poly2& operator-=(float offset);
    
    /**
     * Non-uniformly translates all of the vertices of this polygon.
     *
     * @param offset The inverse of the non-uniform translation amount
     *
     * @return This polygon, translated non-uniformly.
     */
    Poly2& operator-=(const Vec2 offset);
    
    /**
     * Returns a new polygon by scaling the vertices uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * Note: This method does not modify the polygon.
     *
     * @param scale The uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator*(float scale) const { return Poly2(*this) *= scale; }
    
    /**
     * Returns a new polygon by scaling the vertices non-uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * Note: This method does not modify the polygon.
     *
     * @param scale The non-uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator*(const Vec2 scale) const { return Poly2(*this) *= scale; }
    

    /**
     * Returns a new polygon by transforming all of the vertices of this polygon.
     *
     * Note: This method does not modify the polygon.
     *
     * @param transform The affine transform
     *
     * @return The transformed polygon
     */
    Poly2 operator*(const Affine2& transform) const { return Poly2(*this) *= transform; }

    /**
     * Returns a new polygon by transforming all of the vertices of this polygon.
     *
     * The vertices are transformed as points. The z-value is 0.
     *
     * Note: This method does not modify the polygon.
     *
     * @param transform The transform matrix
     *
     * @return The transformed polygon
     */
    Poly2 operator*(const Mat4& transform) const { return Poly2(*this) *= transform; }
    
    /**
     * Returns a new polygon by scaling the vertices uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * Note: This method does not modify the polygon.
     *
     * @param scale The inverse of the uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator/(float scale) const { return Poly2(*this) /= scale; }
    
    /**
     * Returns a new polygon by scaling the vertices non-uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * Note: This method does not modify the polygon.
     *
     * @param scale The inverse of the non-uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator/(const Vec2 scale) const { return Poly2(*this) /= scale; }
    
    /**
     * Returns a new polygon by translating the vertices uniformly.
     *
     * Note: This method does not modify the polygon.
     *
     * @param offset The uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator+(float offset) const { return Poly2(*this) += offset; }
    
    /**
     * Returns a new polygon by translating the vertices non-uniformly.
     *
     * Note: This method does not modify the polygon.
     *
     * @param offset The non-uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator+(const Vec2 offset) const { return Poly2(*this) += offset; }
    
    /**
     * Returns a new polygon by translating the vertices uniformly.
     *
     * Note: This method does not modify the polygon.
     *
     * @param offset The inverse of the uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator-(float offset) { return Poly2(*this) -= offset; }
    
    /**
     * Returns a new polygon by translating the vertices non-uniformly.
     *
     * Note: This method does not modify the polygon.
     *
     * @param offset The inverse of the non-uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator-(const Vec2 offset) { return Poly2(*this) -= offset; }
    
    /**
     * Returns a new polygon by scaling the vertices uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * @param scale The uniform scaling factor
     * @param poly 	The polygon to scale
     *
     * @return The scaled polygon
     */
    friend Poly2 operator*(float scale, const Poly2& poly) { return poly*scale; }
    
    /**
     * Returns a new polygon by scaling the vertices non-uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This
     * means that if the origin is not in the interior of this polygon, the
     * polygon will be effectively translated by the scaling.
     *
     * @param scale The non-uniform scaling factor
     * @param poly 	The polygon to scale
     *
     * @return The scaled polygon
     */
    friend Poly2 operator*(const Vec2 scale, const Poly2& poly) { return poly*scale; }

    
#pragma mark -
#pragma mark Geometry Methods
    /**
     * Returns the set of points forming the convex hull of this polygon.
     *
     * The returned set of points is guaranteed to be a counter-clockwise traversal
     * of the hull.
     *
     * The points on the convex hull define the "border" of the shape.  In addition
     * to minimizing the number of vertices, this is useful for determining whether
     * or not a point lies on the boundary.
     *
     * This implementation is adapted from the example at
     *
     *   http://www.geeksforgeeks.org/convex-hull-set-2-graham-scan/
     *
     * @return the set of points forming the convex hull of this polygon.
     */
    std::vector<Vec2> convexHull() const;
    
    /**
     * Returns true if this polygon contains the given point.
     *
     * This method returns false is the geometry is `POINTS`. If the geometry is
     * `SOLID`, it checks for containment within the associated triangle mesh.
     * Otherwise, it uses an even-odd crossing rule on the polygon edges (either
     * explicit or implicit) to determine containment.
     *
     * If the value implicit is true, it will treat the polygon implicitly, even
     * if it has a mesh (and no matter the geometry).
     *
     * Containment is not strict. Points on the boundary are contained within
     * this polygon.
     *
     * @param  point    The point to test
     * @param implicit  Whether to ignore indices and use even-odd on vertices
     *
     * @return true if this polygon contains the given point.
     */
    bool contains(Vec2 point, bool implicit=false) const {
        return contains(point.x,point.y,implicit);
    }
    
    /**
     * Returns true if this polygon contains the given point.
     *
     * This method returns false is the geometry is `POINTS`. If the geometry is
     * `SOLID`, it checks for containment within the associated triangle mesh.
     * Otherwise, it uses an even-odd crossing rule on the polygon edges (either
     * explicit or implicit) to determine containment.
     *
     * If the value implicit is true, it will treat the polygon implicitly, even
     * if it has a mesh (and no matter the geometry).
     *
     * Containment is not strict. Points on the boundary are contained within
     * this polygon.
     *
     * @param x         The x-coordinate to test
     * @param y         The y-coordinate to test
     * @param implicit  Whether to ignore indices and use even-odd on vertices
     *
     * @return true if this polygon contains the given point.
     */
    bool contains(float x, float y, bool implicit=false) const;
    
    /**
     * Returns true if the given point is on the boundary of this polygon.
     *
     * This method generates uses {@link Geometry} to determine the boundaries.
     * For `POINTS, it returns true if point is with margin of error of a vertex.
     * For all other shapes it returns true if it is within margin of error of a
     * line segment.
     *
     * @param point The point to check
     * @param err   The distance tolerance
     *
     * @return true if the given point is on the boundary of this polygon.
     */
    bool incident(Vec2 point, float err=CU_MATH_EPSILON) const {
        return incident(point.x,point.y,err);
    }

    /**
     * Returns true if the given point is on the boundary of this polygon.
     *
     * This method generates uses {@link Geometry} to determine the boundaries.
     * For `POINTS, it returns true if point is with margin of error of a vertex.
     * For all other shapes it returns true if it is within margin of error of a
     * line segment.
     *
     * @param x     The x-coordinate to test
     * @param y     The y-coordinate to test
     * @param err   The distance tolerance
     *
     * @return true if the given point is on the boundary of this polygon.
     */
    bool incident(float x, float y, float err=CU_MATH_EPSILON) const;

#pragma mark -
#pragma mark Orientation Methods
    /**
     * Returns -1, 0, or 1 indicating the polygon orientation.
     *
     * If the method returns -1, this is a counter-clockwise polygon. If 1, it
     * is a clockwise polygon.  If 0, that means it is undefined.  The
     * orientation of an `IMPLICIT` polygon is always defined as long as it
     * has at least three vertices.  Polygons with `POINTS` geometry never
     * have a defined orientation.
     *
     * For polygons with `PATH` geometry, the orientation is determined by
     * following the path.  However, if the path is disconnected, this could
     * result in more than one orientation, making the orientation undefined.
     *
     * For polygons with `SOLID` geometry, the orientiation is that of the
     * triangles in the triangle mesh. However, if this value is not uniform
     * (some triangles have one orientation and others do not), then this
     * orientation is undefined.
     *
     * @return -1, 0, or 1 indicating the polygon orientation.
     */
    int orientation() const;

    /**
     * Returns -1, 0, or 1 indicating the orientation of a -> b -> c
     *
     * If the function returns -1, this is a counter-clockwise turn.  If 1, it is
     * a clockwise turn.  If 0, it is colinear.
     *
     * @param a     The first point
     * @param b     The second point
     * @param c     The third point
     *
     * @return -1, 0, or 1 indicating the orientation of a -> b -> c
     */
    static int orientation(Vec2 a, Vec2 b, Vec2 c);
    
    /**
     * Reverses the orientation of this polygon.
     *
     * If the polygon orientation is undefined, then this method does nothing.
     * Otherwise, it reorders either the vertices or the mesh indices to reverse
     * the orientation. Which one is resorted (vertices or indices) is undefined.
     */
    void reverse();

#pragma mark -
#pragma mark Conversion Methods
    /**
     * Returns a string representation of this polygon for debugging purposes.
     *
     * If verbose is true, the string will include class information.  This
     * allows us to unambiguously identify the class.
     *
     * @param verbose Whether to include class information
     *
     * @return a string representation of this polygon for debuggging purposes.
     */
    std::string toString(bool verbose = false) const;
    
    /** Cast from Poly to a string. */
    operator std::string() const { return toString(); }
    
	/** Cast from Poly2 to a Rect. */
    operator Rect() const;

    
#pragma mark -
#pragma mark Internal Helper Methods
private:
    /**
     * Compute the type for this polygon.
     *
     * The bounding box is the minimal rectangle that contains all of the vertices in
     * this polygon.  It is recomputed whenever the vertices are set.
     */
    void computeBounds();

    /**
     * Returns the barycentric coordinates for a point relative to a triangle.
     *
     * The triangle is identified by the given index.  For index ii, it is the
     * triangle defined by indices 3*ii, 3*ii+1, and 3*ii+2.
     *
     * This method is not defined if the polygon is not SOLID.
     *
     * @param point The point to convert
     * @param index The triangle index in this polygon
     *
     * @return the barycentric coordinates for a point relative to a triangle.
     */
    Vec3 getBarycentric(Vec2 point, Uint32 index) const;
    
    static bool isColinear(Vec2 v, Vec2 w, Vec2 p, float err);
    /**
     * Returns true if this polygon contains the given point.
     *
     * This method uses an even-odd crossing rule on the polygon edges
     * (either explicit or implicit) to determine containment.  That is, edges
     * are determined by indices if they exist and vertex order otherwise.
     *
     * Containment is not strict. Points on the boundary are contained within
     * this polygon.  However, this method always returns false if the geometry
     * is {@link Geometry#Points}.
     *
     * @param x     The x-coordinate of the point to check
     * @param y     The y-coordinate of the point to check
     *
     * @return true if this polygon contains the given point.
     */
    bool containsCrossing(float x, float y) const;
    
    Uint32 hullPoint() const;
    
    Uint32 hullPoint(const std::vector<Uint32> indices) const;

    // Make friends with the factory classes
    friend class PolyFactory;
    friend class PolySplineFactory;
    friend class SimpleTriangulator;
    friend class ComplexTriangulator;
    friend class SimpleExtruder;
    friend class ComplexExtruder;
    friend class PathSmoother;
};

}
#endif /* __CU_POLY2_H__ */
