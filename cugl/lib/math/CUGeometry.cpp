//
//  CUGeometry.h
//  Cornell University Game Library (CUGL)
//
//  This module provides an very robust enum that that categorizes polygon
//  geometry. In earlier versions of the CUGL, this type was built into the
//  Poly2 class.  However, we realized that there is a lot of semantic
//  information in this type that is separate from the polygon that it is
//  attached to.  Therefore, we have made it into its own separate class.
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
//
#include <cugl/math/CUGeometry.h>
#include <cugl/math/polygon/CUSimpleTriangulator.h>
#include <cugl/util/CUDebug.h>
#include <unordered_map>

using namespace cugl;

#pragma mark Matching
/**
 * Returns the OpenGL drawing code for this geometry
 *
 * If the geometry is {@link #IMPLICIT}, this method returns -1.
 *
 * @return the OpenGL drawing code for this geometry
 */
GLuint Geometry::glCommand() const {
    switch (_value) {
        case Geometry::IMPLICIT:
            return -1;
        case Geometry::POINTS:
            return GL_POINTS;
        case Geometry::SOLID:
            return GL_TRIANGLES;
        case Geometry::PATH:
            return GL_LINES;
    }
    return -1;
}
      
/**
 * Returns true if the indices match this geometry.
 *
 * If the geometry is {@link #SOLID}, this method will only detect if the
 * number of indices is divisible by three. For {@link #PATH}, it will
 * detect if the number of indices is not divisible by 2. Finally,
 * {@link #IMPLICIT} requires the indices be empty, but the geometry
 * {@link #POINTS} matches anything.
 *
 * This method does not validate that the indices are with in range, as that
 * requires knowledge of the vertices.
 *
 * @param indices   The indices to test
 *
 * @return true if the indices match this geometry.
 */
bool Geometry::matches(const std::vector<Uint32>& indices) const {
    return matches(indices.data(),indices.size());
}

/**
 * Returns true if the indices match this geometry.
 *
 * If the geometry is {@link #SOLID}, this method will only detect if the
 * number of indices is divisible by three. For {@link #PATH}, it will
 * detect if the number of indices is not divisible by 2. Finally,
 * {@link #IMPLICIT} requires the indices be empty, but the geometry
 * {@link #POINTS} matches anything.
 *
 * This method does not validate that the indices are with in range, as that
 * requires knowledge of the vertices.
 *
 * @param indices   The indices to test
 * @param size      The index size
 *
 * @return true if the indices match this geometry.
 */
bool Geometry::matches(const Uint32* indices, size_t size) const {
    bool result = true;
    switch (_value) {
        case Geometry::IMPLICIT:
            result = indices == nullptr || size == 0;
        case Geometry::SOLID:
            result = indices != nullptr && (size >= 3) && (size % 3 == 0);
        break;
        case Geometry::PATH:
            result = indices != nullptr && (size >= 2) && (size % 2 == 0);
            break;
        case Geometry::POINTS:
            break;
    }
    return result;
}
    
/**
 * Returns the best guess for the geometry of these indices
 *
 * If the indices form a continuous link of pairs, then it will identify
 * them as a {@link #PATH}. Otherwise, this method can only guess at the
 * geometry from the parity.  If the number of indices is a multiple of 3,
 * it assumes it is {@link #SOLID}. If it is a multiple of 2, it assumes
 * it is a {@link #PATH}. If neither is true, it identifies these as
 * {@link #POINTS}.  It is only {@link #IMPLICIT} if indices are null
 * or empty.
 *
 * @param indices   The indices to analyze
 *
 * @return the best guess for the geometry of these indices
 */
Geometry Geometry::categorize(const std::vector<Uint32>& indices) {
    return categorize(indices.data(),indices.size());
}

/**
 * Returns the best guess for the geometry of these indices
 *
 * If the indices form a continuous link of pairs, then it will identify
 * them as a {@link #PATH}. Otherwise, this method can only guess at the
 * geometry from the parity.  If the number of indices is a multiple of 3,
 * it assumes it is {@link #SOLID}. If it is a multiple of 2, it assumes
 * it is a {@link #PATH}. If neither is true, it identifies these as
 * {@link #POINTS}.  It is only {@link #IMPLICIT} if indices are null
 * or empty.
 *
 * @param indices   The indices to analyze
 * @param size      The index size
 *
 * @return the best guess for the geometry of these indices
 */
Geometry Geometry::categorize(const Uint32* indices, size_t size) {
    if (indices == nullptr || size == 0) {
        return Geometry::IMPLICIT;
    }
    
    bool linked = false;
    if (size >= 2 && size % 2 == 0) {
        linked = true;
        for(int ii = 1; linked && ii < size-2; ii += 2) {
            linked = indices[ii] == indices[ii+1];
        }
    }
    
    if (linked) {
        return Geometry::PATH;
    } else if (size >= 3 && size % 3 == 0) {
        return Geometry::SOLID;
    } else if (size >= 2 && size % 2 == 0) {
        return Geometry::PATH;
    }
    
    return Geometry::POINTS;
}
    
#pragma mark -
#pragma mark Index Generation
/**
 * Returns a canonical set of indices for the given vertex list
 *
 * This takes a vertex list and generates a canonical index list that
 * matches this geometry. For {@link #IMPLICIT}, this will return an
 * empty index set. For {@link #POINTS} it is the index of all the
 * vertices. For {@link #PATH}, it is a linked set of segment indices
 * of the vertices in order.
 *
 * For {@link #SOLID} it triangulates the vertices. For simplicity,
 * this method does not support vertex lists with self-crossings. Such
 * polygons will need to be decomposed first. This method always uses a
 * {@link SimpleTriangulator}. If you wish to use another algorithm (such
 * as {@link ComplexTriangulator}), you will need to generate the indices
 * separately.
 *
 * @param vertices  The vertex list
 *
 * @return a canonical set of indices for the given vertex list
 */
std::vector<Uint32> Geometry::index(const std::vector<Vec2>& vertices) const {
    std::vector<Uint32> indices;
    switch (_value) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            indices.reserve( vertices.size() );
            for(Uint32 ii = 0; ii < vertices.size(); ii++) {
                indices.push_back(ii);
            }
            break;
        case Geometry::PATH:
            indices.reserve( vertices.size()*2-2);
            for(int ii = 0; ii < vertices.size()-1; ii++) {
                indices.push_back(ii);
                indices.push_back(ii+1);
            }
        case Geometry::SOLID:
            // The simple triangulator is always faster
            SimpleTriangulator triang;
            triang.set(vertices);
            triang.calculate();
            triang.getTriangulation(indices);
            break;
    }
    return indices;
}

/**
 * Returns a canonical set of indices for the given vertex list
 *
 * This takes a vertex list and generates a canonical index list that
 * matches this geometry. For {@link #IMPLICIT}, this will return an
 * empty index set. For {@link #POINTS} it is the index of all the
 * vertices. For {@link #PATH}, it is a linked set of segment indices
 * of the vertices in order.
 *
 * For {@link #SOLID} it triangulates the vertices. For simplicity,
 * this method does not support vertex lists with self-crossings. Such
 * polygons will need to be decomposed first. This method always uses a
 * {@link SimpleTriangulator}. If you wish to use another algorithm (such
 * as {@link ComplexTriangulator}), you will need to generate the indices
 * separately.
 *
 * @param vertices  The vertex list
 *
 * @return a canonical set of indices for the given vertex list
 */
std::vector<Uint32> Geometry::index(const std::vector<float>& vertices) const {
    return index(vertices.data(),vertices.size());
}

/**
 * Returns a canonical set of indices for the given vertex list
 *
 * This takes a vertex list and generates a canonical index list that
 * matches this geometry. For {@link #IMPLICIT}, this will return an
 * empty index set. For {@link #POINTS} it is the index of all the
 * vertices. For {@link #PATH}, it is a linked set of segment indices
 * of the vertices in order.
 *
 * For {@link #SOLID} it triangulates the vertices. For simplicity,
 * this method does not support vertex lists with self-crossings. Such
 * polygons will need to be decomposed first. This method always uses a
 * {@link SimpleTriangulator}. If you wish to use another algorithm (such
 * as {@link ComplexTriangulator}), you will need to generate the indices
 * separately.
 *
 * @param vertices  The vertex list
 * @param size      The number of vertices
 *
 * @return a canonical set of indices for the given vertex list
 */
std::vector<Uint32> Geometry::index(const float* vertices, size_t size) const {
    CUAssertLog(size % 2 == 0, "Size %d has the wrong parity", (Uint32)size);
    std::vector<Uint32> indices;
    switch (_value) {
        case Geometry::IMPLICIT:
            break;
        case Geometry::POINTS:
            indices.reserve( size/2 );
            for(Uint32 ii = 0; ii < size/2; ii++) {
                indices.push_back(ii);
            }
            break;
        case Geometry::PATH:
            indices.reserve( size-2 );
            for(int ii = 0; ii < size/2-1; ii++) {
                indices.push_back(ii);
                indices.push_back(ii+1);
            }
            break;
        case Geometry::SOLID:
            // The simple triangulator is always faster
            SimpleTriangulator triang;
            std::vector<Vec2> verts;
            for(size_t ii = 0; ii < size; ii += 2) {
                verts.push_back(Vec2(vertices[ii],vertices[ii+1]));
            }
            triang.set(verts);
            triang.calculate();
            triang.getTriangulation(indices);
            break;
    }
    return indices;
}

#pragma mark -
#pragma mark Boundary Extraction
/**
 * Returns the set of indices that are on a boundary of this geometry
 *
 * This method is really only relevant for {@link #SOLID} geometry. For other
 * geometries, it simply returns a set of the indices.  In the case of solid
 * geometries, it identifies the outer hull (not necessarily convex).
 *
 * This method can identify the outer hull using the graph properties of the
 * triangle mesh. An internal node if the number of neighbors is the same as
 * the number of attached triangles. An index that is not internal is external.
 *
 * Unlike {@link #boundaries}, this method does not order the boundary indices
 * or decompose them into connected components.
 *
 * @param indices   The indices to process
 *
 * @return the set of indices that are on a boundary of this geometry
 */
std::unordered_set<Uint32> Geometry::exterior(const std::vector<Uint32> indices) const {
    return exterior(indices.data(),indices.size());
}
    
/**
 * Returns the set of indices that are on a boundary of this geometry
 *
 * This method is really only relevant for {@link #SOLID} geometry. For other
 * geometries, it simply returns a set of the indices.  In the case of solid
 * geometries, it identifies the outer hull (not necessarily convex).
 *
 * This method can identify the outer hull using the graph properties of the
 * triangle mesh. An internal node if the number of neighbors is the same as
 * the number of attached triangles. An index that is not internal is external.
 *
 * Unlike {@link #boundaries}, this method does not order the boundary indices
 * or decompose them into connected components.
 *
 * @param indices   The indices to process
 * @param size      The index size
 *
 * @return the set of indices that are on a boundary of this geometry
 */
std::unordered_set<Uint32> Geometry::exterior(const Uint32* indices, size_t size) const {
    std::unordered_set<Uint32> result;
    if (_value != Geometry::SOLID) {
        result.reserve(size);
        for(size_t ii = 0; ii < size; ii++) {
            result.emplace(indices[ii]);
        }
        return result;
    }

    std::unordered_map<Uint32,std::unordered_set<Uint32>*> neighbors;
    std::unordered_map<Uint32,Uint32> count;
    for(int ii = 0; ii < size; ii += 3) {
        for(int jj = 0; jj < 3; jj++) {
            Uint32 indx = indices[ii+jj];
            auto search = neighbors.find(indx);
            std::unordered_set<Uint32>* slot = nullptr;
            if (search == neighbors.end()) {
                slot = new std::unordered_set<Uint32>();
                neighbors[indx] = slot;
                count[indx] = 0;
            } else {
                slot = search->second;
            }

            slot->emplace( indices[ii+((jj+1) % 3)] );
            slot->emplace( indices[ii+((jj+2) % 3)] );
            count[indx] = count[indx]+1;
        }
    }

    result.reserve(size);
    for(size_t ii = 0; ii < size; ii++) {
        Uint32 indx = indices[ii];
        if (neighbors[indx]->size() > count[indx]) {
            result.emplace(indx);
        }
    }
    
    // Clean up
    for(auto it = neighbors.begin(); it != neighbors.end(); ++it) {
        delete it->second;
        it->second = nullptr;
    }
    return result;
}

/**
 * Returns the connected boundary components for this geometry.
 *
 * This method allows us to reconstruct the exterior boundary of a solid
 * shape, or to compose a pathwise connected curve into components.
 *
 * For the geometry {@link #PATH}, the elements of the outer array are
 * connected links of line segments, with the indices listed in order of the
 * path traversal. A closed path only has one component, so the outer array
 * will only have one element. An open path may have several components if
 * there are discontinuities in the path. {@link #IMPLICIT} geometries are
 * treated as if they were closed a closed path.
 *
 * For {@link #SOLID}, this method detriangulates the mesh, returning the
 * outer hull, and discarding any interior points.  This hull need not be
 * convex (as concave or convex cannot be inferred from indices alone).
 * If the mesh represents a simple polygon, only one boundary will be returned.
 * If the mesh is not continuous, the outer array will contain the boundary
 * of each disjoint polygon. If the mesh has holes, each hole will be returned
 * as a separate boundary. There is no guarantee on the order of boundaries
 * returned.
 *
 * For {@link #POINTS}, there are no connected components, so this method
 * returns an array of single-element arrays.  {@link #IMPLICIT} returns
 * null, as it is impossible to infer the boundaries without the vertices.
 *
 * @param indices   The indices to process
 *
 * @return the connected boundary components for this geometry.
 */
std::vector<std::vector<Uint32>> Geometry::boundaries(const std::vector<Uint32>& indices) const {
    return boundaries(indices.data(),indices.size());
}

/**
 * Returns the connected boundary components for this geometry.
 *
 * This method allows us to reconstruct the exterior boundary of a solid
 * shape, or to compose a pathwise connected curve into components.
 *
 * For the geometry {@link #PATH}, the elements of the outer array are
 * connected links of line segments, with the indices listed in order of the
 * path traversal. A closed path only has one component, so the outer array
 * will only have one element. An open path may have several components if
 * there are discontinuities in the path. {@link #IMPLICIT} geometries are
 * treated as if they were closed a closed path.
 *
 * For {@link #SOLID}, this method detriangulates the mesh, returning the
 * outer hull, and discarding any interior points.  This hull need not be
 * convex (as concave or convex cannot be inferred from indices alone).
 * If the mesh represents a simple polygon, only one boundary will be returned.
 * If the mesh is not continuous, the outer array will contain the boundary
 * of each disjoint polygon. If the mesh has holes, each hole will be returned
 * as a separate boundary. There is no guarantee on the order of boundaries
 * returned.
 *
 * For {@link #POINTS}, there are no connected components, so this method
 * returns an array of single-element arrays.  {@link #IMPLICIT} returns
 * null, as it is impossible to infer the boundaries without the vertices.
 *
 * @param indices   The indices to process
 * @param size      The index size
 *
 * @return the connected boundary components for this geometry.
 */
std::vector<std::vector<Uint32>> Geometry::boundaries(const Uint32* indices, size_t size) const {
    std::vector<std::vector<Uint32>> result;
    switch (_value) {
        case Geometry::IMPLICIT:
        {
            result.push_back(std::vector<Uint32>());
            std::vector<Uint32> *array = &result.back();
            for(size_t ii = 0; ii < size; ii++) {
                Uint32 idx = indices[ii];
                array->push_back(idx);
            }
            break;
        }
        case Geometry::POINTS:
        {
            for(size_t ii = 0; ii < size; ii++) {
                Uint32 idx = indices[ii];
                result.push_back(std::vector<Uint32>());
                std::vector<Uint32> *array = &result.back();
                array->push_back(idx);
            }
            break;
        }
        case Geometry::PATH:
        {
            result.push_back(std::vector<Uint32>());
            std::vector<Uint32> *array = &result.back();
            array->reserve(size/2);
            for(size_t ii = 0; ii < size; ii += 2) {
                if (array->size() != 0) {
                    if (indices[ii] != indices[ii-1]) {
                        result.push_back(std::vector<Uint32>());
                        array = &result.back();
                        array->reserve((size-ii)/2);
                        array->push_back(indices[ii]);
                    } else if (indices[ii] == array->at(0)) {
                        result.push_back(std::vector<Uint32>());
                        array = &result.back();
                        array->reserve((size-ii)/2);
                    } else {
                        array->push_back(indices[ii]);
                    }
                } else {
                    array->push_back(indices[ii]);
                }
            }
            break;
        }
        case Geometry::SOLID:
            return detriangulate(indices,size);
    }
    return result;
}

    
#pragma mark -
#pragma mark Detriangulation Support
/**
 * Returns a detriangulation of this mesh
 *
 * The detriangulation consists of the indices of the outer hull, discarding
 * any interior points. This hull need not be convex (as concave or convex
 * cannot be inferred from indices alone).
 *
 * If the mesh represents a simple polygon, only one boundary will be returned.
 * If the mesh is not continuous, the outer array will contain the boundary
 * of each disjoint polygon. If the mesh has holes, each hole will be returned
 * as a separate boundary.  There is no guarantee on the order of boundaries
 * returned.
 *
 * @param indices   The indices to process
 * @param size      The index size
 *
 * @return a detriangulation of this mesh
 */
std::vector<std::vector<Uint32>> Geometry::detriangulate(const Uint32* indices, size_t size) const {
    // Create the decomposition
    std::unordered_map<std::string,TreeNode*> decomp;
    for(int ii = 0; ii < size; ii += 3) {
        std::string key = TreeNode::toString(indices[ii],indices[ii+1],indices[ii+2]);
        auto search = decomp.find(key);
        if (search == decomp.end()) {
            TreeNode* current = new TreeNode(indices[ii],indices[ii+1],indices[ii+2]);
            for(auto it = decomp.begin(); it != decomp.end(); ++it) {
                TreeNode* node = it->second;
                if (node->adjacent( current )) {
                    node->neighbors.emplace( current );
                    current->neighbors.emplace(node);
                }
            }
            decomp[key] = current;
        }
    }
    
    // Create arrays for the result and to track our progress
    std::vector<std::vector<Uint32>> result;
    std::unordered_set<Uint32> inuse;
    std::unordered_set<Uint32> total;
    for(size_t ii = 0; ii < size; ii++) {
        total.emplace(indices[ii]);
    }
    
    // Keep going until all boundaries found
    result.push_back(std::vector<Uint32>());
    std::vector<Uint32>* array = &result.back();
    bool abort = false;
    while (inuse.size() != total.size() && !abort) {
        // Pick a valid (exterior) starting point at the correct position
        TreeNode* current = nullptr;
        int start = -1;
        for(auto it = decomp.begin(); start == -1 && it != decomp.end(); ++it) {
            TreeNode* node = it->second;
            start = node->pick(inuse);
            if (start != -1) {
                current = node;
            }
        }
        
        // Self-crossings may allow a point to be reused, so we
        // need a local "visited" set for each path.
        std::unordered_set<Uint32> visited;
        if (start != -1) {
            // Follow the path until no more indices to pick
            int index = start;
            current = current->follow( index );
            while (current != nullptr) {
                visited.emplace( index );
                array->push_back( index );
                index = current->pick( visited );
                current = current->follow( index );
            }
            // Add this to the global results
            for(auto it = visited.begin(); it != visited.end(); ++it) {
                inuse.emplace(*it);
            }
            result.push_back(std::vector<Uint32>());
            array = &result.back();

            // Reset the tree node internal state for next pass
            if (inuse.size() != total.size()) {
                for(auto it = decomp.begin(); it != decomp.end(); ++it) {
                    it->second->previous = nullptr;
                }
            }
        } else {
            // All the indices found were internal
            abort = true;
        }
    }
    
    // Clean up the decomposition
    for(auto it = decomp.begin(); it != decomp.end(); ++it) {
        delete it->second;
        it->second = nullptr;
    }

    return result;
}

/**
 * Returns the hash code for the given TreeNode
 *
 * @param node  The TreeNode to hash
 *
 * @return the hash code for the given TreeNode
 */
std::size_t Geometry::TreeNode::TreeHasher::operator()(TreeNode* node) const {
    size_t one = std::hash<int>()((node->elements)[0]) & 0xff;
    size_t two = std::hash<int>()((node->elements)[1]) & 0xff;
    size_t tre = std::hash<int>()((node->elements)[2]) & 0xff;
    return (one | (two << 8) | (tre << 16));
}

/**
 * Creates a TreeNode from the given three elements.
 *
 * @param a The first element
 * @param b The second element
 * @param c The third element
 */
Geometry::TreeNode::TreeNode(int a, int b, int c) {
    elements[0] = std::min( a, std::min( b, c ) );
    elements[2] = std::max( a, std::max( b, c ) );
    elements[1] = a;
    if (elements[0] == elements[1] || elements[2] == elements[1]) {
        elements[1] = b;
    }
    if (elements[0] == elements[1] || elements[2] == elements[1]) {
        elements[1] = c;
    }
    CUAssertLog(elements[0] < elements[1] && elements[1] < elements[2],
                "The triangle [%d, %d, %d] is degenerate.", a, b, c );
}

/**
 * Delets a TreeNode, destroying all resources.
 */
Geometry::TreeNode::~TreeNode() {
    // TreeNodes own nothing
    neighbors.clear();
    previous = nullptr;
}
            
/**
 * Returns true if o is a TreeNode equal to this one
 *
 * Since TreeNode objects sort their contents, o must have its elements in
 * the same order as this on.
 *
 * @return true if o is a TreeNode equal to this one
 */
bool Geometry::TreeNode::operator==(const TreeNode& o) const {
    return elements[0] == o.elements[0] && elements[1] == o.elements[1] && elements[2] == o.elements[2];
}
            
/**
 * Returns a string representation of this tree node
 *
 * @return a string representation of this tree node
 */
std::string Geometry::TreeNode::toString() const {
    char buff[100];
    snprintf(buff, sizeof(buff), "[%d,%d,%d]", elements[0], elements[1], elements[2]);
    std::string result = buff;
    return result;
}

/**
 * Returns a string representation of a tree node with the given elements
 *
 * This method allows us to get the string of a tree node (from its contents)
 * without actually having to construct the tree node itself.  This is useful
 * for hashtable lookups.
 *
 * @param a The first element
 * @param b The second element
 * @param c The third element
 *
 * @return a string representation of a tree node with the given elements
 */
std::string Geometry::TreeNode::toString(int a, int b, int c) {
    int indx1 = std::min( a, std::min( b, c ) );
    int indx3 = std::max( a, std::max( b, c ) );
    int indx2 = a;
    if (indx1 == indx2 || indx3 == indx2) {
        indx2 = b;
    }
    if (indx1 == indx2 || indx3 == indx2) {
        indx2 = c;
    }
    char buff[100];
    snprintf(buff, sizeof(buff), "[%d,%d,%d]", indx1, indx2, indx3);
    std::string result = buff;
    return result;
}
            
/**
 * Returns true if x is an element in this node
 *
 * @param x The element to check
 *
 * @return true if x is an element in this node
 */
bool Geometry::TreeNode::contains(int x) const {
    return x >= 0 && (elements[0] == x || elements[1] == x || elements[2] == x);
}
    
/**
 * Returns true if node is adjacent to this one.
 *
 * A node is adjacent if it shares exactly one side.
 *
 * @param node  The node to check
 *
 * @return true if node is adjacent to this one.
 */
bool Geometry::TreeNode::adjacent(const TreeNode* node) const {
    int count = 0;
    for (int ii = 0; ii < 3; ii++) {
        count += contains( node->elements[ii] ) ? 1 : 0;
    }
    return count == 2;
}
    
/**
 * Returns a boundary index from the node, not in inuse
 *
 * A boundary index is either one that does not appear in any
 * of its neighbors (so this is an ear in a triangulation) or
 * only appears in one neighbor (so this is the either the first
 * or last triangle with this index in a normal traversal).
 *
 * If no boundary index can be found, or they are all already
 * in inuse, this method returns -1.
 *
 * @param inuse The indices to exclude from the search
 *
 * @return a boundary index from the node, not in inuse
 */
Uint32 Geometry::TreeNode::pick(const std::unordered_set<Uint32>& inuse) const {
    int count[3];
    for (auto it = neighbors.begin(); it != neighbors.end(); ++it) {
        TreeNode* node = *it;
        for (int ii = 0; ii < 3; ii++) {
            if (node->contains( elements[ii] )) {
                    count[ii]++;
            }
        }
    }
    
    int ptr = -1;
    for (int ii = 0; ii < 3; ii++) {
        if ((count[ii] == 0 || count[ii] == 1)) {
            if (inuse.find(elements[ii]) != inuse.end()) {
                if (ptr == -1) {
                    ptr = ii;
                } else if (count[ii] < count[ptr]) {
                    ptr = ii;
                }
            }
        }
    }
    
    return ptr != -1 ? elements[ptr] : -1;
}
            
/**
 * Returns the opposite transition point for the given index.
 *
 * A transition point is a node that contains index and for which index is
 * a boundary value (either it has no neighbors with the same index or only
 * one neighbor).  It represents the first and/or last triangle with this
 * index in a normal traversal.
 *
 * If there is only one triangle with this index, this method returns this
 * node.  Otherwise, if this node corresponds to the first triangle, it
 * returns the last, and vice versa.  By following indices, we create a
 * traversal that can find an exterior boundary.
 *
 * @param index The index defining the traversal
 *
 * @return the opposite transition point for the given index.
 */
Geometry::TreeNode* Geometry::TreeNode::follow(Uint32 index) {
    previous = nullptr;
    return crawl( index );
}

/**
 * Returns the opposite transition point for the given index.
 *
 * This method is the recursive helper for {@link #follow}. It uses
 * the internal previous attribute to track direction.
 *
 * @param index The index defining the traversal
 *
 * @return the opposite transition point for the given index.
 */
Geometry::TreeNode* Geometry::TreeNode::crawl(int index) {
    if (!contains(index)) {
        return nullptr;
    }

    TreeNode* next = nullptr;
    for (auto it = neighbors.begin(); it != neighbors.end(); ++it) {
        TreeNode* node = *it;
        if (node != previous && node->contains( index )) {
            next = node;
        }
    }

    if (next == nullptr) {
        return this;
    } else if (next->previous == this) {
        return nullptr;
    }
    next->previous = this;
    return next->crawl( index );
}
