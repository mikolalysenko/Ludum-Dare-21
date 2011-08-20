#ifndef MESH_CONTOUR_H
#define MESH_CONTOUR_H

#include <cassert>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Dense>

#include "mesh/implementation/util.h"
#include "mesh/core/trimesh.h"

namespace Mesh {

/**
 * Computes a mesh estimate for the 0-level set of the given function.
 * Note:  Will evaluate the function f outside the region [lo,hi] in order
 * to acheive correct behaviour at the boundary.
 *
 * 
 *  Mesh is of type TriMesh<VertexData>
 *  DensityFunc is a lambda of type Eigen::Vector3f -> float
 *  AttributeFunc is a lambda of type Eigen::Vector3f -> VertexData
 *
 */
template<
	typename Mesh,
	typename DensityFunc,
	typename AttributeFunc,
	typename Vector>
void isocontour(
	Mesh& mesh,
	DensityFunc& f,
	AttributeFunc& attr,
	Vector lo,
	Vector hi,
	Eigen::Vector3i res) {
	
	//Edge intersections
	typename impl::SpatialGrid<Eigen::Vector4f>::type edges[3];
	
	//Mesh vertices
	typename impl::SpatialGrid<int>::type vertices;	
	
	//Grid size
	auto h = (hi - lo).array() / Vector(res[0], res[1], res[2]).array();
	lo -= h.matrix();
	for(int i=0; i<3; ++i)
		res[i] += 2;
	
	
	//Find edge intersections
	{
		//Initialize edge bounds
		const size_t sz = (res[0] + 1) * (res[1] + 1);
	
		float* above = new float[sz];
		float* below = new float[sz];
		impl::ScopedArray<float> aguard(above), bguard(below);
		
		int idx = 0;
		for(int x=0; x<=res[0]; ++x)
		for(int y=0; y<=res[1]; ++y) {
			below[idx++] = f((Vector(x,y,0).array() * h + lo.array()).matrix());
		}
		
		for(int z=0; z<res[2]; ++z) {
			//Initialize array
			idx = 0;
			for(int x=0; x<res[0]; ++x) {
				for(int y=0; y<res[1]; ++y) {
					const Eigen::Vector3i coord(x, y, z);
			
					//Evaluate necessary function values
					const Vector p = (Eigen::Array3f(x,y,z) * h + lo.array()).matrix();
					float z_f = above[idx] = f(p + Eigen::Vector3f(0, 0, h[2]));
					const float c_f = below[idx];
					++idx;
					const Vector e_f(below[idx+res[1]], below[idx], z_f);
			
					//Compute edge intersection
					for(int e=0; e<3; ++e) {
			
						//Do edge intersection test
						if(c_f < -FP_TOLERANCE) {
							if(e_f[e] < -FP_TOLERANCE) {
								continue;
							}
						}
						else if(c_f > FP_TOLERANCE) {
							if(e_f[e] > FP_TOLERANCE) {
								continue;
							}
						}
						else {
							if(abs(e_f[e]) < FP_TOLERANCE) {
								continue;
							}
						}
				
						//Find intercept
						Eigen::Vector3f e_p(p);
						e_p[e] += h[e];
						const float t = c_f / (c_f - e_f[e]);
						const Eigen::Vector3f intercept = (1.-t)*p + t*e_p;
						edges[e][coord] = Eigen::Vector4f(
							intercept[0], intercept[1], intercept[2], (c_f < e_f[e]) ? 1 : -1);
		
						//Mark neighboring vertices
						for(int k=0; k<3; ++k) {
							if(coord[k] == res[k]-1)
								continue;
						}
						
						const int u_dir = (e+1)%3;
						const int v_dir = (e+2)%3;
						for(int u=0; u<=1; ++u) {
							if(coord[u_dir]-u < 0)
								continue;
							for(int v=0; v<=1; ++v) {
								if(coord[v_dir]-v < 0)
									continue;
								Eigen::Vector3i tmp(coord);
								tmp[u_dir] -= u;
								tmp[v_dir] -= v;
								vertices[tmp] = -1;
							}
						}
					}
				}
				above[idx++] = f((Eigen::Array3f(x,res[1],z+1) * h + lo.array()).matrix());				
			}
			
			if(z < res[2]) {
				//Fill in the x-row
				Eigen::Vector3f p = (Eigen::Array3f(res[0],0,z+1) * h + lo.array()).matrix();
				for(int y=0; y<res[1]; ++y, p[1] += h[1]) {
					above[idx++] = f(p);
				}
						
				//Swap arrays
				float* tmp = above;
				above = below;
				below = tmp;
			}
		}
	}
		
	
	//Compute vertices
	for(auto iter=vertices.begin(); iter!=vertices.end(); ++iter) {
		int n = 0;
		Eigen::Vector4f center(0, 0, 0, 0);
		
		//Read in all the planes
		for(int e=0; e<3; ++e) {
			const int u_dir = (e + 1)%3;
			const int v_dir = (e + 2)%3;

			for(int u=0; u<=1; ++u)
			for(int v=0; v<=1; ++v) {
			
				Eigen::Vector3i tmp(iter->first);
				tmp[u_dir] += u;
				tmp[v_dir] += v;
				
				auto e_iter = edges[e].find(tmp);
				if(e_iter == edges[e].end())
					continue;
					
				center += e_iter->second;
				++n;
			}
		}
		
		center /= (float)n;
		iter->second = mesh.add_vertex(attr(Eigen::Vector3f(center[0], center[1], center[2])));
	}
	
	//Generate faces
	for(int e=0; e<3; ++e) {
		const int u_dir = (e+1) % 3;
		const int v_dir = (e+2) % 3;
		
		for(auto iter=edges[e].begin(); iter!=edges[e].end(); ++iter) {
			auto coord = iter->first;
			int vert[4], n=0;
			
			if(	coord[u_dir] <= 0 || coord[v_dir] <= 0 ||
				coord[u_dir] >= res[u_dir]-1 || 
				coord[v_dir] >= res[v_dir]-1 || 
				coord[e] >= res[e] - 2)
				continue;
			
			for(int u=0; u<=1; ++u)
			for(int v=0; v<=1; ++v) {
				Eigen::Vector3i tmp(coord);
				tmp[u_dir] -= u;
				tmp[v_dir] -= v;				
				vert[n++] = vertices[tmp];
			}		
		
			if(iter->second[3] < 0) {
				mesh.add_triangle(vert[0], vert[1], vert[2]);
				mesh.add_triangle(vert[2], vert[1], vert[3]);
			}
			else {
				mesh.add_triangle(vert[0], vert[2], vert[1]);
				mesh.add_triangle(vert[1], vert[2], vert[3]);		
			}
		}
	}
}

};

#endif

