#pragma once

#include "scaffold.h"

#define P0_KITTEN_SPR "assets/orange-kitten.png"
#define P1_KITTEN_SPR "assets/black-kitten.png"

extern int cat_type;

typedef struct {
	scaffold_node* game_manager;
	int player_id;
	int x, y;
} cat_data;

void cat_move(scaffold_node* cat, int new_x, int new_y);

scaffold_node* cat_create(scaffold_node* drawer, scaffold_node* game_manager, int player_id, int x, int y);

