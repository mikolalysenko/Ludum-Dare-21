#ifndef PLAYER_H
#define PLAYER_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "surface_coordinate.h"
#include "particle.h"

struct Player {

	//Camera parameters
	Eigen::Quaternionf	camera_rotation;
	Eigen::Vector3f		camera_position, camera_target;
	
	//Mouse state/input
	bool button_pressed;
	Eigen::Vector3f		mouse_state[2];
	Eigen::Quaternionf	drotation;

	//Physical parameters
	Particle particle;

	//Constructor
	Player();
	
	//Event handlers
	void input();
	void tick(float dt);
	void set_gl_matrix();

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif
