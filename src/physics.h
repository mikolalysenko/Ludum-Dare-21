#ifndef PHYSICS_H
#define PHYSICS_H

#include <Eigen/Core>
#include "solid.h"
#include "btBulletDynamicsCommon.h"

struct Body {
	Solid* solid;
	btRigidBody* physics;
	
	Body(Solid* solid, btTransform const& xform, bool dynamic=true);
};

struct Physics {
	btDiscreteDynamicsWorld* world;
	
	static Physics instance;
	
	void init();
	void tick();
};

#endif

