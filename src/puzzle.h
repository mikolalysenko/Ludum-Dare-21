#ifndef PUZZLE_H
#define PUZZLE_H

#include <vector>

//Puzzle object class
#include "solid.h"
#include "surface_coordinate.h"
#include "particle.h"
#include "player.h"

//The game entity interface
struct Entity {
	IntrinsicCoordinate coordinate;
	struct Puzzle*	puzzle;

	Entity(IntrinsicCoordinate const& coord) :
		coordinate(coord),
		puzzle(NULL) {}

	virtual ~Entity() {}
	virtual void init() {}
	virtual void tick(float dt) {}
	virtual void draw() {}
};

//Special entity that sets position player start position
struct StartEntity : public Entity {
	
	Eigen::Quaternionf	camera_rotation;
	Eigen::Vector3f		camera_position;
	
	StartEntity(IntrinsicCoordinate const& coord, 
		Eigen::Quaternionf c_rot,
		Eigen::Vector3f c_pos) :
		Entity(coord),
		camera_rotation(c_rot),
		camera_position(c_pos) {}
	
	virtual ~StartEntity() {}
	virtual void init() {}
	virtual void tick(float dt) {}
	virtual void draw() {}
};

//A puzzle/level object
struct Puzzle {
	std::vector<Solid*>	solids;
	std::vector<Entity*> entities;
	Player* player;

	~Puzzle() {
		for(int i=0; i<solids.size(); ++i) {
			delete solids[i];
		}
		for(int j=0; j<entities.size(); ++j) {
			delete entities[j];
		}
	}

	void init(Player* player);
	void tick(float dt);
	void draw();
	
	void add_entity(Entity* e) {
		e->puzzle = this;
		entities.push_back(e);
	}
	void add_solid(Solid* solid) {
		solids.push_back(solid);
	}
};

//Sets up a puzzle
template<typename LevelGenerator_t>
void setup_puzzle(
	Puzzle& puzzle,
	LevelGenerator_t generator,
	Player* player) {
		
	generator(puzzle);
	puzzle.init(player);
}

#endif

