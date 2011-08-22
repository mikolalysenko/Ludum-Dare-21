struct Level2Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = 
			sphere(v, 50.f) +
			sin(v[0] * 0.1) +
			sin(v[1] * 0.1) +
			sin(v[2] * 0.1);
		
		result.friction = 1.0/4.0;
		
		return result;
	}
};

struct Level2Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.color = v;
		
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
			Vector3f(-8, -8, -8),
			Vector3f( 8,  8,  8));
		Level2Solid	level_func;
		Level2Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start/end location
		auto start_pt = level->random_point();
		puzzle->add_entity(new LevelStartEntity(start_pt));
		
		auto end_pt = level->random_point();
		puzzle->add_entity(new LevelExitEntity(end_pt));
	}
};

