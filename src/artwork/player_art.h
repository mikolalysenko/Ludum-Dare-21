//Player model
struct PlayerModelFunc {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		Cell result;
		result.density = sqrt(.5) - v.dot(v);
		result.friction = 0;
		return result;
	}
};

//Player shader
struct PlayerStyleFunc {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		float t = drand48();
		result.color = Vector3f(t,t*0.2,t*(0.4 * t + (1-t)*0.6));
		return result;
	}
};

Solid* make_player() {
	PlayerModelFunc player_model;
	PlayerStyleFunc player_style;
	auto player_art = new Solid(
		Vector3i(16, 16, 16),
		Vector3f(-1, -1, -1),
		Vector3f( 1,  1,  1));
	setup_solid(*player_art, player_model, player_style);
	
	return player_art;
}

