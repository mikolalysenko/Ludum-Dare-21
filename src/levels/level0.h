struct Level0Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = sphere(v, 50.f);
		result.friction = 1.0/4.0;
		return result;
	}
};

struct Level0Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.normal = Vector3f(1, 0, 0);
		result.color = Vector3f(0.2,0.2,0.2)*drand48();
		return result;
	}
};

struct Level0 : public PuzzleGenerator {

	virtual ~Level0() {}

	virtual void setup(Puzzle* puzzle) {
		using namespace Eigen;
		
		//Create geometry
		auto level = new Solid(
			Vector3i( 64,  64,  64),
			Vector3f(-10, -10, -10),
			Vector3f( 10,  10,  10));
		Level0Solid	level_func;
		Level0Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start/end location
		auto start_pt = level->closest_point(Vector3f(10, 0, 0));
		puzzle->add_entity(new LevelStartEntity(start_pt));
		
		auto end_pt = level->closest_point(Vector3f(-10, 0, 0));		
		puzzle->add_entity(new LevelExitEntity(end_pt));
		
		//Add some teleporters
		for(int i=2; i<48; ++i) {
			float theta = (float)i * M_PI / 25.;
			
			puzzle->add_entity(new TeleporterEntity(
				level->closest_point(Vector3f(0, -cos(theta), sin(theta))*10.),
				start_pt));
		}
		
		//Add a button
		auto button = new ButtonEntity(
			level->closest_point(Vector3f(10, 10, 0)),
			BUTTON_TIMED,
			20.f);
		puzzle->add_entity(button);
		
		//Add a removable obstacle
		auto obstacle = new ObstacleEntity(
			get_artwork("player_model"),
			Affine3f(Translation3f(0, -10, 0) * Scaling(5.f)),
			0,
			button);
		puzzle->add_entity(obstacle);
	}
};

