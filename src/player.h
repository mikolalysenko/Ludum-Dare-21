#ifndef PLAYER_H
#define PLAYER_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "surface_coordinate.h"
#include "particle.h"

struct Player {
	//The player force rotation
	Eigen::Quaternionf	force_rotation;

	//Camera parameters
	Eigen::Vector3f		camera_position, camera_up, target_position;
	
	//Mouse state/input
	bool button_pressed;
	Eigen::Vector3f		mouse_state[2];
	Eigen::Quaternionf	drotation;

	//Physical parameters
	Particle particle;
	
	//Puzzle data
	struct Puzzle* puzzle;

	//Constructor
	Player();
	
	//Event handlers
	void reset();
	void input();
	void tick(float dt);
	void set_gl_matrix();
	void draw();

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#include "puzzle.h"

#endif

