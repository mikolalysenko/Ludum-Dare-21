struct ButtonFunc {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		
		result.density = max(
			0.1f - fabsf(v[2]),
			1.f - (v[0]*v[0] + v[1]*v[1]));
		
		return result;
	}
};

struct ButtonOn {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.color = Vector3f(0.9, 0.2, 0.1);
		
		return result;
	}
};

struct ButtonOff {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.color = Vector3f(0.2, 0.95, 0.1);
		
		return result;
	}
};

Solid* make_button_on() {
	ButtonFunc player_model;
	ButtonOn   player_style;
	auto player_art = new Solid(
		Vector3i(16, 16, 4),
		Vector3f(-1, -1, -0.25),
		Vector3f( 1,  1,  0.25));
	setup_solid(*player_art, player_model, player_style);
	
	return player_art;
}

Solid* make_button_off() {
	ButtonFunc player_model;
	ButtonOff   player_style;
	auto player_art = new Solid(
		Vector3i(16, 16, 4),
		Vector3f(-1, -1, -0.25),
		Vector3f( 1,  1,  0.25));
	setup_solid(*player_art, player_model, player_style);
	
	return player_art;
}

