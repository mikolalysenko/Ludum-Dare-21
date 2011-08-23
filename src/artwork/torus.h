struct TorusFunc {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		
		Cell result;
		
		result.density = torus(v, 2, 1.1);
		result.friction = 1.0/4.0;
		
		return result;
	}
};

struct TorusAttr {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		result.color = Vector3f(0.88, 0.9, 0.3);
		
		return result;
	}
};

Solid* make_torus() {
	TorusFunc player_model;
	TorusAttr player_style;
	auto player_art = new Solid(
		Vector3i(16, 16, 8),
		Vector3f(-4, -4, -2),
		Vector3f( 4,  4,  2));
	setup_solid(*player_art, player_model, player_style);
	
	return player_art;
}
