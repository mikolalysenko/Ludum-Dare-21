#include <Eigen/Core>
#include "terrain.h"
#include "noise.h"


using namespace Eigen;

namespace App {

float TerrainGenerator::operator()(Vector3f const& p) {
	return (p[1]-128.0)+ 90*simplexNoise3D(0.01*p[0], 0.01*p[1], 0.01*p[2], 3);
}

TerrainVertex TerrainAttribute::operator()(Vector3f const& p) {
	TerrainVertex result;
	result.position = p;
	
	float c0 = terrain(p);
	result.normal = Vector3f(
		c0 - terrain(p + Vector3f(0.001, 0, 0)),
		c0 - terrain(p + Vector3f(0, 0.001, 0)),
		c0 - terrain(p + Vector3f(0, 0, 0.001)));
	result.normal.normalize();
	
	result.color = result.normal;
	
	return result;	
}


};
