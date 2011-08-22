#ifndef ASSETS_H
#define ASSETS_H

#include "solid.h"
#include "puzzle.h"

void init_assets();
Solid* get_artwork(const char* model_name);
PuzzleGenerator* get_level(int level_id);

#endif

