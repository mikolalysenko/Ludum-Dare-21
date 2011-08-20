#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GL/glfw.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <mesh/mesh.h>

#include "solid.h"
#include "levels.h"
#include "surface_coordinate.h"

using namespace std;
using namespace Eigen;
using namespace Mesh;

namespace App {

bool running = true;

GLuint scene_display_list = 0;
double fov=45., znear=1., zfar=1000.;

int mx=0, my=0, mz=0;
double vw=1., vx=0., vy=0., vz=0.;
double tx = 0., ty=0., tz=100.;

Solid puzzle(
	Vector3i(16, 16, 16),
	Vector3f(-10, -10, -10),
	Vector3f( 10,  10,  10));

IntrinsicCoordinate coord;
Vector3f velocity(0, 0, 0);

void init() {
	Level0		level_func;
	Level0Attr	attr_func;
	setup_solid(puzzle, level_func, attr_func);
	
	auto t = puzzle.mesh.triangle(0);
	coord = IntrinsicCoordinate(
		0,
		puzzle.mesh.vertex(t.v[0]).position,
		&puzzle.mesh);
}

void input() {

    if( glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS || 
        !glfwGetWindowParam(GLFW_OPENED) ) {
        running = false;
    }
    
    static int pressed = false;
    
    if( glfwGetKey('A') == GLFW_PRESS ) {
    	if(!pressed) {
    		velocity = coord.advect(velocity);
    		pressed = true;
    	}
    }
    else {
    	pressed = false;
    }
    
    int w, h;
    glfwGetWindowSize(&w, &h);
    
    //Update view
    int px = mx, py = my, pz = mz;
    glfwGetMousePos(&mx, &my);
    mx -= w/2;
    my -= h/2;
    mz = glfwGetMouseWheel();
    
    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
        double  nx = w * (my - py),
                ny = w * (px - mx),
                nz = mx * py - my * px,
                nw = mx * px + my * py + w * w;
                
        double  qw = vw * nw - vx * nx - vy * ny - vz * nz,
                qx = vw * nx + vx * nw + vy * nz - vz * ny,
                qy = vw * ny - vx * nz + vy * nw + vz * nx,
                qz = vw * nz + vx * ny - vy * nx + vz * nw;
                
        double l = sqrt(qw * qw + qx * qx + qy * qy + qz * qz);
        
        if(l < 0.0001) {
            vx = vy = vz = 0.;
            vw = 1.;
        }
        else {
            vw = qw / l;
            vx = qx / l;
            vy = qy / l;
            vz = qz / l;
        }
    }
    
    tz += (pz - mz) * 10;
}


void tick() {

	Quaternionf quat(vw, vx, vy, vz);

	velocity += quat * Vector3f(0, 0, 0.1);
	float m = velocity.norm();
	velocity = coord.advect(velocity * 0.0001).normalized() * m * 0.995;
}

void draw() {

	srand(time(NULL));

    int w, h;
    glfwGetWindowSize(&w, &h);
    glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);

    glClearColor(0.3, 0.3, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (float)w / (float)h, znear, zfar);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glTranslated(-tx, -ty, -tz);

    double m = sqrt(1. - vw*vw);
    if(m > 0.0001) {
        if(abs(vw) > 0.0001) {    
            glRotated(-acos(vw) * 360. / M_PI, vx / m, vy / m, vz / m);
        }
        else {
            glRotated(180., vx, vy, vz);
        }
    }
    
   	glTranslated(-coord.position[0], -coord.position[1], -coord.position[2]);

    
    //Draw the level
    puzzle.draw();
    
    glPointSize(5);
    glColor3f(1, 0, 0);
    glBegin(GL_POINTS);
    glVertex3f(coord.position[0], coord.position[1], coord.position[2]);
    glEnd();
}

};

int main(int argc, char* argv[]) {
    glfwInit();
    if (!glfwOpenWindow(1280, 1024, 8, 8, 8, 8, 16, 0, GLFW_WINDOW)) {
        glfwTerminate();
        return -1;
    }
    glfwSetWindowTitle("Mesh Demo");
    
    App::init();
    while(App::running) {
        App::input();
        App::draw();
        App::tick();
        glfwSwapBuffers();
    }

    glfwTerminate();
    return 0;
}

