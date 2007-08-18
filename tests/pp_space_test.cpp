#include "pp_space.h"
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
test_pp_space()
{
	int ret = 0;

	/* test the basic constructor */
	pp_binding_ptr bind1 = new_test_binding();
	pp_space_ptr space1 = new_pp_space(bind1);

	/* define a datatype */
	pp_datatype_ptr type1 = new_pp_int();
	space1->add_datatype("type1", type1); //FIXME: handle errors?
	pp_const_datatype_ptr type2 = space1->datatype("type1");
	if (type2 != type1) {
		PP_TEST_ERROR("pp_space::add_datatype()");
		ret++;
	}
	pp_const_datatype_ptr type3 = space1->datatype(0);
	if (type3 != type1) {
		PP_TEST_ERROR("pp_space::add_datatype()");
		ret++;
	}

	/* define a field */
	pp_register_ptr reg1 = new_pp_register(bind1.get(), 1, BITS16);
	pp_direct_field_ptr field1 = new_pp_direct_field(type1);
	field1->add_regbits(reg1.get(), 0, pp_value(0xffff), 0);
	space1->add_field("field1", field1);
	const pp_field *field2 =
	    pp_field_from_dirent(space1->dirent("field1"));
	if (field2 != field1.get()) {
		PP_TEST_ERROR("pp_space::add_field()");
		ret++;
	}

	/* test sub-scopes */
	pp_scope_ptr scope1 = new_pp_scope();
	space1->add_scope("subscope", scope1);

	//display_tree(space1);

	return ret;
}

int
main()
{
	int r;

	r = test_pp_space();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
