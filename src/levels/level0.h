struct Level0Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = 50.0 - v.dot(v);
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
		
		//Create start location
		auto start_pt = level->closest_point(Vector3f(10, 0, 0));
		auto end_pt = level->closest_point(Vector3f(-10, 0, 0));
		
		puzzle->player.particle.coordinate = start_pt;
		puzzle->add_entity(new LevelExitEntity(end_pt));
		
		//Add some teleporters
		for(int i=0; i<20; ++i) {
			float theta = (float)i * M_PI / 10.;
			puzzle->add_entity(new TeleporterEntity(
				level->closest_point(Vector3f(0, cos(i), sin(i))*10.),
				start_pt));
		}
	}
	
	virtual void post_init(Puzzle* puzzle) {
		//Nothing here for now
	}
};

