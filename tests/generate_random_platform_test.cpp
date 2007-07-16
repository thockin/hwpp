#include "test_helpers.h"

int
main ()
{
	/* Lets test our tree generator! */
	pp_platform_ptr root = generate_random_platform();
	display_tree(root);
	return 0;
}
