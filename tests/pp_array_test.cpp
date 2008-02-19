#include "pp_array.h"
#include "pp.h"
#include "pp_test.h"
#include "test_helpers.h"
#include "pp_dirent.h"
using namespace std;

int
test_ctors()
{
	int ret = 0;

	// test the basic constructor
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_SCOPE);
		if (array->array_type() != PP_DIRENT_SCOPE) {
			TEST_ERROR("pp_array::pp_array()");
			ret++;
		}
	}
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_REGISTER);
		if (array->array_type() != PP_DIRENT_REGISTER) {
			TEST_ERROR("pp_array::pp_array()");
			ret++;
		}
	}
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_FIELD);
		if (array->array_type() != PP_DIRENT_FIELD) {
			TEST_ERROR("pp_array::pp_array()");
			ret++;
		}
	}
	{
		pp_array_ptr array = new_pp_array(PP_DIRENT_ARRAY);
		if (array->array_type() != PP_DIRENT_ARRAY) {
			TEST_ERROR("pp_array::pp_array()");
			ret++;
		}
	}

	return ret;
}

int
test_indexing()
{
	int ret = 0;

	pp_array_ptr array = new_pp_array(PP_DIRENT_SCOPE);
	if (array->size() != 0) {
		TEST_ERROR("pp_array::size()");
		ret++;
	}

	pp_scope_ptr scope = new_pp_scope();
	array->append(scope);
	if (array->size() != 1) {
		TEST_ERROR("pp_array::append()");
		ret++;
	}

	if (array->at(0) != scope.get()) {
		TEST_ERROR("pp_array::at()");
		ret++;
	}

	try {
		pp_array_ptr array2 = new_pp_array(PP_DIRENT_FIELD);
		array2->append(new_pp_scope());
		TEST_ERROR("pp_array::append()");
		ret++;
	} catch (exception &e) {
	}

	return ret;
}

int
main()
{
	int r = 0;

	r += test_ctors();
	r += test_indexing();

	if (r)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
