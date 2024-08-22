#pragma once

#define P0_KITTEN_SPR "assets/orange-kitten.png"
#define P1_KITTEN_SPR "assets/black-kitten.png"

#include "scaffold.h"

#include "player.h"

extern int cat_type;

typedef struct {
	scaffold_node* game_manager;
	int x, y;

	player_data* player;
} cat_data;

void cat_move(scaffold_node* cat, int new_x, int new_y);

scaffold_node* cat_create(scaffold_node* drawer, scaffold_node* game_manager, player_data* player, int x, int y);

