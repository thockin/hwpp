#include "generate_test_platform.h"

int main () {
	/* Lets test our tree generator! */
	pp_platform_ptr root = generate_platform();
	display_tree(root, 0);
	return 0;
}
