//
//  CUPathNode.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides a scene graph node that supports extruded paths. When
//  extruding paths, this node is better than PolygonNode, because it will align
//  the extruded path to the original wireframe.
//
//  This class is loosely coupled with PathExtruder.  You can use PathExtruder
//  independent of the PathNode, but all functionality is present in this class.
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
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
//  Version: 8/20/20
#include <cugl/assets/CUAssetManager.h>
#include <cugl/assets/CUScene2Loader.h>
#include <cugl/scene2/graph/CUPathNode.h>
#include <cugl/util/CUDebug.h>
#include <cugl/render/CUGradient.h>

using namespace cugl::scene2;

/** For handling JSON issues */
#define UNKNOWN_STR "<unknown>"

#pragma mark Constructors
/**
 * Creates an empty path node.
 *
 * You must initialize this PathNode before use.
 *
 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
 * the heap, use one of the static constructors instead.
 */
PathNode::PathNode() : TexturedNode(),
_stroke(1.0f),
_closed(true),
_joint(poly2::Joint::NONE),
_endcap(poly2::EndCap::NONE) {
    _classname = "PathNode";
}

/**
 * Intializes a path with the given vertices and stroke width.
 *
 * You do not need to set the texture; rendering this into a SpriteBatch
 * will simply use the blank texture. Hence the wireframe will have a solid
 * color.
 *
 * The polygon will be extruded using the given sequence of vertices.
 * First it will traverse the vertices using either a closed or open
 * traveral.  Then it will extrude that polygon with the given joint
 * and cap. PathNode objects share a single extruder, so this initializer
 * is not thread safe.
 *
 * @param vertices  The vertices to texture (expressed in image space)
 * @param stroke    The stroke width of the extruded path.
 * @param joint     The joint between extrusion line segments.
 * @param cap       The end caps of the extruded paths.
 * @param closed    The whether the vertex path is open or closed.
 *
 * @return  true if the path node is initialized properly, false otherwise.
 */
bool PathNode::initWithVertices(const std::vector<Vec2>& vertices, float stroke,
                                poly2::Joint joint, poly2::EndCap cap, bool closed) {
    _joint  = joint;
    _endcap = cap;
    _closed = true;
    _stroke = stroke;
    return init(vertices);
}

/**
 * Intializes a path node with the given polygon and stroke width.
 *
 * You do not need to set the texture; rendering this into a SpriteBatch
 * will simply use the blank texture. Hence the wireframe will have a solid
 * color.
 *
 * The polygon will be extruded using the given polygon, assuming that it
 * is a (connected) path. It will extrude that polygon with the given joint
 * and cap.  It will assume the polygon is closed if the number of indices
 * is twice the number of vertices. PathNode objects share a single extruder,
 * so this initializer is not thread safe.
 *
 * @param poly      The polygon to texture (expressed in image space)
 * @param stroke    The stroke width of the extruded path.
 * @param joint     The joint between extrusion line segments.
 * @param cap       The end caps of the extruded paths.
 *
 * @return  true if the path node is initialized properly, false otherwise.
 */
bool PathNode::initWithPoly(const Poly2& poly, float stroke,
                            poly2::Joint joint, poly2::EndCap cap) {
    _joint  = joint;
    _endcap = cap;
    _closed = canonicallyClosed(poly);
    _stroke = stroke;
    return init(poly);
}

/**
 * Initializes a node with the given JSON specificaton.
 *
 * This initializer is designed to receive the "data" object from the
 * JSON passed to {@link SceneLoader}.  This JSON format supports all
 * of the attribute values of its parent class.  In addition, it supports
 * the following additional attributes:
 *
 *      "texture":  The name of a previously loaded texture asset
 *      "polygon":  An even array of polygon vertices (numbers)
 *      "indices":  An array of unsigned ints defining triangles from the
 *                  the vertices. The array size should be a multiple of 3.
 *      'stroke':   A number specifying the stroke width.
 *      'joint':    One of 'mitre', 'bevel', or 'round'.
 *      'cap':      One of 'square' or 'round'.
 *      'closed':   A boolean specifying if the path is closed.
 *
 * All attributes are optional.  However, it is generally a good idea to
 * specify EITHER the texture or the polygon.
 *
 * @param loader    The scene loader passing this JSON file
 * @param data      The JSON object specifying the node
 *
 * @return true if initialization was successful.
 */
bool PathNode::initWithData(const Scene2Loader* loader,
                            const std::shared_ptr<JsonValue>& data) {

    if (_texture != nullptr) {
        CUAssertLog(false, "%s is already initialized",_classname.c_str());
        return false;
    } else if (!data) {
        return init();
    } else if (!SceneNode::initWithData(loader, data)) {
        return false;
    }
    
    // All of the code that follows can corrupt the position.
    Vec2 coord = getPosition();
    
    // Set the texture (it might be null)
    const AssetManager* assets = loader->getManager();
    setTexture(assets->get<Texture>(data->getString("texture",UNKNOWN_STR)));

    // Get the geometry
    std::vector<Vec2> vertices;
    std::vector<Uint32> indices;

    if (data->has("polygon")) {
        JsonValue* poly = data->get("polygon").get();
        CUAssertLog(poly->size() % 2 == 0, "'polygon' should be an even list of numbers");
        for(int ii = 0; ii < poly->size(); ii += 2) {
            Vec2 vert;
            vert.x = poly->get(ii  )->asFloat(0.0f);
            vert.y = poly->get(ii+1)->asFloat(0.0f);
            vertices.push_back(vert);
        }
    }

    if (data->has("indices")) {
        JsonValue* index = data->get("indices").get();
        for(int ii = 0; ii < index->size(); ii++) {
            indices.push_back(index->get(ii  )->asInt(0));
        }
    }
    
    // If size was set explicitly, we will need to restore it after polygon
    bool sizefit = data->has("size");
    Size size = getSize();
    
    _stroke = data->getFloat("stroke", 1.0f);
    std::string joint = data->getString("joint",UNKNOWN_STR);
    if (joint == "mitre") {
        _joint = poly2::Joint::MITRE;
    } else if (joint == "bevel") {
        _joint = poly2::Joint::SQUARE;
    } else if (joint == "interior") {
        _joint = poly2::Joint::ROUND;
    } else {
        _joint = poly2::Joint::NONE;
    }

    std::string cap = data->getString("cap",UNKNOWN_STR);
    if (cap == "square") {
        _endcap = poly2::EndCap::SQUARE;
    } else if (cap == "round") {
        _endcap = poly2::EndCap::ROUND;
    } else {
        _endcap = poly2::EndCap::NONE;
    }
    
    if (data->has("closed")) {
        _closed = data->getBool("closed",false);
    } else {
        _closed = canonicallyClosed(_polygon);
    }
    if (_closed) {
        normalize();
    }
    
    if (vertices.empty() && indices.empty()) {
        Rect bounds = Rect::ZERO;
        bounds.size = _texture->getSize();
        setPolygon(bounds);
    } else if (indices.empty()) {
        setPolygon(vertices);
    } else {
        Poly2 poly(vertices,indices);
        if (indices.size() % 2 == 0) {
            poly.setGeometry(Geometry::PATH);
        }
        setPolygon(poly);
    }

    // Redo the size if necessary
    if (sizefit) {
        setContentSize(size);
    }

    setPosition(coord);
    return true;
}


#pragma mark -
#pragma mark Attributes
/**
 * Sets the stroke width of the path.
 *
 * This method affects the extruded polygon, but not the original path
 * polygon.
 *
 * @param stroke    The stroke width of the path
 */
void PathNode::setStroke(float stroke) {
    CUAssertLog(stroke >= 0, "Stroke width is invalid");
    bool changed = (stroke != _stroke);
    _stroke = stroke;

    if (changed) {
        clearRenderData();
        _extrusion.clear();
        updateExtrusion();
    }
}

/**
 * Sets whether the path is closed.
 *
 * If set to true, this will smooth the polygon to remove all gaps,
 * regardless of the original inidices in the polygon. Furthermore,
 * previous information about existing gaps is lost, so that setting
 * the value back to false will only open the curve at the end.
 *
 * @param closed    Whether the path is closed.
 */
void PathNode::setClosed(bool closed) {
    bool changed = (closed != _closed);
    _closed = closed;
    
    if (changed) {
        clearRenderData();
        if (_closed) {
            normalize();
        }
        updateExtrusion();
    }
}

/**
 * Sets the joint type between path segments.
 *
 * This method affects the extruded polygon, but not the original path
 * polygon.
 *
 * @param joint The joint type between path segments
 */
void PathNode::setJoint(poly2::Joint joint) {
    bool changed = (joint != _joint);
    _joint = joint;
    
    if (changed && _stroke > 0) {
        clearRenderData();
        _extrusion.clear();
        updateExtrusion();
    }
}

/**
 * Sets the cap shape at the ends of the path.
 *
 * This method affects the extruded polygon, but not the original path
 * polygon.
 *
 * @param cap   The cap shape at the ends of the path.
 */
void PathNode::setCap(poly2::EndCap cap) {
    bool changed = (cap != _endcap);
    _endcap = cap;
    
    if (changed && _stroke > 0) {
        clearRenderData();
        _extrusion.clear();
        updateExtrusion();
    }
}


#pragma mark -
#pragma mark Polygons
/**
 * Sets the polgon to the vertices expressed in texture space.
 *
 * The polygon will be extruded using the given sequence of vertices.
 * First it will traverse the vertices according to where it is closed
 * or open. Then it will extrude that polygon with the current joint
 * and cap. This method uses {@link SimpleExtruder}, as it is safe for
 * framerate calculation.
 *
 * @param vertices  The vertices to texture
 */
void PathNode::setPolygon(const std::vector<Vec2>& vertices) {
    CUAssertLog(vertices.size() > 1, "Path must have at least two vertices");
    _polygon.set(vertices);
    for(Uint32 ii = 0; ii < vertices.size()-1; ii++) {
        _polygon.indices().push_back(ii);
        _polygon.indices().push_back(ii+1);
    }
    if (_closed) {
        _polygon.indices().push_back((Uint32)vertices.size()-1);
        _polygon.indices().push_back(0);
    }
    _polygon.setGeometry(Geometry::IMPLICIT);
    setPolygon(_polygon);
}

/**
 * Sets the polygon to the given one in texture space.
 *
 * This method will extrude that polygon with the current joint and cap.
 * The polygon is assumed to be closed if the number of indices is twice
 * the number of vertices. PathNode objects share a single extruder, so
 * this method is not thread safe.
 *
 * @param poly  The polygon to texture
 */
void PathNode::setPolygon(const Poly2& poly) {
    CUAssertLog(poly.getGeometry() == Geometry::IMPLICIT || poly.getGeometry() == Geometry::PATH,
                "Cannot extrude a polygon with this geometry");
    CUAssertLog(poly.vertices().size() > 1, "Path must have at least two vertices");
    TexturedNode::setPolygon(poly);
    updateExtrusion();
}

/**
 * Sets the texture polygon to one equivalent to the given rect.
 *
 * The rectangle will be converted into a Poly2, using the standard outline.
 * This is the same as passing Poly2(rect,false). It will then be extruded
 * with the current joint and cap. PathNode objects share a single extruder,
 * so this method is not thread safe.
 *
 * @param rect  The rectangle to texture
 */
void PathNode::setPolygon(const Rect rect) {
    Poly2 poly(rect,false);
    if (!_closed) {
        poly.indices().pop_back();
        poly.indices().pop_back();
    }
    setPolygon(poly);
}


#pragma mark -
#pragma mark Rendering
/**
 * Draws this Node via the given SpriteBatch.
 *
 * This method only worries about drawing the current node.  It does not
 * attempt to render the children.
 *
 * This is the method that you should override to implement your custom
 * drawing code.  You are welcome to use any OpenGL commands that you wish.
 * You can even skip use of the SpriteBatch.  However, if you do so, you
 * must flush the SpriteBatch by calling end() at the start of the method.
 * in addition, you should remember to call begin() at the start of the
 * method.
 *
 * This method provides the correct transformation matrix and tint color.
 * You do not need to worry about whether the node uses relative color.
 * This method is called by render() and these values are guaranteed to be
 * correct.  In addition, this method does not need to check for visibility,
 * as it is guaranteed to only be called when the node is visible.
 *
 * @param batch     The SpriteBatch to draw with.
 * @param matrix    The global transformation matrix.
 * @param tint      The tint to blend with the Node color.
 */
void PathNode::draw(const std::shared_ptr<SpriteBatch>& batch, const Mat4& transform, Color4 tint) {
    if (!_rendered) {
        generateRenderData();
    }
    
    batch->setColor(tint);
    batch->setTexture(_texture);
    if (_gradient) {
        auto local = Gradient::alloc(_gradient);
        local->setTintColor(tint);
        local->setTintStatus(true);
        batch->setGradient(local);
    }
    batch->setBlendEquation(_blendEquation);
    batch->setBlendFunc(_srcFactor, _dstFactor);
    if (_stroke > 0) {
        batch->fill(_mesh, transform);
    } else {
        batch->outline(_mesh, transform);
    }
    batch->setGradient(nullptr);
}

/**
 * Allocate the render data necessary to render this node.
 */
void PathNode::generateRenderData() {
    CUAssertLog(!_rendered, "Render data is already present");
    if (_texture == nullptr) {
        return;
    }
    
    const Poly2 source = (_stroke > 0 ? _extrusion : _polygon);
    _mesh.set(source);
    _mesh.command = (_stroke > 0 ? GL_TRIANGLES : GL_LINES);

    Size nsize = getContentSize();
    Size bsize = _polygon.getBounds().size;
    Size tsize = _texture->getSize();
    Vec2 scale = Vec2::ONE;
    if (nsize != bsize) {
        scale.x = (bsize.width > 0 ? nsize.width/bsize.width : 0);
        scale.y = (bsize.height > 0 ? nsize.height/bsize.height : 0);
    }

    Vec2 offset = _polygon.getBounds().origin;
    for(auto it = _mesh.vertices.begin(); it != _mesh.vertices.end(); ++it) {
        it->position *= scale;
        if (!_absolute) {
            it->position -= offset*scale;
        }
        
        float s = (it->position.x)/tsize.width;
        float t = (it->position.y)/tsize.height;
        if (_flipHorizontal) { s = 1-s; }
        if (!_flipVertical)  { t = 1-t; }
        
        it->texcoord.x = s*_texture->getMaxS()+(1-s)*_texture->getMinS();
        it->texcoord.y = t*_texture->getMaxT()+(1-t)*_texture->getMinT();
        
        if (_gradient) {
            float s = (it->position.x)/_polygon.getBounds().size.width;
            float t = (it->position.y)/_polygon.getBounds().size.width;
            if (_flipHorizontal) { s = 1-s; }
            if (!_flipVertical)  { t = 1-t; }
            it->color = Vec4(s,t,0,0);
        }
    }
    
    _rendered = true;
}

#pragma mark -
#pragma mark Internal Methods
/**
 * Updates the extrusion polygon, based on the current settings.
 */
void PathNode::updateExtrusion() {
    clearRenderData();
    if (_stroke > 0) {
        SimpleExtruder extruder;
        if (_polygon.getGeometry() == Geometry::IMPLICIT) {
            extruder.set(_polygon.vertices(),_closed);
        } else {
            extruder.set(_polygon);
        }
        extruder.setJoint(_joint);
        extruder.setEndCap(_endcap);
        extruder.calculate(_stroke);
        extruder.getPolygon(&_extrusion);
        _extrbounds = _extrusion.getBounds();
        _extrbounds.origin -= _polygon.getBounds().origin;
    } else {
        _extrbounds.set(Vec2::ZERO,getContentSize());
    }
}

/**
 * Normalizes the source so that it is a closed curve with no gaps
 */
void PathNode::normalize() {
    bool smooth = true;
    size_t last = 0;
    for(size_t ii = 0; smooth && ii < _polygon.indices().size(); ii+=2) {
        smooth = (ii == 0) || _polygon.indices()[ii] == _polygon.indices()[ii-1];
        last = ii;
    }
    bool closed = _polygon.indices()[last+1] == _polygon.indices()[0];
    if (!smooth) {
        std::vector<Uint32> indices;
        for(size_t ii = 0; smooth && ii < _polygon.indices().size()-1; ii+=2) {
            bool gap = (ii == 0) || _polygon.indices()[ii] == _polygon.indices()[ii-1];
            if (gap) {
                indices.push_back(_polygon.indices()[ii-1]);
                indices.push_back(_polygon.indices()[ii]  );
            }
            indices.push_back(_polygon.indices()[ii]  );
            indices.push_back(_polygon.indices()[ii+1]);
        }
        Uint32 end = (Uint32)_polygon.indices().size()-1;
        if (_polygon.indices()[end] != _polygon.indices()[end-1]) {
            indices.push_back(_polygon.indices()[end-1]);
            indices.push_back(_polygon.indices()[end]  );
        }
        indices.push_back(_polygon.indices()[end]);
        indices.push_back(_polygon.indices()[0]);
        _polygon.setIndices(indices);
        _polygon.setGeometry(Geometry::PATH);
    } else if (!closed) {
        Uint32 end = (Uint32)_polygon.indices().size()-1;
        _polygon.indices().push_back(_polygon.indices()[end]);
        _polygon.indices().push_back(_polygon.indices()[0]);
    }
}

/**
 * Returns true if the indices of poly indicate the path is closed
 *
 * @param poly  The path to test
 *
 * @return true if the indices of poly indicate the path is closed
 */
bool PathNode::canonicallyClosed(const Poly2& poly) const {
    if (poly.indices().empty()) {
        return poly.vertices().size() > 2;
    }
    bool smooth = true;
    size_t last = 0;
    for(size_t ii = 0; smooth && ii < poly.indices().size(); ii+=2) {
        smooth = (ii == 0) || poly.indices()[ii] == poly.indices()[ii-1];
        last = ii;
    }
    return poly.indices()[last+1] == poly.indices()[0];
}
