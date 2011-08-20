#ifndef MESH_CONNECTED_COMPONENTS_H
#define MESH_CONNECTED_COMPONENTS_H

#include <vector>
#include <cstring>

#include "mesh/implementation/util.h"
#include "mesh/core/trimesh.h"

namespace Mesh {

/**
 * Splits a mesh into its connected components.
 */
template<typename Mesh_t>
std::vector<Mesh_t> connected_components(Mesh_t const& mesh) {
	//Result array
	std::vector<Mesh_t> result;
		
	//Allocate visited arrays
	int nv = mesh.vertices().size();
	int nt = mesh.triangles().size();
	int* visited_v = (int*)malloc(nv * sizeof(int));
	int* visited_t = (int*)malloc(nt * sizeof(int));
	impl::ScopedFree tguard(visited_v), vguard(visited_t);
	memset(visited_v, -1, nv*sizeof(int));
	memset(visited_t, -1, nt*sizeof(int));

	//To-visit stack
	std::vector<int> to_visit;
	
	for(int i=0; i<nv; ++i) {
		if(visited_v[i] >= 0) {
			continue;
		}
		
		Mesh_t m;
		to_visit.push_back(i);
		visited_v[i] = m.add_vertex(mesh.vertex(i));
		while(to_visit.size() > 0) {

			//Pop vertex
			int v = to_visit.back();
			to_visit.pop_back();
			
			//Visit neighboring vertices
			auto incidence = mesh.vertex_incidence(v);
			for(int j=incidence.size()-1; j>=0; --j) {
				int t = incidence[j];
				if(visited_t[t] >= 0) {
					continue;
				}
				
				//Add neighbors
				auto tri = mesh.triangle(t);
				int n[3];
				for(int k=0; k<3; ++k) {
					n[k] = visited_v[tri.v[k]];
					if(n[k] < 0) {
						int u = tri.v[k];
						to_visit.push_back(u);
						n[k] = visited_v[u] = m.add_vertex(mesh.vertex(u));
					}
				}
				visited_t[t] = m.add_triangle(n[0], n[1], n[2], mesh.triangle_data(t));
			}
		}
		
		result.push_back(m);
	}
	
	return result;
}

};

#endif

