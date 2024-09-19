#include <stdlib.h>

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

	int new_player = data->game_manager->curr_player->id;

	if (new_player == data->old_player) goto end;

	// turn changed
	data->turn_label_data->shape.text = new_player == 0? "Orange's Turn" : "Black's Turn";
	data->turn_label->local_pos.x = (GAME_W - (new_player == 0? data->turn_p0_width : data->turn_p1_width))/2.f;

end:
	data->old_player = new_player;
}

scaffold_node* ui_create(scaffold_node* drawer, game_manager_data* game_manager) {
	ui_data* data = malloc(sizeof(ui_data));
	data->old_player = 0;
	data->game_manager = game_manager;

	scaffold_node* ui = scaffold_node_create(
		&ui_type,
		data,
		process,
		destroy
	);

	scaffold_node* turn_label = mason_label_create(drawer, TEXT_DRAW_ORDER, "Orange's Turn", TURN_FONT_SIZE);

	data->turn_p0_width = mason_drawer_get_text_width(TURN_P0_TEXT, TURN_FONT_SIZE);
	data->turn_p1_width = mason_drawer_get_text_width(TURN_P1_TEXT, TURN_FONT_SIZE);
	turn_label->local_pos.x = (GAME_W - data->turn_p0_width)/2.f;

	scaffold_node_add_child(ui, turn_label);
	data->turn_label = turn_label;
	data->turn_label_data = (mason_sprite_data*)(turn_label->data);

	return ui;
}

