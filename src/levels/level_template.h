struct LevelXXXSolid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = sphere(v, 50.f);
		result.friction = 1.0/4.0;
		return result;
	}
};

struct LevelXXXAttr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.color = position;
		
		return result;
	}
};

struct LevelTemplate : public PuzzleGenerator {

	virtual ~LevelTemplate() {}

	virtual void setup(Puzzle* puzzle) {
		using namespace Eigen;
		
		//Create geometry
		auto level = new Solid(
			Vector3i( 128, 128, 128 ),
			Vector3f(-8, -8, -8),
			Vector3f( 8,  8,  8));
		LevelXXXSolid	level_func;
		LevelXXXAttr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start/end location
		auto start_pt = level->random_point();
		puzzle->add_entity(new LevelStartEntity(start_pt));
		
		auto end_pt = level->random_point();
		puzzle->add_entity(new LevelExitEntity(end_pt));

	}
};

