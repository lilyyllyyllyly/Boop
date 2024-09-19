#pragma once

#include "scaffold.h"

#include "player.h"
#include "cat.h"

#define GAME_W 216
#define GAME_H 216
#define WIN_W 864
#define WIN_H 864
#define TITLE "boop :3"
#define FPS 0 // unlimited

#define HCELLS 6
#define VCELLS 6

#define CELL_W 24
#define CELL_H 24

#define PLINE_CAT_COUNT 3

#define BOARD_OFFSET_X 36
#define BOARD_OFFSET_Y 24

extern int game_manager_type;

typedef struct {
	scaffold_node* cats[PLINE_CAT_COUNT];
	cat_data* datas[PLINE_CAT_COUNT];
} promotion_line;

typedef struct {
	scaffold_node* drawer;

	player_data* curr_player;
	player_data* player0;
	player_data* player1;

	scaffold_node* cells[VCELLS][HCELLS];

	int ended;
	int choosing_turn;

	scaffold_list* promotion_lines;
	promotion_line chosen_line;
	int curr_choose_id;
} game_manager_data;

void game_manager_set_cell(scaffold_node* game_manager, int x, int y, scaffold_node* value);
scaffold_node* game_manager_get_cell(scaffold_node* game_manager, int x, int y);

int is_cell_valid(int x, int y);

scaffold_node* game_manager_create(scaffold_node* drawer, player_data* player0, player_data* player1);

