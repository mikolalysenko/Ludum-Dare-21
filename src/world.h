#ifndef WORLD_H
#define WORLD_H

#include <cmath>
#include <cassert>
#include <Eigen/Core>
#include <mesh/mesh.h>
#include "terrain.h"

#define MAP_SIZE		256
#define NUM_CHUNKS		16

struct CellValue {
	char density;
};

struct Chunk {
	bool dirty;
	TriMesh<TerrainVertex>	mesh;
	GLuint	display_list;
};

struct World {
	CellValue	terrain[MAP_SIZE][MAP_SIZE][MAP_SIZE];
	Chunk		chunks[NUM_CHUNKS][NUM_CHUNKS][NUM_CHUNKS];

	World();
	
	float operator()(Eigen::Vector3f const& v) const {
		int 	iv[3];
		float	fv[3];
		for(int i=0; i<3; ++i) {
			if(v[i] < 0 || v[i] >= MAP_SIZE - 1)
				return 0.f;
			iv[i] = v[i];
			fv[i] = v[i] - iv[i];
		}
		
		float t = 0.0f;
		for(int ix=0; ix<2; ++ix)
		for(int iy=0; iy<2; ++iy)
		for(int iz=0; iz<2; ++iz) {
			t += terrain[iv[0]+ix][iv[1]+iy][iv[2]+iz].density * abs((ix-fv[0])*(iy-fv[1])*(iz-fv[2]));
		}
		return t / 128.0;
	}
	
	CellValue& cell(Eigen::Vector3i const& v) {
		assert(v[0] >= 0 && v[1] >=0 && v[2] >= 0 &&
			v[0] < MAP_SIZE && v[1] < MAP_SIZE && v[2] < MAP_SIZE);
		return terrain[v[0]][v[1][v[2]];
	}

	
	
};

#endif

