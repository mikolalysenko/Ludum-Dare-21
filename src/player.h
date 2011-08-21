#ifndef PLAYER_H
#define PLAYER_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <GL/glfw.h>

#include "surface_coordinate.h"
#include "particle.h"

struct Player {
	//Camera parameters
	float camera_stiffness, camera_distance, camera_height;
	Eigen::Vector3f		camera_position, camera_up, target_position;
	
	//Mouse state/input
	bool button_pressed;
	Eigen::Vector3f	force_up, force_right;
	Eigen::Vector2f mouse_state[2];
	GLdouble model_matrix[16], projection_matrix[16];
	GLint viewport[4];

	//Physical parameters
	Particle particle;
	
	//Puzzle data
	struct Puzzle* puzzle;

	//Conversion between window coordinates
	Eigen::Vector3f unproject(Eigen::Vector2f const& window) const;
	Eigen::Vector2f project(Eigen::Vector3f const& position) const;

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

