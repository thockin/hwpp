#include "pp_scope.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
#include "pp_datatypes.h"
#include "pp_register.h"
#include "pp_fields.h"
#include "keyed_vector.h"
using namespace std;

void
dump_scope(const pp_scope_ptr &scope)
{
	for (size_t i = 0; i < scope->datatypes().size(); i++) {
		cout << "datatype: "
		     << scope->datatypes().key_at(i) << endl;
	}
	for (size_t i = 0; i < scope->dirents.size(); i++) {
		cout << "dirent:   "
		     << scope->dirents.key_at(i) << endl;
		if (scope->dirents[i].is_scope()) {
			pp_scope_ptr sub = scope->dirents[i].as_scope();
			dump_scope(sub);
		}
	}
}

int
test_pp_scope()
{
	int ret = 0;

	/* test the basic constructor */
	pp_scope_ptr scope1 = new_pp_scope();

	/* define a datatype */
	pp_datatype_ptr type1 = new_pp_int();
	scope1->add_datatype("type1", type1);
	pp_const_datatype_ptr type2 = scope1->datatypes()["type1"];
	if (type2 != type1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}
	pp_const_datatype_ptr type3 = scope1->datatypes()[0];
	if (type3 != type1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}

	/* define a register */
	pp_binding_ptr bind1 = new_test_binding();
	pp_register_ptr reg1 = new_pp_register(bind1, 1, BITS16);
	scope1->add_register("%reg1", reg1);
	pp_register_ptr reg2 = scope1->dirents["%reg1"].as_register();
	if (reg2 != reg1) {
		PP_TEST_ERROR("pp_scope::add_register()");
		ret++;
	}

	/* define a field */
	pp_direct_field_ptr field1 = new_pp_direct_field(type1);
	field1->add_regbits(reg1, 0, pp_value(0xffff), 0);
	scope1->add_field("field1", field1);
	pp_field_ptr field2 = scope1->dirents["field1"].as_field();
	if (field2 != field1) {
		PP_TEST_ERROR("pp_scope::add_field()");
		ret++;
	}

	/* test sub-scopes */
	pp_scope_ptr scope2 = new_pp_scope();
	scope1->add_scope("subscope", scope2);

	//dump_scope(scope1);

	return ret;
}

int
main()
{
	int r;

	r = test_pp_scope();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
