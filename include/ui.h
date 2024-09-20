#pragma once

#include "scaffold.h"
#include "mason.h"

#include "game_manager.h"

#define TEXT_DRAW_ORDER 10

#define TURN_FONT_SIZE 10
#define TURN_P0_TEXT "Orange's Turn"
#define TURN_P1_TEXT "Black's Turn"

#define CHOOSING_FONT_SIZE 10
#define CHOOSING_TEXT_DEFAULT "Choose a line of 3 kittens... (0/3)"
#define CHOOSING_TEXT_FORMAT  "Choose a line of 3 kittens... (%d/%d)"

extern int ui_type;

typedef struct {
	scaffold_node* drawer;
	game_manager_data* game_manager;

	scaffold_node* turn_label;
	mason_sprite_data* turn_label_data;
	int turn_p0_width;
	int turn_p1_width;
	int old_player;

	scaffold_node* choosing_label;
	mason_sprite_data* choosing_label_data;
	int choosing_width;
	int choosing_strlen;
	int old_choose_id;
} ui_data;

scaffold_node* ui_create(scaffold_node* drawer, game_manager_data* game_manager);

