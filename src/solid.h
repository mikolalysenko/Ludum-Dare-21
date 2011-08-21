#ifndef SOLID_H
#define SOLID_H

#include <array>
#include <vector>
#include <iostream>
#include <cmath>
#include <cassert>
#include <GL/glfw.h>
#include <Eigen/Core>
#include <mesh/mesh.h>

typedef Eigen::Transform<float, 3, Eigen::Affine> Transform3f;

struct Vertex {
	Eigen::Vector3f	position, normal, color;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct Cell {
	float density, friction;
	
	//Friction ~ 1/number of seconds before stopping
};

struct Solid {
	const Eigen::Array3f scale;
	const Eigen::Vector3i resolution;
	const Eigen::Vector3f lower_bound, upper_bound;
	std::vector<Cell> data;
	Mesh::TriMesh<Vertex> mesh;
	GLuint display_list;
	float mass;

	Solid(
		Eigen::Vector3i const& res,
		Eigen::Vector3f const& lo,
		Eigen::Vector3f const& hi ) :
		resolution(res),
		lower_bound(lo),
		upper_bound(hi),
		data(res[0]*res[1]*res[2]),
		scale(Eigen::Array3f(res[0], res[1], res[2]) / (hi - lo).array()) {}

	void setup_data();
	void draw();
	
	bool coordinate_parts(
		Eigen::Vector3f v,
		Eigen::Vector3i& iv,
		Eigen::Vector3f& fv) const {
		v = ((v - lower_bound).array() * scale).matrix();
		for(int i=0; i<3; ++i) {
			if(v[i] < 0 || v[i] >= resolution[i] - 1)
				return false;
			iv[i] = v[i];
			fv[i] = v[i] - iv[i];
		}
		return true;
	}
	
	float operator()(Eigen::Vector3f const&v) const {
		using namespace Eigen;
		Vector3i iv;
		Vector3f fv;
		if(!coordinate_parts(v, iv, fv))
			return -1000.f;
		
		float t = 0.0f;
		for(int ix=0; ix<2; ++ix)
		for(int iy=0; iy<2; ++iy)
		for(int iz=0; iz<2; ++iz) {
			float w = fabsf((1.f-ix-fv[0])*(1.f-iy-fv[1])*(1.f-iz-fv[2]));
			t += cell(iv[0]+ix, iv[1]+iy, iv[2]+iz).density * w;
		}
		
		return t;
	}
	
	float friction(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		Vector3i iv;
		Vector3f fv;
		if(!coordinate_parts(v, iv, fv))
			return 0.f;
				
		float t = 0.0f;
		for(int ix=0; ix<2; ++ix)
		for(int iy=0; iy<2; ++iy)
		for(int iz=0; iz<2; ++iz) {
			float w = fabsf((1.f-ix-fv[0])*(1.f-iy-fv[1])*(1.f-iz-fv[2]));
			t += cell(iv[0]+ix, iv[1]+iy, iv[2]+iz).friction * w;
		}
		return t;
	}
	
	Eigen::Vector3f gradient(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		Vector3i iv;
		Vector3f fv;
		if(!coordinate_parts(v, iv, fv))
			return Vector3f(0, 0, 0);
				
		Vector3f r(0,0,0);
		for(int ix=0; ix<2; ++ix)
		for(int iy=0; iy<2; ++iy)
		for(int iz=0; iz<2; ++iz) {
			Vector3f w(
				(ix?1:-1)*fabsf((1.f-iy-fv[1])*(1.f-iz-fv[2])),
				(iy?1:-1)*fabsf((1.f-ix-fv[0])*(1.f-iz-fv[2])),
				(iz?1:-1)*fabsf((1.f-iy-fv[1])*(1.f-ix-fv[0])));
			r += cell(iv[0]+ix, iv[1]+iy, iv[2]+iz).density * w;
		}
		return r;
	}

	Cell& cell(int i, int j, int k) {
		return data[i + resolution[0]*(j + resolution[1]*k)];
	}
	const Cell& cell(int i, int j, int k) const {
		return data[i + resolution[0]*(j + resolution[1]*k)];
	}

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

//Reinitializes a puzzle with the given implicit function
template<typename ImplicitFunc_t, typename StyleFunc_t>
void setup_solid(Solid& solid, ImplicitFunc_t& func, StyleFunc_t& style_func) {
	using namespace Eigen;
	
	//Fill in data
	Cell* ptr = &solid.data[0];
	Eigen::Array3f step = (solid.upper_bound - solid.lower_bound).array() / (solid.resolution.array()).cast<float>();
	for(float z=solid.lower_bound[2]; z<solid.upper_bound[2]; z+=step[2])
	for(float y=solid.lower_bound[1]; y<solid.upper_bound[1]; y+=step[1])
	for(float x=solid.lower_bound[0]; x<solid.upper_bound[0]; x+=step[0]) {
		*(ptr++) = func( Vector3f(x, y, z) );
	}
	
	//Rebuild mesh
	Mesh::isocontour(
		solid.mesh,
		solid,
		style_func,
		solid.lower_bound,
		solid.upper_bound,
		solid.resolution );
		
	//Generate display/collision stuff
	solid.setup_data();
}

#endif

