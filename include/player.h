#pragma once

#include "scaffold.h"

#define MAX_PIECE_COUNT 8

#define UI_CAT_SPACING 8

#define UI_P0_LIST_X 0
#define UI_P1_LIST_X 192

#define UI_KITTEN_LIST_Y 0
#define UI_CAT_LIST_Y 72

extern int player_type;

typedef struct {
	scaffold_node* node;
	scaffold_node* drawer;
	int id;

	int kitten_count;
	int cat_count;

	scaffold_list* kittens;
	scaffold_list* cats;
} player_data;

void player_add_kitten(player_data* player);
void player_del_kitten(player_data* player);
void player_add_cat(player_data* player);
void player_del_cat(player_data* player);

scaffold_node* player_create(scaffold_node* drawer, int player_id);

