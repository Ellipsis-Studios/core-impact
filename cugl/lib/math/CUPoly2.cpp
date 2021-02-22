//
//  CUPoly2.cpp
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

#include <algorithm>
#include <vector>
#include <stack>
#include <sstream>
#include <cmath>
#include <iterator>
#include <cugl/util/CUDebug.h>
#include <cugl/util/CUStrings.h>
#include <cugl/math/CUPoly2.h>
#include <cugl/math/CURect.h>
#include <cugl/math/CUMat4.h>
#include <cugl/math/CUAffine2.h>

using namespace std;
using namespace cugl;

#pragma mark Setters
/**
 * Sets this polygon to be have the resources of the given one.
 *
 * @param other  The polygon to take from
 *
 * @return This polygon, returned for chaining
 */
Poly2& Poly2::operator=(Poly2&& other) {
	_vertices = std::move(other._vertices);
	_indices = std::move(other._indices);
	_bounds = std::move(other._bounds);
	_geom = other._geom;
	return *this;
}
    
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
Poly2& Poly2::set(const vector<Vec2>& vertices) {
    _vertices.assign(vertices.begin(),vertices.end());
    _indices.clear();
    _geom = Geometry::IMPLICIT;
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const vector<Vec2>& vertices, const vector<Uint32>& indices) {
    _vertices.assign(vertices.begin(),vertices.end());
    _indices.assign(indices.begin(),indices.end());
    _geom = Geometry::categorize(indices);
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const vector<float>& vertices) {
    vector<Vec2>* ref = (vector<Vec2>*)&vertices;
    _vertices.assign(ref->begin(),ref->end());
    _indices.clear();
    _geom = Geometry::IMPLICIT;
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const vector<float>& vertices, const vector<Uint32>& indices) {
    vector<Vec2>* ref = (vector<Vec2>*)&vertices;
    _vertices.assign(ref->begin(),ref->end());
    _indices.assign(indices.begin(),indices.end());
    _geom = Geometry::categorize(indices);
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const Vec2* vertices, size_t vertsize) {
    _vertices.assign(vertices,vertices+vertsize);
    _indices.clear();
    _geom = Geometry::IMPLICIT;
    computeBounds();
    return *this;
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
Poly2& Poly2::set(const Vec2* vertices, size_t vertsize,
                  const Uint32* indices, size_t indxsize) {
    _vertices.assign(vertices,vertices+vertsize);
    _indices.assign(indices, indices+indxsize);
    _geom = Geometry::categorize(indices,indxsize);
    computeBounds();
    return *this;
}

/**
 * Creates a copy of the given polygon.
 *
 * Both the vertices and the indices are copied.  No references to the
 * original polygon are kept.
 *
 * @param poly  The polygon to copy
 */
Poly2& Poly2::set(const Poly2& poly) {
    _vertices.assign(poly._vertices.begin(),poly._vertices.end());
    _indices.assign(poly._indices.begin(),poly._indices.end());
    _bounds = poly._bounds;
    _geom = poly._geom;
    return *this;
}

/**
 * Sets the polygon to represent the given rectangle.
 *
 * The polygon will have four vertices, one for each corner of the rectangle.
 * This optional argument (which is true by default) will initialize the
 * indices with a triangulation of the rectangle.  In other words, the type
 * will be SOLID. This is faster than using one of the more heavy-weight
 * triangulators.
 *
 * If solid is false, it will still generate indices, but will be a PATH
 * instead.
 *
 * @param rect  The rectangle to copy
 * @param solid Whether to treat this rectangle as a solid polygon
 *
 * @return This polygon, returned for chaining
 */
Poly2& Poly2::set(const Rect rect, bool solid) {
    _vertices.resize(4,Vec2::ZERO);
    
    _vertices[0] = rect.origin;
    _vertices[1] = Vec2(rect.origin.x+rect.size.width, rect.origin.y);
    _vertices[2] = Vec2(rect.origin.x+rect.size.width, rect.origin.y+rect.size.height);
    _vertices[3] = Vec2(rect.origin.x, rect.origin.y+rect.size.height);
    
    if (solid) {
        _indices.resize(6,0);
        _indices[0] = 0;
        _indices[1] = 1;
        _indices[2] = 2;
        _indices[3] = 0;
        _indices[4] = 2;
        _indices[5] = 3;
        _geom = Geometry::SOLID;
    } else {
        _indices.resize(8,0);
        _indices[0] = 0;
        _indices[1] = 1;
        _indices[2] = 1;
        _indices[3] = 2;
        _indices[4] = 2;
        _indices[5] = 3;
        _indices[6] = 3;
        _indices[7] = 0;
        _geom = Geometry::PATH;
    }
    _bounds = rect;
    return *this;
}

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
Poly2& Poly2::setIndices(const vector<Uint32>& indices) {
    _indices.assign(indices.begin(), indices.end());
    _geom = Geometry::categorize(indices);
    return *this;
}

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
Poly2& Poly2::setIndices(const Uint32* indices, size_t indxsize) {
    _indices.assign(indices, indices+indxsize);
    _geom = Geometry::categorize(indices,indxsize);
    return *this;
}

/**
 * Clears the contents of this polygon and sets the type to UNDEFINED
 *
 * @return This polygon, returned for chaining
 */
Poly2& Poly2::clear() {
    _vertices.clear();
    _indices.clear();
    _geom = Geometry::IMPLICIT;
    _bounds = Rect::ZERO;
    return *this;
}

#pragma mark -
#pragma mark Polygon Operations

/**
 * Uniformly scales all of the vertices of this polygon.
 *
 * The vertices are scaled from the origin of the coordinate space.  This
 * means that if the origin is not in the interior of this polygon, the
 * polygon will be effectively translated by the scaling.
 *
 * @param scale The uniform scaling factor
 *
 * @return This polygon, scaled uniformly.
 */
Poly2& Poly2::operator*=(float scale) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii] *= scale;
    }
    
    computeBounds();
    return *this;
}

/**
 * Nonuniformly scales all of the vertices of this polygon.
 *
 * The vertices are scaled from the origin of the coordinate space.  This
 * means that if the origin is not in the interior of this polygon, the
 * polygon will be effectively translated by the scaling.
 *
 * @param scale The non-uniform scaling factor
 *
 * @return This polygon, scaled non-uniformly.
 */
Poly2& Poly2::operator*=(const Vec2 scale) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x *= scale.x;
        _vertices[ii].y *= scale.y;
    }
    
    computeBounds();
    return *this;
}

/**
 * Transforms all of the vertices of this polygon.
 *
 * @param transform The affine transform
 *
 * @return This polygon with the vertices transformed
 */
Poly2& Poly2::operator*=(const Affine2& transform) {
    Vec2 tmp;
    for(int ii = 0; ii < _vertices.size(); ii++) {
        Affine2::transform(transform,_vertices[ii], &tmp);
        _vertices[ii] = tmp;
    }
    
    computeBounds();
    return *this;
}

/**
 * Transforms all of the vertices of this polygon.
 *
 * The vertices are transformed as points. The z-value is 0.
 *
 * @param transform The transform matrix
 *
 * @return This polygon with the vertices transformed
 */
Poly2& Poly2::operator*=(const Mat4& transform) {
    Vec2 tmp;
    for(int ii = 0; ii < _vertices.size(); ii++) {
        Mat4::transform(transform,_vertices[ii], &tmp);
        _vertices[ii] = tmp;
    }
    
    computeBounds();
    return *this;
}

/**
 * Uniformly scales all of the vertices of this polygon.
 *
 * The vertices are scaled from the origin of the coordinate space.  This
 * means that if the origin is not in the interior of this polygon, the
 * polygon will be effectively translated by the scaling.
 *
 * @param scale The inverse of the uniform scaling factor
 *
 * @return This polygon, scaled uniformly.
 */
Poly2& Poly2::operator/=(float scale) {
    CUAssertLog(scale != 0, "Division by 0");
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x /= scale;
        _vertices[ii].y /= scale;
    }
    
    computeBounds();
    return *this;
}

/**
 * Nonuniformly scales all of the vertices of this polygon.
 *
 * The vertices are scaled from the origin of the coordinate space.  This
 * means that if the origin is not in the interior of this polygon, the
 * polygon will be effectively translated by the scaling.
 *
 * @param scale The inverse of the non-uniform scaling factor
 *
 * @return This polygon, scaled non-uniformly.
 */
Poly2& Poly2::operator/=(const Vec2 scale) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x /= scale.x;
        _vertices[ii].y /= scale.y;
    }
    
    computeBounds();
    return *this;
}

/**
 * Uniformly translates all of the vertices of this polygon.
 *
 * @param offset The uniform translation amount
 *
 * @return This polygon, translated uniformly.
 */
Poly2& Poly2::operator+=(float offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x += offset;
        _vertices[ii].y += offset;
    }
    
    computeBounds();
    return *this;
}

/**
 * Non-uniformly translates all of the vertices of this polygon.
 *
 * @param offset The non-uniform translation amount
 *
 * @return This polygon, translated non-uniformly.
 */
Poly2& Poly2::operator+=(const Vec2 offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii] += offset;
    }
    
    computeBounds();
    return *this;
}

/**
 * Uniformly translates all of the vertices of this polygon.
 *
 * @param offset The inverse of the uniform translation amount
 *
 * @return This polygon, translated uniformly.
 */
Poly2& Poly2::operator-=(float offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x -= offset;
        _vertices[ii].y -= offset;
    }
    
    computeBounds();
    return *this;
}

/**
 * Non-uniformly translates all of the vertices of this polygon.
 *
 * @param offset The inverse of the non-uniform translation amount
 *
 * @return This polygon, translated non-uniformly.
 */
Poly2& Poly2::operator-=(const Vec2 offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii] -= offset;
    }
    
    computeBounds();
    return *this;
}

#pragma mark -
#pragma mark Geometry Methods
/**
 * Returns true if the given point is incident to the given line segment.
 *
 * The variance specifies the tolerance that we allow for begin off the line
 * segment.
 *
 * @param point     The point to check
 * @param a         The start of the line segment
 * @param b         The end of the line segment
 * @param variance  The distance tolerance
 *
 * @return true if the given point is incident to the given line segment.
 */
static bool onsegment(const Vec2 point, const Vec2 a, const Vec2 b, float variance) {
    float d1 = point.distance(a);
    float d2 = point.distance(b);
    float d3 = a.distance(b);
    return fabsf(d3-d2-d1) <= variance;
}

/**
 * This class implements a pivot for the Graham Scan convex hull algorithm.
 *
 * This pivot allows us to have a relative comparison function to an
 * anchor point.
 */
class GSPivot {
public:
    /** The pivot anchor */
    Vec2* anchor;
    
    /** Constructs an empty pivot */
    GSPivot() : anchor(nullptr) {}
    
    /**
     * Returns true if a < b. in the polar order with respect to the pivot.
     *
     * The polar order is computed relative to the pivot anchor.
     *
     * @param a     The first point
     * @param b     The second point
     *
     * @return rue if a < b. in the polar order with respect to the pivot.
     */
    bool compare(const Vec2 a, const Vec2 b) {
        int order = Poly2::orientation(*anchor, a, b);
        if (order == 0) {
            float d1 = anchor->distanceSquared(a);
            float d2 = anchor->distanceSquared(b);
            return (d1 < d2 || (d1 == d2 && a < b));
        }
        return (order == -1);
    }
};

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
std::vector<Vec2> Poly2::convexHull() const {
    std::vector<Vec2> points;
    std::copy(_vertices.begin(), _vertices.end(), std::back_inserter(points));
    std::vector<Vec2> hull;

    // Find the bottommost point (or chose the left most point in case of tie)
    int n = (int)points.size();
    int ymin = 0;
    for (int ii = 1; ii < n; ii++) {
        float y1 = points[ii].y;
        float y2 = points[ymin].y;
        if (y1 < y2 || (y1 == y2 && points[ii].x < points[ymin].x)) {
            ymin = ii;
        }
    }
    
    // Place the bottom-most point at first position
    Vec2 temp = points[0];
    points[0] = points[ymin];
    points[ymin] = temp;

    // Set the pivot at this first point
    GSPivot pivot;
    pivot.anchor = &points[0];
    
    // Sort the remaining points by polar angle.
    // This creates a counter-clockwise traversal of the points.
    std::sort(points.begin()+1, points.end(),
              [&](const Vec2 a, const Vec2 b) { return pivot.compare(a,b); });
    
    
    // Remove the colinear points.
    int m = 1;
    for (int ii = 1; ii < n; ii++) {
        // Keep removing i while angle of i and i+1 is same with respect to pivot
        while (ii < n-1 && orientation(*(pivot.anchor), points[ii], points[ii+1]) == 0) {
            ii++;
        }
        
        points[m] = points[ii];
        m++;   // Update size of modified array
    }
    points.resize(m);

    // If modified array of points has less than 3 points, convex hull is not possible
    if (m < 3) {
        return hull;
    }
    
    // Push first three points to the vector (as a stack).
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    hull.push_back(points[2]);
    
    // Process remaining n-3 points
    for (int ii = 3; ii < m; ii++) {
        // Keep removing back whenever we make a non-left turn
        Vec2* atback   = &(hull[hull.size()-1]);
        Vec2* nextback = &(hull[hull.size()-2]);
        
        while (orientation(*nextback, *atback, points[ii]) != -1) {
            hull.pop_back();
            atback = nextback;
            nextback = &(hull[hull.size()-2]);
        }
        hull.push_back(points[ii]);
    }
    
    return hull;
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
bool Poly2::contains(float x, float y, bool implicit) const {
    switch (_geom) {
        case Geometry::POINTS:
            return false;
        case Geometry::IMPLICIT:
        case Geometry::PATH:
            return containsCrossing( x,y );
        case Geometry::SOLID:
            bool inside = false;
            for (int ii = 0; !inside && 3 * ii < _indices.size(); ii++) {
                Vec2 temp2(x,y);
                Vec3 temp3 = getBarycentric( temp2, ii );
                inside = (0 <= temp3.x && temp3.x <= 1 &&
                          0 <= temp3.y && temp3.y <= 1 &&
                          0 <= temp3.z && temp3.z <= 1);
            }
            return inside;
    }
    return false;
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
bool Poly2::incident(float x, float y, float err) const {
    switch(_geom) {
        case Geometry::IMPLICIT:
        {
            for (int ii = 0; ii < _vertices.size()-1; ii++) {
                if (isColinear(_vertices[ii],_vertices[ii+1],Vec2(x,y),err)) {
                    return true;
                }
            }
            if (isColinear(_vertices[_vertices.size()-1],_vertices[0],Vec2(x,y),err)) {
                return true;
            }
            break;
        }
        case Geometry::POINTS:
        {
            for (int ii = 0; ii < _vertices.size(); ii++) {
                float dx = fabsf(x-_vertices[ii].x);
                float dy = fabsf(y-_vertices[ii].y);
                if (dx < err && dy < err) {
                    return true;
                }
            }
            break;
        }
        case Geometry::PATH:
        {
            for (size_t ii = 0; ii < _indices.size(); ii += 2) {
                Vec2 v = _vertices[_indices[ii]];
                Vec2 w = _vertices[_indices[ii+1]];
                if (isColinear(v,w,Vec2(x,y),err)) {
                    return true;
                }
            }
            break;
        }
        case Geometry::SOLID:
        {
            std::vector<std::vector<Uint32>> boundaries = _geom.boundaries(_indices);
            for(auto it = boundaries.begin(); it != boundaries.end(); ++it) {
                for (size_t ii = 0; ii < it->size(); ii += 2) {
                    Vec2 v = _vertices[it->at(ii)];
                    Vec2 w = _vertices[it->at(ii+1)];
                    if (isColinear(v,w,Vec2(x,y),err)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

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
int Poly2::orientation() const {
    switch(_geom) {
        case Geometry::IMPLICIT:
        {
            Uint32 idx = hullPoint();
            Uint32 bx = idx == 0 ? (Uint32)_vertices.size()-1 : idx-1;
            Uint32 ax = idx == (Uint32)_vertices.size()-1 ? 0 : idx+1;
            return orientation(_vertices[bx],_vertices[idx],_vertices[ax]);
        }
        case Geometry::POINTS:
        {
            return 0;
        }
        case Geometry::PATH:
        {
            std::vector<std::vector<Uint32>> boundaries = _geom.boundaries(_indices);
            int orient = -2;
            for(auto it = boundaries.begin(); it != boundaries.end(); ++it) {
                Uint32 idx = hullPoint(*it);
                Uint32 bx = idx == 0 ? (Uint32)it->size()-1 : idx-1;
                Uint32 ax = idx == (Uint32)it->size()-1 ? 0 : idx+1;
                Uint32 temp = orientation(_vertices[_indices[bx]],
                                          _vertices[_indices[idx]],
                                          _vertices[_indices[ax]]);
                if (orient == -2) {
                    orient = temp;
                } else if (orient != temp || temp == 0) {
                    return 0;
                }
            }
            return orient;
        }
        case Geometry::SOLID:
        {
            int orient = -2;
            for(size_t ii = 0; ii < _indices.size(); ii+=3) {
                Uint32 temp = orientation(_vertices[_indices[ii  ]],
                                          _vertices[_indices[ii+1]],
                                          _vertices[_indices[ii+2]]);
                if (orient == -2) {
                    orient = temp;
                } else if (orient != temp || temp == 0) {
                    return 0;
                }
            }
            return orient;
        }
    }
    return 0;
}

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
int Poly2::orientation(Vec2 a, Vec2 b, Vec2 c) {
    float val = (b.y - a.y) * (c.x - a.x) - (b.x - a.x) * (c.y - a.y);
    if (-CU_MATH_EPSILON < val && val < CU_MATH_EPSILON) return 0;  // colinear
    return (val > 0) ? 1: -1; // clock or counterclock wise
}

/**
 * Reverses the orientation of this polygon.
 *
 * If the polygon orientation is undefined, then this method does nothing.
 * Otherwise, it reorders either the vertices or the mesh indices to reverse
 * the orientation. Which one is resorted (vertices or indices) is undefined.
 */
void Poly2::reverse() {
    if (orientation() == 0) {
        return;
    }
    
    switch(_geom) {
        case Geometry::IMPLICIT:
            std::reverse(_vertices.begin(),_vertices.end());
            break;
        case Geometry::POINTS:
            break;
        case Geometry::PATH:
            std::reverse(_indices.begin(),_indices.end());
            break;
        case Geometry::SOLID:
            for(size_t ii = 0; ii < _indices.size(); ii+=3) {
                Uint32 temp = _indices[ii];
                _indices[ii] = _indices[ii+2];
                _indices[ii+2] = temp;
            }
            break;
    }
}



#pragma mark -
#pragma mark Conversion Methods
/**
 * Returns a string representation of this rectangle for debugging purposes.
 *
 * If verbose is true, the string will include class information.  This
 * allows us to unambiguously identify the class.
 *
 * @param verbose Whether to include class information
 *
 * @return a string representation of this rectangle for debuggging purposes.
 */
std::string Poly2::toString(bool verbose) const {
    std::stringstream ss;
    ss << (verbose ? "cugl::Poly2[" : "[");
    switch (_geom) {
    case Geometry::IMPLICIT:
    case Geometry::POINTS:
    	ss << (_geom == Geometry:: IMPLICIT ? "IMPLICIT" : "POINTS");
    	for(auto it = _vertices.begin(); it != _vertices.end(); ++it) {
    		ss << (it == _vertices.begin() ? "; " : ", ");
    		ss << it->toString();
    	}
    	break;
    case Geometry::SOLID:
    	ss << "SOLID";
    	for(size_t ii = 0; ii < _indices.size(); ii += 3) {
    		ss << (ii == 0 ? "; " : ", ");
    		ss << "{ ";
    		ss << _vertices[_indices[ii+0]].toString();
    		ss << ", ";
    		ss << _vertices[_indices[ii+1]].toString();
    		ss << ", ";
    		ss << _vertices[_indices[ii+2]].toString();
    		ss << " }";
    	}
    	break;
    case Geometry::PATH:
    	ss << "PATH; ";
    	for(size_t ii = 0; ii < _indices.size(); ii += 2) {
    		ss << (ii == 0 ? "; " : ", ");
    		ss << _vertices[_indices[ii+0]].toString();
    		ss << "---";
    		ss << _vertices[_indices[ii+1]].toString();
    	}
    	break;
    }
    ss << "]";
    return ss.str();
}

/** Cast from Poly2 to a Rect. */
Poly2::operator Rect() const {
	return _bounds;
}

#pragma mark -
#pragma mark Internal Helpers

/**
 * Compute the bounding box for this polygon.
 *
 * The bounding box is the minimal rectangle that contains all of the vertices in
 * this polygon.  It is recomputed whenever the vertices are set.
 */
void Poly2::computeBounds() {
    float minx, maxx;
    float miny, maxy;
    
    minx = _vertices[0].x;
    maxx = _vertices[0].x;
    miny = _vertices[0].y;
    maxy = _vertices[0].y;
    for(auto it = _vertices.begin()+1; it != _vertices.end(); ++it) {
        if (it->x < minx) {
            minx = it->x;
        } else if (it->x > maxx) {
            maxx = it->x;
        }
        if (it->y < miny) {
            miny = it->y;
        } else if (it->y > maxy) {
            maxy = it->y;
        }
    }
    
    _bounds.origin.x = minx;
    _bounds.origin.y = miny;
    _bounds.size.width  = maxx-minx;
    _bounds.size.height = maxy-miny;
}

/**
 * Returns the barycentric coordinates for a point relative to a triangle.
 *
 * The triangle is identified by the given index.  For index ii, it is the
 * triangle defined by indices 3*ii, 3*ii+1, and 3*ii+2.
 *
 * This method is not defined if the polygon is not SOLID.
 */
Vec3 Poly2::getBarycentric(Vec2 point, Uint32 index) const {
    Vec2 a = _vertices[_indices[3*index  ]];
    Vec2 b = _vertices[_indices[3*index+1]];
    Vec2 c = _vertices[_indices[3*index+2]];
    
    float det = (b.y-c.y)*(a.x-c.x)+(c.x-b.x)*(a.y-c.y);
    Vec3 result;
    result.x = (b.y-c.y)*(point.x-c.x)+(c.x-b.x)*(point.y-c.y);
    result.y = (c.y-a.y)*(point.x-c.x)+(a.x-c.x)*(point.y-c.y);
    result.x /= det;
    result.y /= det;
    result.z = 1 - result.x - result.y;
    return result;
}

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
bool Poly2::containsCrossing(float x, float y) const {
    // Use a winding rule otherwise
    int intersects = 0;
    
    if (_geom == Geometry::IMPLICIT) {
        for (size_t ii = 0; ii < _vertices.size(); ii++) {
            Vec2 v1 = _vertices[ii];
            Vec2 v2 = _vertices[ii+1];
            if (((v1.y <= y && y < v2.y) || (v2.y <= y && y < v1.y)) && x < ((v2.x - v1.x) / (v2.y - v1.y) * (y - v1.y) + v1.x)) {
                intersects++;
            }
        }
    } else {
        for (size_t ii = 0; ii < _indices.size()-2; ii += 2) {
            Vec2 v1 = _vertices[_indices[ii]  ];
            Vec2 v2 = _vertices[_indices[ii+1]];
            if (((v1.y <= y && y < v2.y) || (v2.y <= y && y < v1.y)) && x < ((v2.x - v1.x) / (v2.y - v1.y) * (y - v1.y) + v1.x)) {
                intersects++;
            }
        }
    }
    return (intersects & 1) == 1;

}

bool Poly2::isColinear(Vec2 v, Vec2 w, Vec2 p, float err) {
    const float l2 = (w-v).lengthSquared();
    double distance = 0.0f;
    if (l2 == 0.0) {
        distance = p.distance(v);
    } else {
        const float t = std::max(0.0f, std::min(1.0f, (p - v).dot(w - v) / l2));
        Vec2 projection = v + t * (w - v);
        distance = p.distance(projection);
    }

    return (distance <= err);
}

Uint32 Poly2::hullPoint() const {
    CUAssertLog(!_vertices.empty(), "The polygon is empty");
    
    // Find the min point
    double mx = _vertices[0].x;
    double my = _vertices[0].y;
    size_t pos = 0;
    for(size_t ii = 1; ii < _vertices.size(); ii++) {
        if (_vertices[ii].x < mx) {
            mx = _vertices[ii].x;
            my = _vertices[ii].y;
            pos = ii;
        } else if (_vertices[ii].x == mx && _vertices[ii].y < my) {
            my = _vertices[ii].y;
            pos = ii;
        }
    }
    return (Uint32)pos;
}
    
    
Uint32 Poly2::hullPoint(const std::vector<Uint32> indices) const {
    CUAssertLog(!_indices.empty(), "The polygon is empty");
    
    // Find the min point
    double mx = _vertices[_indices[0]].x;
    double my = _vertices[_indices[0]].y;
    size_t pos = 0;
    for(size_t ii = 1; ii < _vertices.size(); ii++) {
        if (_vertices[_indices[ii]].x < mx) {
            mx = _vertices[_indices[ii]].x;
            my = _vertices[_indices[ii]].y;
            pos = ii;
        } else if (_vertices[_indices[ii]].x == mx && _vertices[_indices[ii]].y < my) {
            my = _vertices[_indices[ii]].y;
            pos = ii;
        }
    }
    return (Uint32)pos;
}
