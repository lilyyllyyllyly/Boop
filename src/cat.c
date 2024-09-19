#include <stdlib.h>

#include "scaffold.h"
#include "mason.h"

int cat_type = NODE_TYPE_UNASSIGNED;

#include "cat.h"
#include "game_manager.h"
#include "player.h"

static void destroy(scaffold_node* cat) {
	cat_data* data = (cat_data*)(cat->data);

	if (data->promote) {
		player_add_cat(data->player); // add promoted cat to player's hand
	} else {
		player_add_kitten(data->player); // add kitten back to player's hand
	}

	game_manager_set_cell(data->game_manager, data->x, data->y, NULL);
	free(data);
	scaffold_node_destroy(cat);
}

void cat_move(scaffold_node* cat, int new_x, int new_y) {
	cat_data* data = (cat_data*)(cat->data);

	// delete cat if it falls outside the board (they fall off the bed according to the rules)
	if (!is_cell_valid(new_x, new_y)) {
		game_manager_set_cell(data->game_manager, data->x, data->y, NULL);
		scaffold_queue_destroy(cat);
		return;
	}

	if (game_manager_get_cell(data->game_manager, new_x, new_y)) return; // abort if there's already a cat there

	// replace previous cell with NULL
	game_manager_set_cell(data->game_manager, data->x, data->y, NULL);

	// change cat cell to new position
	data->x = new_x;
	data->y = new_y;
	game_manager_set_cell(data->game_manager, new_x, new_y, cat);

	// update real node position
	cat->local_pos = (scaffold_vector2){new_x * CELL_W, new_y * CELL_H};
}

scaffold_node* cat_create(scaffold_node* drawer, scaffold_node* game_manager, player_data* player, int x, int y, int level) {
	cat_data* data = malloc(sizeof(cat_data));
	data->game_manager = game_manager;
	data->x = x;
	data->y = y;

	data->player = player;
	
	data->promote = 0;
	data->level = level;

	data->promotion_lines = 0;

	if (level) {
		player_del_cat(player); // remove cat from player's hand
	} else {
		player_del_kitten(player); // remove kitten from player's hand
	}

	scaffold_node* cat = scaffold_node_create(
		&cat_type,
		data,
		NULL,
		destroy
	);

	cat->local_pos = (scaffold_vector2){x * CELL_W, y * CELL_H};
	game_manager_set_cell(data->game_manager, x, y, cat);

	if (level) {
		scaffold_node* sprite = mason_texture_create(drawer, CAT_DRAW_ORDER, player->id == 0? P0_CAT_SPR : P1_CAT_SPR);
		scaffold_node_add_child(cat, sprite);
	} else {
		scaffold_node* sprite = mason_texture_create(drawer, CAT_DRAW_ORDER, player->id == 0? P0_KITTEN_SPR : P1_KITTEN_SPR);
		scaffold_node_add_child(cat, sprite);
	}

	return cat;
}

