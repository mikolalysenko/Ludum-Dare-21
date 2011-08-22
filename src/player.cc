#include <vector>
#include <iostream>

#include <GL/glfw.h>
#include <GL/glu.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "player.h"
#include "puzzle.h"
#include "assets.h"
#include "entity.h"

using namespace std;
using namespace Eigen;


Vector3f Player::unproject(Vector2f const& window) const {
	double obj[3];	
	gluUnProject(window[0], viewport[3]-window[1], 0.0,
		model_matrix,
		projection_matrix,
		viewport,
		obj, obj+1, obj+2);
	return Vector3f(obj[0], obj[1], obj[2]);
}

Vector2f Player::project(Vector3f const& position) const {
	double win[3];
	gluProject(position[0], position[1], position[2],
		model_matrix,
		projection_matrix,
		viewport,
		win, win+1, win+2);
	return Vector2f(win[0], viewport[3]-win[1]);
}


//Resets the player
void Player::reset() {
	model = get_artwork("player_model");
	particle.coordinate = IntrinsicCoordinate(-1, Vector3f(0,0,0), NULL);
	particle.velocity = Vector3f(1, 0, 0);
	particle.forces = Vector3f(0, 0, 0);
	particle.rotation = Quaternionf(1, 0, 0, 0);
	particle.radius = 0.5f;
	particle.mass = 1.0f;
	camera_stiffness = 5.0;
	camera_distance = 3.0;
	camera_height = 9.0;
	camera_position = Vector3f(0, 0, 0);
	camera_shake_mag = 0;
	camera_shake_time = 1.;
	target_position = Vector3f(0, 0, 0);
	camera_up = Vector3f(0, 0, 0);
	mouse_state[0] = mouse_state[1] = Vector2f(0,0);
	button_pressed = false;
	
	strength = 5.f;
}


void Player::input() {
	//Input scale factor
    int w, h;
    glfwGetWindowSize(&w, &h);
    float s = w;
    
    //Update mouse input
    int mx, my;
    glfwGetMousePos(&mx, &my);

    mouse_state[0] = mouse_state[1];    
    mouse_state[1][0] = mx;
    mouse_state[1][1] = my;
    
	auto nstate = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
	
	if(nstate & !button_pressed) {
		Vector3f p = particle.coordinate.position,
				 c = camera_position,
				 n = particle.coordinate.interpolated_normal();
		force_right = camera_up.cross(c - p).normalized();
		force_up = n.cross(force_right).normalized();
	}

	button_pressed = nstate;
}

void clip_camera(
	Vector3f const& local_position,
	Vector3f& camera_position,
	Solid* s,
	Affine3f const& transform) {

	auto tinv = transform.inverse();
	auto q = tinv * local_position;
	auto p = tinv * camera_position;
	
	float lo=0, hi=1.0, m;
	for(hi=0.05; hi<=1.0; hi+=1./32.) {
		auto x = q*(1.-hi) + p*hi;
		if( (*s)(x) > -1e-6 ) {
			break;
		}
	}
	
	hi = min(hi, 1.f);
	
	while(abs(lo - hi) > 1e-6) {
		m = 0.5 * (lo + hi);
		
		auto x = q*(1.-m) + p*m;
		if( (*s)(x) > -1e-6 ) {
			hi = m;
		}
		else {
			lo = m;
		}
	}
	
	camera_position = camera_position*m + local_position*(1.-m);
}


void Player::tick(float dt) {
	//Apply player input force
	if(button_pressed) {
		//Reproject forces
		force_up = particle.coordinate.project_to_tangent_space(force_up).normalized();
		force_right = particle.coordinate.project_to_tangent_space(force_right);
		force_right = (force_right - force_up.dot(force_right) * force_up).normalized();
	
		Vector2f dmouse(
			2.f * (mouse_state[1][0] - (float)viewport[0]) / (float)viewport[2] - 1.f,
			2.f * (mouse_state[1][1] - (float)viewport[1]) / (float)viewport[3] - 1.f);
			
		if(dmouse.squaredNorm() > 1.f) {
			dmouse.normalize();
		}
		
		Vector3f force = strength * (dmouse[0] * force_right - dmouse[1] * force_up);
		
		particle.apply_force(force);
	}

	//Integrate position
	particle.integrate(dt);

	//Compute target camera position and orientation
	Vector3f
		n = particle.coordinate.interpolated_normal(),
		vel = particle.velocity, 
		p = particle.coordinate.position;
	
	if(button_pressed) {
		target_position = p - force_up * camera_distance + n * camera_height;
	}
	else {
		float vmag = vel.norm();
		if(vmag > 1e-6) {
			target_position = p - vel.normalized() * camera_distance + n * camera_height;
		}
		else {
			target_position = p + n * camera_height + particle.coordinate.project_to_tangent_space(camera_position - p).normalized() * camera_distance;
		}
	}

	//Interpolate camera to target
	float tau = exp(-camera_stiffness*dt);
	camera_position = tau * camera_position + (1.f - tau) * target_position;
	camera_up = (tau * camera_up + (1.f - tau) * n).normalized();
	
	//Keep camera from colliding with solids
	for(int i=0; i<puzzle->solids.size(); ++i) {
		clip_camera(p, camera_position, puzzle->solids[i], Affine3f::Identity());
	}
	
	//Keep camera from colliding with obstacles
	for(int i=0; i<puzzle->entities.size(); ++i) {
		auto entity = dynamic_cast<ObstacleEntity*>(puzzle->entities[i]);
		if(entity != NULL) {
			clip_camera(p, camera_position, entity->model, entity->transform);
		}
	}
	
	//Update camera shake
	if(camera_shake_mag > 1e-6) {
		camera_shake_mag *= exp(-dt * camera_shake_time);
	}
	else {
		camera_shake_mag = 0.;
		camera_shake_time = 1.;
	}
}

void Player::set_gl_matrix() {
    glLoadIdentity();

	auto p = particle.coordinate.position,
		 e = camera_position;
		 
	if(camera_shake_mag > 1e-8) {
		e += Vector3f(drand48(), drand48(), drand48()) * camera_shake_mag;
	}
	
	gluLookAt(
		e[0], e[1], e[2],
		p[0], p[1], p[2],
		camera_up[0], camera_up[1], camera_up[2]);	

	glGetDoublev(GL_MODELVIEW_MATRIX, model_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

void Player::draw() {
	auto p = particle.coordinate.position;

	glPushMatrix();
	auto c = particle.center();
	auto rot = AngleAxisf(particle.rotation);
	glTranslatef(c[0], c[1], c[2]);
	glRotatef(rot.angle() * (180./M_PI), rot.axis()[0], rot.axis()[1], rot.axis()[2]);
	glScalef(2*particle.radius,2*particle.radius,2*particle.radius);
	model->draw();
	glPopMatrix();
	
	if(button_pressed) {
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_LINES);

		glColor3f(1, 0, 0);
		glVertex3f(p[0], p[1], p[2]);
		auto fu = p + force_up;
		glVertex3f(fu[0], fu[1], fu[2]);
	
		glColor3f(0, 0, 1);
		glVertex3f(p[0], p[1], p[2]);
		auto fr = p + force_right;
		glVertex3f(fr[0], fr[1], fr[2]);

		glColor3f(0, 1, 0);
		glVertex3f(p[0], p[1], p[2]);
		Vector2f dmouse(
			2.f * (mouse_state[1][0] - (float)viewport[0]) / (float)viewport[2] - 1.f,
			2.f * (mouse_state[1][1] - (float)viewport[1]) / (float)viewport[3] - 1.f);	
		auto f = p + dmouse[0] * force_right - dmouse[1] * force_up;
		glVertex3f(f[0], f[1], f[2]);
	
		glEnd();
		glDisable(GL_DEPTH_TEST);
	}
}

