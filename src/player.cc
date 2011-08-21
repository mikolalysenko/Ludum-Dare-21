#include <vector>
#include <iostream>

#include <GL/glfw.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "player.h"

using namespace std;
using namespace Eigen;

Player::Player() :
	camera_rotation(1, 0, 0, 0),
	camera_position(0, 0, 0),
	camera_target(0, 0, 0),
	button_pressed(false),
	drotation(1, 0, 0, 0)
{
	mouse_state[0] = mouse_state[1] = Vector3f(0,0,0);
	button_pressed = false;
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
        
    	camera_rotation = (camera_rotation * drotation).normalized();
    }
}

void Player::tick(float dt) {

	//Integrate position
	//particle.integrate(dt);
	
	//TODO: Update camera position
}

void Player::set_gl_matrix() {
    glLoadIdentity();

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
}

