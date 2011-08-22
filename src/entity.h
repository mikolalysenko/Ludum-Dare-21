#ifndef ENTITY_H
#define ENTITY_H

#include "particle.h"
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

//Buttons!  Can toggle walls and other stuff
struct ButtonEntity : public Entity {
	bool pressed, toggleable, last_state;
	
	ButtonEntity(
		IntrinsicCoordinate const& coord,
		bool toggle = false) :
		Entity(coord),
		pressed(false),
		toggleable(toggle),
		last_state(false) {}

	virtual ~ButtonEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

//Obstacle entity
struct ObstacleEntity : public Entity {
	
	Solid* model;
	Eigen::Affine3f transform;
	ButtonEntity* button;
	
	ObstacleEntity(
		Solid* m,
		Eigen::Affine3f f,
		ButtonEntity* b=NULL) :
		Entity(IntrinsicCoordinate()),
		model(m),
		transform(f),
		button(b) {}
	
	virtual ~ObstacleEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
	
	bool active() const {
		if(button == NULL)
			return true;
		return !button->pressed;
	}
	
	//Handles collision detection/response
	void process_collision(Particle* particle, float dt);
};

#endif

