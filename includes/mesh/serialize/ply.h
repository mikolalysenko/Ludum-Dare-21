#ifndef MESH_SERIALIZE_PLY_H
#define MESH_SERIALIZE_PLY_H

#include <cstdio>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "mesh/core/trimesh.h"

namespace Mesh {

template<typename VertexFormat>
void ply_ascii_serialize(
	FILE* fout,
	TriMesh<VertexFormat> const& mesh) {

	auto verts = mesh.verts();
	auto tris = mesh.tris();

	fprintf(fout, 
		"ply\n"
		"format ascii 1.0\n"
		"element vertex %d\n"
		"property float x\n"
		"property float y\n"
		"property float z\n"
		"element face %d\n"
		"property list uchar int vertex_index\n"
		"end header\n",
		(int)verts.size(),
		(int)tris.size());
	
	for(int i=0; i<verts.size(); ++i) {
		fprintf(fout, "%f %f %f\n", verts[i][0], verts[i][1], verts[i][2]);	
	}
	
	for(int i=0; i<tris.size(); ++i) {
		fprintf(fout, "%u %u %u\n", tris[i].v[0], tris[i].v[1], tris[i].v[2]);
	}
}
	
//TODO: Add more file serialization stuff here	

};

#endif

