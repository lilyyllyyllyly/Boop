#include <stdlib.h>
#include <stdio.h>

#include "scaffold.h"
#include "mason.h"

int game_manager_type = NODE_TYPE_UNASSIGNED;

#include "game_manager.h"
#include "cat.h"
#include "player.h"

void game_manager_set_cell(scaffold_node* game_manager, int x, int y, scaffold_node* value) {
	((game_manager_data*)(game_manager->data))->cells[y][x] = value;
}

scaffold_node* game_manager_get_cell(scaffold_node* game_manager, int x, int y) {
	return ((game_manager_data*)(game_manager->data))->cells[y][x];
}

int is_cell_valid(int x, int y) {
	return x >= 0 && x < HCELLS && y >= 0 && y < VCELLS;
}

static void push_away(game_manager_data* data, scaffold_node* pusher, int pushed_x, int pushed_y) {
	cat_data* pusher_data = (cat_data*)(pusher->data);

	scaffold_node* pushed = data->cells[pushed_y][pushed_x];
	cat_data* pushed_data = (cat_data*)(pushed->data);

	// only big cats can boop other big cats
	if (pusher_data->level < pushed_data->level) return;

	// calculate boop direction
	int new_x = 2*pushed_x - pusher_data->x;
	int new_y = 2*pushed_y - pusher_data->y;

	// move cat to new position
	cat_move(pushed, new_x, new_y);
}

int check_promotion(game_manager_data* data, scaffold_node* cat, scaffold_vector2 other1_pos, scaffold_vector2 other2_pos) {
	if (!is_cell_valid(other1_pos.x, other1_pos.y) || !is_cell_valid(other2_pos.x, other2_pos.y)) return 0; // abort if either position is invalid

	scaffold_node* other1 = data->cells[(int)other1_pos.y][(int)other1_pos.x];
	scaffold_node* other2 = data->cells[(int)other2_pos.y][(int)other2_pos.x];

	if (other1 == NULL || other2 == NULL) return 0; // if either position doesnt have a cat, promotion wont happen

	cat_data* cat1_data   = (cat_data*)(cat->data);
	cat_data* other1_data = (cat_data*)(other1->data);
	cat_data* other2_data = (cat_data*)(other2->data);

	if (other1_data->player->id != cat1_data->player->id || other2_data->player->id != cat1_data->player->id) return 0; // if any kitten is from a different player, promotion wont happen

	if (other1_data->level > 0 || other2_data->level > 0 || cat1_data->level > 0) {
		// TODO: win if theyre all cats
		return 0; // if any of them is a cat not a kitten, promotion wont happen
	}

	// promote
	cat1_data->promote = 1;
	other1_data->promote = 1;
	other2_data->promote = 1;
	scaffold_queue_destroy(cat);
	scaffold_queue_destroy(other1);
	scaffold_queue_destroy(other2);

	return 1;
}

static void process(scaffold_node* game_manager, double delta) {
	game_manager_data* data = (game_manager_data*)(game_manager->data);

	int promote = 0;
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !(promote = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && data->curr_player->cat_count)) return;

	// on click

	// get mouse position
	scaffold_vector2 mouse_pos = mason_drawer_screen_to_game_pos(data->drawer, (scaffold_vector2){GetMouseX(), GetMouseY()});
	int xid = mouse_pos.x / CELL_W;
	int yid = mouse_pos.y / CELL_H;

	if (!is_cell_valid(xid, yid) || data->cells[yid][xid]) return; // abort if theres already a cat in the position (or its invalid)

	// spawn cat
	scaffold_node* cat = cat_create(data->drawer, game_manager, data->curr_player, xid, yid, promote);
	scaffold_node_add_child(game_manager, cat);
	data->cells[yid][xid] = cat;

	// push cats around it
	for (int pushed_y = yid-1; pushed_y <= yid+1; ++pushed_y) {
		for (int pushed_x = xid-1; pushed_x <= xid+1; ++pushed_x) {
			if (!is_cell_valid(pushed_x, pushed_y) || !data->cells[pushed_y][pushed_x]) continue;
			if (pushed_x == xid && pushed_y == yid) continue;
			push_away(data, cat, pushed_x, pushed_y);
		}
	}

	// check for promotion
	for (scaffold_node* cat = game_manager->first_child; cat != NULL; cat = cat->next_sibling) {
		cat_data* cat_data_ = (cat_data*)(cat->data);
		int cat_x = cat_data_->x;
		int cat_y = cat_data_->y;

		{// horizontal
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x-1, cat_y};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x+1, cat_y};
			if (check_promotion(data, cat, other1_pos, other2_pos)) continue;
		}

		{// vertical
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x, cat_y-1};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x, cat_y+1};
			if (check_promotion(data, cat, other1_pos, other2_pos)) continue;
		}

		{// diagonal (top-left to bottom-right)
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x-1, cat_y-1};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x+1, cat_y+1};
			if (check_promotion(data, cat, other1_pos, other2_pos)) continue;
		}

		{// diagonal (bottom-left to top-right)
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x-1, cat_y+1};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x+1, cat_y-1};
			if (check_promotion(data, cat, other1_pos, other2_pos)) continue;
		}
	}

	// other player's turn
	data->curr_player = data->curr_player == data->player0? data->player1 : data->player0;

	printf("player 0: %d kittens, %d cats\n", data->player0->kitten_count, data->player0->cat_count);
	printf("player 1: %d kittens, %d cats\n", data->player1->kitten_count, data->player1->cat_count);
}

scaffold_node* game_manager_create(scaffold_node* drawer, player_data* player0, player_data* player1) {
	game_manager_data* data = calloc(1, sizeof(game_manager_data));
	data->drawer = drawer;

	data->curr_player = player0;
	data->player0 = player0;
	data->player1 = player1;

	return scaffold_node_create(
		&game_manager_type,
		data,
		process,
		NULL
	);
}

