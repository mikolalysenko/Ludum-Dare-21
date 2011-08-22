#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "particle.h"

struct Projectile {

	Particle particle;
	
	void tick(float dt);
	void draw();
};


#endif

