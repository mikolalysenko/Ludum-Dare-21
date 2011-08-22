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


