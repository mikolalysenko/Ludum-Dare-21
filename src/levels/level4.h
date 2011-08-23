struct Level4Solid0 {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		
		result.density = torus(v, 20, 5*5);
		result.friction = 1.0/4.0;
		
		return result;
	}
};

struct Level4Attr0 {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		
		float t = v[2] + simplexNoise3D(v[0] * 0.1, v[1] *0.1,  v[2]*0.1, 2);
		
		if(t > 0) {
			result.color = Vector3f(132, 90, 40)/255.;
		} else if(drand48() < 0.05) {
			result.color = Vector3f(drand48(), drand48(), drand48());
		} else {
			result.color = Vector3f(1, 1, 1);
		}
		
		return result;
	}
};

struct Level4Solid1 {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		
		result.density = sphere(v, 12*12);
		result.friction = 1.0/4.0;
		
		return result;
	}
};

struct Level4Attr1 {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		
		
		float t = cos(atan2(v[0], v[1]) * 8 + v[2] * 2);
		
		
		if( t < -0.3 ) {
			result.color = Vector3f(1, 0, 0);
		}
		else {
			result.color = Vector3f(1,1,1);
		}
		return result;
	}
};


struct Level4 : public PuzzleGenerator {

	virtual ~Level4() {}

	virtual void setup(Puzzle* puzzle) {
		using namespace Eigen;
		
		//Create geometry
		auto part0 = new Solid(
			Vector3i( 128, 128, 32 ),
			Vector3f(-30, -30, -8),
			Vector3f( 30,  30,  8));
		{
			Level4Solid0	level_func;
			Level4Attr0		attr_func;
			setup_solid(*part0, level_func, attr_func);
		}
		puzzle->add_solid(part0);



		//Create geometry
		auto part1 = new Solid(
			Vector3i( 64, 64, 64 ),
			Vector3f(-13, -13, -13),
			Vector3f( 13, 13, 13));
		{
			Level4Solid1	level_func;
			Level4Attr1		attr_func;
			setup_solid(*part1, level_func, attr_func);
		}
				
		puzzle->add_solid(part1);


		
		//Create start/end location
		auto start_pt = part0->closest_point(Vector3f(-30, 0, -30));
		puzzle->add_entity(new LevelStartEntity(start_pt));
		
		auto end_pt = part0->closest_point(Vector3f(30, 0, -30));
		puzzle->add_entity(new LevelExitEntity(end_pt));
		

		puzzle->add_entity(spike_monster(part1->closest_point(Vector3f(30, 0, 0))) );
		
		auto button = new ButtonEntity(
			part1->closest_point(Vector3f(-30, 0, 0)),
			BUTTON_TIMED,
			25);
			
		puzzle->add_entity(button);
		
		
		
		
	}
};

