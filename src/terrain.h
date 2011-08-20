#ifndef CHUNK_H
#define CHUNK_H

#include <Eigen/Core>

namespace App {

struct TerrainVertex {
	Eigen::Vector3f position, color;
};

struct TerrainGenerator {
	float operator()(Eigen::Vector3f const& p);
};

struct TerrainAttribute {
	TerrainAttribute(TerrainGenerator& t) : terrain(t) {}
	TerrainVertex operator()(Eigen::Vector3f const& p);
	
private:
	TerrainGenerator& terrain;
};

};

#endif

