#include "pp_container.h"
#include "pp.h"
#include "pp_test.h"
#include "pp_datatypes.h"
using namespace std;

int
test_pp_container()
{
	int ret = 0;

	/* test the basic constructor */
	pp_container_ptr proot = new_pp_container();
	if (proot->parent() != proot) {
		PP_TEST_ERROR("pp_container::pp_container()");
		ret++;
	}
	/* test the is_root() method */
	if (!proot->is_root()) {
		PP_TEST_ERROR("pp_container::is_root()");
		ret++;
	}

	/* test add_datatype() */
	pp_datatype_ptr type1 = new_pp_int();
	proot->add_datatype("type1", type1);
	if (proot->datatypes()[0] != type1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}

	/* test resolve_datatype() */
	pp_const_datatype_ptr type2 = proot->resolve_datatype("type1");
	if (type2 != type1) {
		PP_TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	/* test the set_parent() method */
	pp_container_ptr psub = new_pp_container();
	psub->set_parent(proot);
	if (psub->parent() != proot) {
		PP_TEST_ERROR("pp_container::set_parent()");
		ret++;
	}

	/* test recursive resolve_datatype() */
	pp_const_datatype_ptr type3 = psub->resolve_datatype("type1");
	if (type3 != type1) {
		PP_TEST_ERROR("pp_scope::resolve_datatype()");
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
