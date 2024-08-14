#include <stdlib.h>

#include "scaffold.h"
#include "mason.h"

int game_manager_type = NODE_TYPE_UNASSIGNED;

#include "game_manager.h"

#define P0_KITTEN_SPR "assets/orange-kitten.png"
#define P1_KITTEN_SPR "assets/black-kitten.png"

static int is_cell_valid(int x, int y) {
	return x >= 0 && x < HCELLS && y >= 0 && y < VCELLS;
}

static void push_away(game_manager_data* data, int pushed_x, int pushed_y, int pusher_x, int pusher_y) {
	if (!is_cell_valid(pushed_x, pushed_y) || !data->cells[pushed_y][pushed_x]) return;
	if (!is_cell_valid(pusher_x, pusher_y)) return;

	int new_x = 2*pushed_x - pusher_x;
	int new_y = 2*pushed_y - pusher_y;
	if (!is_cell_valid(new_x, new_y) || data->cells[new_y][new_x]) return;

	data->cells[new_y][new_x] = data->cells[pushed_y][pushed_x];
	data->cells[pushed_y][pushed_x] = NULL;

	((scaffold_node*)(data->cells[new_y][new_x]))->local_pos = (scaffold_vector2){new_x * CELL_W, new_y * CELL_H};
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
	scaffold_node* cat = mason_sprite_create(data->drawer, data->curr_player == 0? P0_KITTEN_SPR : P1_KITTEN_SPR);
	cat->local_pos = (scaffold_vector2){xid * CELL_W, yid * CELL_H};
	scaffold_node_add_child(game_manager, cat);
	data->cells[yid][xid] = cat;

	// push cats around it
	for (int pushed_y = yid-1; pushed_y <= yid+1; ++pushed_y) {
		for (int pushed_x = xid-1; pushed_x <= xid+1; ++pushed_x) {
			if (pushed_x == xid && pushed_y == yid) continue;
			push_away(data, pushed_x, pushed_y, xid, yid);
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

