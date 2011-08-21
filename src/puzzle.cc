#include "puzzle.h"

void Puzzle::init(Player* pl) {
	player = pl;
	
	//Reset player
	player->reset();
	player->puzzle = this;
	
	//Set initial position and camera
	for(int i=0; i<entities.size(); ++i) {
		auto ptr = dynamic_cast<StartEntity*>(entities[i]);
		if(ptr == NULL)
			continue;
		
		player->particle.coordinate = ptr->coordinate;
		break;
	}
}

void Puzzle::tick(float dt) {
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->tick(dt);
	}
}

void Puzzle::draw() {
	for(int i=0; i<solids.size(); ++i) {
		solids[i]->draw();
	}
	for(int i=0; i<entities.size(); ++i) {
		entities[i]->draw();
	}
}
