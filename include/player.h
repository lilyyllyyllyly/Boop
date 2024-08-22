#pragma once

#include "scaffold.h"

#define MAX_PIECE_COUNT 8

extern int player_type;

typedef struct {
	int id;
	int kitten_count;
	int cat_count;
} player_data;

scaffold_node* player_create(int player_id);

