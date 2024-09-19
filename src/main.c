#include <stdio.h>
#include <signal.h>

#include "scaffold.h"
#include "mason.h"

#include "game_manager.h"
#include "ui.h"
#include "player.h"

#define BACKGROUND_SPRITE "assets/bg.png"

int interrupt = 0;

void handle_interrupt(int signal) {
	interrupt = 1;
}

int main() {
	signal(SIGINT, handle_interrupt);

	scaffold_node* root = scaffold_initialize();

	scaffold_node* drawer = mason_drawer_create(GAME_W, GAME_H, TITLE, FPS);
	scaffold_node_add_child(root, drawer);

	mason_drawer_set_window_size(WIN_W, WIN_H);

	scaffold_node* player0 = player_create(drawer, 0);
	scaffold_node_add_child(root, player0);
	scaffold_node* player1 = player_create(drawer, 1);
	scaffold_node_add_child(root, player1);

	scaffold_node* game_manager = game_manager_create(drawer, player0->data, player1->data);
	game_manager->local_pos = (scaffold_vector2){BOARD_OFFSET_X, BOARD_OFFSET_Y};
	scaffold_node_add_child(root, game_manager);

	game_manager_data* gm_data = (game_manager_data*)(game_manager->data);

	scaffold_node* ui = ui_create(drawer, gm_data);
	scaffold_node_add_child(root, ui);

	scaffold_node* bg = mason_texture_create(drawer, 0, BACKGROUND_SPRITE);
	scaffold_node_add_child(root, bg);

	while (!gm_data->ended && !interrupt) {
		scaffold_process_cleanup(root, mason_drawer_get_frame_time());
	}

	if (interrupt) {
		puts("Interrupted by SIGINT.\n");
		goto end;
	}

	if (gm_data->curr_player == gm_data->player0) {
		puts("Orange Wins!\n");
	} else {
		puts("Black Wins!\n");
	}

end:
	scaffold_queue_destroy(root);
}

