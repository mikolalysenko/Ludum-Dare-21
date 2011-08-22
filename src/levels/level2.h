struct Level2Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = 
			-(cos(v[0] * 2.*M_PI / 20.) +
			 cos(v[1] * 2.*M_PI / 20.) +
			 cos(v[2] * 2.*M_PI / 20.));
		
		result.friction = 1./4.;
		
		return result;
	}
};

struct Level2Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		
		Array3f p = (v.array()/60.f + 0.5f) * 10.f;		
		float noise1 = simplexNoise3D(p[0], p[1], p[2], 3);
		p *= 4.0f;
		float noise2 = simplexNoise3D(p[0], p[1], p[2], 5);
		
		result.color = Vector3f(0.2, 1, 0.5) * noise1 + Vector3f(0.8, 0.8, 0.8) * noise2;
		
		return result;
	}
};

struct Level2 : public PuzzleGenerator {

	virtual ~Level2() {}

	virtual void setup(Puzzle* puzzle) {
		using namespace Eigen;
		
		//Create geometry
		auto level = new Solid(
			Vector3i( 128, 128, 128 ),
			Vector3f(-30, -30, -30),
			Vector3f( 30. +60./128., 30.+60./128., 30.+60./128.));
		Level2Solid	level_func;
		Level2Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start/end location
		auto start_pt = level->closest_point(Vector3f(-31, 10, 0));
		puzzle->add_entity(new LevelStartEntity(start_pt, 50));
		
		auto end_pt = level->closest_point(Vector3f(0, 0, 0));
		puzzle->add_entity(new LevelExitEntity(end_pt));
		
		
		//Add patroling goons
		puzzle->add_entity(patrol_spike_monster({
			level->closest_point(Vector3f(-29, -29, -31)),
			level->closest_point(Vector3f( 29, -29, -31)),
			level->closest_point(Vector3f( 29,  29, -31)),
			level->closest_point(Vector3f(-29,  29, -31)),
		}));
		
		puzzle->add_entity(patrol_spike_monster({
			level->closest_point(Vector3f(-29, -29, 31)),
			level->closest_point(Vector3f( 29, -29, 31)),
			level->closest_point(Vector3f( 29,  29, 31)),
			level->closest_point(Vector3f(-29,  29, 31)),
		}));

		puzzle->add_entity(patrol_spike_monster({
			level->closest_point(Vector3f(-29, 31, -29)),
			level->closest_point(Vector3f( 29, 31, -29)),
			level->closest_point(Vector3f( 29, 31,  29)),
			level->closest_point(Vector3f(-29, 31,  29)),
		}));

		puzzle->add_entity(patrol_spike_monster({
			level->closest_point(Vector3f(-29, -31, -29)),
			level->closest_point(Vector3f( 29, -31, -29)),
			level->closest_point(Vector3f( 29, -31,  29)),
			level->closest_point(Vector3f(-29, -31,  29)),
		}));

		puzzle->add_entity(patrol_spike_monster({
			level->closest_point(Vector3f(31, -29, -29)),
			level->closest_point(Vector3f(31, 29, -29)),
			level->closest_point(Vector3f(31, 29,  29)),
			level->closest_point(Vector3f(31, -29,  29)),
		}));
	}
};

