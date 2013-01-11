#include "hwpp.h"
#include "scope.h"
#include "test_binding.h"
#include "datatype_types.h"
#include "register_types.h"
#include "field_types.h"
#include "util/test.h"

TEST(test_ctors)
{
	// test the basic constructor
	hwpp::ScopePtr scope = new_hwpp_scope();

	if (!scope->is_root()) {
		TEST_FAIL("hwpp::Scope::is_root()");
	}
	if (scope->parent() != scope) {
		TEST_FAIL("hwpp::Scope::parent()");
	}
	if (scope->binding()) {
		TEST_FAIL("hwpp::Scope::binding()");
	}
	if (scope->is_bound()) {
		TEST_FAIL("hwpp::Scope::is_bound()");
	}

	// test the hwpp::Binding constructor
	hwpp::BindingPtr bind = new_test_binding();
	hwpp::ScopePtr scope2 = new_hwpp_scope(bind);

	if (!scope2->is_root()) {
		TEST_FAIL("hwpp::Scope::is_root()");
	}
	if (scope2->parent() != scope2) {
		TEST_FAIL("hwpp::Scope::parent()");
	}
	if (scope2->binding() != bind) {
		TEST_FAIL("hwpp::Scope::binding()");
	}
	if (!scope2->is_bound()) {
		TEST_FAIL("hwpp::Scope::is_bound()");
	}
}

TEST(test_parentage)
{
	hwpp::ScopePtr scope = new_hwpp_scope();
	if (!scope->is_root()) {
		TEST_FAIL("hwpp::Scope::is_root()");
	}

	hwpp::ScopePtr scope2 = new_hwpp_scope();
	if (!scope2->is_root()) {
		TEST_FAIL("hwpp::Scope::is_root()");
	}

	// test the set_parent() method
	scope2->set_parent(scope);
	if (scope2->parent() != scope) {
		TEST_FAIL("hwpp::Scope::set_parent()");
	}
	if (scope2->is_root()) {
		TEST_FAIL("hwpp::Scope::is_root()");
	}
}

TEST(test_datatypes)
{
	hwpp::ScopePtr scope = new_hwpp_scope();

	// nonexistent datatypes
	if (scope->datatype("nonexistent_type") != NULL) {
		TEST_FAIL("hwpp::Scope::datatype()");
	}
	if (scope->resolve_datatype("nonexistent_type") != NULL) {
		TEST_FAIL("hwpp::Scope::resolve_datatype()");
	}

	// test add_datatype() and datatype()
	hwpp::DatatypePtr type = new_hwpp_int_datatype();
	scope->add_datatype("type", type);
	if (scope->datatype(0) != type) {
		TEST_FAIL("hwpp::Scope::add_datatype()");
	}
	if (scope->datatype("type") != type) {
		TEST_FAIL("hwpp::Scope::add_datatype()");
	}

	// test n_datatypes()
	if (scope->n_datatypes() != 1) {
		TEST_FAIL("hwpp::Scope::n_datatypes()");
	}

	// test datatype_name()
	if (scope->datatype_name(0) != "type") {
		TEST_FAIL("hwpp::Scope::datatype_name()");
	}

	// test resolve_datatype()
	hwpp::ConstDatatypePtr type2 = scope->resolve_datatype("type");
	if (type2 != type) {
		TEST_FAIL("hwpp::Scope::resolve_datatype()");
	}

	// test recursive resolve_datatype()
	hwpp::ScopePtr psub = new_hwpp_scope();
	psub->set_parent(scope);
	type2 = psub->resolve_datatype("type");
	if (type2 != type) {
		TEST_FAIL("hwpp::Scope::resolve_datatype()");
	}

	// nonexistent type for chained scopes
	type2 = psub->resolve_datatype("nonexistent-type");
	if (type2 != NULL) {
		TEST_FAIL("hwpp::Scope::resolve_datatype()");
	}
}

TEST(test_exceptions)
{
	hwpp::ScopePtr scope = new_hwpp_scope();

	// test out-of-bounds accesses
	TEST_ASSERT(scope->n_dirents() == 0, "hwpp::Scope::n_dirents()");
	if (scope->dirent(0) != NULL) {
		TEST_FAIL("hwpp::Scope::dirent()");
	}
	if (scope->dirent("foo") != NULL) {
		TEST_FAIL("hwpp::Scope::dirent()");
	}
	try {
		scope->dirent_name(0);
		TEST_FAIL("hwpp::Scope::dirent_name()");
	} catch (std::out_of_range &e) {
	}

	// add a field
	hwpp::DatatypePtr dt = new_hwpp_int_datatype();
	hwpp::ConstantFieldPtr field = new_hwpp_constant_field(dt, 0);
	scope->add_dirent("field", field);

	// add an array
	hwpp::ArrayPtr array = new_hwpp_array(hwpp::DIRENT_TYPE_FIELD);
	scope->add_dirent("array", array);

	// test invalid path elements
	try {
		scope->add_dirent("123", hwpp::FieldPtr());
		TEST_FAIL("hwpp::Scope::add_dirent()");
	} catch (hwpp::Path::InvalidError &e) {
	}
	try {
		scope->lookup_dirent("foo/123");
		TEST_FAIL("hwpp::Scope::lookup_dirent()");
	} catch (hwpp::Path::InvalidError &e) {
	}

	// test array screwups
	try {
		scope->lookup_dirent("field[0]");
		TEST_FAIL("hwpp::Scope::lookup_dirent()");
	} catch (hwpp::Dirent::ConversionError &e) {
	}
	if (scope->lookup_dirent("array[0]") != NULL) {
		TEST_FAIL("hwpp::Scope::lookup_dirent()");
	}
	try {
		scope->add_dirent("array[0]", new_hwpp_scope());
		TEST_FAIL("hwpp::Scope::add_dirent()");
	} catch (hwpp::Path::InvalidError &e) {
	}
}

TEST(test_dirents)
{
	// create a root scope
	hwpp::ScopePtr root = new_hwpp_scope();
	TEST_ASSERT(root->n_dirents() == 0,
	    "hwpp::Scope::n_dirents()");

	// create a scope and add it to the root
	hwpp::ScopePtr scope0 = new_hwpp_scope(new_test_binding());
	scope0->set_parent(root);
	root->add_dirent("scope0", scope0);
	TEST_ASSERT(root->n_dirents() == 1,
	    "hwpp::Scope::n_dirents()");

	// bookmark it
	scope0->add_bookmark("bookmark");
	TEST_ASSERT(scope0->has_bookmark("bookmark"),
	    "hwpp::Scope::add_bookmark()");

	// create a field and add it to scope0
	hwpp::DatatypePtr dt = new_hwpp_int_datatype();
	hwpp::ConstantFieldPtr field1 = new_hwpp_constant_field(dt, 0);
	scope0->add_dirent("field1", field1);
	TEST_ASSERT(scope0->n_dirents() == 1,
	    "hwpp::Scope::n_dirents()");

	// create a register and add it to scope0
	hwpp::RegisterPtr reg1 = new_hwpp_bound_register(scope0->binding(),
	                                             1, hwpp::BITS16);
	scope0->add_dirent("reg1", reg1);
	TEST_ASSERT(scope0->n_dirents() == 2,
	    "hwpp::Scope::n_dirents()");

	// create a scope and add it to scope0
	hwpp::ScopePtr scope1 = new_hwpp_scope();
	scope1->set_parent(scope0);
	scope0->add_dirent("scope1", scope1);
	TEST_ASSERT(scope0->n_dirents() == 3,
	    "hwpp::Scope::n_dirents()");

	// create an array
	hwpp::ArrayPtr array1 = new_hwpp_array(hwpp::DIRENT_TYPE_FIELD);
	// create a field and add it to array1
	hwpp::ConstantFieldPtr field2 = new_hwpp_constant_field(dt, 0);
	array1->append(field2);
	// add array1 to scope0
	scope0->add_dirent("array1", array1);
	TEST_ASSERT(scope0->n_dirents() == 4,
	    "hwpp::Scope::n_dirents()");

	// create an array of fields in scope0
	hwpp::ConstantFieldPtr field3 = new_hwpp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field3);
	TEST_ASSERT(scope0->n_dirents() == 5, "hwpp::Scope::n_dirents()");
	hwpp::ConstArrayPtr ar = hwpp::array_from_dirent(
			scope0->lookup_dirent("array2"));
	TEST_ASSERT(ar->size() == 1, "hwpp::Array::size()");
	hwpp::ConstantFieldPtr field4 = new_hwpp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field4);
	TEST_ASSERT(scope0->n_dirents() == 5, "hwpp::Scope::n_dirents()");
	TEST_ASSERT(ar->size() == 2, "hwpp::Array::size()");

	// test dirent()
	{
		const hwpp::ConstFieldPtr &f =
		    hwpp::field_from_dirent(scope0->dirent(0));
		TEST_ASSERT(f == field1,
		    "hwpp::Scope::dirent(int)");
	}
	{
		const hwpp::ConstFieldPtr &f =
		    hwpp::field_from_dirent(scope0->dirent("field1"));
		TEST_ASSERT(f == field1,
		    "hwpp::Scope::dirent(string)");
	}
	// test dirent_name()
	{
		TEST_ASSERT(scope0->dirent_name(0) == "field1",
			"hwpp::Scope::dirent_name()");
	}
	// test lookup_dirent()
	{
		hwpp::ConstDirentPtr de;

		// search for a field, exists
		de = root->lookup_dirent("scope0/field1");
		TEST_ASSERT(hwpp::field_from_dirent(de) == field1,
		    "hwpp::Scope::lookup_dirent()");

		// search for a register, exists
		de = root->lookup_dirent("scope0/reg1");
		TEST_ASSERT(hwpp::register_from_dirent(de) == reg1,
		    "hwpp::Scope::lookup_dirent()");

		// search for a scope, exists
		de = root->lookup_dirent("scope0/scope1");
		TEST_ASSERT(hwpp::scope_from_dirent(de) == scope1,
		    "hwpp::Scope::lookup_dirent()");

		// search for a dirent, non-existing
		if (root->lookup_dirent("scope0/foo") != NULL) {
			TEST_FAIL("hwpp::Scope::lookup_dirent()");
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_dirent("scope0/reg1/foo");
			TEST_FAIL("hwpp::Scope::lookup_dirent()");
		} catch (hwpp::Dirent::ConversionError &e) {
		}

		// search for an item through a path with ".."
		de = root->lookup_dirent("scope0/scope1/../scope1");
		TEST_ASSERT(hwpp::scope_from_dirent(de) == scope1,
		    "hwpp::Scope::lookup_dirent()");

		// search for an array-held item
		de = root->lookup_dirent("scope0/array1");
		TEST_ASSERT(hwpp::array_from_dirent(de) == array1,
		    "hwpp::Scope::lookup_dirent()");
		de = root->lookup_dirent("scope0/array1[0]");
		TEST_ASSERT(hwpp::field_from_dirent(de) == field2,
		    "hwpp::Scope::lookup_dirent()");
	}
	// test dirent_defined()
	{
		bool found;

		// search for a field, exists
		found = root->dirent_defined("scope0/field1");
		TEST_ASSERT(found, "hwpp::Scope::dirent_defined()");

		// search for a register, exists
		found = root->dirent_defined("scope0/reg1");
		TEST_ASSERT(found, "hwpp::Scope::dirent_defined()");

		// search for a scope, exists
		found = root->dirent_defined("scope0/scope1");
		TEST_ASSERT(found, "hwpp::Scope::dirent_defined()");

		// search for a dirent, non-existing
		found = root->dirent_defined("scope0/foo");
		TEST_ASSERT(!found, "hwpp::Scope::dirent_defined()");

		// search for an item that is not a leaf node
		try {
			found = root->dirent_defined("scope0/reg1/foo");
			TEST_FAIL("hwpp::Scope::dirent_defined()");
		} catch (hwpp::Dirent::ConversionError &e) {
		}

		// search for an item through a path with ".."
		found = root->dirent_defined("scope0/scope1/../scope1");
		TEST_ASSERT(found, "hwpp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/scope1/../foo");
		TEST_ASSERT(!found, "hwpp::Scope::dirent_defined()");

		// search for an array-held item
		found = root->dirent_defined("scope0/array1");
		TEST_ASSERT(found, "hwpp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[]");
		TEST_ASSERT(!found, "hwpp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[0]");
		TEST_ASSERT(found, "hwpp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[1]");
		TEST_ASSERT(!found, "hwpp::Scope::dirent_defined()");
	}
	// test resolve_path()
	{
		hwpp::Path final;

		// resolve a path, exists
		final = root->resolve_path("scope0/field1");
		TEST_ASSERT(final == "scope0/field1")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve a path, non-existing
		final = root->resolve_path("scope0/foo");
		TEST_ASSERT(!final.is_initialized())
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a path with leading ".."
		final = scope1->resolve_path("../field1");
		TEST_ASSERT(final == "../field1")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a path with embedded ".."
		final = root->resolve_path("scope0/scope1/../field1");
		TEST_ASSERT(final == "scope0/field1")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through an absolute path
		final = root->resolve_path("/scope0");
		TEST_ASSERT(final == "scope0")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through an absolute path
		final = scope1->resolve_path("/scope0");
		TEST_ASSERT(final == "../../scope0")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, positive index, in bounds
		final = root->resolve_path("scope0/array1[0]");
		TEST_ASSERT(final == "scope0/array1[0]")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, positive index, out of bounds
		final = root->resolve_path("scope0/array1[3]");
		TEST_ASSERT(!final.is_initialized())
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, negative index, in bounds
		final = root->resolve_path("scope0/array1[-1]");
		TEST_ASSERT(final == "scope0/array1[0]")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, negative index, out of bounds
		final = root->resolve_path("scope0/array1[-3]");
		TEST_ASSERT(!final.is_initialized())
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a bookmark
		final = scope1->resolve_path("$bookmark/field1");
		TEST_ASSERT(final == "../field1")
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a bookmark
		final = scope1->resolve_path("$bad_bookmark/field1");
		TEST_ASSERT(!final.is_initialized())
		    << "hwpp::Scope::resolve_path(): got '" << final << "'";
	}
}
