#ifndef PLAYER_H
#define PLAYER_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <GL/glfw.h>

#include "surface_coordinate.h"
#include "particle.h"

struct Player {
	//Player model
	Solid* model;

	//Camera parameters
	float camera_stiffness, camera_distance, camera_height;
	Eigen::Vector3f		camera_position, camera_up, target_position;
	float camera_shake_mag, camera_shake_time;
	
	//Mouse state/input
	bool button_pressed;
	Eigen::Vector3f	force_up, force_right;
	Eigen::Vector2f mouse_state[2];
	GLdouble model_matrix[16], projection_matrix[16];
	GLint viewport[4];

	//Physical parameters
	Particle particle;
	
	//Puzzle data
	const struct Puzzle* puzzle;

	//Conversion between window coordinates
	Eigen::Vector3f unproject(Eigen::Vector2f const& window) const;
	Eigen::Vector2f project(Eigen::Vector3f const& position) const;

	//Create player
	Player(Puzzle* p) : puzzle(p) {}

	//Event handlers
	void reset();
	void input();
	void tick(float dt);
	void set_gl_matrix();
	void draw();
	
	//Functions
	void shake_camera(float mag, float t) {
		camera_shake_mag += mag;
		camera_shake_time = 1.0 / (t + 1.0/camera_shake_time);
	}

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#include "puzzle.h"

#endif

