#ifndef SURFACE_COORDINATE_H
#define SURFACE_COORDINATE_H

#include <cassert>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <array>
#include <vector>

#include <Eigen/Core>
#include "mesh/mesh.h"
#include "solid.h"

template<typename t> t clamp(t const& a, t const& b, t const& c) {
	if(a < b) { return b; }
	if(a > c) { return c; }
	return a;
}

//A surface coordinate!
struct IntrinsicCoordinate {
	int triangle_index;
	Eigen::Vector3f	position;
	Mesh::TriMesh<Vertex>* mesh;
	
	//Boiler plate constructors
	IntrinsicCoordinate() : 
		triangle_index(0), 
		position(0,0,0), 
		mesh(NULL) {}
	IntrinsicCoordinate(IntrinsicCoordinate const& c) : 
		triangle_index(c.triangle_index),
		position(c.position),
		mesh(c.mesh) {}
	IntrinsicCoordinate(int t, Eigen::Vector3f const& p, Mesh::TriMesh<Vertex>* m) :
		triangle_index(t),
		position(p),
		mesh(m) {}
	IntrinsicCoordinate& operator=(IntrinsicCoordinate const& c) {
		triangle_index = c.triangle_index;
		position = c.position;
		mesh = c.mesh;
		return *this;
	}		
	
	//Recovers local triangle vertices
	std::array<Eigen::Vector3f, 3> triangle_vertices() const {
		auto tri = mesh->triangle(triangle_index);	
		std::array<Eigen::Vector3f,3> v;
		for(int i=0; i<3; ++i) {
			v[i] = mesh->vertex(tri.v[i]).position;
		}
		return v;
	}
	
	//Project the vector v to the tangent space at this point
	Eigen::Vector3f project_to_tangent_space(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		auto tri = mesh->triangle(triangle_index);
		auto verts = triangle_vertices();
		Vector3f du = verts[1] - verts[0],
				 dv = verts[2] - verts[0];			
		Vector3f n = du.cross(dv).normalized();
		return v - n * n.dot(v);
	}
	
	//Recovers a barycentric coordinate
	static Eigen::Vector3f barycentric(
		Eigen::Vector3f const& p,
		Eigen::Vector3f const& n,
		Eigen::Vector3f const& du,
		Eigen::Vector3f const& dv,
		Eigen::Vector3f const& base) {
		using namespace Eigen;
		Vector3f p0 = p - base, mu;
		p0 -= n * n.dot(p0);
		float dot00 = du.dot(du),
			  dot01 = du.dot(dv),
			  dot02 = du.dot(p0),
			  dot11 = dv.dot(dv),
			  dot12 = dv.dot(p0);
		float invDenom = 1.f / (dot00 * dot11 - dot01 * dot01);
		mu[2] = (dot11 * dot02 - dot01 * dot12) * invDenom;
		mu[1] = (dot00 * dot12 - dot01 * dot02) * invDenom;
		mu[0] = 1.f - mu[2] - mu[1];
		return mu;
	}
	
	static void clamp_barycentric(Eigen::Vector3f& b) {
		for(int i=0; i<3; ++i) {
			if(b[i] < 0) {
				float x = b[(i+1)%3],
					  y = b[(i+2)%3];
				float s = (x + y);
				b[i]       = 0.f;
				b[(i+1)%3] = x / s;
				b[(i+2)%3] = y / s;
			}
		}
	}
	
	//Advects a point along the surface, while parallel transporting v
	Eigen::Vector3f advect(Eigen::Vector3f const& v) {
		using namespace Eigen;
		using namespace std;
		
		//Set up initial vectors
		Vector3f v_dir = project_to_tangent_space(v);
		float    v_mag = v_dir.norm();
		v_dir /= v_mag;
		const float i_mag = v_mag;
		
		/*
		cout << "Start advect:" << endl
			 << "v = " << v << endl
		     << "v_dir = " << v_dir << endl
		     << "v_mag = " << v_mag << endl;
		*/
		
		while(v_mag > 1e-8) {
			//Calculate tangent space
			auto tri = mesh->triangle(triangle_index);
			auto verts = triangle_vertices();
			Vector3f du = verts[2] - verts[0],
					 dv = verts[1] - verts[0];			
			Vector3f n = du.cross(dv).normalized();
			
			/*
			cout << "Advecting: p = " << position << endl;
			for(int i=0; i<3; ++i) {
				cout << "v[" << i << "]=" << verts[i] << endl;
			}
			cout << "Triangle index = " << triangle_index << endl
				 << "Vertices = ";
			for(int i=0; i<3; ++i) {
				cout << tri.v[i] << ',';
			}
			cout << endl;
			*/
			
			//Transport velocity
			Vector3f residual_velocity = v_dir * v_mag;
			v_dir = (residual_velocity - n * n.dot(residual_velocity)).normalized();
			residual_velocity = v_dir * v_mag;
			
			/*
			cout << "Velocity direction = " << v_dir << endl 
				 << "Velocity magnitude = " << v_mag << endl
				 << "Velocity           = " << residual_velocity << endl;
			*/
			
			//Compute advected position in barycentric coordinates
			Vector3f mu = barycentric(position + residual_velocity, n, du, dv, verts[0]),
					 nu = barycentric(position, n, du, dv, verts[0]);
			clamp_barycentric(nu);
			
			/*
			cout << "mu = " << mu << endl
				 << "nu = " << nu << endl;
			*/
			
			//Find intersection with edge of triangle
			Vector3f db = mu - nu;
			
			//cout << "db = " << db << endl;
			float t = 1.f;
			int last_edge = -1;
			for(int i=0; i<3; ++i) {
				int e = i;
				if(abs(nu[i]) <= 1e-6) {
					nu[i] += 1e-6;
				}
				if(abs(db[e]) > 1e-6 && nu[e] + t * db[e] < -1e-8) {
					last_edge = e;
					t = -nu[e] / db[e];
					//cout << "Hit an edge: " << e << endl;
				}
			}
			
			assert(-1e8 <= t && t <= 1.f+1e8);
			
			//Update position and residual velocity
			Vector3f b = nu + db * t;
			clamp_barycentric(b);
			auto nposition = verts[0] + b[2]*du + b[1]*dv;
			float dposition = (nposition - position).norm();
			v_mag = max(0.f, v_mag - dposition);
			position = nposition;
			
			/*
			cout << "t = " << t << endl;
			cout << "dposition = " << dposition << endl;
			cout << "last_edge = " << last_edge << endl;
			cout << "Barycentric intersection = " << b << endl;
			*/
			
			//If we are still in the triangle, then we are done!
			if(last_edge == -1) {
				break;
			}
			
			//Otherwise, find next triangle, and continue advecting
			int e[3];
			for(int i=0; i<3; ++i) {
				e[i] = tri.v[(last_edge+i)%3];
			}
			auto ntris = mesh->vertex_incidence(e[1]);
			for(int i=0; i<ntris.size(); ++i) {
				auto ntri = mesh->triangle(ntris[i]);
				if(ntri.index_of(e[2]) >= 0 && ntri.index_of(e[0]) < 0) {
					triangle_index = ntris[i];
					break;
				}
			}
		}
		
		//cout << "Done" << endl;
		
		return v_dir * i_mag;
	}
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif
