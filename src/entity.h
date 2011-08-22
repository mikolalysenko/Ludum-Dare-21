#ifndef ENTITY_H
#define ENTITY_H

#include "particle.h"
#include "puzzle.h"

//The level start coordinate entity, initializes player
struct LevelStartEntity : public Entity {
	IntrinsicCoordinate coordinate;
	float camera_stiffness, camera_distance, camera_height;

	LevelStartEntity(
		IntrinsicCoordinate const& coord,
		float height = 25.0,
		float distance = 1.0,
		float stiffness = 2.0) :
		coordinate(coord),
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
	IntrinsicCoordinate coordinate;

	LevelExitEntity(IntrinsicCoordinate const& coord) :
		coordinate(coord) {}
	virtual ~LevelExitEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

//A teleporter entity
struct TeleporterEntity : public Entity {

	IntrinsicCoordinate coordinate, target_coordinate;

	TeleporterEntity(
		IntrinsicCoordinate const& src,
		IntrinsicCoordinate const& dst) :
			coordinate(src),
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
	
	IntrinsicCoordinate coordinate;
	ButtonType type;
	float time_limit, time_left;
	bool pressed, last_state;
	
	ButtonEntity(
		IntrinsicCoordinate const& coord,
		ButtonType t = BUTTON_PRESS,
		float time = 5.0f) :
		coordinate(coord),
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

//Monsters!
enum MonsterFlags {
	MONSTER_FLAG_COLLIDES	= 1,
	MONSTER_FLAG_CHASE	 	= 2,
	MONSTER_FLAG_IMMORTAL	= 4,
	MONSTER_FLAG_DEADLY		= 8,
	MONSTER_FLAG_PATROL		= 16,
};

enum MonsterStateFlags {
	MONSTER_STATE_DEAD	= 1,
};

struct MonsterEntity : public Entity {

	Solid* model;
	Particle particle;
	int state, current_waypoint;
	
	//!!!INITIAL STATE STUFF!!!!  Do not modify after construction
	int flags, initial_state;
	float vision_radius, power, draw_scale;
	Particle initial_particle;
	std::vector<IntrinsicCoordinate> patrol_points;
	
	MonsterEntity(
		Solid* m,
		Particle const& p,
		int flags_ = MONSTER_FLAG_COLLIDES,
		int state_ = 0,
		float power_ = 1.0,
		float vision = -1.0,
		float draw_scale_ = 1.0) :
		model(m),
		initial_particle(p),
		flags(flags_),
		initial_state(state_),
		power(power_),
		vision_radius(vision),
		draw_scale(draw_scale_) {}
	
	virtual ~MonsterEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
	
	//Kills the monster
	void kill();
};

//Lasers!
struct LaserEntity : public Entity {

	IntrinsicCoordinate coordinate;
	float beam_length, beam_step;
	Eigen::Vector3f beam_direction;
	
	LaserEntity(
		IntrinsicCoordinate const& coord,
		float length,
		float step,
		Eigen::Vector3f direction) :
		coordinate(coord),
		beam_length(length),
		beam_step(step),
		beam_direction(direction) {}
	
	virtual ~LaserEntity();
	virtual void init();
	virtual void tick(float dt);
	virtual void draw();
};

#endif

