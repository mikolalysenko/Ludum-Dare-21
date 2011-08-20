#ifndef PUZZLE_H
#define PUZZLE_H

#include <cmath>
#include <cassert>
#include <GL/glfw.h>
#include <Eigen/Core>
#include <mesh/mesh.h>

#define PUZZLE_SIZE		64

struct PuzzleVertex {
	Eigen::Vector3f	position, normal, color;
};

struct CellValue {
	char density;
};

struct Puzzle {
	CellValue data[PUZZLE_SIZE][PUZZLE_SIZE][PUZZLE_SIZE];
	Mesh::TriMesh<PuzzleVertex> mesh;
	GLuint display_list;

	void init();
	void reset();
	
	float operator()(Eigen::Vector3f const& v) const {
		int 	iv[3];
		float	fv[3];
		for(int i=0; i<3; ++i) {
			if(v[i] < 0 || v[i] >= PUZZLE_SIZE - 1)
				return 0.f;
			iv[i] = v[i];
			fv[i] = v[i] - iv[i];
		}
		
		float t = 0.0f;
		for(int ix=0; ix<2; ++ix)
		for(int iy=0; iy<2; ++iy)
		for(int iz=0; iz<2; ++iz) {
			t += data[iv[0]+ix][iv[1]+iy][iv[2]+iz].density * abs((ix-fv[0])*(iy-fv[1])*(iz-fv[2]));
		}
		return t / 128.0;
	}
};

//Reinitializes a puzzle with the given implicit function
template<typename PuzzleFunc_t, typename StyleFunc_t>
void setup_puzzle(Puzzle& puzzle, PuzzleFunc_t& func, StyleFunc_t& func) {
	puzzle.reset();
	for(int i=0; i<PUZZLE_SIZE; ++i)
	for(int j=0; j<PUZZLE_SIZE; ++j)
	for(int k=0; k<PUZZLE_SIZE; ++k) {
		puzzle.data[i][j][k] = func( Eigen::Vector3i(i,j,k) );
	}
	
	//Rebuild puzzle mesh

}

#endif

