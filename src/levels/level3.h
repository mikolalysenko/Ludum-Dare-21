struct Level3Solid {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		
		v /= 3.;
		result.density = -20*(cos(v[0]) + cos(v[1]) + cos(v[2])) + sphere(v, (M_PI*M_PI));
		result.friction = 1.0/4.0;
		return result;
	}
};

struct Level3Attr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		
		float h = sin(sqrt(v[0]*v[0] + v[1]*v[1]) * 10.) + 2*(simplexNoise3D(v[0]*0.5, v[1]*0.5, v[2]*0.5, 2) - 0.5);
		
		auto c =
		  color_interp(
			{Vector4f(132, 90, 40, 0.),
			 Vector4f(170, 112, 48, 0.2),
			 Vector4f(164, 100, 39, 0.6),
			 Vector4f(192, 141, 52, 0.2)},
			 h /2.);
			 
		result.color = Vector3f(c[0], c[1], c[2]) / 255;
		
		return result;
	}
};

struct Level3 : public PuzzleGenerator {

	virtual ~Level3() {}

	virtual void setup(Puzzle* puzzle) {
		using namespace Eigen;
		
		//Create geometry
		auto level = new Solid(
			Vector3i( 128, 128, 128 ),
			Vector3f(-20, -20, -20),
			Vector3f( 20,  20,  20));
		Level3Solid	level_func;
		Level3Attr	attr_func;
		setup_solid(*level, level_func, attr_func);
		
		puzzle->add_solid(level);
		
		//Create start/end location
		auto start_pt = level->closest_point(Vector3f(-20,-20,-20));
		puzzle->add_entity(new LevelStartEntity(start_pt));
		
		auto end_pt = level->closest_point(Vector3f(20,20,20));
		puzzle->add_entity(new LevelExitEntity(end_pt));


		//Spikes!
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f( 20, 20, -20))));
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f( 20,-20,  20))));
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f(-20, 20, 20))));
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f(-20, 20,-20))));
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f( 20,-20,-20))));
		puzzle->add_entity(spike_monster(
			level->closest_point(Vector3f(-20,-20, 20))));

	}
};

