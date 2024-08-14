#pragma once

#include "scaffold.h"

#define HCELLS 6
#define VCELLS 6

#define CELL_W 24
#define CELL_H 24

extern int game_manager_type;

typedef struct {
	scaffold_node* drawer;
	int curr_player;

	scaffold_node* cells[VCELLS][HCELLS];
} game_manager_data;

scaffold_node* game_manager_create(scaffold_node* drawer);

