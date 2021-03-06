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
#include "noise.h"

using namespace std;
using namespace Eigen;

namespace Assets {

//Shape definitions
#include "shapes.h"

//Monster definitions
#include "monsters.h"

//Level data
#include "levels/level0.h"
#include "levels/level1.h"
#include "levels/level2.h"
#include "levels/level3.h"
#include "levels/level4.h"

//Artwork
#include "artwork/player_art.h"
#include "artwork/spikeball.h"
#include "artwork/prisimgate.h"
#include "artwork/torus.h"
#include "artwork/button.h"

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
		level_database[2] = new Level2();
		level_database[3] = new Level3();
		level_database[4] = new Level4();
	}

	//Create player artwork
	{
		artwork_database["player_model"] = make_player();
		artwork_database["spikeball"] = make_spikeball();
		artwork_database["prisimgate"] = make_prisimgate();
		artwork_database["torus"] = make_torus();
		artwork_database["button_on"] = make_button_on();
		artwork_database["button_off"] = make_button_off();
	}
}


