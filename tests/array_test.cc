#include "hwpp.h"
#include "array.h"
#include "dirent.h"
#include "scope.h"
#include "util/test.h"

TEST(test_ctors)
{
	// test the basic constructor
	{
		hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_SCOPE);
		if (array->array_type() != hwpp::DIRENT_TYPE_SCOPE) {
			TEST_FAIL("hwpp::Array::Array()");
		}
	}
	{
		hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_REGISTER);
		if (array->array_type() != hwpp::DIRENT_TYPE_REGISTER) {
			TEST_FAIL("hwpp::Array::Array()");
		}
	}
	{
		hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_FIELD);
		if (array->array_type() != hwpp::DIRENT_TYPE_FIELD) {
			TEST_FAIL("hwpp::Array::Array()");
		}
	}
	{
		hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_ARRAY);
		if (array->array_type() != hwpp::DIRENT_TYPE_ARRAY) {
			TEST_FAIL("hwpp::Array::Array()");
		}
	}
	{
		hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_ALIAS);
		if (array->array_type() != hwpp::DIRENT_TYPE_ALIAS) {
			TEST_ERROR("hwpp::Array::Array()");
		}
	}
}

TEST(test_indexing)
{
	hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_SCOPE);
	if (array->size() != 0) {
		TEST_FAIL("hwpp::Array::size()");
	}

	hwpp::ScopePtr scope = new_hwpp_scope();
	array->append(scope);
	if (array->size() != 1) {
		TEST_FAIL("hwpp::Array::append()");
	}

	if (array->at(0) != scope) {
		TEST_FAIL("hwpp::Array::at()");
	}

	try {
		hwpp::ArrayPtr array2 = new_hwpp_array(hwpp::DIRENT_TYPE_FIELD);
		array2->append(new_hwpp_scope());
		TEST_FAIL("hwpp::Array::append()");
	} catch (std::exception &e) {
	}
}
