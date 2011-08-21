#ifndef LEVELS_H
#define LEVELS_H

#include <cmath>
#include "solid.h"

struct Level0 {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = -(v[0] * v[1] * v[2]);
		result.friction = 1.;
		return result;
	}
};

struct Level0Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.normal = Vector3f(1, 0, 0);
		result.color = v;
		return result;
	}
};



#endif

