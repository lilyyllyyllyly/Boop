#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "scaffold.h"
#include "mason.h"

int game_manager_type = NODE_TYPE_UNASSIGNED;

#include "game_manager.h"
#include "cat.h"
#include "player.h"

void game_manager_set_cell(scaffold_node* game_manager, int x, int y, scaffold_node* value) {
	((game_manager_data*)(game_manager->data))->cells[y][x] = value;
}

scaffold_node* game_manager_get_cell(scaffold_node* game_manager, int x, int y) {
	return ((game_manager_data*)(game_manager->data))->cells[y][x];
}

int is_cell_valid(int x, int y) {
	return x >= 0 && x < HCELLS && y >= 0 && y < VCELLS;
}

int is_cell_adjacent(int x1, int y1, int x2, int y2) {
	return abs(x1 - x2) < 2 && abs(y1 - y2) < 2;
}

int line_contains_cat(promotion_line line, cat_data* cat) {
	for (int i = 0; i < PLINE_CAT_COUNT; ++i) {
		if (line.datas[i] == cat) return 1;
	}

	return 0;
}

static void push_away(game_manager_data* data, scaffold_node* pusher, int pushed_x, int pushed_y) {
	cat_data* pusher_data = (cat_data*)(pusher->data);

	scaffold_node* pushed = data->cells[pushed_y][pushed_x];
	cat_data* pushed_data = (cat_data*)(pushed->data);

	// only big cats can boop other big cats
	if (pusher_data->level < pushed_data->level) return;

	// calculate boop direction
	int new_x = 2*pushed_x - pusher_data->x;
	int new_y = 2*pushed_y - pusher_data->y;

	// move cat to new position
	cat_move(pushed, new_x, new_y);
}

int check_promotion(game_manager_data* data, scaffold_node* cat, scaffold_vector2 other1_pos, scaffold_vector2 other2_pos) {
	if (!is_cell_valid(other1_pos.x, other1_pos.y) || !is_cell_valid(other2_pos.x, other2_pos.y)) return 0; // abort if either position is invalid

	scaffold_node* other1 = data->cells[(int)other1_pos.y][(int)other1_pos.x];
	scaffold_node* other2 = data->cells[(int)other2_pos.y][(int)other2_pos.x];

	if (other1 == NULL || other2 == NULL) return 0; // if either position doesnt have a cat, promotion wont happen

	cat_data* cat1_data   = (cat_data*)(cat->data);
	cat_data* other1_data = (cat_data*)(other1->data);
	cat_data* other2_data = (cat_data*)(other2->data);

	if (other1_data->player->id != cat1_data->player->id || other2_data->player->id != cat1_data->player->id) return 0; // if any kitten is from a different player, promotion wont happen

	if (other1_data->level > 0 && other2_data->level > 0 && cat1_data->level > 0) {
		data->ended = 1; // if all are cats, game ended
		return 1; // count as promotion to not check other possibilities
	} else if (other1_data->level > 0 || other2_data->level > 0 || cat1_data->level > 0) {
		return 0; // otherwise, if any of them is a cat not a kitten, promotion wont happen
	}

	if (cat1_data->promotion_lines && other1_data->promotion_lines && other2_data->promotion_lines) {
		return 0; // if all cats are already in a line, this line has probably already been counted (there may be situations where this is not true, may deal with this later)
	}

	// queue for promotion
	promotion_line* new_line = malloc(sizeof(promotion_line));
	*new_line = (promotion_line){
		.cats  = {cat,       other1,      other2},
		.datas = {cat1_data, other1_data, other2_data},
	};
	data->promotion_lines = scaffold_list_insert(data->promotion_lines, new_line);

	++cat1_data->promotion_lines;
	++other1_data->promotion_lines;
	++other2_data->promotion_lines;

	return 1;
}

static void process(scaffold_node* game_manager, double delta) {
	game_manager_data* data = (game_manager_data*)(game_manager->data);

	int promote = 0;
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !(promote = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && data->curr_player->cat_count)) return;

	// on click

	// get mouse position
	scaffold_vector2 mouse_pos = mason_drawer_screen_to_game_pos(data->drawer, (scaffold_vector2){GetMouseX(), GetMouseY()});
	int xid = mouse_pos.x / CELL_W;
	int yid = mouse_pos.y / CELL_H;

	if (data->choosing_turn) { // there are 2 or more available promotion lines, player must choose one
		printf("CHOOSING TURN - selection number %d\n", data->curr_choose_id);

		if (!is_cell_valid(xid, yid) || !data->cells[yid][xid]) return; // abort if the cell is invalid or there isnt a cat there

		scaffold_node* cat = data->cells[yid][xid];
		cat_data* cat_data_ = (cat_data*)(cat->data);

		if (cat_data_->player != data->curr_player) return; // abort if the player chose the opponent's cat

		if (cat_data_->promotion_lines < 1) return; // abort if player clicked a cat that isnt in a line

		// add chosen cat to the chosen line
		data->chosen_line.cats[data->curr_choose_id] = cat;
		data->chosen_line.datas[data->curr_choose_id] = cat_data_;

		if (data->curr_choose_id <= 0) { // if it was the first cat chosen, end here
			++data->curr_choose_id;
			return;
		}

		// otherwise, check if there is a line that contains all the chosen cats

		scaffold_list* elem = data->promotion_lines;

		for (; elem != NULL; elem = elem->next) { // for each line
			promotion_line* line = (promotion_line*)(elem->data);

			for (int i = 0; i <= data->curr_choose_id; ++i) { // for each chosen cat
				cat_data* curr_cat = data->chosen_line.datas[i];
				if (!line_contains_cat(*line, curr_cat)) goto line_invalid; // if its not on the line, the line is invalid
			}

			// all cats are in the line
			break;

			// one of the cats wasnt in the line (continue)
			line_invalid:;
		}

		if (elem == NULL) { // no valid line containing all the chosen cats, restart selection
			printf("INVALID CHOICE\n");
			data->curr_choose_id = 0;
			return;
		}

		if (data->curr_choose_id < PLINE_CAT_COUNT-1) { // selected one of the first two cats, move on to next one
			++data->curr_choose_id;
			return;
		}

		// last cat chosen

		// delete line
		for (int i = 0; i < PLINE_CAT_COUNT; ++i) {
			data->chosen_line.datas[i]->promote = 1;
			scaffold_queue_destroy(data->chosen_line.cats[i]);
		}

		// reset promotion_lines
		for (scaffold_list* elem = data->promotion_lines; elem != NULL; elem = elem->next) {
			free(elem->data);
		}

		scaffold_list_destroy(data->promotion_lines);
		data->promotion_lines = NULL;

		// reset cat promotion line counts
		for (scaffold_node* child = game_manager->first_child; child != NULL; child = child->next_sibling) {
			cat_data* child_data = (cat_data*)(child->data);
			child_data->promotion_lines = 0;
		}

		// go to next player's regular turn
		data->choosing_turn = 0;
		goto next_turn;
	}

	if (data->curr_player->kitten_count <= 0 && data->curr_player->cat_count <= 0) { // player has no cats or kittens, promote one placed kitten
		if (!is_cell_valid(xid, yid) || !data->cells[yid][xid]) return; // abort if theres no cat in the position (or its invalid)

		scaffold_node* cat = data->cells[yid][xid];
		cat_data* cat_data_ = (cat_data*)(cat->data);

		if (cat_data_->player != data->curr_player) return; // abort if the player chose the opponent's cat

		if (cat_data_->level > 0) { // if the player clicked a cat, simply remove it from the board
			scaffold_queue_destroy(cat);
			goto next_turn;
		}

		// if the player clicked a kitten, promote it
		cat_data_->promote = 1;
		scaffold_queue_destroy(cat);

		goto next_turn;
	} else if (data->curr_player->kitten_count <= 0 && !promote) { // abort if player tried to place a kitten with no kittens left, but still has cats left
		return;
	}

	if (!is_cell_valid(xid, yid) || data->cells[yid][xid]) return; // abort if theres already a cat in the position (or its invalid)

	// spawn cat
	scaffold_node* cat = cat_create(data->drawer, game_manager, data->curr_player, xid, yid, promote);
	scaffold_node_add_child(game_manager, cat);
	data->cells[yid][xid] = cat;

	// push cats around it
	for (int pushed_y = yid-1; pushed_y <= yid+1; ++pushed_y) {
		for (int pushed_x = xid-1; pushed_x <= xid+1; ++pushed_x) {
			if (!is_cell_valid(pushed_x, pushed_y) || !data->cells[pushed_y][pushed_x]) continue;
			if (pushed_x == xid && pushed_y == yid) continue;
			push_away(data, cat, pushed_x, pushed_y);
		}
	}

	// check for promotion
	for (scaffold_node* cat = game_manager->first_child; cat != NULL; cat = cat->next_sibling) {
		cat_data* cat_data_ = (cat_data*)(cat->data);
		int cat_x = cat_data_->x;
		int cat_y = cat_data_->y;

		{// horizontal
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x-1, cat_y};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x+1, cat_y};
			if (check_promotion(data, cat, other1_pos, other2_pos)) goto check_end;
		}

		{// vertical
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x, cat_y-1};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x, cat_y+1};
			if (check_promotion(data, cat, other1_pos, other2_pos)) goto check_end;
		}

		{// diagonal (top-left to bottom-right)
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x-1, cat_y-1};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x+1, cat_y+1};
			if (check_promotion(data, cat, other1_pos, other2_pos)) goto check_end;
		}

		{// diagonal (bottom-left to top-right)
			scaffold_vector2 other1_pos = (scaffold_vector2){cat_x-1, cat_y+1};
			scaffold_vector2 other2_pos = (scaffold_vector2){cat_x+1, cat_y-1};
			if (check_promotion(data, cat, other1_pos, other2_pos)) goto check_end;
		}

check_end:
		if (data->ended) return;
	}

	if (!data->promotion_lines) goto next_turn;

	if (!data->promotion_lines->next) {
		// only one promotion line, player doesnt need to pick one
		promotion_line* line = (promotion_line*)(data->promotion_lines->data);

		for (int i = 0; i < PLINE_CAT_COUNT; ++i) {
			line->datas[i]->promote = 1;
			scaffold_queue_destroy(line->cats[i]);
		}

		free(line);
		data->promotion_lines = NULL;

		goto next_turn;
	}

	// multiple promotion lines, player must pick one
	data->choosing_turn = 1;
	return;

next_turn:
	// other player's turn
	data->curr_player = data->curr_player == data->player0? data->player1 : data->player0;

	printf("player 0: %d kittens, %d cats\n", data->player0->kitten_count, data->player0->cat_count);
	printf("player 1: %d kittens, %d cats\n", data->player1->kitten_count, data->player1->cat_count);
}

static void destroy(scaffold_node* game_manager) {
	game_manager_data* data = (game_manager_data*)(game_manager->data);

	for (scaffold_list* elem = data->promotion_lines; elem != NULL; elem = elem->next) {
		free(elem->data);
	}

	scaffold_list_destroy(data->promotion_lines);

	free(data);

	scaffold_node_destroy(game_manager);
}

scaffold_node* game_manager_create(scaffold_node* drawer, player_data* player0, player_data* player1) {
	game_manager_data* data = calloc(1, sizeof(game_manager_data));
	data->drawer = drawer;

	data->curr_player = player0;
	data->player0 = player0;
	data->player1 = player1;

	return scaffold_node_create(
		&game_manager_type,
		data,
		process,
		destroy
	);
}

