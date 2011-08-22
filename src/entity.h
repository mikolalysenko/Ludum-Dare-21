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

//A teleporter entity
struct TeleporterEntity : public Entity {

	IntrinsicCoordinate target_coordinate;

	TeleporterEntity(
		IntrinsicCoordinate const& src,
		IntrinsicCoordinate const& dst) :
			Entity(src),
			target_coordinate(dst) {}

	virtual ~TeleporterEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

#endif

