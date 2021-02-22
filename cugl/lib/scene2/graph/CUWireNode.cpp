//
//  CUWireNode.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides a scene graph node that supports wireframes.  The
//  primary use case is to have a node that outlines physics bodies.
//
//  This class is loosely coupled with PathOutliner.  You can use PathOutliner
//  independent of the WireNode, but all functionality is present in this class.
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
#include <cugl/scene2/graph/CUWireNode.h>
#include <cugl/assets/CUScene2Loader.h>
#include <cugl/assets/CUAssetManager.h>
#include <cugl/render/CUGradient.h>

using namespace cugl;
using namespace cugl::scene2;

/** For handling JSON issues */
#define UNKNOWN_STR "<unknown>"

#pragma mark Constructors
/**
 * Intializes a polygon with the given triangulation and traversal
 *
 * The traversal will be defined exactly as the one provided by
 * {@link PolyFactory#makeTraversal}. This initializer will fail if the
 * provided polygon is not `SOLID` (e.g. does not have a triangulation).
 *
 * You do not need to set the texture; rendering this into a SpriteBatch
 * will simply use the blank texture. Hence the wireframe will have a solid
 * color.
 *
 * @param poly      The triangulated polygon
 * @param traversal The path traversal for index generation
 *
 * @return  true if the wireframe is initialized properly, false otherwise.
 */
bool WireNode::initWithTraversal(const Poly2& poly, poly2::Traversal traversal) {
    if (traversal == poly2::Traversal::INTERIOR && poly.getGeometry() != Geometry::SOLID) {
        CUAssertLog(false, "Cannot construct a wireframe of an untriangulated polygon");
        return false;
    }
    _traversal = traversal;
    return init(poly);
}

/**
 * Intializes a polygon with the given vertices and traversal
 *
 * The polygon will be defined as if it were a traversal on the implicit
 * polygon with these vertices. An interior traversal will first cause the
 * vertices to be triangulated using {@link SimpleTriangulator}.
 *
 * You do not need to set the texture; rendering this into a SpriteBatch
 * will simply use the blank texture. Hence the wireframe will have a solid
 * color.
 *
 * @param vertices  The vertices to texture (expressed in image space)
 * @param traversal The path traversal for index generation
 *
 * @return  true if the wireframe is initialized properly, false otherwise.
 */

bool WireNode::initWithTraversal(const std::vector<Vec2>& vertices, poly2::Traversal traversal) {
    _traversal = traversal;
    return init(vertices);
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
 *      "traveral": One of 'open', 'closed', or 'interior'
 *      "indices":  An array of unsigned ints defining triangles from the
 *                  the vertices. The array size should be a multiple of 3.
 *
 * All attributes are optional.  However, it is generally a good idea to
 * specify EITHER the texture or the polygon.  If you specify the indices,
 * then the traversal will be ignored.
 *
 * @param loader    The scene loader passing this JSON file
 * @param data      The JSON object specifying the node
 *
 * @return true if initialization was successful.
 */
bool WireNode::initWithData(const Scene2Loader* loader, const std::shared_ptr<JsonValue>& data) {
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

    // Set the texture
    const AssetManager* assets = loader->getManager();
    
    // This might be null
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
        CUAssertLog(index->size() % 3 == 0, "'indices' should be an list of numbers in multiples of 3");
        for(int ii = 0; ii < index->size(); ii += 3) {
            indices.push_back(index->get(ii  )->asInt(0));
            indices.push_back(index->get(ii+1)->asInt(0));
            indices.push_back(index->get(ii+2)->asInt(0));
        }
    }
    
    std::string traverse = data->getString("traversal",UNKNOWN_STR);
    poly2::Traversal plan;
    if (traverse == "open") {
        plan = poly2::Traversal::OPEN;
    } else if (traverse == "closed") {
        plan = poly2::Traversal::CLOSED;
    } else if (traverse == "interior") {
        plan = poly2::Traversal::INTERIOR;
    } else {
        plan = poly2::Traversal::NONE;
    }
    setTraversal(plan);
    
    if (vertices.empty() && indices.empty()) {
        Rect bounds = Rect::ZERO;
        bounds.size = _texture->getSize();
        setPolygon(bounds);
    } else if (indices.empty()) {
        setPolygon(vertices);
    } else {
        setPolygon(Poly2(vertices,indices));
    }
    
    if (data->has("size")) {
        JsonValue* size = data->get("size").get();
        Size size1, size2;
        size1.width  = size->get(0)->asFloat(0.0f);
        size1.height = size->get(1)->asFloat(0.0f);
        size2 = _polygon.getBounds().size;
        
        if (size1 != size2) {
            Vec2 scale;
            scale.x = _scale.x * size1.width/size2.width;
            scale.y = _scale.y * size1.height/size2.height;
            setScale(scale);
        }
    }
    
    setPosition(coord);
    return true;
}

/**
 * Creates a wireframe with the given rect.
 *
 * The rectangle will be converted into a Poly2, and the traversal is
 * `CLOSED`.
 *
 * @param   rect    The rectangle for the wireframe path
 *
 * @return  An autoreleased wireframe node
 */
std::shared_ptr<WireNode> WireNode::alloc(const Rect rect) {
    std::shared_ptr<WireNode> node = std::make_shared<WireNode>();
    if (!node->init()) {
        return nullptr;
    }
    node->setPolygon(rect);
    return node;
}

#pragma mark -
#pragma mark Attributes
/**
 * Sets the traversal of this path.
 *
 * If the traversal is different from the current known traversal, it will
 * recompute the traveral using the PathOutliner. All WireNode objects share
 * a single path outliner, so this method is not thread safe.
 *
 * @param traversal The new wireframe traversal
 */
void WireNode::setTraversal(poly2::Traversal traversal) {
    if (_traversal == traversal) {
        return;
    }
    _traversal = traversal;
    if (!_source.vertices().empty()) {
        PolyFactory factory;
        _polygon = factory.makeTraversal(_source,traversal);
        _mesh.command = _polygon.getGeometry().glCommand();
        TexturedNode::setPolygon(_polygon);
    }
}

/**
 * Sets the wireframe polgon to the vertices expressed in texture space.
 *
 * The polygon will be defined as if it were a traversal on the implicit
 * polygon with these vertices. An interior traversal will first cause the
 * vertices to be triangulated using {@link SimpleTriangulator}.
 *
 * @param vertices  The vertices to draw
 */
void WireNode::setPolygon(const std::vector<Vec2>& vertices) {
    _source.clear();
    if (_traversal == poly2::Traversal::INTERIOR) {
        SimpleTriangulator triang;
        triang.set(vertices);
        triang.calculate();
        _source = triang.getPolygon();
    } else {
        _source.set(vertices);
    }
    PolyFactory factory;
    _polygon.clear();
    factory.setGeometry(Geometry::PATH);
    factory.makeTraversal(&_polygon,_source,_traversal);
    setContentSize(_polygon.getBounds().size);
    _mesh.command = _polygon.getGeometry().glCommand();
}

/**
 * Sets the wireframe polygon to the given one in texture space.
 *
 * The provided polygon will be used as the source for the traversal.
 * The traversal will be defined exactly as the one provided by
 * {@link PolyFactory#makeTraversal}. This constructor will fail on an
 * interior traversal if the provided polygon is not `SOLID`
 * (e.g. does not have a triangulation).
 *
 * @param poly  The polygon to draw
 */
void WireNode::setPolygon(const Poly2& poly) {
    _source = poly;
    
    PolyFactory factory;
    _polygon.clear();
    factory.setGeometry(Geometry::PATH);
    factory.makeTraversal(&_polygon,_source,_traversal);
    setContentSize(_polygon.getBounds().size);
    _mesh.command = _polygon.getGeometry().glCommand();
}

/**
 * Sets the wireframe polygon to one equivalent to the given rect.
 *
 * The rectangle will be converted into a Poly2, using the current
 * traversal method.
 *
 * @param rect  The rectangle to draw
 */
void WireNode::setPolygon(const Rect rect) {
    _source.set(rect);
    _polygon.clear();
    _polygon.set(_source.vertices());
    switch (_traversal) {
        case poly2::Traversal::NONE:
            break;
        case poly2::Traversal::OPEN:
            _polygon.indices().push_back(0);
            _polygon.indices().push_back(1);
            _polygon.indices().push_back(1);
            _polygon.indices().push_back(2);
            _polygon.indices().push_back(2);
            _polygon.indices().push_back(3);
            break;
        case poly2::Traversal::CLOSED:
            _polygon.indices().push_back(0);
            _polygon.indices().push_back(1);
            _polygon.indices().push_back(1);
            _polygon.indices().push_back(2);
            _polygon.indices().push_back(2);
            _polygon.indices().push_back(3);
            _polygon.indices().push_back(3);
            _polygon.indices().push_back(0);
            break;
        case poly2::Traversal::INTERIOR:
            _polygon.indices().push_back(0);
            _polygon.indices().push_back(1);
            _polygon.indices().push_back(2);
            _polygon.indices().push_back(2);
            _polygon.indices().push_back(3);
            _polygon.indices().push_back(0);
            break;
    }
    _polygon.setGeometry(Geometry::PATH);
    setContentSize(_polygon.getBounds().size);
    _mesh.command = _polygon.getGeometry().glCommand();
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
void WireNode::draw(const std::shared_ptr<SpriteBatch>& batch, const Mat4& transform, Color4 tint) {
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
    batch->outline(_mesh, transform);
    batch->setGradient(nullptr);

}
