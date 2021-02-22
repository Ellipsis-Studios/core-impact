//
//  CUComplexExtruder.cpp
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
#include <cugl/math/polygon/CUComplexExtruder.h>
#include <cugl/math/polygon/CUComplexTriangulator.h>
#include <cugl/util/CUDebug.h>
#include <cugl/util/CUTimestamp.h>
#include <iterator>

using namespace cugl;

/** The default Clipper resolution */
#define RESOLUTION 8

#pragma mark -
#pragma mark Initialization
/**
 * Creates an extruder with no vertex data.
 */
ComplexExtruder::ComplexExtruder() :
_calculated(false),
_resolution(RESOLUTION) {
}

/**
 * Creates an extruder with the given vertex data.
 *
 * The vertex data is copied.  The extruder does not retain any references
 * to the original data.
 *
 * @param points    The vertices to extrude
 * @param closed    Whether the path is closed
 */
ComplexExtruder::ComplexExtruder(const std::vector<Vec2>& points, bool closed) :
_calculated(false),
_resolution(RESOLUTION) {
    set(points,closed);
}
  
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
ComplexExtruder::ComplexExtruder(const Poly2& poly) :
_calculated(false),
_resolution(RESOLUTION) {
    set(poly);
}

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
void ComplexExtruder::set(const Poly2& poly) {
    reset();
    switch (poly.getGeometry()) {
        case Geometry::IMPLICIT:
            _input.push_back(poly._vertices);
            _closed.push_back(true);
            break;
        case Geometry::PATH:
        {
            size_t first = 0;
            while (first < poly._indices.size()) {
                size_t last = first;
                _input.push_back(std::vector<Vec2>());
                std::vector<Vec2>* input = &_input.back();
                bool smooth = true;
                for(size_t ii = first; smooth && ii < poly._indices.size(); ii+=2) {
                    smooth = (ii == first) || poly._indices[ii] == poly._indices[ii-1];
                    if (smooth) {
                        input->push_back(poly._vertices[poly._indices[ii]]);
                    }
                    last = ii;
                }
                last = smooth ? last+2 : last;
                input->push_back(poly._vertices[poly._indices[last-1]]);
                _closed.push_back(poly._indices[last-1] == poly._indices[first]);
                first = last;
            }
        }
            break;
        default:
            CUAssertLog(false,"Polygon geometry does not support extrusion");
            break;
    }
}

#pragma mark -
#pragma mark Clipper Attributes
/**
 * Sets the joint value for the extrusion.
 *
 * The joint type determines how the extrusion joins the extruded
 * line segments together.  See {@link poly2::Joint} for the
 * description of the types.
 *
 * @param joint     The extrusion joint type
 */
void ComplexExtruder::setJoint(poly2::Joint joint) {
    switch (joint) {
        case poly2::Joint::SQUARE:
            _joint = ClipperLib::jtSquare;
        case poly2::Joint::MITRE:
            _joint = ClipperLib::jtMiter;
        case poly2::Joint::ROUND:
            _joint = ClipperLib::jtRound;
        default:
            _joint = ClipperLib::jtSquare;
    }
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
poly2::Joint ComplexExtruder::getJoint() const {
    switch (_joint) {
        case ClipperLib::jtSquare:
            return poly2::Joint::SQUARE;
        case ClipperLib::jtMiter:
            return poly2::Joint::MITRE;
        case ClipperLib::jtRound:
            return poly2::Joint::ROUND;
    }
    return poly2::Joint::SQUARE;
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
void ComplexExtruder::setEndCap(poly2::EndCap endcap) {
    switch (endcap) {
        case poly2::EndCap::NONE:
            _endcap = ClipperLib::etOpenButt;
        case poly2::EndCap::SQUARE:
            _endcap = ClipperLib::etOpenSquare;
        case poly2::EndCap::ROUND:
            _endcap = ClipperLib::etOpenRound;
    }
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
poly2::EndCap ComplexExtruder::getEndCap() const {
    switch (_endcap) {
        case ClipperLib::etOpenButt:
            return poly2::EndCap::NONE;
        case ClipperLib::etOpenSquare:
            return poly2::EndCap::SQUARE;
        case ClipperLib::etOpenRound:
            return poly2::EndCap::ROUND;
        default:
            return poly2::EndCap::NONE;
    }
    return poly2::EndCap::NONE;
}

#pragma mark -
#pragma mark Calculation
/**
 * Clears all computed data, but still maintains the settings.
 *
 * This method preserves all initial vertex data, as well as the joint,
 * cap, and precision settings.
 */
void ComplexExtruder::reset() {
    _output.clear();
    _calculated = false;
}

/**
 * Clears all internal data, including initial vertex data.
 *
 * When this method is called, you will need to set a new vertices before
 * calling {@link #calculate}.  However, the joint, cap, and precision
 * settings are preserved.
 */
void ComplexExtruder::clear() {
    reset();
    _input.clear();
    _closed.clear();
}

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
void ComplexExtruder::calculate(float stroke) {
    if (_input.size() == 0) {
        _calculated = true;
        return;
    }
    
    _output.clear();
    for(size_t seg = 0; seg < _input.size(); seg++) {
        std::vector<Vec2>* input = &_input[seg];
        bool closed = _closed[seg];

        ClipperLib::Path inverts;
        ClipperLib::PolyTree solution;
        for(auto it = input->begin(); it != input->end(); ++it) {
            inverts << ClipperLib::IntPoint((ClipperLib::cInt)(it->x*_resolution),
                                            (ClipperLib::cInt)(it->y*_resolution));
        }
        
        ClipperLib::ClipperOffset worker;
        worker.AddPath( inverts, _joint, closed ?  ClipperLib::etClosedLine : _endcap );
        worker.Execute(solution, stroke*_resolution);
        for(auto it = solution.Childs.begin(); it != solution.Childs.end(); it++) {
            processNode(*it);
        }
    }

    _output._geom = Geometry::SOLID;
    _output.computeBounds();
    _calculated = true;
}

/**
 * Processes a single node of a Clipper PolyTree
 *
 * This method is used to extract the data from the Clipper solution
 * and convert it to a cugl Poly2 object. This is a recursive method
 * and assumes that the PolyNode is a outer polygon and not a hole.
 *
 * @param node  The PolyNode to accumulate
 */
void ComplexExtruder::processNode(const ClipperLib::PolyNode* node) {
    std::vector<Vec2> outer;
    std::vector<Vec2> hole;
    for(auto it = node->Contour.begin(); it != node->Contour.end(); ++it) {
        outer.push_back(Vec2((float)(it->X/(double)_resolution),(float)(it->Y/(double)_resolution)));
    }

    ComplexTriangulator triang(outer);
    for(auto it = node->Childs.begin(); it != node->Childs.end(); ++it) {
        hole.clear();
        for(auto jt = (*it)->Contour.begin(); jt != (*it)->Contour.end(); ++jt) {
            hole.push_back(Vec2((float)(jt->X/(double)_resolution),(float)(jt->Y/(double)_resolution)));
        }
        triang.addHole(hole);
    }

    triang.calculate();
    triang.getPolygon(&_output);

    for(auto it = node->Childs.begin(); it != node->Childs.end(); ++it) {
        for(auto jt = (*it)->Childs.begin(); jt != (*it)->Childs.end(); ++jt) {
            ClipperLib::PolyNode* next = (*jt);
            processNode(next);
        }
    }

}


#pragma mark -
#pragma mark Materialization
/**
 * Returns a polygon representing the path extrusion.
 *
 * The polygon contains the original vertices together with the new
 * indices defining the wireframe path.  The extruder does not maintain
 * references to this polygon and it is safe to modify it.
 *
 * If the calculation is not yet performed, this method will return the
 * empty polygon.
 *
 * @return a polygon representing the path extrusion.
 */
Poly2 ComplexExtruder::getPolygon() {
    return _output;
}

/**
 * Stores the path extrusion in the given buffer.
 *
 * This method will add both the original vertices, and the corresponding
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
Poly2* ComplexExtruder::getPolygon(Poly2* buffer) {
    CUAssertLog(buffer, "Destination buffer is null");
    CUAssertLog(buffer->_geom == Geometry::SOLID || buffer->_geom == Geometry::IMPLICIT,
                "Buffer geometry is incompatible with this result.");
    if (_calculated) {
        if (buffer->_vertices.size() == 0) {
            buffer->_vertices = _output.vertices();
            buffer->_indices  = _output.indices();
        } else {
            int offset = (int)buffer->_vertices.size();
            buffer->_vertices.reserve(offset+_output.vertices().size());
            std::copy(_output.vertices().begin(),_output.vertices().end(),std::back_inserter(buffer->_vertices));
            
            buffer->_indices.reserve(buffer->_indices.size()+_output.indices().size());
            for(auto it = _output.indices().begin(); it != _output.indices().end(); ++it) {
                buffer->_indices.push_back(offset+*it);
            }
        }
        buffer->_geom = Geometry::SOLID;
        buffer->computeBounds();
    }
    return buffer;
}

