#include <unordered_map>
#include <string>
#include "art.h"

using namespace std;
using namespace Eigen;

unordered_map<string, Solid*> artwork_database;

//Player model
struct PlayerModelFunc {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		Cell result;
		result.density = 0.25 - v.dot(v);
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


//Initialize all the artwork
void init_artwork() {
	//Create player artwork
	{
		PlayerModelFunc player_model;
		PlayerStyleFunc player_style;
		auto player_art = new Solid(
			Vector3i(16, 16, 16),
			Vector3f(-1, -1, -1),
			Vector3f( 1,  1,  1));
		setup_solid(*player_art, player_model, player_style);
		artwork_database["player_model"] = player_art;
	}
}

Solid* get_artwork(const char* asset_name) {
	if(artwork_database.find(asset_name) != artwork_database.end()) {
		return artwork_database[asset_name];
	}
	return NULL;
}

