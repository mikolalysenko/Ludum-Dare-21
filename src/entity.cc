#include <cmath>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <iostream>

#include <GL/glfw.h>

#include "solid.h"
#include "entity.h"
#include "sound.h"
#include "text.h"

using namespace std;
using namespace Eigen;

//Level start-------------------------------------------
LevelStartEntity::~LevelStartEntity() {}

void LevelStartEntity::init() {
	auto player = &puzzle->player;
	player->particle.coordinate = coordinate;
	player->camera_height = camera_height;
	player->camera_distance = camera_distance;
	player->camera_stiffness = camera_stiffness;
	
	player->camera_position = coordinate.position + coordinate.interpolated_normal();
}

void LevelStartEntity::tick(float dt) {}
void LevelStartEntity::draw() {}

//Level exit--------------------------------------
LevelExitEntity::~LevelExitEntity() {}

void LevelExitEntity::init() {
}

void LevelExitEntity::tick(float dt) {
	auto p = &puzzle->player.particle;
	float d = (p->center() - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius+1.0) {
		puzzle->level_complete = true;
	}
}

void LevelExitEntity::draw() {

	float theta = puzzle->elapsed_time * 180;
	float h = puzzle->elapsed_time * M_PI / 4.0;

	auto n = coordinate.interpolated_normal();
	auto p = coordinate.position + n * (1. + sin(h));
	auto r = n.cross(Vector3f(0, 0, 1));
	
	float v = -asin(r.norm());
	r.normalize();

	glPushMatrix();
	glTranslatef(p[0], p[1], p[2]);
	glRotatef(theta, n[0], n[1], n[2]);
	glRotatef(v*180./M_PI, r[0], r[1], r[2]);
	glColor3f(1, 1, 1);
	show_text("ESCAPE", -0.5*text_width("ESCAPE"), -0.02);
	glPopMatrix();
}

//Teleporter--------------------------------------
TeleporterEntity::~TeleporterEntity() {}

void TeleporterEntity::init() {
}

void TeleporterEntity::tick(float dt) {
	auto p = &puzzle->player.particle;
	float d = (p->center() - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius + 0.5) {
		//Update coordinate
		p->coordinate = target_coordinate;
		
		//Update velocity
		auto v_dir = target_coordinate.project_to_tangent_space(p->velocity).normalized();
		p->velocity = p->velocity.norm() * v_dir;
		
		//Update camera position
		auto n = p->coordinate.interpolated_normal();
		puzzle->player.camera_position = p->coordinate.position + n * puzzle->player.camera_height;
		
		//Turn off button press
		puzzle->player.button_pressed = false;
		
		//Special effects
		puzzle->player.shake_camera(1.0, 0.25);
		play_sound_from_group(SOUND_GROUP_TELEPORT);
	}
}

void TeleporterEntity::draw() {
	auto p = coordinate.position;
	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(drand48(), drand48(), drand48());
	glVertex3f(p[0], p[1], p[2]);
	glEnd();
}


//Button--------------------------------------
ButtonEntity::~ButtonEntity() {}

void ButtonEntity::init() {
	pressed = false;
	last_state = false;
	time_left = 0.f;
}

void ButtonEntity::tick(float dt) {

	//Check for player press
	auto p = &puzzle->player.particle;
	float d = (p->coordinate.position - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius) {
	
		switch(type) {
			case BUTTON_PRESS:
				pressed = true;
			break;
			
			case BUTTON_SWITCH:
				if(!last_state) {
					pressed = (pressed ? false : true);
				}
			break;
			
			case BUTTON_TIMED:
				pressed = true;
				time_left = time_limit;
			break;
			
			default: assert(false);
		}
	
		if(!last_state) {
			//TODO: Play a sound effect/special effect here
		}
		last_state = true;	
	}
	else {
		last_state = false;		
		if(type == BUTTON_PRESS) {
			pressed = false;
		}
	}
	
	if(type == BUTTON_TIMED && pressed) {
		time_left -= dt;
		
		if(time_left < 0) {
			time_left = 0;
			pressed = false;
			
			//TODO: Play a sound for the button deactivating here
		}
	}
}

void ButtonEntity::draw() {
	auto p = coordinate.position;
	glPointSize(10);
	glBegin(GL_POINTS);
	if(pressed) {
		glColor3f(0, 1, 0);
	} else {
		glColor3f(1, 0, 0);
	}
	glVertex3f(p[0], p[1], p[2]);
	glEnd();
}

//Obstacle-----------------------------------------
ObstacleEntity::~ObstacleEntity() {}

void ObstacleEntity::init() {
}

void ObstacleEntity::tick(float dt) {
	if((flags & OBSTACLE_NO_COLLIDE) ||
		!active()) {
		return;
	}
	
	if(process_collision(&puzzle->player.particle, dt)) {
		//Kill the player if we are deadly!
		if(flags & OBSTACLE_DEADLY) {
			puzzle->kill_player();
		}
	}
}

void ObstacleEntity::draw() {
	if(!active()) {
		return;
	}
	
	glPushMatrix();
	glMultMatrixf(transform.data());
	model->draw();
	glPopMatrix();
}

bool ObstacleEntity::process_collision(Particle* part, float dt) {
	
	auto tinv = transform.inverse();
	auto npos = tinv * part->center();
	auto grad = (transform.linear() * model->gradient(npos)).normalized();
	
	if((*model)(npos) > -1e-6) {
		part->apply_force(-grad * 100.0);
		
		//FIXME Add a collision sound
		return true;
	} else {	
		auto spos = tinv * (part->coordinate.position + grad * part->radius);	
		if((*model)(spos) > 0) {
			float d = part->velocity.dot(grad);
			if( d > 0 ) {
				part->apply_force(-grad * part->velocity.dot(grad) * 2. / dt);
				//FIXME: Add a collision sound
			}
			return true;
		}
	}
	
	return false;
}

