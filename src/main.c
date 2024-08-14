#include <signal.h>

#include "scaffold.h"
#include "mason.h"

#include "game_manager.h"

#define WIN_W 800
#define WIN_H 600
#define TITLE "boop :3"
#define FPS 0 // unlimited

int end = 0;

void handle_interrupt(int signal) {
	end = 1;
}

int main() {
	signal(SIGINT, handle_interrupt);

	scaffold_node* root = scaffold_initialize();

	scaffold_node* drawer = mason_drawer_create(HCELLS * CELL_W, VCELLS * CELL_H, TITLE, FPS);
	scaffold_node_add_child(root, drawer);

	mason_drawer_set_window_size(WIN_W, WIN_H);

	scaffold_node* game_manager = game_manager_create(drawer);
	scaffold_node_add_child(root, game_manager);

	while (!end) {
		scaffold_process_cleanup(root, mason_drawer_get_frame_time());
	}

	scaffold_queue_destroy(root);
}

