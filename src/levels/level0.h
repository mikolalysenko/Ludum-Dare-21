#ifndef LEVELS_H
#define LEVELS_H

#include <cmath>

#include "solid.h"
#include "surface_coordinate.h"
#include "puzzle.h"

namespace Level0 {

struct Level0Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = 50.0 - v.dot(v);
		result.friction = 1.0/4.0;
		return result;
	}
};

struct Level0Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.normal = Vector3f(1, 0, 0);
		result.color = Vector3f(0.2,0.2,0.2)*drand48();
		return result;
	}
};

//Builds level 0
struct Level0 {
	void operator()(Puzzle& puzzle) const {
		using namespace Eigen;
		
		//Create geometry
		auto level = new Solid(
			Vector3i( 64,  64,  64),
			Vector3f(-10, -10, -10),
			Vector3f( 10,  10,  10));
		Level0Solid	level_func;
		Level0Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle.add_solid(level);
		
		//Create start location
		auto tri = level->mesh.triangle(0);
		puzzle.add_entity(new StartEntity(
			IntrinsicCoordinate(
				0,
				level->mesh.vertex(tri.v[0]).position,
				level)) );
	}
};

};

#endif
