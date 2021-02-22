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
#ifndef __CU_GEOMETRY_H__
#define __CU_GEOMETRY_H__
#define _ENABLE_GEOM_SWITCH_ 1
#include <cugl/math/CUMathBase.h>
#include <cugl/math/CUVec2.h>
#include <unordered_set>
#include <vector>

namespace cugl {
/**
 * This class (enum) is used to provide meaning to a set of vertex indices.
 *
 * Rendering classes should use this type as a hint for how to render a polygon.
 * In particular, the method {@link #glCommand()} returns an OpenGL command
 * corresponding the index geometry.
 *
 * There is not a one-to-one correspondence between geometries and OpenGL drawing
 * commands. Geometries are concerned with structure, not the optimal organization
 * of indices.  So we do not make a distinction between triangles, triangle strips,
 * or triangle fans.
 *
 * In fact, the geometries supported all correspond to decomposable indices. These
 * are the OpenGL commands TRIANGLES, LINES, or POINTS. These correspond to indices
 * that we can easily split and combine on vertex concatenation.
 */
class Geometry {
public:
    /**
     * The geometry values
     *
     * This enum provides the values for this enum class, but the code in this class
     * allows us to "skip over" it. That is, we can write Geometry::IMPLICIT in place
     * of Geometry::Value::IMPLICIT, and so on. So we can ignore this enum.
     */
    enum Value : uint8_t {
        /**
         * This geometry corresponds to an empty set of vertices.
         *
         * In this case the geometry is inferred from the vertex order. Such an
         * object is useful for pure math computations, but has no drawing mesh.
         */
        IMPLICIT,
        /**
         * This geometry corresponds to an unstructured set of indices..
         *
         * In this case indices represent the vertices that are relevant.  Vertices
         * not in the index set are ignored.
         */
        POINTS,
        /**
         * This geometry represents an open path outline.
         *
         * The indices are a sequence of line segments.  That is, the number of indices
         * is divisible by two, with each pair forming a segment.  These segments may or
         * be linked to each other.
         */
        PATH,
        /**
         * This geometry represents a solid shape with a triangulation.
         *
         * The indices are a sequence of triangles.  That is, the number of indices
         * is divisible by three, with each triplet forming a triangle.
         */
        SOLID
    };

#pragma mark Enum Simulation
    /**
     * Disable the explicity Geomtry constructor
     */
    Geometry() = default;
    
    /**
     * Construct a geometry with one of the enumeration values
     *
     * @param val   The enumeration value
     */
    constexpr Geometry(Value val) : _value(val) { }

#if _ENABLE_GEOM_SWITCH_
    /**
     * Returns the current value of this geometry.
     *
     * This method enables both direct comparisons and swith statements.
     *
     * @return the current value of this geometry.
     */
    operator Value() const { return _value; }
    
    /** Prevent usage: if(geom) */
    explicit operator bool() = delete;
#else
    /**
     * Returns true if other geometry value is equal
     *
     * @param g The geometry value to compare
     *
     * @return true if other geometry value is equal
     */
    constexpr bool operator==(Geometry a) const { return _value == a._value; }

    /**
     * Returns true if other geometry value is not equal
     *
     * @param g The geometry value to compare
     *
     * @return true if other geometry value is not equal
     */
    constexpr bool operator!=(Geometry a) const { return _value != a._value; }
#endif

#pragma mark Matching
    /**
     * Returns the OpenGL drawing code for this geometry
     *
     * If the geometry is {@link #IMPLICIT}, this method returns -1.
     *
     * @return the OpenGL drawing code for this geometry
     */
    GLuint glCommand() const;
      
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
    bool matches(const std::vector<Uint32>& indices) const;

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
    bool matches(const Uint32* indices, size_t size) const;

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
    static Geometry categorize(const std::vector<Uint32>& indices);

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
    static Geometry categorize(const Uint32* indices, size_t size);
    
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
    std::vector<Uint32> index(const std::vector<Vec2>& vertices) const;
    
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
    std::vector<Uint32> index(const std::vector<float>& vertices) const;

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
    std::vector<Uint32> index(const float* vertices, size_t size) const;

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
    std::unordered_set<Uint32> exterior(const std::vector<Uint32> indices) const;
    
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
    std::unordered_set<Uint32> exterior(const Uint32* indices, size_t size) const;

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
    std::vector<std::vector<Uint32>> boundaries(const std::vector<Uint32>& indices) const;

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
    std::vector<std::vector<Uint32>> boundaries(const Uint32* indices, size_t size) const;
    
private:
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
    std::vector<std::vector<Uint32>> detriangulate(const Uint32* indices, size_t size) const;

    /** Forward declaration for the hash function */
    class TreeNode;
    
    /**
     * This class is a triangle in a mesh, interpreted as a node in tree decomposition
     *
     * Two triangles are adjacent in this decomposition if they share an edge. This
     * dual graph is not connected, though we do track direction when we are recursively
     * following a path.
     *
     * The elements in a tree node are ordered in ascending order, so that we can
     * uniquely identify a tree node from its contents.
     */
    class TreeNode {
    public:
        /** Hash function for a tree node */
        struct TreeHasher {
            /**
             * Returns the hash code for the given TreeNode
             *
             * @param node  The TreeNode to hash
             *
             * @return the hash code for the given TreeNode
             */
            std::size_t operator()(TreeNode* node) const;
        };
        
        /** The elements of this triangle */
        int elements[3];
        /** The adjacent neighbors to this node */
        std::unordered_set<TreeNode*,TreeHasher> neighbors;
            
        /** The node pointing to this one in a traversal */
        TreeNode* previous;
            
        /**
         * Creates a TreeNode from the given three elements.
         *
         * @param a The first element
         * @param b The second element
         * @param c The third element
         */
        TreeNode(int a, int b, int c);

        /**
         * Delets a TreeNode, destroying all resources.
         */
        ~TreeNode();

        /**
         * Returns true if o is a TreeNode equal to this one
         *
         * Since TreeNode objects sort their contents, o must have its elements in
         * the same order as this on.
         *
         * @return true if o is a TreeNode equal to this one
         */
        bool operator==(const TreeNode& o) const;
            
        /**
         * Returns a string representation of this tree node
         *
         * @return a string representation of this tree node
         */
        std::string toString() const;
            
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
        static std::string toString(int a, int b, int c);
            
        /**
         * Returns true if x is an element in this node
         *
         * @param x The element to check
         *
         * @return true if x is an element in this node
         */
        bool contains(int x) const;
            
        /**
         * Returns true if node is adjacent to this one.
         *
         * A node is adjacent if it shares exactly one side.
         *
         * @param node  The node to check
         *
         * @return true if node is adjacent to this one.
         */
        bool adjacent(const TreeNode* node) const;
    
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
        Uint32 pick(const std::unordered_set<Uint32>& inuse) const;
            
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
        TreeNode* follow(Uint32 index);

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
        TreeNode* crawl(int index);
    };

private:
    /** The geometry value */
    Value _value;
};

}
#endif /* __CU_GEOMETRY_H__ */
