#include "puzzle.h"

//Sets up the puzzle
void Puzzle::setup(PuzzleGenerator* generator) {
	assert(generator != NULL);
	
	clear();
	player.reset();
	generator->setup(this);
	init();
	generator->post_init(this);
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
	//Initialize entities
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->init();
	}
}

//Handle input event
void Puzzle::input() {
	player.input();
}

//Ticks the puzzle
void Puzzle::tick(float dt) {
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->tick(dt);
	}
	player.tick(dt);
}

//Draw the puzzle
void Puzzle::draw() {
	player.set_gl_matrix();
	for(int i=0; i<solids.size(); ++i) {
		solids[i]->draw();
	}
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->draw();
	}
	player.draw();
}

