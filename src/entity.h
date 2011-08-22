#ifndef ENTITY_H
#define ENTITY_H

#include "puzzle.h"

//Level exit entity object
struct LevelExitEntity : public Entity {
	LevelExitEntity(IntrinsicCoordinate const& coord) :
		Entity(coord) {}
	virtual ~LevelExitEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

#endif

