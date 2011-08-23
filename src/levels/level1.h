struct Level1Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = 
		result.density = 
		min(-box(Vector3f(v[0], v[1] + 34, v[2]), 6, 6, 6),
		max(torus(Vector3f(v[0], v[1] - 18, v[2]), 16, 16),
		max(torus(Vector3f(v[0], v[1] + 18, v[2]), 16, 16),
		torus(Vector3f(v[2], v[1], v[0]), 16, 16))));
		//float val = 2 - sqrt(v[0] * v[0] + v[1] * v[1]);
		//result.density = val * val + v[2] * v[2] - 64;
		result.friction = 1.0/4.0;
		return result;
	}
};

struct Level1Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.normal = Vector3f(1, 0, 0);
		result.color = Vector3f(0.2,0.2,0.2)*drand48();
		return result;
	}
};

struct Level1 : public PuzzleGenerator {

	virtual ~Level1() {}

	virtual void setup(Puzzle* puzzle) {
		using namespace Eigen;
		
		//Create geometry
		auto level = new Solid(
			Vector3i( 64,  256,  64),
			Vector3f(-22, -50, -22),
			Vector3f( 22,  50,  22));
		Level1Solid	level_func;
		Level1Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start/end location
		auto start_pt = level->closest_point(Vector3f(0, 36, 0));
		puzzle->add_entity(new LevelStartEntity(start_pt));
		
		auto end_pt = level->closest_point(Vector3f(0, 0, 18));		
		puzzle->add_entity(new LevelExitEntity(end_pt));
		
		puzzle->add_entity(new TeleporterEntity(
			level->closest_point(Vector3f(1, -36, 0)),
			level->closest_point(Vector3f(0, 0, -18))));
		
		auto m = Affine3f::Identity();
		m.translate(Vector3f(0.f, 0.f, 0.f)).scale(Vector3f(1, 8, 8));
		
		auto obstacle = new ObstacleEntity(
		get_artwork("prisimgate"),
		m,
		0);
		puzzle->add_entity(obstacle);
		
		auto button = new ButtonEntity(
			level->closest_point(Vector3f(-1, -36, 0)),
			BUTTON_TIMED,
			30.f);
		puzzle->add_entity(button);
		
		auto m2 = Affine3f::Identity();
		m2.translate(Vector3f(16.f, -18.f, 0.f)).scale(Vector3f(8, 1, 8));
		
		auto obstacle2 = new ObstacleEntity(
		get_artwork("prisimgate"),
		m2,
		0,
		button);
		puzzle->add_entity(obstacle2);
		
		/*
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f( -18, 18, 0))));
			
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f( 0, -10, 0))));
			
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f( 0, 10, 0))));
		*/
	}
};

