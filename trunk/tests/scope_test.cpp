#include "pp/pp.h"
#include "pp/scope.h"
#include "test_binding.h"
#include "test_helpers.h"
#include "pp/datatype_types.h"
#include "pp/register_types.h"
#include "pp/field_types.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// test the basic constructor
	pp::ScopePtr scope = new_pp_scope();

	if (!scope->is_root()) {
		TEST_FAIL("pp::Scope::is_root()");
	}
	if (scope->parent() != scope) {
		TEST_FAIL("pp::Scope::parent()");
	}
	if (scope->binding()) {
		TEST_FAIL("pp::Scope::binding()");
	}
	if (scope->is_bound()) {
		TEST_FAIL("pp::Scope::is_bound()");
	}

	// test the pp::Binding constructor
	pp::BindingPtr bind = new_test_binding();
	pp::ScopePtr scope2 = new_pp_scope(bind);

	if (!scope2->is_root()) {
		TEST_FAIL("pp::Scope::is_root()");
	}
	if (scope2->parent() != scope2) {
		TEST_FAIL("pp::Scope::parent()");
	}
	if (scope2->binding() != bind) {
		TEST_FAIL("pp::Scope::binding()");
	}
	if (!scope2->is_bound()) {
		TEST_FAIL("pp::Scope::is_bound()");
	}
}

TEST(test_parentage)
{
	pp::ScopePtr scope = new_pp_scope();
	if (!scope->is_root()) {
		TEST_FAIL("pp::Scope::is_root()");
	}

	pp::ScopePtr scope2 = new_pp_scope();
	if (!scope2->is_root()) {
		TEST_FAIL("pp::Scope::is_root()");
	}

	// test the set_parent() method
	scope2->set_parent(scope);
	if (scope2->parent() != scope) {
		TEST_FAIL("pp::Scope::set_parent()");
	}
	if (scope2->is_root()) {
		TEST_FAIL("pp::Scope::is_root()");
	}
}

TEST(test_datatypes)
{
	pp::ScopePtr scope = new_pp_scope();

	// nonexistent datatypes
	if (scope->datatype("nonexistent_type") != NULL) {
		TEST_FAIL("pp::Scope::datatype()");
	}
	if (scope->resolve_datatype("nonexistent_type") != NULL) {
		TEST_FAIL("pp::Scope::resolve_datatype()");
	}

	// test add_datatype() and datatype()
	pp::DatatypePtr type = new_pp_int_datatype();
	scope->add_datatype("type", type);
	if (scope->datatype(0) != type) {
		TEST_FAIL("pp::Scope::add_datatype()");
	}
	if (scope->datatype("type") != type) {
		TEST_FAIL("pp::Scope::add_datatype()");
	}

	// test n_datatypes()
	if (scope->n_datatypes() != 1) {
		TEST_FAIL("pp::Scope::n_datatypes()");
	}

	// test datatype_name()
	if (scope->datatype_name(0) != "type") {
		TEST_FAIL("pp::Scope::datatype_name()");
	}

	// test resolve_datatype()
	pp::ConstDatatypePtr type2 = scope->resolve_datatype("type");
	if (type2 != type) {
		TEST_FAIL("pp::Scope::resolve_datatype()");
	}

	// test recursive resolve_datatype()
	pp::ScopePtr psub = new_pp_scope();
	psub->set_parent(scope);
	type2 = psub->resolve_datatype("type");
	if (type2 != type) {
		TEST_FAIL("pp::Scope::resolve_datatype()");
	}

	// nonexistent type for chained scopes
	type2 = psub->resolve_datatype("nonexistent-type");
	if (type2 != NULL) {
		TEST_FAIL("pp::Scope::resolve_datatype()");
	}
}

TEST(test_exceptions)
{
	pp::ScopePtr scope = new_pp_scope();

	// test out-of-bounds accesses
	TEST_ASSERT(scope->n_dirents() == 0, "pp::Scope::n_dirents()");
	if (scope->dirent(0) != NULL) {
		TEST_FAIL("pp::Scope::dirent()");
	}
	if (scope->dirent("foo") != NULL) {
		TEST_FAIL("pp::Scope::dirent()");
	}
	try {
		scope->dirent_name(0);
		TEST_FAIL("pp::Scope::dirent_name()");
	} catch (std::out_of_range &e) {
	}

	// add a field
	pp::DatatypePtr dt = new_pp_int_datatype();
	pp::ConstantFieldPtr field = new_pp_constant_field(dt, 0);
	scope->add_dirent("field", field);

	// add an array
	pp::ArrayPtr array = new_pp_array(pp::DIRENT_TYPE_FIELD);
	scope->add_dirent("array", array);

	// test invalid path elements
	try {
		scope->add_dirent("123", pp::FieldPtr());
		TEST_FAIL("pp::Scope::add_dirent()");
	} catch (pp::Path::InvalidError &e) {
	}
	try {
		scope->lookup_dirent("foo/123");
		TEST_FAIL("pp::Scope::lookup_dirent()");
	} catch (pp::Path::InvalidError &e) {
	}

	// test array screwups
	try {
		scope->lookup_dirent("field[0]");
		TEST_FAIL("pp::Scope::lookup_dirent()");
	} catch (pp::Dirent::ConversionError &e) {
	}
	if (scope->lookup_dirent("array[0]") != NULL) {
		TEST_FAIL("pp::Scope::lookup_dirent()");
	}
	try {
		scope->add_dirent("array[0]", new_pp_scope());
		TEST_FAIL("pp::Scope::add_dirent()");
	} catch (pp::Path::InvalidError &e) {
	}
}

TEST(test_dirents)
{
	// create a root scope
	pp::ScopePtr root = new_pp_scope();
	TEST_ASSERT(root->n_dirents() == 0,
	    "pp::Scope::n_dirents()");

	// create a scope and add it to the root
	pp::ScopePtr scope0 = new_pp_scope(new_test_binding());
	scope0->set_parent(root);
	root->add_dirent("scope0", scope0);
	TEST_ASSERT(root->n_dirents() == 1,
	    "pp::Scope::n_dirents()");

	// bookmark it
	scope0->add_bookmark("bookmark");
	TEST_ASSERT(scope0->has_bookmark("bookmark"),
	    "pp::Scope::add_bookmark()");

	// create a field and add it to scope0
	pp::DatatypePtr dt = new_pp_int_datatype();
	pp::ConstantFieldPtr field1 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("field1", field1);
	TEST_ASSERT(scope0->n_dirents() == 1,
	    "pp::Scope::n_dirents()");

	// create a register and add it to scope0
	pp::RegisterPtr reg1 = new_pp_bound_register(scope0->binding(),
	                                             1, pp::BITS16);
	scope0->add_dirent("reg1", reg1);
	TEST_ASSERT(scope0->n_dirents() == 2,
	    "pp::Scope::n_dirents()");

	// create a scope and add it to scope0
	pp::ScopePtr scope1 = new_pp_scope();
	scope1->set_parent(scope0);
	scope0->add_dirent("scope1", scope1);
	TEST_ASSERT(scope0->n_dirents() == 3,
	    "pp::Scope::n_dirents()");

	// create an array
	pp::ArrayPtr array1 = new_pp_array(pp::DIRENT_TYPE_FIELD);
	// create a field and add it to array1
	pp::ConstantFieldPtr field2 = new_pp_constant_field(dt, 0);
	array1->append(field2);
	// add array1 to scope0
	scope0->add_dirent("array1", array1);
	TEST_ASSERT(scope0->n_dirents() == 4,
	    "pp::Scope::n_dirents()");

	// create an array of fields in scope0
	pp::ConstantFieldPtr field3 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field3);
	TEST_ASSERT(scope0->n_dirents() == 5, "pp::Scope::n_dirents()");
	pp::ConstArrayPtr ar = pp::array_from_dirent(
			scope0->lookup_dirent("array2"));
	TEST_ASSERT(ar->size() == 1, "pp::Array::size()");
	pp::ConstantFieldPtr field4 = new_pp_constant_field(dt, 0);
	scope0->add_dirent("array2[]", field4);
	TEST_ASSERT(scope0->n_dirents() == 5, "pp::Scope::n_dirents()");
	TEST_ASSERT(ar->size() == 2, "pp::Array::size()");

	// test dirent()
	{
		const pp::ConstFieldPtr &f =
		    pp::field_from_dirent(scope0->dirent(0));
		TEST_ASSERT(f == field1,
		    "pp::Scope::dirent(int)");
	}
	{
		const pp::ConstFieldPtr &f =
		    pp::field_from_dirent(scope0->dirent("field1"));
		TEST_ASSERT(f == field1,
		    "pp::Scope::dirent(string)");
	}
	// test dirent_name()
	{
		TEST_ASSERT(scope0->dirent_name(0) == "field1",
			"pp::Scope::dirent_name()");
	}
	// test lookup_dirent()
	{
		pp::ConstDirentPtr de;

		// search for a field, exists
		de = root->lookup_dirent("scope0/field1");
		TEST_ASSERT(pp::field_from_dirent(de) == field1,
		    "pp::Scope::lookup_dirent()");

		// search for a register, exists
		de = root->lookup_dirent("scope0/reg1");
		TEST_ASSERT(pp::register_from_dirent(de) == reg1,
		    "pp::Scope::lookup_dirent()");

		// search for a scope, exists
		de = root->lookup_dirent("scope0/scope1");
		TEST_ASSERT(pp::scope_from_dirent(de) == scope1,
		    "pp::Scope::lookup_dirent()");

		// search for a dirent, non-existing
		if (root->lookup_dirent("scope0/foo") != NULL) {
			TEST_FAIL("pp::Scope::lookup_dirent()");
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_dirent("scope0/reg1/foo");
			TEST_FAIL("pp::Scope::lookup_dirent()");
		} catch (pp::Dirent::ConversionError &e) {
		}

		// search for an item through a path with ".."
		de = root->lookup_dirent("scope0/scope1/../scope1");
		TEST_ASSERT(pp::scope_from_dirent(de) == scope1,
		    "pp::Scope::lookup_dirent()");

		// search for an array-held item
		de = root->lookup_dirent("scope0/array1");
		TEST_ASSERT(pp::array_from_dirent(de) == array1,
		    "pp::Scope::lookup_dirent()");
		de = root->lookup_dirent("scope0/array1[0]");
		TEST_ASSERT(pp::field_from_dirent(de) == field2,
		    "pp::Scope::lookup_dirent()");
	}
	// test dirent_defined()
	{
		bool found;

		// search for a field, exists
		found = root->dirent_defined("scope0/field1");
		TEST_ASSERT(found, "pp::Scope::dirent_defined()");

		// search for a register, exists
		found = root->dirent_defined("scope0/reg1");
		TEST_ASSERT(found, "pp::Scope::dirent_defined()");

		// search for a scope, exists
		found = root->dirent_defined("scope0/scope1");
		TEST_ASSERT(found, "pp::Scope::dirent_defined()");

		// search for a dirent, non-existing
		found = root->dirent_defined("scope0/foo");
		TEST_ASSERT(!found, "pp::Scope::dirent_defined()");

		// search for an item that is not a leaf node
		try {
			found = root->dirent_defined("scope0/reg1/foo");
			TEST_FAIL("pp::Scope::dirent_defined()");
		} catch (pp::Dirent::ConversionError &e) {
		}

		// search for an item through a path with ".."
		found = root->dirent_defined("scope0/scope1/../scope1");
		TEST_ASSERT(found, "pp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/scope1/../foo");
		TEST_ASSERT(!found, "pp::Scope::dirent_defined()");

		// search for an array-held item
		found = root->dirent_defined("scope0/array1");
		TEST_ASSERT(found, "pp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[]");
		TEST_ASSERT(!found, "pp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[0]");
		TEST_ASSERT(found, "pp::Scope::dirent_defined()");
		found = root->dirent_defined("scope0/array1[1]");
		TEST_ASSERT(!found, "pp::Scope::dirent_defined()");
	}
	// test resolve_path()
	{
		pp::Path final;

		// resolve a path, exists
		final = root->resolve_path("scope0/field1");
		TEST_ASSERT(final == "scope0/field1")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve a path, non-existing
		final = root->resolve_path("scope0/foo");
		TEST_ASSERT(!final.is_initialized())
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a path with leading ".."
		final = scope1->resolve_path("../field1");
		TEST_ASSERT(final == "../field1")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a path with embedded ".."
		final = root->resolve_path("scope0/scope1/../field1");
		TEST_ASSERT(final == "scope0/field1")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through an absolute path
		final = root->resolve_path("/scope0");
		TEST_ASSERT(final == "scope0")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through an absolute path
		final = scope1->resolve_path("/scope0");
		TEST_ASSERT(final == "../../scope0")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, positive index, in bounds
		final = root->resolve_path("scope0/array1[0]");
		TEST_ASSERT(final == "scope0/array1[0]")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, positive index, out of bounds
		final = root->resolve_path("scope0/array1[3]");
		TEST_ASSERT(!final.is_initialized())
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, negative index, in bounds
		final = root->resolve_path("scope0/array1[-1]");
		TEST_ASSERT(final == "scope0/array1[0]")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an array-held item, negative index, out of bounds
		final = root->resolve_path("scope0/array1[-3]");
		TEST_ASSERT(!final.is_initialized())
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a bookmark
		final = scope1->resolve_path("$bookmark/field1");
		TEST_ASSERT(final == "../field1")
		    << "pp::Scope::resolve_path(): got '" << final << "'";

		// resolve an item through a bookmark
		final = scope1->resolve_path("$bad_bookmark/field1");
		TEST_ASSERT(!final.is_initialized())
		    << "pp::Scope::resolve_path(): got '" << final << "'";
	}
}
