#include <stdlib.h>

#include "scaffold.h"
#include "mason.h"

int cat_type = NODE_TYPE_UNASSIGNED;

#include "cat.h"
#include "game_manager.h"

static void destroy(scaffold_node* cat) {
	free(cat->data);
	scaffold_node_destroy(cat);
}

scaffold_node* cat_create(scaffold_node* drawer, int player_id, int x, int y) {
	cat_data* data = malloc(sizeof(cat_data));
	data->player_id = player_id;
	data->x = x;
	data->y = y;

	scaffold_node* cat = scaffold_node_create(
		&cat_type,
		data,
		NULL,
		destroy
	);

	cat->local_pos = (scaffold_vector2){x * CELL_W, y * CELL_H};

	scaffold_node* sprite = mason_sprite_create(drawer, player_id == 0? P0_KITTEN_SPR : P1_KITTEN_SPR);
	scaffold_node_add_child(cat, sprite);

	return cat;
}

