#include <stdlib.h>

#include "scaffold.h"
#include "mason.h"

int player_type = NODE_TYPE_UNASSIGNED;

#include "player.h"
#include "cat.h"

void player_add_kitten(player_data* player) {
	scaffold_node* new_kitten = mason_texture_create(player->drawer, CAT_DRAW_ORDER + player->kitten_count, player->id == 0? P0_KITTEN_SPR : P1_KITTEN_SPR);

	new_kitten->local_pos = (scaffold_vector2){
		.x = player->id == 0? UI_P0_LIST_X : UI_P1_LIST_X,
		.y = UI_KITTEN_LIST_Y + UI_CAT_SPACING * player->kitten_count
	};

	scaffold_node_add_child(player->node, new_kitten);

	player->kittens = scaffold_list_insert(player->kittens, new_kitten);

	++player->kitten_count;
}

void player_del_kitten(player_data* player) {
	if (player->kittens == NULL) return;

	// delete first kitten
	scaffold_queue_destroy((scaffold_node*)(player->kittens->data));
	player->kittens = scaffold_list_delete_element(player->kittens, player->kittens);

	--player->kitten_count;
}

void player_add_cat(player_data* player) {
	scaffold_node* new_cat = mason_texture_create(player->drawer, CAT_DRAW_ORDER + player->cat_count, player->id == 0? P0_CAT_SPR : P1_CAT_SPR);

	new_cat->local_pos = (scaffold_vector2){
		.x = player->id == 0? UI_P0_LIST_X : UI_P1_LIST_X,
		.y = UI_CAT_LIST_Y + UI_CAT_SPACING * player->cat_count
	};

	scaffold_node_add_child(player->node, new_cat);

	player->cats = scaffold_list_insert(player->cats, new_cat);

	++player->cat_count;
}

void player_del_cat(player_data* player) {
	if (player->cats == NULL) return;

	// delete first cat
	scaffold_queue_destroy((scaffold_node*)(player->cats->data));
	player->cats = scaffold_list_delete_element(player->cats, player->cats);

	--player->cat_count;
}

static void destroy(scaffold_node* player) {
	free(player->data);
	scaffold_node_destroy(player);
}

scaffold_node* player_create(scaffold_node* drawer, int player_id) {
	player_data* data = malloc(sizeof(player_data));

	data->drawer = drawer;
	data->id = player_id;

	data->kitten_count = 0;
	data->cat_count = 0;

	data->kittens = NULL;
	data->cats    = NULL;

	scaffold_node* player = scaffold_node_create(
		&player_type,
		data,
		NULL,
		destroy
	);

	data->node = player;

	for (int i = 0; i < MAX_PIECE_COUNT; ++i) {
		player_add_kitten(data);
	}

	return player;
}

