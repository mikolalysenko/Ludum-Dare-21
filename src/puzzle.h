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
	virtual void init() = 0;
	virtual void tick(float dt) = 0;
	virtual void draw() = 0;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

//A puzzle generator object
struct PuzzleGenerator {

	virtual ~PuzzleGenerator() {}
	virtual void setup(Puzzle* puzzle) = 0;
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

//A puzzle/level object
struct Puzzle {
	std::vector<Solid*>	solids;
	std::vector<Entity*> entities;
	Player player;
	bool level_complete;
	float elapsed_time;

	Puzzle() : player(this) {}
	~Puzzle() { clear(); }

	void setup(PuzzleGenerator* generator);	
	void clear();
	void init();
	void input();
	void tick(float dt);
	void draw();
	
	//Called when the player should die
	void kill_player();
	
	void add_entity(Entity* e) {
		e->puzzle = this;
		entities.push_back(e);
	}
	void add_solid(Solid* solid) {
		solids.push_back(solid);
	}
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif

