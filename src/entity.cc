#include <cassert>
#include <cstdlib>
#include <vector>
#include <iostream>

#include <GL/glfw.h>

#include "solid.h"
#include "entity.h"

using namespace std;
using namespace Eigen;

LevelExitEntity::~LevelExitEntity() {}

void LevelExitEntity::init() {
}

void LevelExitEntity::tick(float dt) {
	auto p = &puzzle->player.particle;
	float d = (p->coordinate.position - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius) {
		puzzle->level_complete = true;
	}
}

void LevelExitEntity::draw() {
	auto p = coordinate.position;
	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(1, 1, 1);
	glVertex3f(p[0], p[1], p[2]);
	glEnd();
}

TeleporterEntity::~TeleporterEntity() {}

void TeleporterEntity::init() {
}

void TeleporterEntity::tick(float dt) {
	auto p = &puzzle->player.particle;
	float d = (p->coordinate.position - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius) {
		p->coordinate = target_coordinate;
		puzzle->player.shake_camera(1.0, 0.25);
		
		puzzle->player.button_pressed = false;		
		auto v_dir = target_coordinate.project_to_tangent_space(p->velocity).normalized();
		p->velocity = p->velocity.norm() * v_dir;
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



