#include "pp_scope.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
#include "test_helpers.h"
#include "pp_datatypes.h"
#include "pp_registers.h"
#include "pp_fields.h"
#include "keyed_vector.h"
using namespace std;

int
test_ctors()
{
	int ret = 0;

	// test the basic constructor
	pp_scope_ptr scope = new_pp_scope();

	if (!scope->is_root()) {
		TEST_ERROR("pp_scope::is_root()");
		ret++;
	}
	if (scope->parent() != scope) {
		TEST_ERROR("pp_scope::parent()");
		ret++;
	}
	if (scope->binding()) {
		TEST_ERROR("pp_scope::binding()");
		ret++;
	}
	if (scope->is_bound()) {
		TEST_ERROR("pp_scope::is_bound()");
		ret++;
	}

	// test the pp_binding constructor
	pp_binding_ptr bind = new_test_binding();
	pp_scope_ptr scope2 = new_pp_scope(bind);

	if (!scope2->is_root()) {
		TEST_ERROR("pp_scope::is_root()");
		ret++;
	}
	if (scope2->parent() != scope2) {
		TEST_ERROR("pp_scope::parent()");
		ret++;
	}
	if (scope2->binding() != bind) {
		TEST_ERROR("pp_scope::binding()");
		ret++;
	}
	if (!scope2->is_bound()) {
		TEST_ERROR("pp_scope::is_bound()");
		ret++;
	}

	return ret;
}

int
test_parentage()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();
	if (!scope->is_root()) {
		TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	pp_scope_ptr scope2 = new_pp_scope();
	if (!scope2->is_root()) {
		TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	// test the set_parent() method
	scope2->set_parent(scope);
	if (scope2->parent() != scope) {
		TEST_ERROR("pp_scope::set_parent()");
		ret++;
	}
	if (scope2->is_root()) {
		TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	return ret;
}

int
test_datatypes()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();

	// nonexistent datatypes
	if (scope->resolve_datatype("nonexistent-type") != NULL) {
		TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	// test add_datatype() and datatype()
	pp_datatype_ptr type = new_pp_int_datatype();
	scope->add_datatype("type", type);
	if (scope->datatype(0) != type) {
		TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}
	if (scope->datatype("type") != type) {
		TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}

	// test n_datatypes()
	if (scope->n_datatypes() != 1) {
		TEST_ERROR("pp_scope::n_datatypes()");
		ret++;
	}

	// test datatype_name()
	if (scope->datatype_name(0) != "type") {
		TEST_ERROR("pp_scope::datatype_name()");
		ret++;
	}

	// test resolve_datatype()
	pp_datatype_const_ptr type2 = scope->resolve_datatype("type");
	if (type2 != type) {
		TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	// test recursive resolve_datatype()
	pp_scope_ptr psub = new_pp_scope();
	psub->set_parent(scope);
	type2 = psub->resolve_datatype("type");
	if (type2 != type) {
		TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	// nonexistent type for chained scopes
	type2 = psub->resolve_datatype("nonexistent-type");
	if (type2 != NULL) {
		TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	return ret;
}

int
test_exceptions()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();

	// test out-of-bounds accesses
	ret += TEST_ASSERT(scope->n_dirents() == 0, "pp_scope::n_dirents()");
	if (scope->dirent(0) != NULL) {
		TEST_ERROR("pp_scope::dirent()");
		ret ++;
	}
	if (scope->dirent("foo") != NULL) {
		TEST_ERROR("pp_scope::dirent()");
		ret ++;
	}
	try {
		scope->dirent_name(0);
		TEST_ERROR("pp_scope::dirent_name()");
		ret ++;
	} catch (std::out_of_range &e) {
	}

	// add a field
	pp_datatype_ptr dt = new_pp_int_datatype();
	pp_constant_field_ptr field = new_pp_constant_field(dt, 0);
	scope->add_dirent("field", field);

	// add an array
	pp_array_ptr array = new_pp_array(PP_DIRENT_FIELD);
	scope->add_dirent("array", array);

	// test invalid path elements
	try {
		scope->add_dirent("123", pp_field_ptr());
		TEST_ERROR("pp_scope::add_dirent()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}
	try {
		scope->lookup_dirent("foo/123");
		TEST_ERROR("pp_scope::lookup_dirent()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}
	try {
		scope->lookup_field("foo/123");
		TEST_ERROR("pp_scope::lookup_field()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}
	try {
		scope->lookup_register("foo/123");
		TEST_ERROR("pp_scope::lookup_register()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}
	try {
		scope->lookup_scope("foo/123");
		TEST_ERROR("pp_scope::lookup_scope()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}
	try {
		scope->lookup_array("foo/123");
		TEST_ERROR("pp_scope::lookup_array()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}

	// test array screwups
	try {
		scope->lookup_dirent("field[0]");
		TEST_ERROR("pp_scope::lookup_dirent()");
		ret++;
	} catch (pp_dirent::conversion_error &e) {
	}
	if (scope->lookup_dirent("array[0]") != NULL) {
		TEST_ERROR("pp_scope::lookup_dirent()");
		ret++;
	}
	try {
		scope->add_dirent("array[0]", new_pp_scope());
		TEST_ERROR("pp_scope::add_dirent()");
		ret++;
	} catch (pp_path::invalid_error &e) {
	}

	return ret;
}

int
test_dirents()
{
	int ret = 0;

	// create a root scope
	pp_scope_ptr root = new_pp_scope();
	ret += TEST_ASSERT(root->n_dirents() == 0,
	    "pp_scope::n_dirents()");

	// create a scope and add it to the root
	pp_scope_ptr scope0 = new_pp_scope(new_test_binding());
	root->add_dirent("scope0", scope0);
	ret += TEST_ASSERT(root->n_dirents() == 1,
	    "pp_scope::n_dirents()");

	// bookmark it
	scope0->add_bookmark("bookmark");
	ret += TEST_ASSERT(scope0->has_bookmark("bookmark"),
	    "pp_scope::add_bookmark()");

	// create a field and add it to scope0
	pp_datatype_ptr dt = new_pp_int_datatype();
	pp_constant_field_ptr field1 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("field1", field1);
	ret += TEST_ASSERT(scope0->n_dirents() == 1,
	    "pp_scope::n_dirents()");

	// create a register and add it to scope0
	pp_register_ptr reg1 = new_pp_bound_register(scope0->binding(), 1, BITS16);
	scope0->add_dirent("reg1", reg1);
	ret += TEST_ASSERT(scope0->n_dirents() == 2,
	    "pp_scope::n_dirents()");

	// create a scope and add it to scope0
	pp_scope_ptr scope1 = new_pp_scope();
	scope0->add_dirent("scope1", scope1);
	ret += TEST_ASSERT(scope0->n_dirents() == 3,
	    "pp_scope::n_dirents()");

	// create an array
	pp_array_ptr array1 = new_pp_array(PP_DIRENT_FIELD);
	// create a field and add it to array1
	pp_constant_field_ptr field2 = new_pp_constant_field(dt, 0);
	array1->append(field2);
	// add array1 to scope0
	scope0->add_dirent("array1", array1);
	ret += TEST_ASSERT(scope0->n_dirents() == 4,
	    "pp_scope::n_dirents()");

	// create an array of fields in scope0
	pp_constant_field_ptr field3 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field3);
	ret += TEST_ASSERT(scope0->n_dirents() == 5,
	    "pp_scope::n_dirents()");
	ret += TEST_ASSERT(scope0->lookup_array("array2")->size() == 1,
	    "pp_scope::n_dirents()");
	pp_constant_field_ptr field4 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field4);
	ret += TEST_ASSERT(scope0->n_dirents() == 5,
	    "pp_scope::n_dirents()");
	ret += TEST_ASSERT(scope0->lookup_array("array2")->size() == 2,
	    "pp_scope::n_dirents()");

	// test dirent()
	{
		const pp_field_const_ptr &f =
		    pp_field_from_dirent(scope0->dirent(0));
		ret += TEST_ASSERT(f == field1,
		    "pp_scope::dirent(int)");
	}
	{
		const pp_field_const_ptr &f =
		    pp_field_from_dirent(scope0->dirent("field1"));
		ret += TEST_ASSERT(f == field1,
		    "pp_scope::dirent(string)");
	}
	// test dirent_name()
	{
		ret += TEST_ASSERT(scope0->dirent_name(0) == "field1",
			"pp_scope::dirent_name()");
	}
	// test lookup_field()
	{
		pp_field_const_ptr f;

		// search for a field, exists
		f = root->lookup_field("scope0/field1");
		ret += TEST_ASSERT(f == field1,
		    "pp_scope::lookup_field()");

		// search for a field, non-existing
		if (root->lookup_field("scope0/foo") != NULL) {
			TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		}

		// search for a non-field, existing
		try {
			root->lookup_field("scope0/reg1");
			TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_field("scope0/field1/foo");
			TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}
		try {
			root->lookup_field("scope0/reg1/foo");
			TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		f = root->lookup_field("scope0/scope1/../field1");
		ret += TEST_ASSERT(f == field1,
		    "pp_scope::lookup_field()");

		// search for an item through an absolute path
		f = root->lookup_field("/scope0/field1");
		ret += TEST_ASSERT(f == field1,
		    "pp_scope::lookup_field()");

		// search for an array-held item
		f = root->lookup_field("scope0/array1[0]");
		ret += TEST_ASSERT(f == field2,
		    "pp_scope::lookup_field()");

		// search for an item through a bookmark
		f = scope1->lookup_field("$bookmark/field1");
		ret += TEST_ASSERT(f == field1,
		    "pp_scope::lookup_field()");
	}
	// test lookup_register()
	{
		pp_register_const_ptr r;

		// search for a register, exists
		r = root->lookup_register("scope0/reg1");
		ret += TEST_ASSERT(r == reg1,
		    "pp_scope::lookup_register()");

		// search for a register, non-existing
		if (root->lookup_register("scope0/foo") != NULL) {
			TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		}

		// search for a non-register, existing
		try {
			root->lookup_register("scope0/field1");
			TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_register("scope0/reg1/foo");
			TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}
		try {
			root->lookup_register("scope0/field1/foo");
			TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		r = root->lookup_register("scope0/scope1/../reg1");
		ret += TEST_ASSERT(r == reg1,
		    "pp_scope::lookup_register()");
	}
	// test lookup_scope()
	{
		pp_scope_const_ptr s;

		// search for a scope, exists
		s = root->lookup_scope("scope0/scope1");
		ret += TEST_ASSERT(s == scope1,
		    "pp_scope::lookup_scope()");

		// search for a scope, non-existing
		if (root->lookup_scope("scope0/foo") != NULL) {
			TEST_ERROR("pp_scope::lookup_scope()");
			ret++;
		}

		// search for a non-scope, existing
		try {
			root->lookup_scope("scope0/field1");
			TEST_ERROR("pp_scope::lookup_scope()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		s = root->lookup_scope("scope0/scope1/../scope1");
		ret += TEST_ASSERT(s == scope1,
		    "pp_scope::lookup_scope()");

		// search for the root
		s = root->lookup_scope("/");
		ret += TEST_ASSERT(s == root,
		    "pp_scope::lookup_scope()");

		// search for an item through an absolute path
		s = root->lookup_scope("/scope0");
		ret += TEST_ASSERT(s == scope0,
		    "pp_scope::lookup_scope()");
	}
	// test lookup_array()
	{
		pp_array_const_ptr a;

		// search for an array, exists
		a = root->lookup_array("scope0/array1");
		ret += TEST_ASSERT(a == array1,
		    "pp_scope::lookup_array()");

		// search for a array, non-existing
		if (root->lookup_array("scope0/foo") != NULL) {
			TEST_ERROR("pp_scope::lookup_array()");
			ret++;
		}

		// search for a non-array, existing
		try {
			root->lookup_array("scope0/field1");
			TEST_ERROR("pp_scope::lookup_array()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		a = root->lookup_array("scope0/scope1/../array1");
		ret += TEST_ASSERT(a == array1,
		    "pp_scope::lookup_array()");
	}
	// test lookup_dirent()
	{
		pp_dirent_const_ptr de;

		// search for a field, exists
		de = root->lookup_dirent("scope0/field1");
		ret += TEST_ASSERT(pp_field_from_dirent(de) == field1,
		    "pp_scope::lookup_dirent()");

		// search for a register, exists
		de = root->lookup_dirent("scope0/reg1");
		ret += TEST_ASSERT(pp_register_from_dirent(de) == reg1,
		    "pp_scope::lookup_dirent()");

		// search for a scope, exists
		de = root->lookup_dirent("scope0/scope1");
		ret += TEST_ASSERT(pp_scope_from_dirent(de) == scope1,
		    "pp_scope::lookup_dirent()");

		// search for a dirent, non-existing
		if (root->lookup_dirent("scope0/foo") != NULL) {
			TEST_ERROR("pp_scope::lookup_dirent()");
			ret++;
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_dirent("scope0/reg1/foo");
			TEST_ERROR("pp_scope::lookup_dirent()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		de = root->lookup_dirent("scope0/scope1/../scope1");
		ret += TEST_ASSERT(pp_scope_from_dirent(de) == scope1,
		    "pp_scope::lookup_dirent()");

		// search for an array-held item
		de = root->lookup_dirent("scope0/array1");
		ret += TEST_ASSERT(pp_array_from_dirent(de) == array1,
		    "pp_scope::lookup_dirent()");
		de = root->lookup_dirent("scope0/array1[0]");
		ret += TEST_ASSERT(pp_field_from_dirent(de) == field2,
		    "pp_scope::lookup_dirent()");
	}
	// test dirent_defined()
	{
		bool found;

		// search for a field, exists
		found = root->dirent_defined("scope0/field1");
		ret += TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a register, exists
		found = root->dirent_defined("scope0/reg1");
		ret += TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a scope, exists
		found = root->dirent_defined("scope0/scope1");
		ret += TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a dirent, non-existing
		found = root->dirent_defined("scope0/foo");
		ret += TEST_ASSERT(!found, "pp_scope::dirent_defined()");

		// search for an item that is not a leaf node
		try {
			found = root->dirent_defined("scope0/reg1/foo");
			TEST_ERROR("pp_scope::dirent_defined()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		found = root->dirent_defined("scope0/scope1/../scope1");
		ret += TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/scope1/../foo");
		ret += TEST_ASSERT(!found, "pp_scope::dirent_defined()");

		// search for an array-held item
		found = root->dirent_defined("scope0/array1");
		ret += TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[]");
		ret += TEST_ASSERT(!found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[0]");
		ret += TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[1]");
		ret += TEST_ASSERT(!found, "pp_scope::dirent_defined()");
	}

	return ret;
}

TEST_LIST(
	TEST(test_ctors),
	TEST(test_parentage),
	TEST(test_datatypes),
	TEST(test_exceptions),
	TEST(test_dirents),
);
