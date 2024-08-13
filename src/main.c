#include <signal.h>

#include "scaffold.h"
#include "mason.h"

#define WIN_W 800
#define WIN_H 600
#define TITLE "boop :3"
#define FPS 0 // unlimited

#define HCELLS 6
#define VCELLS 6

int end = 0;

void handle_interrupt(int signal) {
	end = 1;
}

int main() {
	signal(SIGINT, handle_interrupt);

	scaffold_node* root = scaffold_initialize();

	scaffold_node* drawer = mason_drawer_create(WIN_W, WIN_H, TITLE, FPS);
	scaffold_node_add_child(root, drawer);

	scaffold_node* sprite = mason_sprite_create(drawer, "assets/orange-kitten.png");
	scaffold_node_add_child(root, sprite);

	scaffold_vector2 spr_size = ((mason_sprite_data*)(sprite->data))->size;
	mason_drawer_set_game_size(drawer, (int)(spr_size.x)*HCELLS, (int)(spr_size.y)*VCELLS);

	for (int i = 0; i < VCELLS; ++i) {
		for (int j = 0; j < HCELLS; ++j) {
			if (i == 0 && j == 0) continue;

			scaffold_node* sprite2 = mason_sprite_create(drawer, i % 2 == j % 2? "assets/orange-kitten.png" : "assets/black-kitten.png");
			sprite2->local_pos = (scaffold_vector2){spr_size.x * i, spr_size.y * j};
			scaffold_node_add_child(root, sprite2);
		}
	}

	while (!end) {
		scaffold_process_cleanup(root, mason_drawer_get_frame_time());
	}

	scaffold_queue_destroy(root);
}

