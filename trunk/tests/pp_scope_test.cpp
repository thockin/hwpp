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
		     << scope->dirents[i]->type() << ": " 
		     << scope->dirents.key_at(i) << endl;
		if (scope->dirents[i]->type() == PP_DIRENT_SCOPE) {
			pp_dirent_ptr tmp = scope->dirents[i];
			pp_scope_ptr sub = pp_scope_from_dirent(tmp);
			dump_scope(sub);
		}
	}
}

int
test_pp_scope()
{
	int ret = 0;

	/* test the basic constructor */
	pp_scope_ptr s1(new pp_scope);

	pp_binding_ptr ts1(new pp_test_binding);

	/* define a constant */
	pp_value c1(1);
	s1->add_constant("const1", c1);
	pp_value c2 = s1->constants["const1"];
	if (c2 != c1) {
		PP_TEST_ERROR("pp_scope::add_constant()");
		ret++;
	}

	/* define a datatype */
	pp_datatype_ptr dt1(new pp_int);
	s1->add_datatype("dt1", dt1); //FIXME: handle errors?
	pp_datatype_ptr pdt1 = s1->datatypes["dt1"];
	if (pdt1 != dt1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}
	pp_datatype_ptr pdt2 = s1->datatypes[0];
	if (pdt2 != dt1) {
		PP_TEST_ERROR("pp_scope::add_datatype()");
		ret++;
	}

	/* define a register */
	pp_register_ptr r1(new pp_register(ts1, 1, BITS16));
	s1->add_register("r1", r1); //FIXME: handle errors
	pp_register_ptr pr1 = s1->registers["r1"];
	if (pr1 != r1) {
		PP_TEST_ERROR("pp_scope::add_register()");
		ret++;
	}

	/* define a field */
	pp_direct_field_ptr f1(new pp_direct_field(pdt1));
	f1->add_regbits(pr1, 0, pp_value(0xffff), 0);
	s1->add_field("f1", f1);
	pp_field_ptr pf1 = pp_field_from_dirent(s1->dirents["f1"]);
	if (pf1 != f1) {
		PP_TEST_ERROR("pp_scope::add_field()");
		ret++;
	}

	/* test sub-scopes */
	pp_scope_ptr s2(new pp_scope);
	s2->add_constant("one", 1);
	s2->add_constant("two", 2);
	s2->add_constant("three", 3);
	s1->add_scope("subscope", s2);

	//dump_scope(s1);

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
