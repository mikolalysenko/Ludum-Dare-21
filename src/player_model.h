#ifndef PLAYER_MODEL_H
#define PLAYER_MODEL_H

#include "solid.h"

struct PlayerModelFunc {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		Cell result;
		result.density = 0.25 - v.dot(v);
		result.friction = 0;
		return result;
	}
};

struct PlayerStyleFunc {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.normal = Vector3f(1, 0, 0);
		
		float t = drand48();
		result.color = Vector3f(t,t*0.2,t*(0.4 * t + (1-t)*0.6));
		return result;
	}
};

#endif

