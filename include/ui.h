#pragma once

#include "scaffold.h"
#include "mason.h"

#include "game_manager.h"

#define TEXT_DRAW_ORDER 10

#define TURN_FONT_SIZE 10
#define TURN_P0_TEXT "Orange's Turn"
#define TURN_P1_TEXT "Black's Turn"

extern int ui_type;

typedef struct {
	game_manager_data* game_manager;
	int old_player;

	scaffold_node* turn_label;
	mason_sprite_data* turn_label_data;
	int turn_p0_width;
	int turn_p1_width;
} ui_data;

scaffold_node* ui_create(scaffold_node* drawer, game_manager_data* game_manager);

