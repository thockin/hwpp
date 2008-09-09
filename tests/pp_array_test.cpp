#include "pp.h"
#include "pp_array.h"
#include "pp_dirent.h"
#include "test_helpers.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// test the basic constructor
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_SCOPE);
		if (array->array_type() != PP_DIRENT_SCOPE) {
			TEST_FAIL("pp_array::pp_array()");
		}
	}
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_REGISTER);
		if (array->array_type() != PP_DIRENT_REGISTER) {
			TEST_FAIL("pp_array::pp_array()");
		}
	}
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_FIELD);
		if (array->array_type() != PP_DIRENT_FIELD) {
			TEST_FAIL("pp_array::pp_array()");
		}
	}
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_ARRAY);
		if (array->array_type() != PP_DIRENT_ARRAY) {
			TEST_FAIL("pp_array::pp_array()");
		}
	}
}

TEST(test_indexing)
{
	pp_array_ptr array = new_pp_array(PP_DIRENT_SCOPE);
	if (array->size() != 0) {
		TEST_FAIL("pp_array::size()");
	}

	pp_scope_ptr scope = new_pp_scope();
	array->append(scope);
	if (array->size() != 1) {
		TEST_FAIL("pp_array::append()");
	}

	if (array->at(0) != scope) {
		TEST_FAIL("pp_array::at()");
	}

	try {
		pp_array_ptr array2 = new_pp_array(PP_DIRENT_FIELD);
		array2->append(new_pp_scope());
		TEST_FAIL("pp_array::append()");
	} catch (std::exception &e) {
	}
}
