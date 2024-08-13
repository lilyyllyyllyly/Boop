#include <signal.h>

#include "scaffold.h"
#include "mason.h"

int end = 0;

void handle_interrupt(int signal) {
	end = 1;
}

int main() {
	signal(SIGINT, handle_interrupt);

	scaffold_node* root = scaffold_initialize();

	scaffold_node* drawer = mason_drawer_create(128, 128, "boop :3", 60);
	scaffold_node_add_child(root, drawer);

	scaffold_node* sprite = mason_sprite_create(drawer, "assets/orange-kitten.png");
	scaffold_node_add_child(root, sprite);

	scaffold_node* sprite2 = mason_sprite_create(drawer, "assets/black-kitten.png");
	sprite2->local_pos = (scaffold_vector2){16, 0};
	scaffold_node_add_child(root, sprite2);

	scaffold_node* rect = mason_rectangle_create(drawer, (scaffold_vector2){16, 16});
	rect->local_pos = (scaffold_vector2){32, 0};
	scaffold_node_add_child(root, rect);

	while (!end) {
		scaffold_process_cleanup(root, mason_drawer_get_frame_time());
	}

	scaffold_queue_destroy(root);
}

