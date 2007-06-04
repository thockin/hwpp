#include "pp_scope.h"
#include "pp.h"
#include "pp_test.h"
#include "pp_test_binding.h"
#include "pp_datatypes.h"
#include "pp_register.h"
#include "pp_fields.h"
#include "keyed_vector.h"
using namespace std;

void
dump_scope(const pp_scope_ptr &scope)
{
	for (size_t i = 0; i < scope->constants.size(); i++) {
		cout << "constant: ";
		cout << scope->constants.key_at(i) << " = " 
		     << scope->constants[i] << endl;
	}
	for (size_t i = 0; i < scope->datatypes.size(); i++) {
		cout << "datatype: "
		     << scope->datatypes.key_at(i) << endl;
	}
	for (size_t i = 0; i < scope->registers.size(); i++) {
		cout << "register: "
		     << scope->registers.key_at(i) << endl;
	}
	for (size_t i = 0; i < scope->dirents.size(); i++) {
		cout << "dirent:   "
		     << scope->dirents[i].type() << ": " 
		     << scope->dirents.key_at(i) << endl;
		if (scope->dirents[i].is_scope()) {
			pp_scope_ptr sub = scope->dirents[i].scope();
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

	/* define a constant */
	pp_value const1(1);
	scope1->add_constant("const1", const1);
	pp_value const2 = scope1->constants["const1"];
	if (const2 != const1) {
		PP_TEST_ERROR("pp_scope::add_constant()");
		ret++;
	}

	/* define a datatype */
	pp_datatype_ptr type1 = new_pp_int();
	scope1->add_datatype("type1", type1); //FIXME: handle errors?
	pp_datatype_ptr type2 = scope1->datatypes["type1"];
	if (type2 != type1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}
	pp_datatype_ptr type3 = scope1->datatypes[0];
	if (type3 != type1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}

	/* define a register */
	pp_binding_ptr bind1 = new_test_binding();
	pp_register_ptr reg1 = new_pp_register(bind1, 1, BITS16);
	scope1->add_register("reg1", reg1); //FIXME: handle errors
	pp_register_ptr reg2 = scope1->registers["reg1"];
	if (reg2 != reg1) {
		PP_TEST_ERROR("pp_scope::add_register()");
		ret++;
	}

	/* define a field */
	pp_direct_field_ptr field1 = new_pp_direct_field(type1);
	field1->add_regbits(reg1, 0, pp_value(0xffff), 0);
	scope1->add_field("field1", field1);
	pp_field_ptr field2 = scope1->dirents["field1"].field();
	if (field2 != field1) {
		PP_TEST_ERROR("pp_scope::add_field()");
		ret++;
	}

	/* test sub-scopes */
	pp_scope_ptr scope2 = new_pp_scope();
	scope2->add_constant("one", 1);
	scope2->add_constant("two", 2);
	scope2->add_constant("three", 3);
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
