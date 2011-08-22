struct Level1Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		result.density = 
		max(torus(Vector3f(v[0], v[1] - 9, v[2]), 7, 9),
		max(torus(Vector3f(v[0], v[1] + 9, v[2]), 7, 9),
		torus(Vector3f(v[2], v[1], v[0]), 7, 9)));
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
			Vector3f(-11, -25, -11),
			Vector3f( 11,  25,  11));
		Level1Solid	level_func;
		Level1Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start location
		auto tri = level->mesh.triangle(0);
		puzzle->player.particle.coordinate =
			IntrinsicCoordinate(
				0,
				level->mesh.vertex(tri.v[0]).position,
				level);
	}
	
	virtual void post_init(Puzzle* puzzle) {
	
		//Nothing here for now
	}
};

