//
//  CUPolyFactory.java
//
//  This module provides a convenient way to generate simple polygons, like lines
//  or circles. It is lighter weight than the other factory classes because it
//  does not separate the calculation step from the materialization step. That
//  is because all of its calculations are very short and do not need to be
//  factored into a separate thread.
//
//  Because math objects are intended to be on the stack, we do not provide
//  any shared pointer support in this class.
//
//  This implementation is largely inspired by the LibGDX implementation from
//  Nicolas Gramlich, Eric Spits, Thomas Cate, and Nathan Sweet.
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
//  Version: 1/23/21
//
#include <cugl/math/polygon/CUPolyFactory.h>
#include <cugl/util/CUDebug.h>

using namespace cugl;

/** The default number of segments to use for a rounded shape */
#define DEFAULT_SEGMENTS 16


#pragma mark Initialization
/**
 * Creates a PolyFactory for generating {@link Geometry#Solid} shapes
 */
PolyFactory::PolyFactory() :
_geometry(Geometry::IMPLICIT),
_segments(DEFAULT_SEGMENTS) {
}

/**
 * Creates a PolyFactory for generating shapes of the given geometry.
 *
 * @param geom  The geometry to use when generating shapes
 */
PolyFactory::PolyFactory(Geometry geom) :
_segments(DEFAULT_SEGMENTS) {
    _geometry = geom;
}

#pragma mark -
#pragma mark Outline Shapes
/**
 * Returns a polygon that represents a line segment from origin to dest.
 *
 * This method will do nothing if the geometry is {@link Geometry#SOLID}.
 * However, it does support all other geometry types, including `POINTS`
 * and `IMPLICIT`.
 *
 * @param ox    The x-coordinate of the origin
 * @param oy    The y-coordinate of the origin
 * @param dx    The x-coordinate of the destination
 * @param dy    The y-coordinate of the destination
 *
 * @return a polygon that represents a line segment from origin to dest.
 */
Poly2 PolyFactory::makeLine(float ox, float oy, float dx, float dy) const {
    Poly2 result;
    makeLine(&result,ox,oy,dx,dy);
    return result;
}

/**
 * Stores a line segment from origin to dest in the provided buffer.
 *
 * The line will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data. This method will
 * do nothing if the current geometry is {@link Geometry#SOLID}. However,
 * it does support all other geometry types, including `POINTS` and
 * `IMPLICIT`.
 *
 * @param poly  The polygon to store the result
 * @param ox    The x-coordinate of the origin
 * @param oy    The y-coordinate of the origin
 * @param dx    The x-coordinate of the destination
 * @param dy    The y-coordinate of the destination
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeLine(Poly2* poly, float ox, float oy, float dx, float dy) const {
    CUAssertLog(_geometry != Geometry::SOLID, "Lines are not solid geometry");
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    Uint32 offset = (Uint32)poly->vertices().size();
    poly->_vertices.push_back(Vec2(ox,oy));
    poly->_vertices.push_back(Vec2(dx,dy));
    
    switch (_geometry) {
        case Geometry::POINTS:
        case Geometry::PATH:
            poly->_indices.push_back(offset);
            poly->_indices.push_back(offset+1);
            break;
        default:
            break;
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}
    
/**
 * Returns a polygon representing a wire frame of an existing polygon.
 *
 * This method provides four types of traversals: `NONE`, `OPEN`, `CLOSED`
 * and `INTERIOR`. No traversal simply copies the given polygon. The open
 * and closed traversals apply to the boundary of the polygon (as determined
 * by {@link Geometry#boundaries}. If there is more than one boundary, then
 * the closed traversal connects the boundaries together in a single loop.
 *
 * Finally, the interior traversal creates a wire frame a polygon
 * triangulation. This means that it can only be applied to a `SOLID`
 * polygon. An interior traversal is meaningless unless the polygon
 * has an existing triangulation.
 *
 * This method does nothing if the geometry is {@link Geometry#SOLID}.
 * However, it does support all other geometry types, including `POINTS`
 * and `IMPLICIT`.
 *
 * @param src   The source polygon to traverse
 * @param type  The traversal type
 *
 * @return a polygon representing a wire frame of an existing polygon.
 */
Poly2 PolyFactory::makeTraversal(const Poly2& src, poly2::Traversal type) const {
    Poly2 result;
    makeTraversal(&result, src, type);
    return result;
}

/**
 * Stores a wire frame of an existing polygon in the provided buffer.
 *
 * This method provides four types of traversals: `NONE`, `OPEN`, `CLOSED`
 * and `INTERIOR`. No traversal simply copies the given polygon. The open
 * and closed traversals apply to the boundary of the polygon (as determined
 * by {@link Geometry#boundaries}. If there is more than one boundary, then
 * the closed traversal connects the boundaries together in a single loop.
 *
 * Finally, the interior traversal creates a wire frame a polygon
 * triangulation. This means that it can only be applied to a `SOLID`
 * polygon. An interior traversal is meaningless unless the polygon
 * has an existing triangulation.
 *
 * This method does nothing if the geometry is {@link Geometry#SOLID}.
 * However, it does support all other geometry types, including `POINTS`
 * and `IMPLICIT`.
 *
 * The traversal will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param src   The source polygon to traverse
 * @param type  The traversal type
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeTraversal(Poly2* poly, const Poly2& src, poly2::Traversal type) const {
    CUAssertLog(_geometry != Geometry::SOLID, "Traversals are not solid geometry");
    CUAssertLog(poly->_geom == Geometry::IMPLICIT || poly->_geom == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    CUAssertLog(poly != nullptr, "Polygon buffer is null");

    switch (type) {
        case poly2::Traversal::NONE:
        {
            CUAssertLog(poly->_geom == Geometry::IMPLICIT || poly->_geom == src._geom,
                        "Buffer geometry is inconsistent with src geometry");
            Uint32 offset = (Uint32)poly->_vertices.size();
            poly->_vertices.reserve(poly->_vertices.size()+offset);
            poly->_vertices.insert(poly->_vertices.end(), src._vertices.begin(), src._vertices.end());
            poly->_indices.reserve(poly->_indices.size()+src._indices.size());
            for(Uint32 ii = 0; ii < src._indices.size(); ii++) {
                poly->_indices.push_back(src._indices[ii]+offset);
            }
            poly->_geom = src._geom;
            poly->computeBounds();
            break;
        }
        case poly2::Traversal::OPEN:
            makeOpenTraversal(poly, src);
            break;
        case poly2::Traversal::CLOSED:
            makeClosedTraversal(poly, src);
            break;
        case poly2::Traversal::INTERIOR:
            makeInteriorTraversal(poly, src);
            break;
    }

    return poly;
}

/**
 * Stores a wire frame of an existing polygon in the provided buffer.
 *
 * This method is dedicated to an `OPEN` traversal.  See the description
 * of {@link #makeTraversal} for more information.  This method simply
 * exists to make the code more readable.
 *
 * The traversal will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param src   The source polygon to traverse
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeOpenTraversal(Poly2* poly, const Poly2& src) const {
    Uint32 offset = (Uint32)poly->_vertices.size();
    if (src._geom == Geometry::IMPLICIT) {
        poly->_vertices.insert(poly->_vertices.end(), src._vertices.begin(), src._vertices.end());
        if (_geometry != Geometry::IMPLICIT) {
            Uint32 icount = (Uint32)(_geometry == Geometry::PATH ? 2*src._vertices.size()-2 : src._vertices.size());
            poly->_indices.reserve(poly->_indices.size()+icount);
            for(Uint32 ii = 0; ii < src._vertices.size()-1; ii++) {
                poly->_indices.push_back(ii+offset);
                if (_geometry == Geometry::PATH) {
                    poly->_indices.push_back(ii+offset+1);
                }
            }
            if (_geometry == Geometry::POINTS) {
                poly->_indices.push_back(offset+(Uint32)src._vertices.size()-1);
            }
        }
    } else {
        std::vector<std::vector<Uint32>> bounds = src._geom.boundaries(src._indices);
        Uint32 vcount = offset;
        Uint32 icount = (Uint32)poly->_indices.size();
        for(auto it = bounds.begin(); it != bounds.end(); ++it) {
            icount += (Uint32)(_geometry == Geometry::PATH ? it->size()*2-2 : it->size());
            poly->_vertices.reserve(vcount+it->size());
            for(Uint32 pos = 0; pos < it->size(); pos++) {
                poly->_vertices.push_back(src._vertices[it->at(pos)]);
            }

            if (_geometry != Geometry::IMPLICIT) {
                poly->_indices.reserve(icount);
                for(Uint32 pos = 0; pos < it->size()-1; pos++) {
                    poly->_indices.push_back(pos+vcount);
                    if (_geometry == Geometry::PATH) {
                        poly->_indices.push_back(pos+vcount+1);
                    }
                }
                if (_geometry == Geometry::POINTS) {
                    poly->_indices.push_back(vcount+(Uint32)it->size()-1);
                }
            }
            vcount += (Uint32)it->size();
        }
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}

/**
 * Stores a wire frame of an existing polygon in the provided buffer.
 *
 * This method is dedicated to an `CLOSED` traversal.  See the description
 * of {@link #makeTraversal} for more information.  This method simply
 * exists to make the code more readable.
 *
 * The traversal will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param src   The source polygon to traverse
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeClosedTraversal(Poly2* poly, const Poly2& src) const {
    Uint32 offset = (Uint32)poly->_vertices.size();
    if (src._geom == Geometry::IMPLICIT) {
        poly->_vertices.insert(poly->_vertices.end(), src._vertices.begin(), src._vertices.end());
        if (_geometry != Geometry::IMPLICIT) {
            Uint32 icount = (Uint32)(_geometry == Geometry::PATH ? 2*src._vertices.size() : src._vertices.size());
            poly->_indices.reserve(poly->_indices.size()+icount);
            for(Uint32 ii = 0; ii < src._vertices.size()-1; ii++) {
                poly->_indices.push_back(ii+offset);
                if (_geometry == Geometry::PATH) {
                    poly->_indices.push_back(ii+offset+1);
                }
            }
            poly->_indices.push_back(offset+(Uint32)src._vertices.size()-1);
            if (_geometry == Geometry::PATH) {
                poly->_indices.push_back(0);
            }
        }
    } else {
        std::vector<std::vector<Uint32>> bounds = src._geom.boundaries(src._indices);
        Uint32 vcount = offset;
        Uint32 icount = (Uint32)poly->_indices.size();
        for(auto it = bounds.begin(); it != bounds.end(); ++it) {
            icount += (Uint32)(_geometry == Geometry::PATH ? it->size()*2 : it->size());
            poly->_vertices.reserve(vcount+it->size());
            for(Uint32 pos = 0; pos < it->size(); pos++) {
                poly->_vertices.push_back(src._vertices[it->at(pos)]);
            }

            if (_geometry != Geometry::IMPLICIT) {
                poly->_indices.reserve(icount);
                if (vcount != offset) {
                    poly->_indices.push_back(vcount);
                }

                for(Uint32 pos = 0; pos < it->size()-1; pos++) {
                    poly->_indices.push_back(pos+vcount);
                    if (_geometry == Geometry::PATH) {
                        poly->_indices.push_back(pos+vcount+1);
                    }
                }
                poly->_indices.push_back(vcount+(Uint32)it->size()-1);
            }
            vcount += (Uint32)it->size();
        }
        if (_geometry == Geometry::PATH && vcount != offset) {
          poly->_indices.push_back(offset);
        }
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}

/**
 * Stores a wire frame of an existing polygon in the provided buffer.
 *
 * This method is dedicated to an `INTERIOR` traversal.  See the description
 * of {@link #makeTraversal} for more information.  This method simply
 * exists to make the code more readable.
 *
 * The traversal will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param src   The source polygon to traverse
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeInteriorTraversal(Poly2* poly, const Poly2& src) const {
    CUAssertLog(src._geom == Geometry::SOLID && !src._indices.empty(),
                "Cannot traverse an untriangluated polygon");

    Uint32 offset = (Uint32)poly->_vertices.size();
    poly->_vertices.reserve(src._vertices.size()+offset);
    poly->_indices.reserve(2*src._indices.size()+poly->_indices.size());
    poly->_vertices.insert(poly->_vertices.end(), src._vertices.begin(), src._vertices.end());
    if (_geometry == Geometry::POINTS) {
        for(int ii = 0; ii < src._indices.size(); ii+=3) {
            Uint32 a = src._indices[ii];
            Uint32 b = src._indices[ii+1];
            Uint32 c = src._indices[ii+2];
            poly->_indices.push_back(a);
            poly->_indices.push_back(b);
            poly->_indices.push_back(c);
        }
    } else if (_geometry == Geometry::PATH) {
        for(int ii = 0; ii < src._indices.size(); ii+=3) {
            Uint32 a = src._indices[ii];
            Uint32 b = src._indices[ii+1];
            Uint32 c = src._indices[ii+2];
            poly->_indices.push_back(a);
            poly->_indices.push_back(b);
            poly->_indices.push_back(b);
            poly->_indices.push_back(c);
            poly->_indices.push_back(c);
            poly->_indices.push_back(a);
        }
    }

    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}


#pragma mark -
#pragma mark Unrounded Shapes
/**
 * Returns a polygon that represents a simple triangle.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid triangle and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * @param  ax   The x-coordinate of the first vertex.
 * @param  ay   The y-coordinate of the first vertex.
 * @param  bx   The x-coordinate of the second vertex.
 * @param  by   The y-coordinate of the second vertex.
 * @param  cx   The x-coordinate of the third vertex.
 * @param  cy   The y-coordinate of the third vertex.
 *
 * @return a polygon that represents a simple triangle.
 */
Poly2 PolyFactory::makeTriangle(float ax, float ay, float bx, float by, float cx, float cy) const {
    Poly2 result;
    makeTriangle(&result,ax,ay,bx,by,cx,cy);
    return result;
}

/**
 * Stores a simple triangle in the provided buffer.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid triangle and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * The triangle will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param ax    The x-coordinate of the first vertex
 * @param ay    The y-coordinate of the first vertex
 * @param bx    The x-coordinate of the second vertex
 * @param by    The y-coordinate of the second vertex
 * @param cx    The x-coordinate of the third vertex
 * @param cy    The y-coordinate of the third vertex
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeTriangle(Poly2* poly, float ax, float ay, float bx, float by, float cx, float cy) const {
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    Uint32 offset = (Uint32)poly->_vertices.size();
    poly->_vertices.reserve( offset+3 );
    poly->_vertices.push_back(Vec2(ax,ay));
    poly->_vertices.push_back(Vec2(bx,by));
    poly->_vertices.push_back(Vec2(cx,cy));

    switch (_geometry) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            poly->_indices.reserve(poly->_indices.size()+3);
            for(Uint32 ii = 0; ii < 3; ii++) {
                poly->_indices.push_back(ii+offset);
            }
            break;
        case Geometry::PATH:
            poly->_indices.reserve(poly->_indices.size()+6);
            for(Uint32 ii = 0; ii < 2; ii++) {
                poly->_indices.push_back(ii+offset);
                poly->_indices.push_back(ii+offset+1);
            }
            poly->_indices.push_back(offset+2);
            poly->_indices.push_back(offset);
            break;
        case Geometry::SOLID:
            poly->_indices.reserve(poly->_indices.size()+3);
            if (Poly2::orientation(Vec2(ax,ay),Vec2(bx,by),Vec2(cx,cy)) >= 0) {
                poly->_indices.push_back(offset+2);
                poly->_indices.push_back(offset+1);
                poly->_indices.push_back(offset);
            } else {
                poly->_indices.push_back(offset);
                poly->_indices.push_back(offset+1);
                poly->_indices.push_back(offset+2);
            }
            break;
    }


    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}


/**
 * Returns a polygon that represents a rectangle
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid rectangle and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * @param x     The x-coordinate of the bottom left corner
 * @param y     The y-coordinate of the bottom left corner
 * @param w     The rectangle width
 * @param h     The rectangle height
 *
 * @return a polygon that represents a rectangle
 */
Poly2 PolyFactory::makeRect(float x, float y, float w, float h) const {
    Poly2 result;
    makeRect(&result,x,y,w,h);
    return result;
}

/**
 * Stores a rectangle in the provided buffer.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid rectangle and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * The rectangle will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param x     The x-coordinate of the bottom left corner
 * @param y     The y-coordinate of the bottom left corner
 * @param w     The rectangle width
 * @param h     The rectangle height
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeRect(Poly2* poly, float x, float y, float w, float h) const {
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    Uint32 offset = (Uint32)poly->_vertices.size();
    poly->_vertices.reserve( offset+4 );
    poly->_vertices.push_back(Vec2(x  ,y  ));
    poly->_vertices.push_back(Vec2(x+w,y  ));
    poly->_vertices.push_back(Vec2(x+w,y+h));
    poly->_vertices.push_back(Vec2(x  ,y+h));

    switch (_geometry) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            poly->_indices.reserve(poly->_indices.size()+4);
            for(Uint32 ii = 0; ii < 4; ii++) {
                poly->_indices.push_back(ii+offset);
            }
            break;
        case Geometry::PATH:
            poly->_indices.reserve(poly->_indices.size()+8);
            for(Uint32 ii = 0; ii < 3; ii++) {
                poly->_indices.push_back(ii+offset);
                poly->_indices.push_back(ii+offset+1);
            }
            poly->_indices.push_back(offset+3);
            poly->_indices.push_back(offset);
            break;
        case Geometry::SOLID:
            poly->_indices.reserve(poly->_indices.size()+6);
            poly->_indices.push_back(offset);
            poly->_indices.push_back(offset+1);
            poly->_indices.push_back(offset+2);
            poly->_indices.push_back(offset+2);
            poly->_indices.push_back(offset+3);
            poly->_indices.push_back(offset);
            break;
    }

    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}
    
#pragma mark -
#pragma mark Rounded Shapes
/**
 * Returns a polygon that represents an ellipse of the given dimensions.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid ellipse and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * @param cx    The x-coordinate of the center point
 * @param cy    The y-coordinate of the center point
 * @param sx    The size (diameter) along the x-axis
 * @param sy    The size (diameter) along the y-axis
 *
 * @return a polygon that represents an ellipse of the given dimensions.
 */
Poly2 PolyFactory::makeEllipse(float cx, float cy, float sx, float sy) const {
    Poly2 result;
    makeEllipse(&result,cx,cy,sx,sy);
    return result;
}

/**
 * Stores an ellipse in the provided buffer.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid ellipse and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * The ellipse will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param cx    The x-coordinate of the center point
 * @param cy    The y-coordinate of the center point
 * @param sx    The size (diameter) along the x-axis
 * @param sy    The size (diameter) along the y-axis
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeEllipse(Poly2* poly, float cx, float cy, float sx, float sy) const {
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    const float coef = 2.0f * (float)M_PI/_segments;
    Uint32 offset = (Uint32)poly->_vertices.size();

    Vec2 vert;
    poly->_vertices.reserve(offset+_segments);
    for(unsigned int ii = 0; ii < _segments; ii++) {
        float rads = ii*coef;
        vert.x = 0.5f * sx * cosf(rads) + cx;
        vert.y = 0.5f * sy * sinf(rads) + cy;
        poly->_vertices.push_back(vert);
    }
    
    switch (_geometry) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            poly->_indices.reserve(poly->_indices.size()+_segments);
            for(Uint32 ii = 0; ii < _segments; ii++) {
                poly->_indices.push_back(ii+offset);
            }
            break;
        case Geometry::PATH:
            poly->_indices.reserve(poly->_indices.size()+2*_segments);
            for(Uint32 ii = 0; ii < _segments-1; ii++) {
                poly->_indices.push_back(offset+ii  );
                poly->_indices.push_back(offset+ii+1);
            }
            poly->_indices.push_back(offset+_segments-1);
            poly->_indices.push_back(offset);
            break;
        case Geometry::SOLID:
            poly->_vertices.push_back(Vec2(cx,cy));
            poly->_indices.reserve(poly->_indices.size()+3*_segments);
            for(Uint32 ii = 0; ii < _segments-1; ii++) {
                poly->_indices.push_back(ii+offset);
                poly->_indices.push_back(ii+offset+1);
                poly->_indices.push_back(_segments+offset);
            }
            poly->_indices.push_back(_segments+offset-1);
            poly->_indices.push_back(offset);
            poly->_indices.push_back(_segments+offset);
            break;
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}

    
/**
 * Returns a polygon that represents a circle of the given dimensions.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid ellipse and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary. This method can be used to generate regular,
 * many-sided polygons by varying the value of {@link #setSegments}.
 *
 * @param cx        The x-coordinate of the center point
 * @param cy        The y-coordinate of the center point
 * @param radius    The circle radius
 *
 * @return a polygon that represents an circle of the given dimensions.
 */
Poly2 PolyFactory::makeCircle(float cx, float cy, float radius) const {
    Poly2 result;
    makeCircle(&result,cx,cy,radius);
    return result;
}


/**
 * Stores a circle in the provided buffer.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid ellipse and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary. This method can be used to generate regular,
 * many-sided polygons by varying the value of {@link #setSegments}.
 *
 * The circle will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly      The polygon to store the result
 * @param cx        The x-coordinate of the center point
 * @param cy        The y-coordinate of the center point
 * @param radius    The circle radius
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeCircle(Poly2* poly, float cx, float cy, float radius) const {
    return makeEllipse(poly, cx, cy, 2*radius, 2*radius);
}

/**
 * Returns a polygon that represents an arc of the given dimensions.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary. All arc measurements are in degrees,
 * not radians.
 *
 * @param cx        The x-coordinate of the center point
 * @param cy        The y-coordinate of the center point
 * @param radius    The radius from the center point
 * @param start     The starting angle in degrees
 * @param degrees   The number of degrees to generate
 *
 * @return a polygon that represents an arc of the given dimensions.
 */
Poly2 PolyFactory::makeArc(float cx, float cy, float radius, float start, float degrees) const {
    Poly2 result;
    makeArc(&result,cx,cy,radius,start,degrees);
    return result;
}

/**
 * Stores an arc in the provided buffer.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary. All arc measurements are in degrees,
 * not radians.
 *
 * The arc will be appended to the buffer.  You should clear the buffer first
 * if you do not want to preserve the original data.
 *
 * @param poly      The polygon to store the result
 * @param cx        The x-coordinate of the center point
 * @param cy        The y-coordinate of the center point
 * @param radius    The radius from the center point
 * @param start     The starting angle in degrees
 * @param degrees   The number of degrees to generate
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeArc(Poly2* poly, float cx, float cy, float radius, float start, float degrees) const {
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    Uint32 offset = (Uint32)poly->_vertices.size();
    Uint32 segs = (degrees < _segments ? (int)degrees : _segments);
    float srad = ((float)M_PI/180.0f)*start;
    float arad = ((float)M_PI/180.0f)*degrees;
    float coef = arad/segs;

    poly->_vertices.reserve(offset+segs+1);
    Vec2 vert;
    for(int ii = 0; ii < segs+1; ii++) {
        float rads = srad+ii*coef;
        vert.x = 0.5f * radius  * cosf(rads) + cx;
        vert.y = 0.5f * radius  * sinf(rads) + cy;
        poly->_vertices.push_back(vert);
    }
    
    switch (_geometry) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            poly->_indices.reserve(poly->_indices.size()+segs+1);
            for(Uint32 ii = 0; ii < segs+1; ii++) {
                poly->_indices.push_back(ii+offset);
            }
            break;
        case Geometry::PATH:
            poly->_indices.reserve(poly->_indices.size()+2*segs);
            for(Uint32 ii = 0; ii < segs; ii++) {
                poly->_indices.push_back(offset+ii  );
                poly->_indices.push_back(offset+ii+1);
            }
            break;
        case Geometry::SOLID:
            poly->_vertices.push_back(Vec2(cx,cy));
            poly->_indices.reserve(poly->_indices.size()+3*segs);
            for(Uint32 ii = 0; ii < segs+1; ii++) {
                poly->_indices.push_back(ii+offset);
                poly->_indices.push_back(ii+offset+1);
                poly->_indices.push_back(segs+offset+1);
            }
            break;
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}


/**
 * Returns a polygon that represents a rounded rectangle of the given dimensions.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary. The radius should not exceed either half
 * the width or half the height.
 *
 * @param x     The x-coordinate of the bottom left corner of the bounding box
 * @param y     The y-coordinate of the bottom left corner of the bounding box
 * @param w     The rectangle width
 * @param h     The rectangle height
 * @param r     The radius of each corner
 *
 * @return a polygon that represents a rounded rectangle of the given dimensions.
 */
Poly2 PolyFactory::makeRoundedRect(float x, float y, float w, float h, float r) const {
    Poly2 result;
    makeRoundedRect(&result,x,y,w,h,r);
    return result;
}

/**
 * Stores a rounded rectangle in the provided buffer.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary. The radius should not exceed either half
 * the width or half the height.
 *
 * The rounded rectangle will be appended to the buffer.  You should clear the
 * buffer first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param x     The x-coordinate of the bottom left corner of the bounding box
 * @param y     The y-coordinate of the bottom left corner of the bounding box
 * @param w     The rectangle width
 * @param h     The rectangle height
 * @param r     The radius of each corner
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeRoundedRect(Poly2* poly, float x, float y, float w, float h, float r) const {
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    CUAssertLog(r <= w/2.0f, "Radius %.3f exceeds width %.3f", r, w);
    CUAssertLog(r <= h/2.0f, "Radius %.3f exceeds height %.3f", r, h);
    Uint32 offset = (Uint32)poly->_vertices.size();
    const float coef = M_PI/(2.0f*_segments);

    float c1x = w >= 0 ? w : 0;
    float c1y = h >= 0 ? h : 0;
    float c2x = w >= 0 ? 0 : w;
    float c2y = h >= 0 ? h : 0;
    float c3x = w >= 0 ? 0 : w;
    float c3y = h >= 0 ? 0 : h;
    float c4x = w >= 0 ? w : 0;
    float c4y = h >= 0 ? 0 : h;
    
    poly->_vertices.reserve(offset+4*_segments+4);
    
    // TOP RIGHT
    float cx = x + c1x - r;
    float cy = y + c1y - r;
    Vec2 vert;
    for(int ii = 0; ii <= _segments; ii++) {
        vert.x = r * cosf(ii*coef) + cx;
        vert.y = r * sinf(ii*coef) + cy;
        poly->_vertices.push_back(vert);
    }
    
    // TOP LEFT
    cx = x + c2x + r;
    cy = y + c2y - r;
    for(int ii = 0; ii <= _segments; ii++) {
        vert.x = cx - r * sinf(ii*coef);
        vert.y = r * cosf(ii*coef) + cy;
        poly->_vertices.push_back(vert);
    }
    
    cx = x + c3x + r;
    cy = y + c3y + r;
    for(int ii = 0; ii <= _segments; ii++) {
        vert.x = cx - r * cosf(ii*coef);
        vert.y = cy - r * sinf(ii*coef);
        poly->_vertices.push_back(vert);
    }

    cx = x + c4x - r;
    cy = y + c4y + r;
    for(int ii = 0; ii <= _segments; ii++) {
        vert.x = r * sinf(ii*coef) + cx;
        vert.y = cy - r * cosf(ii*coef);
        poly->_vertices.push_back(vert);
    }
    
    cx = x + w/2.0f;
    cy = y + h/2.0f;

   switch (_geometry) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            poly->_indices.reserve(poly->_indices.size()+4*_segments+4);
            for(Uint32 ii = 0; ii < 4*_segments+4; ii++) {
                poly->_indices.push_back(ii+offset);
            }
            break;
        case Geometry::PATH:
            poly->_indices.reserve(poly->_indices.size()+8*_segments+8);
            for(Uint32 ii = 0; ii < 4*_segments+3; ii++) {
                poly->_indices.push_back(ii+offset);
                poly->_indices.push_back(ii+offset+1);
            }
            poly->_indices.push_back(4*_segments+3+offset);
            poly->_indices.push_back(offset);
            break;
        case Geometry::SOLID:
        {
            poly->_vertices.push_back(Vec2(cx,cy));
            Uint32 capacity = 4*_segments+4;
            poly->_indices.reserve( poly->_indices.size()+3*capacity );
            for(int ii = 0; ii < capacity-1; ii++) {
                poly->_indices.push_back(offset+ii);
                poly->_indices.push_back(offset+ii+1);
                poly->_indices.push_back(offset+capacity);
            }
            poly->_indices.push_back(offset+capacity-1);
            poly->_indices.push_back(offset);
            poly->_indices.push_back(offset+capacity);
            break;
        }
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}

#pragma mark -
#pragma mark Capsules
/**
 * Returns a polygon that represents a capsule of the given dimensions.
 *
 * A capsule is a pill-like shape that fits inside of given rectangle.  If
 * width < height, the capsule will be oriented vertically with the rounded
 * portions at the top and bottom. Otherwise it will be oriented horizontally.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * @param x     The x-coordinate of the bottom left corner of the bounding box
 * @param y     The y-coordinate of the bottom left corner of the bounding box
 * @param w     The capsule width
 * @param h     The capsule height
 *
 * @return a polygon that represents a capsule of the given dimensions.
 */
Poly2 PolyFactory::makeCapsule(float x, float y, float w, float h) const {
    Poly2 result;
    makeCapsule(&result,poly2::Capsule::FULL,x,y,w,h);
    return result;
}

/**
 * Stores a capsule in the provided buffer.
 *
 * A capsule is a pill-like shape that fits inside of given rectangle.  If
 * width < height, the capsule will be oriented vertically with the rounded
 * portions at the top and bottom.Otherwise it will be oriented horizontally.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * The capsule will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly  The polygon to store the result
 * @param x     The x-coordinate of the bottom left corner of the bounding box
 * @param y     The y-coordinate of the bottom left corner of the bounding box
 * @param w     The capsule width
 * @param h     The capsule height
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeCapsule(Poly2* poly, float x, float y, float w, float h) const {
    return makeCapsule(poly, poly2::Capsule::FULL, x, y, w, h);
}

/**
 * Returns a polygon that represents a (full) capsule of the given dimensions.
 *
 * A capsule typically is a pill-like shape that fits inside of given rectangle.
 * If width < height, the capsule will be oriented vertically with the rounded
 * portions at the top and bottom. Otherwise it will be oriented horizontally.
 *
 * This method allows for the creation of half-capsules, simply by using the
 * enumeration {@link poly2::Capsule}. The enumeration specifies which side
 * should be rounded in case of a half-capsule. Half-capsules are sized so that
 * the corresponding full capsule would fit in the bounding box.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * @param shape     The capsule shape
 * @param x         The x-coordinate of the bottom left corner of the bounding box
 * @param y         The y-coordinate of the bottom left corner of the bounding box
 * @param w         The capsule width
 * @param h         The capsule height
 *
 * @return a polygon that represents a capsule of the given dimensions.
 */
Poly2 PolyFactory::makeCapsule(poly2::Capsule shape, float x, float y, float w, float h) const {
    Poly2 result;
    makeCapsule(&result,shape,x,y,w,h);
    return result;
}

/**
 * Stores a capsule in the provided buffer.
 *
 * A capsule typically is a pill-like shape that fits inside of given rectangle.
 * If width < height, the capsule will be oriented vertically with the rounded
 * portions at the top and bottom. Otherwise it will be oriented horizontally.
 *
 * This method allows for the creation of half-capsules, simply by using the
 * enumeration {@link poly2::Capsule}. The enumeration specifies which side
 * should be rounded in case of a half-capsule. Half-capsules are sized so that
 * the corresponding full capsule would fit in the bounding box.
 *
 * The polygon will be generated by the current geometry. Use {@link Geometry#Solid}
 * for a solid arc and {@link Geometry#Closed} for an outline. Other geometries
 * are supported as well, if necessary.
 *
 * The capsule will be appended to the buffer.  You should clear the buffer
 * first if you do not want to preserve the original data.
 *
 * @param poly      The polygon to store the result
 * @param shape     The capsule shape
 * @param x         The x-coordinate of the bottom left corner of the bounding box
 * @param y         The y-coordinate of the bottom left corner of the bounding box
 * @param w         The capsule width
 * @param h         The capsule height
 *
 * @return a reference to the buffer for chaining.
 */
Poly2* PolyFactory::makeCapsule(Poly2* poly, poly2::Capsule shape, float x, float y, float w, float h) const {
    CUAssertLog(poly->getGeometry() == Geometry::IMPLICIT || poly->getGeometry() == _geometry,
                "Buffer geometry is inconsistent with this polygon factory");
    CUAssertLog(poly != nullptr, "Polygon buffer is null");
    if (shape == poly2::Capsule::DEGENERATE) {
        return makeEllipse(poly, x+w/2, y+h/2, w, h);
    } else if (w == h) {
        return makeCircle(poly, x+w/2, y+h/2, w);
    }
    
    Uint32 offset = (Uint32)poly->_vertices.size();
    const float coef = M_PI/_segments;
        
    const float cx = x + w/2.0f;
    const float cy = y + h/2.0f;
    Uint32 vcount = 0;
    if (w <= h) {
        float radius = w / 2.0f;
        float iy = y+radius;
        float ih = h-w;
        Vec2 vert;
        
        // Start at bottom left of interior rectangle
        if (shape == poly2::Capsule::HALF_REVERSE) {
            vert.x = cx - radius;
            vert.y = iy;
            poly->_vertices.push_back(vert);
            vert.x = cx + radius;
            poly->_vertices.push_back(vert);
            vcount += 2;
        } else {
            poly->_vertices.reserve( offset+_segments+1 );
            for (Uint32 ii = 0; ii <= _segments; ii++) {
                // Try to handle round off gracefully
                float rads = (ii == _segments ? M_PI : ii * coef);
                vert.x = cx - radius * cosf( rads );
                vert.y = iy - radius * sinf( rads );
                poly->_vertices.push_back(vert);
            }
            vcount += _segments+1;
        }
        
        // Now around the top
        if (shape == poly2::Capsule::HALF) {
            vert.x = cx + radius;
            vert.y = iy + ih;
            poly->_vertices.push_back(vert);
            vert.x = cx - radius;
            poly->_vertices.push_back(vert);
            vcount += 2;
        } else {
            poly->_vertices.reserve( offset+vcount+_segments+1 );
            for (Uint32 ii = 0; ii <= _segments; ii++) {
                // Try to handle round off gracefully
                float rads = (ii == _segments ? M_PI : ii * coef);
                vert.x = cx + radius * cosf( rads );
                vert.y = iy + ih + radius * sinf( rads );
                poly->_vertices.push_back(vert);
            }
            vcount += _segments+1;
        }
    } else {
        float radius = h / 2.0f;
        float ix = x+radius;
        float iw = w-h;
        Vec2 vert;
        
        // Start at the top left of the interior rectangle
        if (shape == poly2::Capsule::HALF_REVERSE) {
            vert.x = ix;
            vert.y = cy + radius;
            poly->_vertices.push_back(vert);
            vert.x = cx - radius;
            poly->_vertices.push_back(vert);
            vcount += 2;
        } else {
            poly->_vertices.reserve( offset+_segments+1 );
            for (int ii = 0; ii <= _segments; ii++) {
                // Try to handle round off gracefully
                float rads = (ii == _segments ? M_PI : ii * coef);
                vert.x = ix - radius * cosf( rads );
                vert.y = cy + radius * sinf( rads );
                poly->_vertices.push_back(vert);
            }
            vcount += _segments+1;
        }
        
        // Now around the right side
        if (shape == poly2::Capsule::HALF_REVERSE) {
            vert.x = ix + iw;
            vert.y = cy - radius;
            poly->_vertices.push_back(vert);
            vert.x = cx + radius;
            poly->_vertices.push_back(vert);
            vcount += 2;
        } else {
            poly->_vertices.reserve( offset+vcount+_segments+1 );
            for (int ii = 0; ii <= _segments; ii++) {
                // Try to handle round off gracefully
                float rads = (ii == _segments ? M_PI : ii * coef);
                vert.x = ix + iw + radius * cosf( rads );
                vert.y = cy - radius * sinf( rads );
                poly->_vertices.push_back(vert);
            }
            vcount += _segments+1;
        }
    }
    
    switch (_geometry) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            poly->_indices.reserve(poly->_indices.size()+vcount);
            for(Uint32 ii = 0; ii < vcount; ii++) {
                poly->_indices.push_back(ii+offset);
            }
            break;
        case Geometry::PATH:
            poly->_indices.reserve(poly->_indices.size()+2*vcount);
            for(Uint32 ii = 0; ii < vcount-1; ii++) {
                poly->_indices.push_back(ii+offset);
                poly->_indices.push_back(ii+offset+1);
            }
            poly->_indices.push_back(vcount+offset-1);
            poly->_indices.push_back(offset);
            break;
        case Geometry::SOLID:
        {
            poly->_vertices.push_back(Vec2(cx,cy));
            poly->_indices.reserve( poly->_indices.size()+3*vcount );
            for(int ii = 0; ii < vcount-1; ii++) {
                poly->_indices.push_back(ii);
                poly->_indices.push_back(ii+1);
                poly->_indices.push_back(vcount);
            }
            poly->_indices.push_back(vcount-1);
            poly->_indices.push_back(0);
            poly->_indices.push_back(vcount);
            break;
        }
    }
    
    poly->_geom = _geometry;
    poly->computeBounds();
    return poly;
}
