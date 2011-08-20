#ifndef MESH_REPAIR_H
#define MESH_REPAIR_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <Eigen/Core>

#include "mesh/implementation/util.h"
#include "mesh/core/attributes.h"
#include "mesh/core/trimesh.h"

namespace Mesh {

template<typename Mesh_t>
void repair_mesh_vertices(
	Mesh_t& mesh, 
	float tolerance = FP_TOLERANCE) {

	PositionAttribute< typename Mesh_t::VertexData > pos_attr;
	typename impl::SpatialGrid<int>::type vertex_hash;

	//Perform an initial garbage collection
	mesh.garbage_collect();
	
	for(int i=mesh.vertices().size()-1; i>=0; --i) {
		auto pos = pos_attr.get(mesh.vertex(i));		
		auto fpos = pos * (2.0 / tolerance);
		const Eigen::Vector3i ipos(fpos[0], fpos[1], fpos[2]);
		
		//Get neighbors
		std::vector<int> overlaps;
		for(int x=-1; x<=1; ++x)
		for(int y=-1; y<=1; ++y)
		for(int z=-1; z<=1; ++z) {
			auto iter = vertex_hash.find( Eigen::Vector3i(ipos[0]+x, ipos[1]+y, ipos[2]+z) );
			if(iter != vertex_hash.end() && iter->second > 0)
				overlaps.push_back(iter->second);
		}
				
		//Fuse overlapping vertices
		for(int j=overlaps.size()-1; j>=0; --j) {
			//Check for vertex overlap
			const int v = overlaps[j];
			auto vpos = pos_attr.get(mesh.vertex(v));
			if( (pos - vpos).norm() > tolerance ) {
				continue;
			}
			
			//Fuse overlapping triangles
			auto tris = mesh.vertex_incidence(v);
			for(int k=tris.size()-1; k>=0; --k) {
				int t = tris[k];
				auto tri = mesh.triangle(t);
				mesh.remove_triangle(t);
				if(tri.index_of(i) != -1) {
					continue;
				}
				tri.v[tri.index_of(v)] = i;
				mesh.add_triangle(tri);
			}
			
			//Remove redundant vertex
			mesh.remove_vertex(v);
			auto vfpos = vpos * (2.0 / tolerance);
			vertex_hash[ Eigen::Vector3i(vfpos[0], vfpos[1], vfpos[2]) ] = -1;
		}
		
		//Update hash
		vertex_hash[ipos] = i;
	}
	
	mesh.garbage_collect();
}

};

#endif


