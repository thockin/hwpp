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

	/* test the basic constructor */
	pp_scope_ptr scope = new_pp_scope();

	/* by default, parent == self */
	if (scope->parent() != scope.get()) {
		PP_TEST_ERROR("pp_scope::pp_scope()");
		ret++;
	}

	/* by default, the binding is NULL */
	if (scope->binding()) {
		PP_TEST_ERROR("pp_scope::pp_scope()");
		ret++;
	}

	/* test the pp_binding constructor */
	pp_binding_ptr bind = new_test_binding();
	pp_scope_ptr scope2 = new_pp_scope(bind);

	/* by default, parent == self */
	if (scope->parent() != scope.get()) {
		PP_TEST_ERROR("pp_scope::pp_scope(pp_binding)");
		ret++;
	}

	/* make sure the binding is right */
	if (scope2->binding() != bind.get()) {
		PP_TEST_ERROR("pp_scope::pp_scope(pp_binding)");
		ret++;
	}

	return ret;
}

int
test_parentage()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();

	/* test the is_root() method */
	if (!scope->is_root()) {
		PP_TEST_ERROR("pp_scope::is_root()");
		ret++;
	}

	/* test the set_parent() method */
	pp_scope_ptr psub = new_pp_scope();
	psub->set_parent(scope.get());
	if (psub->parent() != scope.get()) {
		PP_TEST_ERROR("pp_scope::set_parent()");
		ret++;
	}

	return ret;
}

int
test_datatypes()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();

	/* test add_datatype() and datatype() */
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

	/* test n_datatypes() */
	if (scope->n_datatypes() != 1) {
		PP_TEST_ERROR("pp_scope::n_datatypes()");
		ret++;
	}

	/* test datatype_name() */
	if (scope->datatype_name(0) != "type") {
		PP_TEST_ERROR("pp_scope::datatype_name()");
		ret++;
	}

	/* test resolve_datatype() */
	const pp_datatype *type2 = scope->resolve_datatype("type");
	if (type2 != type.get()) {
		PP_TEST_ERROR("pp_scope::resolve_datatype()");
		ret++;
	}

	/* test recursive resolve_datatype() */
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
test_dirents()
{
	int ret = 0;

	pp_scope_ptr scope = new_pp_scope();

	/* test add_dirent() and dirent() */
	pp_binding_ptr bind = new_test_binding();
	pp_register_ptr reg = new_pp_register(bind.get(), 1, BITS16);
	scope->add_dirent("%reg", reg);
	const pp_register *reg2;
	reg2 = pp_register_from_dirent(scope->dirent(0));
	if (reg2 != reg.get()) {
		PP_TEST_ERROR("pp_scope::add_dirent()");
		ret++;
	}
	reg2 = pp_register_from_dirent(scope->dirent("%reg"));
	if (reg2 != reg.get()) {
		PP_TEST_ERROR("pp_scope::add_dirent()");
		ret++;
	}

	/* test n_dirents() */
	if (scope->n_dirents() != 1) {
		PP_TEST_ERROR("pp_scope::n_dirents()");
		ret++;
	}

	/* test dirent_name() */
	if (scope->dirent_name(0) != "%reg") {
		PP_TEST_ERROR("pp_scope::dirent_name()");
		ret++;
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
	r += test_dirents();

	if (r)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
