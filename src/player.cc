#include <vector>
#include <iostream>

#include <GL/glfw.h>
#include <GL/glu.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "player.h"
#include "puzzle.h"

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
	particle.coordinate = IntrinsicCoordinate(-1, Vector3f(0,0,0), NULL);
	particle.velocity = Vector3f(1, 0, 0);
	particle.forces = Vector3f(0, 0, 0);
	particle.color = Vector3f(1, 1, 1);
	particle.mass = 1.0f;
	camera_stiffness = 5.0;
	camera_distance = 3.0;
	camera_height = 9.0;
	camera_position = Vector3f(0, 0, 0);
	target_position = Vector3f(0, 0, 0);
	camera_up = Vector3f(0, 0, 0);
	mouse_state[0] = mouse_state[1] = Vector2f(0,0);
	button_pressed = false;
	puzzle = NULL;
	
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
				 du, dv, n;
		particle.coordinate.tangent_space(du, dv, n);
		force_right = camera_up.cross(c - p).normalized();
		force_up = n.cross(force_right).normalized();
	}

	button_pressed = nstate;
    
/*    
    //Update camera rotation
    if(button_pressed) {
    	drotation = Quaternionf(
    		mouse_state[1][0] * mouse_state[0][0] + mouse_state[1][1] * mouse_state[0][1] + w * w,
    		w * (mouse_state[1][1] - mouse_state[0][1]),
    		w * (mouse_state[0][0] - mouse_state[1][0]),
            mouse_state[1][0] * mouse_state[0][1] - mouse_state[1][1] * mouse_state[0][0]);
        
    	force_rotation = (force_rotation * drotation).normalized();
    }
*/
}

void Player::tick(float dt) {



	//Apply attractive force from camera
	//particle.apply_force( force_rotation * Vector3f(0, -1, 0) );
	if(button_pressed) {

		//Reproject forces	
		force_up = particle.coordinate.project_to_tangent_space(force_up).normalized();
		force_right = particle.coordinate.project_to_tangent_space(force_right);
		force_right = (force_right - force_up.dot(force_right) * force_up).normalized();
	
		Vector2f dmouse(
			2.f * (mouse_state[1][0] - (float)viewport[0]) / (float)viewport[2] - 1.f,
			2.f * (mouse_state[1][1] - (float)viewport[1]) / (float)viewport[3] - 1.f);
		
		Vector3f force = dmouse[0] * force_right - dmouse[1] * force_up;

		cout << "Force up = " << force_up << endl
			 << "Force right = " << force_right << endl
			 << "dmouse = " << dmouse << endl
			 << "Applying force: " << force << endl;
		
		particle.apply_force(force);
	}

	//Integrate position
	particle.integrate(dt);

	
	//Compute target camera position and orientation
	Vector3f du, dv, n, vel = particle.velocity, p = particle.coordinate.position;
	particle.coordinate.tangent_space(du, dv, n);
	
	if(button_pressed) {
		target_position = p - force_up * camera_distance + n * camera_height;
	}
	else {
		float vmag = vel.norm();
		if(vmag > 1e-6) {
			target_position = p - vel.normalized() * camera_distance + n * camera_height;
		}
		else {
			target_position = p + n * camera_height;
		}
	}

	//Interpolate camera to target
	float tau = exp(-camera_stiffness*dt);
	camera_position = tau * camera_position + (1.f - tau) * target_position;
	camera_up = (tau * camera_up + (1.f - tau) * n).normalized();
	
	//Keep camera from colliding
	for(int i=0; i<puzzle->solids.size(); ++i) {
		auto s = puzzle->solids[i];
		
		if( (*s)(camera_position) > -1e6 )
			continue;
		
		float lo=0, hi=1.0, m;
		while(abs(lo - hi) > 1e-6) {
			m = 0.5 * (lo + hi);
			
			auto x = camera_position*(1.-m) + p*m;
			
			if( (*s)(x) > -1e6 ) {
				hi = m;
			}
			else {
				lo = m;
			}
		}
		
		camera_position = camera_position*(1.-m) + p*m;
	}
}

void Player::set_gl_matrix() {
    glLoadIdentity();

	Vector3f p = particle.coordinate.position;
	gluLookAt(camera_position[0], camera_position[1], camera_position[2],
		p[0], p[1], p[2],
		camera_up[0], camera_up[1], camera_up[2]);	

	glGetDoublev(GL_MODELVIEW_MATRIX, model_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

void Player::draw() {
	auto p = particle.coordinate.position;

	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(1, 1, 1);
	glVertex3f(p[0], p[1], p[2]);
	glEnd();
	
	
	
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

