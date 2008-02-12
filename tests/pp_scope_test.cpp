#include "pp_scope.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
#include "test_helpers.h"
#include "pp_datatypes.h"
#include "pp_register.h"
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
		PP_TEST_ERROR("pp_scope::is_root()");
		ret++;
	}
	if (scope->parent() != scope.get()) {
		PP_TEST_ERROR("pp_scope::parent()");
		ret++;
	}
	if (scope->binding()) {
		PP_TEST_ERROR("pp_scope::binding()");
		ret++;
	}
	if (scope->is_bound()) {
		PP_TEST_ERROR("pp_scope::is_bound()");
		ret++;
	}

	// test the pp_binding constructor
	pp_binding_ptr bind = new_test_binding();
	pp_scope_ptr scope2 = new_pp_scope(bind);

	if (!scope2->is_root()) {
		PP_TEST_ERROR("pp_scope::is_root()");
		ret++;
	}
	if (scope2->parent() != scope2.get()) {
		PP_TEST_ERROR("pp_scope::parent()");
		ret++;
	}
	if (scope2->binding() != bind.get()) {
		PP_TEST_ERROR("pp_scope::binding()");
		ret++;
	}
	if (!scope2->is_bound()) {
		PP_TEST_ERROR("pp_scope::is_bound()");
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
		PP_TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	pp_scope_ptr scope2 = new_pp_scope();
	if (!scope2->is_root()) {
		PP_TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	// test the set_parent() method
	scope2->set_parent(scope.get());
	if (scope2->parent() != scope.get()) {
		PP_TEST_ERROR("pp_scope::set_parent()");
		ret++;
	}
	if (scope2->is_root()) {
		PP_TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	return ret;
}

int
test_datatypes()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();

	// test add_datatype() and datatype()
	pp_datatype_ptr type = new_pp_int();
	scope->add_datatype("type", type);
	if (scope->datatype(0) != type.get()) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}
	if (scope->datatype("type") != type.get()) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}

	// test n_datatypes()
	if (scope->n_datatypes() != 1) {
		PP_TEST_ERROR("pp_scope::n_datatypes()");
		ret++;
	}

	// test datatype_name()
	if (scope->datatype_name(0) != "type") {
		PP_TEST_ERROR("pp_scope::datatype_name()");
		ret++;
	}

	// test resolve_datatype()
	const pp_datatype *type2 = scope->resolve_datatype("type");
	if (type2 != type.get()) {
		PP_TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	// test recursive resolve_datatype()
	pp_scope_ptr psub = new_pp_scope();
	psub->set_parent(scope.get());
	type2 = psub->resolve_datatype("type");
	if (type2 != type.get()) {
		PP_TEST_ERROR("pp_scope::resolve_datatype()");
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
	ret += PP_TEST_ASSERT(scope->n_dirents() == 0, "pp_scope::n_dirents()");
	try {
		scope->dirent(0);
		PP_TEST_ERROR("pp_scope::dirent()");
		ret ++;
	} catch (std::out_of_range &e) {
	}
	try {
		scope->dirent("foo");
		PP_TEST_ERROR("pp_scope::dirent()");
		ret ++;
	} catch (std::out_of_range &e) {
	}
	try {
		scope->dirent_name(0);
		PP_TEST_ERROR("pp_scope::dirent_name()");
		ret ++;
	} catch (std::out_of_range &e) {
	}

	return ret;
}

int
test_dirents()
{
	int ret = 0;

	// create a root scope
	pp_scope_ptr root = new_pp_scope();
	ret += PP_TEST_ASSERT(root->n_dirents() == 0,
	    "pp_scope::n_dirents()");

	// create a scope and add it to the root
	pp_scope_ptr scope0 = new_pp_scope(new_test_binding());
	root->add_dirent("scope0", scope0);
	ret += PP_TEST_ASSERT(root->n_dirents() == 1,
	    "pp_scope::n_dirents()");

	// create a field and add it to scope0
	pp_datatype_ptr dt = new_pp_int();
	pp_constant_field_ptr field1 = new_pp_constant_field(dt.get(), 0);
	scope0->add_dirent("field1", field1);
	ret += PP_TEST_ASSERT(scope0->n_dirents() == 1,
	    "pp_scope::n_dirents()");

	// create a register and add it to scope0
	pp_register_ptr reg1 = new_pp_register(scope0->binding(), 1, BITS16);
	scope0->add_dirent("reg1", reg1);
	ret += PP_TEST_ASSERT(scope0->n_dirents() == 2,
	    "pp_scope::n_dirents()");

	// create a scope and add it to scope0
	pp_scope_ptr scope1 = new_pp_scope();
	scope0->add_dirent("scope1", scope1);
	ret += PP_TEST_ASSERT(scope0->n_dirents() == 3,
	    "pp_scope::n_dirents()");

	// test dirent()
	{
		const pp_field *f = pp_field_from_dirent(scope0->dirent(0));
		ret += PP_TEST_ASSERT(f == field1.get(),
		    "pp_scope::dirent(int)");
	}
	{
		const pp_field *f = pp_field_from_dirent(
		    scope0->dirent("field1"));
		ret += PP_TEST_ASSERT(f == field1.get(),
		    "pp_scope::dirent(string)");
	}
	// test dirent_name()
	{
		ret += PP_TEST_ASSERT(scope0->dirent_name(0) == "field1",
			"pp_scope::dirent_name()");
	}
	// test lookup_field()
	{
		const pp_field *f;

		// search for a field, exists
		f = root->lookup_field("scope0/field1");
		ret += PP_TEST_ASSERT(f == field1.get(),
		    "pp_scope::lookup_field()");

		// search for a field, non-existing
		try {
			root->lookup_field("scope0/foo");
			PP_TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (std::out_of_range &e) {
		}

		// search for a non-field, existing
		try {
			root->lookup_field("scope0/reg1");
			PP_TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_field("scope0/field1/foo");
			PP_TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}
		try {
			root->lookup_field("scope0/reg1/foo");
			PP_TEST_ERROR("pp_scope::lookup_field()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		f = root->lookup_field("scope0/scope1/../field1");
		ret += PP_TEST_ASSERT(f == field1.get(),
		    "pp_scope::lookup_field()");
	}
	// test lookup_register()
	{
		const pp_register *r;

		// search for a register, exists
		r = root->lookup_register("scope0/reg1");
		ret += PP_TEST_ASSERT(r == reg1.get(),
		    "pp_scope::lookup_register()");

		// search for a register, non-existing
		try {
			root->lookup_register("scope0/foo");
			PP_TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (std::out_of_range &e) {
		}

		// search for a non-register, existing
		try {
			root->lookup_register("scope0/field1");
			PP_TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_register("scope0/reg1/foo");
			PP_TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}
		try {
			root->lookup_register("scope0/field1/foo");
			PP_TEST_ERROR("pp_scope::lookup_register()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		r = root->lookup_register("scope0/scope1/../reg1");
		ret += PP_TEST_ASSERT(r == reg1.get(),
		    "pp_scope::lookup_register()");
	}
	// test lookup_scope()
	{
		const pp_scope *s;

		// search for a scope, exists
		s = root->lookup_scope("scope0/scope1");
		ret += PP_TEST_ASSERT(s == scope1.get(),
		    "pp_scope::lookup_scope()");

		// search for a scope, non-existing
		try {
			root->lookup_scope("scope0/foo");
			PP_TEST_ERROR("pp_scope::lookup_scope()");
			ret++;
		} catch (std::out_of_range &e) {
		}

		// search for a non-scope, existing
		try {
			root->lookup_scope("scope0/field1");
			PP_TEST_ERROR("pp_scope::lookup_scope()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		s = root->lookup_scope("scope0/scope1/../scope1");
		ret += PP_TEST_ASSERT(s == scope1.get(),
		    "pp_scope::lookup_scope()");
	}
	// test lookup_dirent()
	{
		const pp_dirent *de;

		// search for a field, exists
		de = root->lookup_dirent("scope0/field1");
		ret += PP_TEST_ASSERT(pp_field_from_dirent(de) == field1.get(),
		    "pp_scope::lookup_dirent()");

		// search for a register, exists
		de = root->lookup_dirent("scope0/reg1");
		ret += PP_TEST_ASSERT(pp_register_from_dirent(de) == reg1.get(),
		    "pp_scope::lookup_dirent()");

		// search for a scope, exists
		de = root->lookup_dirent("scope0/scope1");
		ret += PP_TEST_ASSERT(pp_scope_from_dirent(de) == scope1.get(),
		    "pp_scope::lookup_dirent()");

		// search for a dirent, non-existing
		try {
			root->lookup_dirent("scope0/foo");
			PP_TEST_ERROR("pp_scope::lookup_dirent()");
			ret++;
		} catch (std::out_of_range &e) {
		}

		// search for an item that is not a leaf node
		try {
			root->lookup_dirent("scope0/reg1/foo");
			PP_TEST_ERROR("pp_scope::lookup_dirent()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		de = root->lookup_dirent("scope0/scope1/../scope1");
		ret += PP_TEST_ASSERT(pp_scope_from_dirent(de) == scope1.get(),
		    "pp_scope::lookup_dirent()");
	}
	// test dirent_defined()
	{
		bool found;

		// search for a field, exists
		found = root->dirent_defined("scope0/field1");
		ret += PP_TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a register, exists
		found = root->dirent_defined("scope0/reg1");
		ret += PP_TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a scope, exists
		found = root->dirent_defined("scope0/scope1");
		ret += PP_TEST_ASSERT(found, "pp_scope::dirent_defined()");

		// search for a dirent, non-existing
		found = root->dirent_defined("scope0/foo");
		ret += PP_TEST_ASSERT(!found, "pp_scope::dirent_defined()");

		// search for an item that is not a leaf node
		try {
			found = root->dirent_defined("scope0/reg1/foo");
			PP_TEST_ERROR("pp_scope::dirent_defined()");
			ret++;
		} catch (pp_dirent::conversion_error &e) {
		}

		// search for an item through a path with ".."
		found = root->dirent_defined("scope0/scope1/../scope1");
		ret += PP_TEST_ASSERT(found, "pp_scope::dirent_defined()");
		found = root->dirent_defined("scope0/scope1/../foo");
		ret += PP_TEST_ASSERT(!found, "pp_scope::dirent_defined()");
	}

	return ret;
}

int
main()
{
	int r = 0;

	r += test_ctors();
	r += test_parentage();
	r += test_datatypes();
	r += test_exceptions();
	r += test_dirents();

	if (r)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
