#ifndef ENTITY_H
#define ENTITY_H

#include "particle.h"
#include "puzzle.h"

//The level start coordinate entity, initializes player
struct LevelStartEntity : public Entity {
	float camera_stiffness, camera_distance, camera_height;

	LevelStartEntity(
		IntrinsicCoordinate const& coord,
		float height = 25.0,
		float distance = 1.0,
		float stiffness = 2.0) :
		Entity(coord),
		camera_height(height),
		camera_distance(distance),
		camera_stiffness(stiffness) {}
	virtual ~LevelStartEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

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
enum ButtonType {
	BUTTON_PRESS,
	BUTTON_SWITCH,
	BUTTON_TIMED
};

struct ButtonEntity : public Entity {
	
	ButtonType type;
	float time_limit, time_left;
	bool pressed, last_state;
	
	ButtonEntity(
		IntrinsicCoordinate const& coord,
		ButtonType t = BUTTON_PRESS,
		float time = 5.0f) :
		Entity(coord),
		type(t),
		time_limit(time),
		time_left(0.f),
		pressed(false),
		last_state(false) {}

	virtual ~ButtonEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

//Obstacle entity
enum ObstacleFlags {
	OBSTACLE_NO_COLLIDE		= 1,
	OBSTACLE_DEADLY			= 2,
	OBSTACLE_LASER_REFLECT	= 4,
	OBSTACLE_LASER_TRANSMIT	= 8,
};

struct ObstacleEntity : public Entity {
	
	int flags;
	Solid* model;
	Eigen::Affine3f transform;
	ButtonEntity* button;
	
	ObstacleEntity(
		Solid* m,
		Eigen::Affine3f f,
		int fl = 0,
		ButtonEntity* b=NULL) :
		Entity(IntrinsicCoordinate()),
		flags(fl),
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
	bool process_collision(Particle* particle, float dt);
};

//Lasers!
struct LaserEntity : public Entity {

	std::vector<Eigen::Vector3f> beam;
	float beam_length, beam_step;
	Eigen::Vector3f beam_direction;

	
	LaserEntity(
		IntrinsicCoordinate const& coord,
		float length,
		float step,
		Eigen::Vector3f direction) :
		Entity(coord),
		beam_length(length),
		beam_step(step),
		beam_direction(direction) {}
	
	virtual ~LaserEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

#endif

