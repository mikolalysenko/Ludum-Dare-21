#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "player.h"
#include "solid.h"
#include "surface_coordinate.h"
#include "puzzle.h"
#include "assets.h"
#include "entity.h"

using namespace std;
using namespace Eigen;

namespace Assets {

#include "shapes.h"
	
//Level data
#include "levels/level0.h"
#include "levels/level1.h"

//Player artwork
#include "artwork/player_art.h"

};

//Databases
unordered_map<string, Solid*> artwork_database;
unordered_map<int, PuzzleGenerator*> level_database;

//Retrieve some artwork from the database
Solid* get_artwork(const char* asset_name) {
	assert(artwork_database.find(asset_name) != artwork_database.end());
	return artwork_database[asset_name];
}

//Accessor
PuzzleGenerator* get_level(int level_id) {
	assert(level_database.find(level_id) != level_database.end());
	return level_database[level_id];
}

//Initialize all the artwork
void init_assets() {
	using namespace Assets;
	
	//Set up level 0
	{
		level_database[0] = new Level0();
		level_database[1] = new Level1();
	}

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


