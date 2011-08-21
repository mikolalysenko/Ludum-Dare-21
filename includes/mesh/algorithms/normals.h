#ifndef MESH_NORMALS_H
#define MESH_NORMALS_H

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
void estimate_normals(Mesh_t& mesh) {
	using namespace Eigen;

	PositionAttribute< typename Mesh_t::VertexData > pos_attr;
	NormalAttribute< typename Mesh_t::VertexData > normal_attr;

	//Perform an initial garbage collection
	mesh.garbage_collect();
	
	//Compute mesh normals
	for(int i=mesh.vertices().size()-1; i>=0; --i) {
		auto incident_tris = mesh.vertex_incidence(i);
		Vector3f average_normal(0,0,0);
		for(int j=incident_tris.size()-1; j>=0; --j) {
			auto tri = mesh.triangle(incident_tris[j]);
			Vector3f p[3];			
			int v = tri.index_of(i);
			for(int k=0; k<3; ++k) {
				p[k] = pos_attr.get(mesh.vertex(tri.v[(v+k)%3]));
			}
			average_normal +=
				(p[2] - p[0]).normalized().cross((p[1] - p[0]).normalized());
		}
		normal_attr.set(mesh.vertex(i), average_normal.normalized());
	}
}

};

#endif


