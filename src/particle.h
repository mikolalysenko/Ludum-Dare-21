#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "solid.h"
#include "surface_coordinate.h"

struct Particle {
	IntrinsicCoordinate coordinate;
	Eigen::Vector3f velocity, forces;
	Eigen::Quaternionf rotation;
	float mass, radius;
	
	Particle(){}
	Particle(Particle const& p) :
		coordinate(p.coordinate),
		velocity(p.velocity),
		forces(p.forces),
		rotation(p.rotation),
		mass(p.mass),
		radius(p.radius) {}
	Particle(
		IntrinsicCoordinate const& coord,
		Eigen::Vector3f const& v,
		Eigen::Quaternionf const& rot,
		float m,
		float r) :
		coordinate(coord),
		velocity(v),
		forces(0,0,0),
		rotation(rot),
		mass(m),
		radius(r) {}
	Particle& operator=(Particle const& p) {
		coordinate = p.coordinate;
		velocity = p.velocity;
		forces = p.forces;
		rotation = p.rotation;
		mass = p.mass;
		radius = p.radius;
		return *this;
	}
	
	void apply_force(Eigen::Vector3f f) {
		forces += f;
	}
	
	void integrate(float delta_t) {
		using namespace Eigen;
		using namespace std;
		
		//Integrate velocity
		velocity += forces * (delta_t / mass);
		forces = Vector3f(0,0,0);
		velocity = coordinate.project_to_tangent_space(velocity);
		if(coordinate.solid) {
			velocity *= exp(-delta_t * coordinate.friction());
		}
		float mag = velocity.norm();
		
		if(mag > 1e-8) {
			//Integrate rotation
			Vector3f normal = coordinate.interpolated_normal();
			AngleAxisf rot(mag*delta_t/radius, normal.cross(velocity).normalized());
			rotation = rot * rotation;

			//Integrate position
			velocity = coordinate.advect(velocity * delta_t);
			auto m = velocity.norm();
			if( m > 1e-8 ) {
				velocity *= mag / m;
			}
		}
	}
	
	Eigen::Matrix3f frame() const {
		using namespace Eigen;
		Vector3f du, dv, n;
		coordinate.tangent_space(du, dv, n);
		
		Vector3f tangent;
		if(velocity.norm() > 1e-8) {
			tangent = velocity.normalized();
		}
		else if(forces.norm() > 1e-8) {
			tangent = forces.normalized();
		}
		else {
			tangent = du.normalized();
		}
		
		Vector3f binormal = n.cross(tangent).normalized();
		
		Matrix3f result;
		result.row(0) = tangent;
		result.row(1) = n;
		result.row(2) = binormal;
		return result;
	}
	
	//The center of the particle (different than the coordinate position, which is clamped to a surface)
	Eigen::Vector3f center() const {
		if(coordinate.solid == NULL) {
			return coordinate.position;
		}
		return coordinate.position + coordinate.interpolated_normal() * radius;
	}
	
	//Handles collision test
	bool process_collision(Particle& other, float dt) {
		using namespace std;
		using namespace Eigen;
	
		auto p = center();
		auto q = other.center();
		Vector3f dir = p - q;
		
		float d = dir.norm();
		if(d > radius + other.radius) {
			return false;
		}
		
		if(d < 1e-6) {
			d = 1;
		}
		dir /= d;
	
		float ua = dir.dot(velocity), 
			  ub = dir.dot(other.velocity);
	
		if(ua + ub < 0) {
			return true;
		}
	
		apply_force(dir * (other.mass * (ub - ua) + other.mass * ub) / dt);
		other.apply_force(dir * (mass * (ua - ub) + mass * ua) / dt);
		
		return true;
	}
		
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif

