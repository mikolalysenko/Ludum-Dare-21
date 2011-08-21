#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <Eigen/Core>
#include "solid.h"
#include "surface_coordinate.h"

struct Particle {
	IntrinsicCoordinate coordinate;
	Eigen::Vector3f velocity, forces, color;
	float mass;
	
	Particle(){}
	Particle(Particle const& p) :
		coordinate(p.coordinate),
		velocity(p.velocity),
		forces(p.forces),
		color(p.color),
		mass(p.mass) {}
	Particle(
		IntrinsicCoordinate const& coord,
		Eigen::Vector3f const& v,
		Eigen::Vector3f const& c,
		float m) :
		coordinate(coord),
		velocity(v),
		forces(0,0,0),
		color(c),
		mass(m) {}
	Particle& operator=(Particle const& p) {
		coordinate = p.coordinate;
		velocity = p.velocity;
		forces = p.forces;
		color = p.color;
		mass = p.mass;
		return *this;
	}
	
	void apply_force(Eigen::Vector3f f) {
		forces += f;
	}
	
	void integrate(float delta_t) {
		using namespace Eigen;
		
		//Integrate velocity
		velocity += forces * (delta_t / mass);
		forces = Vector3f(0,0,0);
		velocity = coordinate.project_to_tangent_space(velocity);
		if(coordinate.solid) {
			velocity *= coordinate.friction();
		}
		
		//Integrate position
		float mag = velocity.norm();
		velocity = coordinate.advect(velocity * delta_t).normalized() * mag;
	}
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif

