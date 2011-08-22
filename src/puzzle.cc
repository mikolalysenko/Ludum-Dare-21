#include <Eigen/Core>
#include <Eigen/Geometry>
#include "puzzle.h"

using namespace std;
using namespace Eigen;

//Sets up the puzzle
void Puzzle::setup(PuzzleGenerator* generator) {
	assert(generator != NULL);
	
	clear();
	generator->setup(this);
	init();
}

//Resets the puzzle
void Puzzle::clear() {
	for(int i=solids.size()-1; i>=0; --i) {
		delete solids[i];
	}
	for(int i=entities.size()-1; i>=0; --i) {
		delete entities[i];
	}
	solids.clear();
	entities.clear();
	
	//Turn off level complete
	level_complete = false;
}

//Initializes a level
void Puzzle::init() {

	//Reset player coordinates
	player.reset();

	//Initialize entities
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->init();
	}
	
	//Reset level time
	elapsed_time = 0.f;
}

//Handle input event
void Puzzle::input() {
	player.input();
}

//Ticks the puzzle
void Puzzle::tick(float dt) {
	elapsed_time += dt;
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->tick(dt);
	}
	player.tick(dt);
}

//Draw the puzzle
void Puzzle::draw() {
	player.set_gl_matrix();

	//Turn on lighting
	glEnable(GL_NORMALIZE);	
	glEnable(GL_LIGHTING);
	
	//Set up material
	GLfloat specular[] = {0.9f, 0.9f, 0.9f, 0.9f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular );
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.8f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);

	//Set up light
	Vector3f d = (player.particle.center() - player.camera_position).cross(player.camera_up);
	if(d.norm() > 1e-6) {
		d.normalize();
	}
	Vector3f lp = player.camera_position + player.camera_up * 2.f + d * 5.f;
	GLfloat light_pos[4];
	light_pos[0] = lp[0];
	light_pos[1] = lp[1];
	light_pos[2] = lp[2];
	light_pos[3] = 1.f;
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);	
	
	for(int i=0; i<solids.size(); ++i) {
		solids[i]->draw();
	}
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->draw();
	}
	player.draw();
	
	glDisable(GL_NORMALIZE);	
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
}

//Kills the player
void Puzzle::kill_player() {

	//TODO: Add some special effects here
	
	//Reset the puzzle
	init();
}

