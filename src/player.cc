#include <vector>
#include <iostream>

#include <GL/glfw.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "player.h"
#include "puzzle.h"

using namespace std;
using namespace Eigen;

Player::Player() :
	force_rotation(1, 0, 0, 0),
	camera_position(0, 0, 0),
	button_pressed(false),
	drotation(1, 0, 0, 0)
{
	mouse_state[0] = mouse_state[1] = Vector3f(0,0,0);
	button_pressed = false;
}

//Resets the player
void Player::reset() {
	particle.coordinate = IntrinsicCoordinate(-1, Vector3f(0,0,0), NULL);
	particle.velocity = Vector3f(0, 0, 0);
	particle.forces = Vector3f(0, 0, 0);
	particle.color = Vector3f(1, 1, 1);
	particle.mass = 1.0f;
	force_rotation = Quaternionf(1, 0, 0, 0);
	camera_position = Vector3f(0, 0, 0);
	target_position = Vector3f(0, 0, 0);
	camera_up = Vector3f(0, 0, 0);
	drotation = Quaternionf(1, 0, 0, 0);
	button_pressed = false;
	puzzle = NULL;
}


void Player::input() {
	//Input scale factor
    int w, h;
    glfwGetWindowSize(&w, &h);
    float s = w;
    
    //Update mouse input
    int mx, my, mz;
    glfwGetMousePos(&mx, &my);
    mx -= w/2;
    my -= h/2;
    mz = glfwGetMouseWheel();

    mouse_state[0] = mouse_state[1];    
    mouse_state[1][0] = mx;
    mouse_state[1][1] = my;
    mouse_state[1][2] = mz;
    
    button_pressed = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
    
    //Update camera rotation
    if(button_pressed) {
    	drotation = Quaternionf(
    		mouse_state[1][0] * mouse_state[0][0] + mouse_state[1][1] * mouse_state[0][1] + w * w,
    		w * (mouse_state[1][1] - mouse_state[0][1]),
    		w * (mouse_state[0][0] - mouse_state[1][0]),
            mouse_state[1][0] * mouse_state[0][1] - mouse_state[1][1] * mouse_state[0][0]);
        
    	force_rotation = (force_rotation * drotation).normalized();
    }
}

void Player::tick(float dt) {

	//Apply attractive force from camera
	particle.apply_force( force_rotation * Vector3f(0, -1, 0) );

	//Integrate position
	particle.integrate(dt);
		
	//Compute target camera position and orientation
	Vector3f du, dv, n, vel = particle.velocity, p = particle.coordinate.position;
	particle.coordinate.tangent_space(du, dv, n);
	float vmag = vel.norm();
	
	if(vmag > 1e-6) {
		 target_position = p - vel.normalized() * 3.f + n * 4.f;
	}

	float tau = exp(-dt);
	camera_position = tau * camera_position + (1.f - tau) * target_position;
	camera_up = (tau * camera_up + (1.f - tau) * n).normalized();
	
	//Keep camera from colliding
	for(int i=0; i<puzzle->solids.size(); ++i) {
		auto s = puzzle->solids[i];
		if( (*s)(camera_position) > -5. ) {
			for(int j=0; j<64; ++j) {
				auto grad = s->gradient(camera_position);
				
				
				cout << "Colliding: " << camera_position << endl
					 << "grad : " << grad << endl
					 << "potential : " << (*s)(camera_position) << endl;
				
				
				camera_position -= grad.normalized() / 16.f;
				
				if( (*s)(camera_position) <=  -5.f ) {
					break;
				}
			}
			
			if( (*s)(camera_position) > -5.f ) {
				camera_position = target_position;
			}
		}
	}
}

void Player::set_gl_matrix() {
    glLoadIdentity();


/*
	//Set camera position
	glTranslated(0, 0, -100.);
    double m = sqrt(1. - pow(camera_rotation.w(), 2.0));
    if(m > 0.0001) {
        if(abs(camera_rotation.w()) > 0.0001) {    
            glRotated(-acos(camera_rotation.w()) * 360. / M_PI, camera_rotation.x() / m, camera_rotation.y() / m, camera_rotation.z() / m);
        }
        else {
            glRotated(180., camera_rotation.x(), camera_rotation.y(), camera_rotation.z());
        }
    }
*/

	Vector3f p = particle.coordinate.position;
	gluLookAt(camera_position[0], camera_position[1], camera_position[2],
		p[0], p[1], p[2],
		camera_up[0], camera_up[1], camera_up[2]);
	
}

void Player::draw() {
	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(1, 1, 1);
	glVertex3f(particle.coordinate.position[0], particle.coordinate.position[1], particle.coordinate.position[2]);
	glEnd();
}

