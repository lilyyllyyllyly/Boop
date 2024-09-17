#include <stdio.h>
#include <signal.h>

#include "scaffold.h"
#include "mason.h"

#include "game_manager.h"
#include "player.h"

#define WIN_W 800
#define WIN_H 600
#define TITLE "boop :3"
#define FPS 0 // unlimited

int interrupt = 0;

void handle_interrupt(int signal) {
	interrupt = 1;
}

int main() {
	signal(SIGINT, handle_interrupt);

	scaffold_node* root = scaffold_initialize();

	scaffold_node* drawer = mason_drawer_create(HCELLS * CELL_W, VCELLS * CELL_H, TITLE, FPS);
	scaffold_node_add_child(root, drawer);

	mason_drawer_set_window_size(WIN_W, WIN_H);

	scaffold_node* player0 = player_create(0);
	scaffold_node_add_child(root, player0);
	scaffold_node* player1 = player_create(1);
	scaffold_node_add_child(root, player1);

	scaffold_node* game_manager = game_manager_create(drawer, player0->data, player1->data);
	scaffold_node_add_child(root, game_manager);

	game_manager_data* gm_data = (game_manager_data*)(game_manager->data);

	while (!gm_data->ended && !interrupt) {
		scaffold_process_cleanup(root, mason_drawer_get_frame_time());
	}

	if (interrupt) {
		puts("Interrupted by SIGINT.\n");
		goto end;
	}

	if (gm_data->curr_player == gm_data->player0) {
		puts("Player 1 Wins!\n");
	} else {
		puts("Player 2 Wins!\n");
	}

end:
	scaffold_queue_destroy(root);
}

