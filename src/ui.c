#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scaffold.h"
#include "mason.h"

int ui_type = NODE_TYPE_UNASSIGNED;

#include "ui.h"

static void destroy(scaffold_node* ui) {
	free(ui->data);
	scaffold_node_destroy(ui);
}

static void process(scaffold_node* ui, double delta) {
	ui_data* data = (ui_data*)(ui->data);

	int new_player    = data->game_manager->curr_player->id;
	int new_choose_id = data->game_manager->curr_choose_id;

	if (data->game_manager->choosing_turn && !data->choosing_label) {
		// choosing turn started, create hint label
		scaffold_node* choosing_label = mason_label_create(data->drawer, TEXT_DRAW_ORDER, CHOOSING_TEXT_DEFAULT, CHOOSING_FONT_SIZE, 1);

		choosing_label->local_pos = (scaffold_vector2){
			.x = (GAME_W - data->choosing_width)/2.f,
			.y = GAME_H - CHOOSING_FONT_SIZE,
		};

		scaffold_node_add_child(ui, choosing_label);
		data->choosing_label = choosing_label;
		data->choosing_label_data = (mason_sprite_data*)(choosing_label->data);
	} else if (!data->game_manager->choosing_turn && data->choosing_label) {
		// choosing turn started, delete the label
		scaffold_queue_destroy(data->choosing_label);
		data->choosing_label = NULL;
	} else if (new_choose_id != data->old_choose_id) {
		// player selected a new cat (or selection reset), update label text
		snprintf(data->choosing_label_data->shape.text, data->choosing_strlen+1, CHOOSING_TEXT_FORMAT, data->game_manager->curr_choose_id, PLINE_CAT_COUNT);
	}

	if (new_player == data->old_player) goto end;

	// turn changed, update turn text
	data->turn_label_data->shape.text = new_player == 0? "Orange's Turn" : "Black's Turn";
	data->turn_label->local_pos.x = (GAME_W - (new_player == 0? data->turn_p0_width : data->turn_p1_width))/2.f;

end:
	data->old_player    = new_player;
	data->old_choose_id = new_choose_id;
}

scaffold_node* ui_create(scaffold_node* drawer, game_manager_data* game_manager) {
	ui_data* data = malloc(sizeof(ui_data));

	data->drawer = drawer;
	data->game_manager = game_manager;

	data->old_player = 0;
	data->old_choose_id = 0;

	scaffold_node* ui = scaffold_node_create(
		&ui_type,
		data,
		process,
		destroy
	);

	scaffold_node* turn_label = mason_label_create(drawer, TEXT_DRAW_ORDER, TURN_P0_TEXT, TURN_FONT_SIZE, 0);

	data->turn_p0_width = mason_drawer_get_text_width(TURN_P0_TEXT, TURN_FONT_SIZE);
	data->turn_p1_width = mason_drawer_get_text_width(TURN_P1_TEXT, TURN_FONT_SIZE);
	turn_label->local_pos.x = (GAME_W - data->turn_p0_width)/2.f;

	scaffold_node_add_child(ui, turn_label);
	data->turn_label = turn_label;
	data->turn_label_data = (mason_sprite_data*)(turn_label->data);

	data->choosing_label = NULL;
	data->choosing_label_data = NULL;
	data->choosing_width = mason_drawer_get_text_width(CHOOSING_TEXT_DEFAULT, CHOOSING_FONT_SIZE);
	data->choosing_strlen = strlen(CHOOSING_TEXT_DEFAULT);

	return ui;
}

