#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GL/glfw.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <mesh/mesh.h>

#include "solid.h"
#include "surface_coordinate.h"
#include "particle.h"
#include "player.h"
#include "sound.h"

#include "player_model.h"

#include "levels/level0.h"

using namespace std;
using namespace Eigen;
using namespace Mesh;

namespace App {

bool running = true;

GLuint scene_display_list = 0;
double fov=45., znear=1., zfar=1000.;

//vector<Particle> particles;

Solid player_model(
	Vector3i(16, 16, 16),
	Vector3f(-1,-1,-1),
	Vector3f( 1, 1, 1));
Player player;
Puzzle puzzle;

void init() {
	PlayerModelFunc player_func;
	PlayerStyleFunc player_attr;
	setup_solid(player_model, player_func, player_attr);
	player.model = &player_model;

	Level0::Level0 level_gen;
	setup_puzzle(puzzle, level_gen, &player);
	
	/*
	//Generate a bunch of random particles
	for(int i=0; i<100; ++i) {
		int t = rand() % puzzle.mesh.triangles().size();
		particles.push_back(
			Particle(
				IntrinsicCoordinate(t,
					puzzle.mesh.vertex(puzzle.mesh.triangle(t).v[rand()%3]).position,
					&puzzle),
				10.*Vector3f(0.5-drand48(), 0.5-drand48(), 0.5-drand48()),
				Vector3f(drand48(), drand48(), drand48()),
				(float)(drand48()*10.f) ));
	}
	*/
}

void input() {

    if( glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS || 
        !glfwGetWindowParam(GLFW_OPENED) ) {
        running = false;
    }
    
    player.input();
}

void tick() {
	static double last_t = 0.0;
	auto t = glfwGetTime();
	auto dt = t - last_t;
	last_t = t;

	/*
	for(int i=0; i<particles.size(); ++i) {
		particles[i].integrate(dt);
	}
	*/
		
	player.tick(dt);
	puzzle.tick(dt);
}

void draw() {

	srand(time(NULL));

    int w, h;
    glfwGetWindowSize(&w, &h);
    glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

    glClearColor(0.3, 0.3, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (float)w / (float)h, znear, zfar);
    
    glMatrixMode(GL_MODELVIEW);
    
  	//Set camera
    player.set_gl_matrix();
    
    //Draw the level
    puzzle.draw();
    
    //Draw the player
    player.draw();
    
    /*
    //Draw particles
    glPointSize(5);
    glBegin(GL_POINTS);
    for(int i=0; i<particles.size(); ++i) {
    	auto p = particles[i];
    	glColor3f(p.color[0], p.color[1], p.color[2]);
	    glVertex3f(p.coordinate.position[0], p.coordinate.position[1], p.coordinate.position[2]);
	}
    glEnd();
    */
}

};

int initialize_libs()
{
    glfwInit();
    if (!glfwOpenWindow(1280, 1024, 8, 8, 8, 8, 16, 0, GLFW_WINDOW))
        return 0;

	if(!initialize_sound_driver())
		return 0;
	
	return 1;
}

int main(int argc, char* argv[])
{
	if(!initialize_libs())
	{
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

