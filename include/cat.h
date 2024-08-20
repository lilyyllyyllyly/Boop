#include "scaffold.h"

#define P0_KITTEN_SPR "assets/orange-kitten.png"
#define P1_KITTEN_SPR "assets/black-kitten.png"

extern int cat_type;

typedef struct {
	int player_id;
	int x, y;
} cat_data;

scaffold_node* cat_create(scaffold_node* drawer, int player_id, int x, int y);

