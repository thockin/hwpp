#include "pp.h"
#include "pp_scope.h"
#include "test_binding.h"
#include "test_helpers.h"
#include "pp_datatypes.h"
#include "pp_registers.h"
#include "pp_fields.h"
#include "keyed_vector.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// test the basic constructor
	pp_scope_ptr scope = new_pp_scope();

	if (!scope->is_root()) {
		TEST_FAIL("pp_scope::is_root()");
	}
	if (scope->parent() != scope) {
		TEST_FAIL("pp_scope::parent()");
	}
	if (scope->binding()) {
		TEST_FAIL("pp_scope::binding()");
	}
	if (scope->is_bound()) {
		TEST_FAIL("pp_scope::is_bound()");
	}

	// test the pp_binding constructor
	pp_binding_ptr bind = new_test_binding();
	pp_scope_ptr scope2 = new_pp_scope(bind);

	if (!scope2->is_root()) {
		TEST_FAIL("pp_scope::is_root()");
	}
	if (scope2->parent() != scope2) {
		TEST_FAIL("pp_scope::parent()");
	}
	if (scope2->binding() != bind) {
		TEST_FAIL("pp_scope::binding()");
	}
	if (!scope2->is_bound()) {
		TEST_FAIL("pp_scope::is_bound()");
	}
}

TEST(test_parentage)
{
	pp_scope_ptr scope = new_pp_scope();
	if (!scope->is_root()) {
		TEST_FAIL("pp_scope::is_root()");
	}

	pp_scope_ptr scope2 = new_pp_scope();
	if (!scope2->is_root()) {
		TEST_FAIL("pp_scope::is_root()");
	}

	// test the set_parent() method
	scope2->set_parent(scope);
	if (scope2->parent() != scope) {
		TEST_FAIL("pp_scope::set_parent()");
	}
	if (scope2->is_root()) {
		TEST_FAIL("pp_scope::is_root()");
	}
}

TEST(test_datatypes)
{
	pp_scope_ptr scope = new_pp_scope();

	// nonexistent datatypes
	if (scope->datatype("nonexistent_type") != NULL) {
		TEST_FAIL("pp_scope::datatype()");
	}
	if (scope->resolve_datatype("nonexistent_type") != NULL) {
		TEST_FAIL("pp_scope::resolve_datatype()");
	}

	// test add_datatype() and datatype()
	pp_datatype_ptr type = new_pp_int_datatype();
	scope->add_datatype("type", type);
	if (scope->datatype(0) != type) {
		TEST_FAIL("pp_scope::add_datatype()");
	}
	if (scope->datatype("type") != type) {
		TEST_FAIL("pp_scope::add_datatype()");
	}

	// test n_datatypes()
	if (scope->n_datatypes() != 1) {
		TEST_FAIL("pp_scope::n_datatypes()");
	}

	// test datatype_name()
	if (scope->datatype_name(0) != "type") {
		TEST_FAIL("pp_scope::datatype_name()");
	}

	// test resolve_datatype()
	pp_datatype_const_ptr type2 = scope->resolve_datatype("type");
	if (type2 != type) {
		TEST_FAIL("pp_scope::resolve_datatype()");
	}

	// test recursive resolve_datatype()
	pp_scope_ptr psub = new_pp_scope();
	psub->set_parent(scope);
	type2 = psub->resolve_datatype("type");
	if (type2 != type) {
		TEST_FAIL("pp_scope::resolve_datatype()");
	}

	// nonexistent type for chained scopes
	type2 = psub->resolve_datatype("nonexistent-type");
	if (type2 != NULL) {
		TEST_FAIL("pp_scope::resolve_datatype()");
	}
}

TEST(test_exceptions)
{
	pp_scope_ptr scope = new_pp_scope();

	// test out-of-bounds accesses
	TEST_ASSERT(scope->n_dirents() == 0, "pp_scope::n_dirents()");
	if (scope->dirent(0) != NULL) {
		TEST_FAIL("pp_scope::dirent()");
	}
	if (scope->dirent("foo") != NULL) {
		TEST_FAIL("pp_scope::dirent()");
	}
	try {
		scope->dirent_name(0);
		TEST_FAIL("pp_scope::dirent_name()");
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
		TEST_FAIL("pp_scope::add_dirent()");
	} catch (pp_path::invalid_error &e) {
	}
	try {
		scope->lookup_dirent("foo/123");
		TEST_FAIL("pp_scope::lookup_dirent()");
	} catch (pp_path::invalid_error &e) {
	}

	// test array screwups
	try {
		scope->lookup_dirent("field[0]");
		TEST_FAIL("pp_scope::lookup_dirent()");
	} catch (pp_dirent::conversion_error &e) {
	}
	if (scope->lookup_dirent("array[0]") != NULL) {
		TEST_FAIL("pp_scope::lookup_dirent()");
	}
	try {
		scope->add_dirent("array[0]", new_pp_scope());
		TEST_FAIL("pp_scope::add_dirent()");
	} catch (pp_path::invalid_error &e) {
	}
}

TEST(test_dirents)
{
	// create a root scope
	pp_scope_ptr root = new_pp_scope();
	TEST_ASSERT(root->n_dirents() == 0,
	    "pp_scope::n_dirents()");

	// create a scope and add it to the root
	pp_scope_ptr scope0 = new_pp_scope(new_test_binding());
	scope0->set_parent(root);
	root->add_dirent("scope0", scope0);
	TEST_ASSERT(root->n_dirents() == 1,
	    "pp_scope::n_dirents()");

	// bookmark it
	scope0->add_bookmark("bookmark");
	TEST_ASSERT(scope0->has_bookmark("bookmark"),
	    "pp_scope::add_bookmark()");

	// create a field and add it to scope0
	pp_datatype_ptr dt = new_pp_int_datatype();
	pp_constant_field_ptr field1 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("field1", field1);
	TEST_ASSERT(scope0->n_dirents() == 1,
	    "pp_scope::n_dirents()");

	// create a register and add it to scope0
	pp_register_ptr reg1 = new_pp_bound_register(scope0->binding(),
	                                             1, BITS16);
	scope0->add_dirent("reg1", reg1);
	TEST_ASSERT(scope0->n_dirents() == 2,
	    "pp_scope::n_dirents()");

	// create a scope and add it to scope0
	pp_scope_ptr scope1 = new_pp_scope();
	scope1->set_parent(scope0);
	scope0->add_dirent("scope1", scope1);
	TEST_ASSERT(scope0->n_dirents() == 3,
	    "pp_scope::n_dirents()");

	// create an array
	pp_array_ptr array1 = new_pp_array(PP_DIRENT_FIELD);
	// create a field and add it to array1
	pp_constant_field_ptr field2 = new_pp_constant_field(dt, 0);
	array1->append(field2);
	// add array1 to scope0
	scope0->add_dirent("array1", array1);
	TEST_ASSERT(scope0->n_dirents() == 4,
	    "pp_scope::n_dirents()");

	// create an array of fields in scope0
	pp_constant_field_ptr field3 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field3);
	TEST_ASSERT(scope0->n_dirents() == 5, "pp_scope::n_dirents()");
	pp_array_const_ptr ar = pp_array_from_dirent(
			scope0->lookup_dirent("array2"));
	TEST_ASSERT(ar->size() == 1, "pp_array::size()");
	pp_constant_field_ptr field4 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field4);
	TEST_ASSERT(scope0->n_dirents() == 5, "pp_scope::n_dirents()");
	TEST_ASSERT(ar->size() == 2, "pp_array::size()");

	// test dirent()
	{
		const pp_field_const_ptr &f =
		    pp_field_from_dirent(scope0->dirent(0));
		TEST_ASSERT(f == field1,
		    "pp_scope::dirent(int)");
	}
	{
		const pp_field_const_ptr &f =
		    pp_field_from_dirent(scope0->dirent("field1"));
		TEST_ASSERT(f == field1,
		    "pp_scope::dirent(string)");
	}
	// test dirent_name()
	{
		TEST_ASSERT(scope0->dirent_name(0) == "field1",
			"pp_scope::dirent_name()");
	}
	// test lookup_dirent()
	{
		pp_dirent_const_ptr de;

		// search for a field, exists
		de = root->lookup_dirent("scope0/field1");
		TEST_ASSERT(pp_field_from_dirent(de) == field1,
		    "pp_scope::lookup_dirent()");

		// search for a register, exists
		de = root->lookup_dirent("scope0/reg1");
		TEST_ASSERT(pp_register_from_dirent(de) == reg1,
		    "pp_scope::lookup_dirent()");

		// search for a scope, exists
		de = root->lookup_dirent("scope0/scope1");
		TEST_ASSERT(pp_scope_from_dirent(de) == scope1,
		    "pp_scope::lookup_dirent()");

		// search for a dirent, non-existing
		if (root->lookup_dirent("scope0/foo") != NULL) {
			TEST_FAIL("pp_scope::lookup_dirent()");
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_dirent("scope0/reg1/foo");
			TEST_FAIL("pp_scope::lookup_dirent()");
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		de = root->lookup_dirent("scope0/scope1/../scope1");
		TEST_ASSERT(pp_scope_from_dirent(de) == scope1,
		    "pp_scope::lookup_dirent()");

		// search for an array-held item
		de = root->lookup_dirent("scope0/array1");
		TEST_ASSERT(pp_array_from_dirent(de) == array1,
		    "pp_scope::lookup_dirent()");
		de = root->lookup_dirent("scope0/array1[0]");
		TEST_ASSERT(pp_field_from_dirent(de) == field2,
		    "pp_scope::lookup_dirent()");
	}
	// test dirent_defined()
	{
		bool found;

		// search for a field, exists
		found = root->dirent_defined("scope0/field1");
		TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a register, exists
		found = root->dirent_defined("scope0/reg1");
		TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a scope, exists
		found = root->dirent_defined("scope0/scope1");
		TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a dirent, non-existing
		found = root->dirent_defined("scope0/foo");
		TEST_ASSERT(!found, "pp_scope::dirent_defined()");

		// search for an item that is not a leaf node
		try {
			found = root->dirent_defined("scope0/reg1/foo");
			TEST_FAIL("pp_scope::dirent_defined()");
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		found = root->dirent_defined("scope0/scope1/../scope1");
		TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/scope1/../foo");
		TEST_ASSERT(!found, "pp_scope::dirent_defined()");

		// search for an array-held item
		found = root->dirent_defined("scope0/array1");
		TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[]");
		TEST_ASSERT(!found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[0]");
		TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[1]");
		TEST_ASSERT(!found, "pp_scope::dirent_defined()");
	}
	// test resolve_path()
	{
		pp_path final;

		// resolve a path, exists
		final = root->resolve_path("scope0/field1");
		TEST_ASSERT(final == "scope0/field1")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve a path, non-existing
		final = root->resolve_path("scope0/foo");
		TEST_ASSERT(!final.is_initialized())
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an item through a path with leading ".."
		final = scope1->resolve_path("../field1");
		TEST_ASSERT(final == "../field1")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an item through a path with embedded ".."
		final = root->resolve_path("scope0/scope1/../field1");
		TEST_ASSERT(final == "scope0/field1")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an item through an absolute path
		final = root->resolve_path("/scope0");
		TEST_ASSERT(final == "scope0")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an item through an absolute path
		final = scope1->resolve_path("/scope0");
		TEST_ASSERT(final == "../../scope0")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, positive index, in bounds
		final = root->resolve_path("scope0/array1[0]");
		TEST_ASSERT(final == "scope0/array1[0]")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, positive index, out of bounds
		final = root->resolve_path("scope0/array1[3]");
		TEST_ASSERT(!final.is_initialized())
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, negative index, in bounds
		final = root->resolve_path("scope0/array1[-1]");
		TEST_ASSERT(final == "scope0/array1[0]")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, negative index, out of bounds
		final = root->resolve_path("scope0/array1[-3]");
		TEST_ASSERT(!final.is_initialized())
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an item through a bookmark
		final = scope1->resolve_path("$bookmark/field1");
		TEST_ASSERT(final == "../field1")
		    << "pp_scope::resolve_path(): got '" << final << "'";

		// resolve an item through a bookmark
		final = scope1->resolve_path("$bad_bookmark/field1");
		TEST_ASSERT(!final.is_initialized())
		    << "pp_scope::resolve_path(): got '" << final << "'";
	}
}
