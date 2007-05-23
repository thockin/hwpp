#include "pp_space.h"
#include "pp.h"
#include "pp_test.h"
#include "pp_test_binding.h"
using namespace std;

int
test_pp_space()
{
	int ret = 0;

	/* create a binding */
	pp_binding_ptr binding(new pp_test_binding);

	/* test the space constructor */
	pp_space_ptr space(new pp_space(binding));

	/* define a constant */
	pp_value c1(1);
	space->add_constant("const1", c1);
	pp_value c2 = space->constants["const1"];
	if (c2 != c1) {
		PP_TEST_ERROR("pp_space::add_constant()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_space();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
