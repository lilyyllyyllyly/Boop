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

scaffold_node* start_game(scaffold_node* root, scaffold_node* drawer, game_manager_data** gm_data_ret) {
	scaffold_node* game_root = scaffold_initialize();

	scaffold_node* player0 = player_create(drawer, 0);
	scaffold_node_add_child(game_root, player0);
	scaffold_node* player1 = player_create(drawer, 1);
	scaffold_node_add_child(game_root, player1);

	scaffold_node* game_manager = game_manager_create(drawer, player0->data, player1->data);
	game_manager->local_pos = (scaffold_vector2){BOARD_OFFSET_X, BOARD_OFFSET_Y};
	scaffold_node_add_child(game_root, game_manager);

	game_manager_data* gm_data = (game_manager_data*)(game_manager->data);

	scaffold_node* ui = ui_create(drawer, gm_data);
	scaffold_node_add_child(game_root, ui);

	scaffold_node_add_child(root, game_root);

	*gm_data_ret = gm_data;
	return game_root;
}

int main() {
	signal(SIGINT, handle_interrupt);

	scaffold_node* root = scaffold_initialize();

	scaffold_node* drawer = mason_drawer_create(GAME_W, GAME_H, TITLE, FPS);
	scaffold_node_add_child(root, drawer);

	mason_drawer_set_window_size(WIN_W, WIN_H);

	scaffold_node* bg = mason_texture_create(drawer, 0, BACKGROUND_SPRITE);
	scaffold_node_add_child(root, bg);

	game_manager_data* gm_data = NULL;
	scaffold_node* game_root = start_game(root, drawer, &gm_data);

	while (!interrupt) {
		scaffold_process_cleanup(root, mason_drawer_get_frame_time());

		if (gm_data->reset) {
			// restart
			game_root->destroy(game_root);
			game_root = start_game(root, drawer, &gm_data);
		} else if (gm_data->quit) {
			goto end;
		}
	}

	puts("Interrupted by SIGINT.\n");
end:
	root->destroy(root);
}

