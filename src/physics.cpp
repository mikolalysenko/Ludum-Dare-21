#include "btBulletDynamicsCommon.h"
#include "physics.h"

Body::Body(Solid* s, btTransform const& xform, bool dynamic) : solid(s) {

	auto motion_state = new btDefaultMotionState(xform);
	
	btVector3 local_inertia(0,0,0);
	if(dynamic) {
		local_inertia = solid->collision_shape->calculateLocalInertia(solid->mass, local_inertia);
	}
	
	physics = new btRigidBody(
		s->mass,
		&motion,
		s->collision_shape
		local_inertia);
	physics->setContactProcessingThrehsold(BT_LARGE_FLOAT);
	Physics::instance->world->addRigidBody(physics);
		
	if(static_object) {
		physics->setCollisionFlags(staticBody->getCollisionFlags()|btCollisionObject::CF_STATIC_OBJECT);
	}
}


void Physics::init() {
	//Standard boilerplate, just use default values
	auto config = new btDefaultCollisionConfiguration();
	auto dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	auto broadphase = new btDbvtBroadphase();
	auto solver = new btSequentialImpulseConstraintSolver;
	world = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,config);
	
	world->setGravity(btVector3(0,-10,0));
}


void Physics::tick() {
	world->stepSimulation(1.0/30.0);
}
