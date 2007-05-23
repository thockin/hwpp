#include "pp_container.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_pp_container()
{
	int ret = 0;

	/* test the basic constructor */
	pp_container_ptr proot(new pp_container);
	if (proot->parent() != proot) {
		PP_TEST_ERROR("pp_container::pp_container()");
		ret++;
	}
	/* test the is_root() method */
	if (!proot->is_root()) {
		PP_TEST_ERROR("pp_container::is_root()");
		ret++;
	}

	/* test the set_parent() method */
	pp_container_ptr psub(new pp_container);
	psub->set_parent(proot);
	if (psub->parent() != proot) {
		PP_TEST_ERROR("pp_container::set_parent()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_container();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
