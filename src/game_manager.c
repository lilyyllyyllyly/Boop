#include <stdlib.h>

#include "scaffold.h"
#include "mason.h"

int game_manager_type = NODE_TYPE_UNASSIGNED;

#include "game_manager.h"

#define P0_KITTEN_SPR "assets/orange-kitten.png"
#define P1_KITTEN_SPR "assets/black-kitten.png"

static void process(scaffold_node* game_manager, double delta) {
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

	game_manager_data* data = (game_manager_data*)(game_manager->data);

	scaffold_vector2 mouse_pos = mason_drawer_screen_to_game_pos(data->drawer, (scaffold_vector2){GetMouseX(), GetMouseY()});
	int xid = mouse_pos.x / CELL_W;
	int yid = mouse_pos.y / CELL_H;

	scaffold_node* cat = mason_sprite_create(data->drawer, data->curr_player == 0? P0_KITTEN_SPR : P1_KITTEN_SPR);
	cat->local_pos = (scaffold_vector2){xid * CELL_W, yid * CELL_H};
	scaffold_node_add_child(game_manager, cat);

	data->curr_player = !data->curr_player;
}

scaffold_node* game_manager_create(scaffold_node* drawer) {
	game_manager_data* data = malloc(sizeof(game_manager_data));
	data->drawer = drawer;
	data->curr_player = 0;

	return scaffold_node_create(
		&game_manager_type,
		data,
		process,
		NULL
	);
}

