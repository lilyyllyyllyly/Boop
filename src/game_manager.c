#include <stdlib.h>

#include "scaffold.h"
#include "mason.h"

int game_manager_type = NODE_TYPE_UNASSIGNED;

#include "game_manager.h"
#include "cat.h"

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

	// calculate boop direction
	int new_x = 2*pushed_x - pusher_data->x;
	int new_y = 2*pushed_y - pusher_data->y;

	// move cat to new position
	cat_move(data->cells[pushed_y][pushed_x], new_x, new_y);
}

static void process(scaffold_node* game_manager, double delta) {
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

	// on click

	game_manager_data* data = (game_manager_data*)(game_manager->data);

	// get mouse position
	scaffold_vector2 mouse_pos = mason_drawer_screen_to_game_pos(data->drawer, (scaffold_vector2){GetMouseX(), GetMouseY()});
	int xid = mouse_pos.x / CELL_W;
	int yid = mouse_pos.y / CELL_H;

	if (!is_cell_valid(xid, yid) || data->cells[yid][xid]) return; // abort if theres already a cat in the position (or its invalid)

	// spawn cat
	scaffold_node* cat = cat_create(data->drawer, game_manager, data->curr_player, xid, yid);
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

	// other player's turn
	data->curr_player = !data->curr_player;
}

scaffold_node* game_manager_create(scaffold_node* drawer) {
	game_manager_data* data = calloc(1, sizeof(game_manager_data));
	data->drawer = drawer;
	data->curr_player = 0;

	return scaffold_node_create(
		&game_manager_type,
		data,
		process,
		NULL
	);
}

