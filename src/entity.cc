#include <cmath>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <iostream>

#include <GL/glfw.h>

#include "solid.h"
#include "entity.h"
#include "sound.h"
#include "text.h"
#include "assets.h"

using namespace std;
using namespace Eigen;

//Level start-------------------------------------------
LevelStartEntity::~LevelStartEntity() {}

void LevelStartEntity::init() {
	auto player = &puzzle->player;
	player->particle.coordinate = coordinate;
	player->camera_height = camera_height;
	player->camera_distance = camera_distance;
	player->camera_stiffness = camera_stiffness;
	
	player->camera_position = coordinate.position + coordinate.interpolated_normal();
}

void LevelStartEntity::tick(float dt) {}
void LevelStartEntity::draw() {}

//Level exit--------------------------------------
LevelExitEntity::~LevelExitEntity() {}

void LevelExitEntity::init() {
}

void LevelExitEntity::tick(float dt) {
	auto p = &puzzle->player.particle;
	float d = (p->center() - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius+1.0) {
		puzzle->level_complete = true;
	}
}

void LevelExitEntity::draw() {

	float theta = puzzle->elapsed_time * 180;
	float h = puzzle->elapsed_time * M_PI / 4.0;

	auto n = coordinate.interpolated_normal();
	auto p = coordinate.position + n * (1. + sin(h));

	glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(p[0], p[1], p[2]);
	glRotatef(theta, n[0], n[1], n[2]);

	
	auto r = n.cross(Vector3f(0, 0, 1));	
	float m = r.norm();
	if(m) {
		float v = -asin(m);
		r /= m;
		glRotatef(v*180./M_PI, r[0], r[1], r[2]);
	}
	
	glColor3f(1, 1, 1);
	show_text("ESCAPE", -0.5*text_width("ESCAPE"), -0.02);
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
}

//Teleporter--------------------------------------
TeleporterEntity::~TeleporterEntity() {}

void TeleporterEntity::init() {
}

void TeleporterEntity::tick(float dt) {
	auto p = &puzzle->player.particle;
	float d = (p->center() - coordinate.position).norm();
	if(p->coordinate.solid == coordinate.solid && d <= p->radius + 0.5) {
		//Update coordinate
		p->coordinate = target_coordinate;
		
		//Update velocity
		auto v_dir = target_coordinate.project_to_tangent_space(p->velocity).normalized();
		p->velocity = p->velocity.norm() * v_dir;
		
		//Update camera position
		auto n = p->coordinate.interpolated_normal();
		puzzle->player.camera_position = p->coordinate.position + n * puzzle->player.camera_height;
		
		//Turn off button press
		puzzle->player.button_pressed = false;
		
		//Special effects
		puzzle->player.shake_camera(1.0, 0.25);
		play_sound_from_group(SOUND_GROUP_TELEPORT);
	}
}

void TeleporterEntity::draw() {
	glDisable(GL_LIGHTING);

	auto p = coordinate.position;
	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(drand48(), drand48(), drand48());
	glVertex3f(p[0], p[1], p[2]);
	glEnd();
	
	
	glEnable(GL_LIGHTING);
}


//Button--------------------------------------
ButtonEntity::~ButtonEntity() {}

void ButtonEntity::init() {
	pressed = false;
	last_state = false;
	time_left = 0.f;
}

void ButtonEntity::tick(float dt) {

	//Check for player press
	auto p = &puzzle->player.particle;
	float d = (p->coordinate.position - coordinate.position).norm();
	
	bool just_pressed = (p->coordinate.solid == coordinate.solid && d <= p->radius);
	
	//Check for monster press
	for(int i=0; i<puzzle->entities.size(); ++i) {
		auto monster = dynamic_cast<MonsterEntity*>(puzzle->entities[i]);
		if(monster != NULL && (monster->flags & MONSTER_FLAG_COLLIDES)) {
			auto mcoord = &monster->particle.coordinate;
			just_pressed |=
				mcoord->solid == coordinate.solid &&
				((mcoord->position - coordinate.position).norm() < monster->particle.radius);
		}
	}
	
	if(just_pressed) {
	
		switch(type) {
			case BUTTON_PRESS:
				pressed = true;
			break;
			
			case BUTTON_SWITCH:
				if(!last_state) {
					pressed = (pressed ? false : true);
				}
			break;
			
			case BUTTON_TIMED:
				pressed = true;
				time_left = time_limit;
			break;
			
			default: assert(false);
		}
	
		if(!last_state) {
			//TODO: Play a sound effect/special effect here
			play_sound_from_group(SOUND_GROUP_BUTTON);
		}
		last_state = true;	
	}
	else {
		if(type == BUTTON_PRESS) {
			if(last_state) {
				play_sound_from_group(SOUND_GROUP_BUTTON);
			}
			pressed = false;
		}
		last_state = false;		
	}
	
	if(type == BUTTON_TIMED && pressed)
	{
		if(time_left < time_limit - .5)
		{
			float f = time_left;
			float d = dt;
			
			if(time_left < 10)
			{
				f *= 2;
				d *= 2;
			}
			
			if(time_left < 3)
			{
				f *= 2;
				d *= 2;
			}
			
			if(time_left < 1)
			{
				f *= 2;
				d *= 2;
			}
			
			
			if((int)f != (int)(f - d))
			{
				if(tickfreq)
					play_sound_from_group(SOUND_GROUP_TICK_HIGH);
				else
					play_sound_from_group(SOUND_GROUP_TICK_LOW);
				
				tickfreq = !tickfreq;
			}
		}
		
		time_left -= dt;
		
		if(time_left < 0) {
			time_left = 0;
			pressed = false;
			
			play_sound_from_group(SOUND_GROUP_GATE_CLOSE);
			
			//TODO: Play a sound for the button deactivating here
		}
	}
}

void ButtonEntity::draw() {

	auto solid = get_artwork(pressed ? "button_on" : "button_off");
	
	glPushMatrix();

	auto n = coordinate.interpolated_normal();	
	auto p = coordinate.position + n * 0.1;
	
	glTranslatef(p[0], p[1], p[2]);
	
	
	auto r = n.cross(Vector3f(0, 0, 1));
	float m = r.norm();
	if(m) {
		float v = -asin(m);
		r /= m;
		glRotatef(v*180./M_PI, r[0], r[1], r[2]);
	}
	
	glScalef(0.5, 0.5, 1);
	
	solid->draw();
	glPopMatrix();	
}

//Obstacle-----------------------------------------
ObstacleEntity::~ObstacleEntity() {}

void ObstacleEntity::init() {
}

void ObstacleEntity::tick(float dt) {
	if(!active()) {
		return;
	}
	
	//Collide with all monsters
	for(int i=0; i<puzzle->entities.size(); ++i) {
		auto monster = dynamic_cast<MonsterEntity*>(puzzle->entities[i]);
		if(monster != NULL && (monster->flags & MONSTER_FLAG_COLLIDES)) {
			if( process_collision(&monster->particle, dt) && (flags & OBSTACLE_DEADLY) ) {
				monster->kill();	
			}
		}
	}
	
	if(process_collision(&puzzle->player.particle, dt)) {
	
		cout << "HERE?" << endl;
	
		//Kill the player if we are deadly!
		if(flags & OBSTACLE_DEADLY) {
			puzzle->kill_player();
		}
	}
}

void ObstacleEntity::draw() {
	if(!active()) {
		return;
	}
	
	glPushMatrix();
	glMultMatrixf(transform.data());
	model->draw();
	glPopMatrix();
}

bool ObstacleEntity::process_collision(Particle* part, float dt) {

	auto tinv = transform.inverse();
	auto npos = tinv * part->center();
	auto grad = (transform.linear() * model->gradient(npos)).normalized();
	
	if((*model)(npos) > -1e-6) {
		part->apply_force(-grad * 100.0);
		play_sound_from_group(SOUND_GROUP_BOUNCE);
		return true;
	} else {	
		auto spos = tinv * (part->coordinate.position + grad * part->radius);	
		if((*model)(spos) > 0) {
			float d = part->velocity.dot(grad);
			if( d > 0 ) {
				part->apply_force(-grad * part->velocity.dot(grad) * 2. / dt);
				play_sound_from_group(SOUND_GROUP_BOUNCE);
			}
			return true;
		}
	}
	
	return false;
}

//Monster!-----------------------------------------

MonsterEntity::~MonsterEntity() {}

void MonsterEntity::init() {
	particle = initial_particle;
	state = initial_state;
	current_waypoint = 0;
}

void MonsterEntity::tick(float dt) {
	if(state & MONSTER_STATE_DEAD) {
		return;
	}
	
	//Figure out where the target is
	auto center = particle.center();
	Vector3f target_position = center;
	
	
	bool has_target = false;
	float speed_factor = 1.0;

	//Chase player
	if(!has_target && (flags & MONSTER_FLAG_CHASE) ) {
		auto pcenter = puzzle->player.particle.center();
		if(vision_radius < 0 || (pcenter - center).norm() < vision_radius) {	
			target_position = pcenter;
			
			//When chasing, speed up
			speed_factor *= 2.0;
			
			has_target = true;
		}
	}

	//Advance on patrol if necessary
	if(!has_target && (flags & MONSTER_FLAG_PATROL)) {
		target_position = patrol_points[current_waypoint].position;
		if((target_position - center).norm() < 2. * particle.radius) {
			current_waypoint = (current_waypoint + 1) % patrol_points.size();
			target_position = patrol_points[current_waypoint].position;
		}
		
		has_target = true;
	}
	
	//Apply driving forces
	if(has_target) {
	
		Vector3f dir = target_position - center;
		if(dir.squaredNorm() > 1e-8) {
			dir = particle.coordinate.project_to_tangent_space(dir.normalized());
		
			auto force = dir * power * speed_factor;
		
			particle.apply_force(dir * power * speed_factor);
		}
	}
		
	//Update position
	particle.integrate(dt);
}

void MonsterEntity::draw() {
	if(state & MONSTER_STATE_DEAD) {
		return;
	}
	
	glPushMatrix();
	auto c = particle.center();
	auto rot = AngleAxisf(particle.rotation);
	glTranslatef(c[0], c[1], c[2]);
	glRotatef(rot.angle() * (180./M_PI), rot.axis()[0], rot.axis()[1], rot.axis()[2]);
	glScalef(draw_scale, draw_scale, draw_scale);
	model->draw();
	glPopMatrix();
}

//Kills the monster
void MonsterEntity::kill() {
	if(flags & MONSTER_FLAG_IMMORTAL)
		return;
	state = MONSTER_STATE_DEAD;
	
	//FIXME: Create a special effect here/play a sound
}


