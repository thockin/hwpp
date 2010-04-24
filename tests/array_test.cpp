#include "pp/pp.h"
#include "pp/array.h"
#include "pp/dirent.h"
#include "pp/scope.h"
#include "pp/util/test.h"

TEST(test_ctors)
{
	// test the basic constructor
	{
		pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_SCOPE);
		if (array->array_type() != pp::DIRENT_TYPE_SCOPE) {
			TEST_FAIL("pp::Array::Array()");
		}
	}
	{
		pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_REGISTER);
		if (array->array_type() != pp::DIRENT_TYPE_REGISTER) {
			TEST_FAIL("pp::Array::Array()");
		}
	}
	{
		pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_FIELD);
		if (array->array_type() != pp::DIRENT_TYPE_FIELD) {
			TEST_FAIL("pp::Array::Array()");
		}
	}
	{
		pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_ARRAY);
		if (array->array_type() != pp::DIRENT_TYPE_ARRAY) {
			TEST_FAIL("pp::Array::Array()");
		}
	}
	{
		pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_ALIAS);
		if (array->array_type() != pp::DIRENT_TYPE_ALIAS) {
			TEST_ERROR("pp::Array::Array()");
		}
	}
}

TEST(test_indexing)
{
	pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_SCOPE);
	if (array->size() != 0) {
		TEST_FAIL("pp::Array::size()");
	}

	pp::ScopePtr scope = new_pp_scope();
	array->append(scope);
	if (array->size() != 1) {
		TEST_FAIL("pp::Array::append()");
	}

	if (array->at(0) != scope) {
		TEST_FAIL("pp::Array::at()");
	}

	try {
		pp::ArrayPtr array2 = new_pp_array(pp::DIRENT_TYPE_FIELD);
		array2->append(new_pp_scope());
		TEST_FAIL("pp::Array::append()");
	} catch (std::exception &e) {
	}
}
