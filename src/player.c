#include <stdlib.h>

#include "scaffold.h"

int player_type = NODE_TYPE_UNASSIGNED;

#include "player.h"

static void destroy(scaffold_node* player) {
	free(player->data);
	scaffold_node_destroy(player);
}

scaffold_node* player_create(int player_id) {
	player_data* data = malloc(sizeof(player_data));
	data->id = player_id;
	data->kitten_count = MAX_PIECE_COUNT;
	data->cat_count = 0;

	return scaffold_node_create(
		&player_type,
		data,
		NULL,
		destroy
	);
}

