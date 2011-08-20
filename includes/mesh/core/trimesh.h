#ifndef MESH_TRIMESH_H
#define MESH_TRIMESH_H

#include <algorithm>
#include <vector>

#include "mesh/implementation/util.h"
#include "mesh/core/triangle.h"

namespace Mesh {

/*******************************************************************************
 * A data structure for triangulated meshes.
 *
 * This class is the basic data structure the mesh library is built out on.
 * It implements basic topological operations for triangulated surfaces,
 * including incidence queries, vertex and face creation/deletion, and Euler
 * operations.  This is a foundational class, and does not implement things
 * like complex algorithms or geometric operations.
 *
 * The template parameters have the following meaning:
 *
 *   VertexData_t : The data type for a vertex.  Must be copy constructible and
 *		have a default constructor. Other properties may be required for 
 *		additional algorithms.
 *
 *******************************************************************************/
template<typename VertexData_t>
struct TriMesh {
	
	///A list of vertex names
	typedef std::vector<int> IncidenceList;
	
	///Type alias for the vertex data structure.
	typedef VertexData_t VertexData;

	//Constructors/assignment operator boilerplate
	TriMesh() {}
	TriMesh(const TriMesh& other) :
		dead_tris(other.dead_tris),
		tri_data(other.tri_data),
		dead_verts(other.dead_verts),
		vert_data(other.vert_data),
		incidence(other.incidence) {}
	TriMesh(TriMesh&& other) :
		dead_tris(other.dead_tris),
		tri_data(other.tri_data),
		dead_verts(other.dead_verts),
		vert_data(other.vert_data),
		incidence(other.incidence) {}
	TriMesh& operator=(const TriMesh& other) {
		dead_tris	= other.dead_tris;
		tri_data	= other.tri_data;
		dead_verts	= other.dead_verts;
		vert_data	= other.vert_data;
		incidence	= other.incidence;
		return *this;
	}
	TriMesh&& operator=(TriMesh&& other) {
		dead_tris	= std::move(other.dead_tris);
		tri_data	= std::move(other.tri_data);
		dead_verts	= std::move(other.dead_verts);
		vert_data	= std::move(other.vert_data);
		incidence	= std::move(other.incidence);
		return *this;
	}
	
	/**
	 * Reserves space for vertices and triangles.
	 *
	 * Preallocates memory for vertices and triangles.  This can make
	 * future vertex and triangle insertions faster by removing the need
	 * to resize incrementally.
	 *
	 *	nv : Amount of vertices to reserve
	 *	nt : Amount of triangles to reserve
	 */	
	void reserve(int nv, int nt) {
		tri_data.reserve(nt);	
		vert_data.reserve(nv);
		incidence.reserve(nv);
	}
	
	/**
	 * Swaps the contents of this TriMesh with another.
	 *
	 *	other : The TriMesh to swap with.
	 */
	void swap(TriMesh& other) {
		dead_tris.swap(other.dead_tris);
		tri_data.swap(other.tri_data);
		dead_verts.swap(other.dead_verts);
		vert_data.swap(other.vert_data);
		incidence.swap(other.incidence);
	}

	/**
	 * Removes all vertices and triangles from the mesh.
	 */	
	void clear() {
		dead_tris.clear();
		tri_data.clear();
		dead_verts.clear();
		vert_data.clear();
		incidence.clear();
	}
	
	/**
	 * Returns the triangle with the given name.
	 *
	 * Given the name of a triangle, return the Triangle object representing the
	 * vertices which it is incident to.
	 *
	 *	t : The name of the triangle.
	 */
	const Triangle&				triangle(int t) const	{ return tri_data[t]; }
	
	/**
	 * Returns a readable list of all triangles
	 */
	const std::vector<Triangle>& triangles()  const		{ return tri_data; }

	/**
	 * Returns the vertex with the given name.
	 *
	 * Given the name of a vertex, return the vertex data associated to it.
	 *
	 *	v : The name of the vertex.
	 */
	const VertexData&			vertex(int v) const	{ return vert_data[v]; }
	VertexData&					vertex(int v)		{ return vert_data[v]; }
	
	/**
	 * Returns a readable list of all vertices
	 */
	const std::vector<VertexData>&	vertices() const		{ return vert_data; }
	
	/**
	 * Returns the collection of all triangles incident to a given vertex.
	 *
	 *	v : The name of the vertex
	 */
	const IncidenceList&	vertex_incidence(int v) const	{ return incidence[v]; }
	
	/**
	 * Creates a vertex.
	 *
	 *	vdata : The data to assign to the given vertex.
	 *
	 *	Returns : The name of the vertex which was created.
	 */
	int add_vertex(const VertexData& vdata) {
		IncidenceList tmp;
		
		if(dead_verts.size() > 0) {
			int n = dead_verts.back();
			dead_verts.pop_back();
			incidence[n] = tmp;
			vert_data[n] = vdata;
			return n;
		}
		else {
			incidence.push_back(tmp);
			vert_data.push_back(vdata);
			return incidence.size() - 1;
		}
	}
	
	/**
	 * Creates a triangle.
	 *
	 *	tri : The vertex data associated to a triangle.
	 *
	 *	Returns : The name of a triangle.
	 */
	int add_triangle(const Triangle& tri) {
		int n;
		if(dead_tris.size() > 0) {
			n = dead_tris.back();
			dead_tris.pop_back();
			tri_data[n] = tri;
		}
		else {
			n = tri_data.size();
			tri_data.push_back(tri);
		}
		for(int i=0; i<3; ++i) {
			incidence[tri.v[i]].push_back(n);
		}
		return n;
	}
	
	/**
	 * A convenient alias for add_triangle.
	 *
	 * v0, v1, v2 : The names of the vertices of the triangle.
	 *
	 * Returns the name of the triangle
	 */
	int add_triangle(int v0, int v1, int v2) {
		return add_triangle(Triangle(v0, v1, v2));
	}


	/**
	 * Removes a vertex from the mesh.
	 *
	 * After removing a vertex, it is no longer safe to reference its name, though 
	 * it will remain in the list until a vertex either overwrite or a garbage collection
	 * is initiated.
	 *
	 *	n : The name of the vertex to remove
	 */
	void remove_vertex(int n) {
		for(int i = incidence[n].size()-1; i>=0; --i) {
			remove_triangle(incidence[n][i]);
		}
		dead_verts.push_back(n);
	}

	/**
	 * Removes a triangle from the mesh.
	 *
	 * After removing a triangle, it is no longer safe to reference its name, though 
	 * it will remain in the list until either a newly added triangle overwrites it, 
	 * or a garbage collection is initiated.
	 *
	 *	n : The name of the triangle to remove
	 */
	void remove_triangle(int n) {
		for(int i=0; i<3; ++i) {
			IncidenceList& ind = incidence[tri_data[n].v[i]];
			for(int j=0; j<ind.size(); ++j) {
				if(ind[j] == n) {
					ind[j] = std::move(ind[ind.size()-1]);
					ind.resize(ind.size()-1);
					break;
				}
			}
		}
		dead_tris.push_back(n);
	}
	
	
	/**
	 * Garbage collection.
	 *
	 * This method removes all the dead vertices/triangles in the mesh.  
	 *
	 * To improve performance, vertices and triangles are allocated in place.
	 * However, in order to remove a vertex or triangle, it is necessary to 
	 * move some of the existing vertices/triangles in order to ensure optimal usage of the
	 * available space.  This can cause the name/index of a triangle to change,
	 * so this operation is not applicable while some vertices/triangles are in use.
	 * As a result, it is important that this method only be called after all modifications
	 * to a mesh are complete.
	 *
	 * cleanup_orphan_vertices : If this flag is set, any vertices which are not
	 *	attached to a triangle will be removed.  It costs no more than a linear 
	 *  amount of time w/r to the vertices of the mesh.
	 */
	void garbage_collect(bool cleanup_orphan_vertices=false) {	
		if(cleanup_orphan_vertices) {
			for(int i=vert_data.size()-1; i>=0; --i) {
				if(incidence[i].size() == 0) {
					remove_vertex(i);
				}
			}
		}
	
		//Process vertices first
		if(dead_verts.size() > 0) {
			sort(dead_verts.begin(), dead_verts.end());
			int n = vert_data.size()-1;
			for(int i=0, j=dead_verts.size()-1; i < dead_verts.size(); ++i) {
				//Skip duplicates
				if(i > 0) while(i < dead_verts.size() && dead_verts[i-1] == dead_verts[i]) {
					++i;
				}
				if(i == dead_verts.size()) {
					break;
				}
				
				//Skip removals from end of list
				while(j >= i && dead_verts[j] == n) {
					--n;
					--j;
				}
				
				//Get vertex
				int v = dead_verts[i];
				if(n < v) {
					break;
				}
				
				//Relabel vertex n to v
				for(int l=incidence[n].size()-1; l>=0; --l) {
					int t = incidence[n][l];
					for(int k=0; k<3; ++k) {
						if(tri_data[t].v[k] == n) {
							tri_data[t].v[k] = v;
							break;
						}
					}
				}
				
				//Swap positions
				vert_data[v] = std::move(vert_data[n]);
				incidence[v] = std::move(incidence[n]);
				--n;
			}
			
			++n;
			vert_data.resize(n);
			incidence.resize(n);
			dead_verts.clear();
		}
		
		//Garbage collect triangles
		if(dead_tris.size() > 0) {
			sort(dead_tris.begin(), dead_tris.end());
			int n = tri_data.size()-1;
			for(int i=0, j=dead_tris.size()-1; i < dead_tris.size(); ++i) {
				if(i > 0) while(i < dead_tris.size() && dead_tris[i-1] == dead_tris[i]) {
					++i;
				}
				if(i == dead_tris.size()) {
					break;
				}
				
				//Skip removals from end of list
				while(j >= i && dead_tris[j] == n) {
					--n;
					--j;
				}
				
				int t = dead_tris[i];
				if(n < t) {
					break;
				}
				
				tri_data[t] = std::move(tri_data[n]);
				--n;
			}
			tri_data.resize(n+1);
			dead_tris.clear();
		}
	}
	
	/**
	 * Retrieves index/vertex buffers for drawing.
	 *
	 * This is useful for OpenGL/DirectX interoperability.  Note that
	 * garbage_collect should be called before performing this method.
	 */
	void get_buffers(
		const VertexData** vert_buffer,
		int* vert_size,
		const int** index_buffer,
		int* index_size) const {

		*index_buffer = (int*)(void*)(&tri_data[0]);
		*index_size = 3 * tri_data.size(); 
		*vert_buffer = &vert_data[0];
		*vert_size = vert_data.size();
	}
	
protected:
	std::vector<int>			dead_tris;
	std::vector<Triangle>		tri_data;
	
	std::vector<int>			dead_verts;
	std::vector<VertexData>		vert_data;
	std::vector<IncidenceList>	incidence;
};

};

#endif

