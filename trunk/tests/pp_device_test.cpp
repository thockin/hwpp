#include "pp_device.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
#include "test_helpers.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "keyed_vector.h"
using namespace std;

int
test_pp_device()
{
	int ret = 0;

	/* test the basic constructor */
	pp_device_ptr dev = new_pp_device();

	/* define a datatype */
	pp_datatype_ptr type1 = new_pp_int();
	dev->add_datatype("type1", type1); //FIXME: handle errors?
	pp_const_datatype_ptr type2 = dev->datatypes()["type1"];
	if (type2 != type1) {
		PP_TEST_ERROR("pp_device::add_datatype()");
		ret++;
	}
	pp_const_datatype_ptr type3 = dev->datatypes()[0];
	if (type3 != type1) {
		PP_TEST_ERROR("pp_device::add_datatype()");
		ret++;
	}

	/* test adding a field */

	pp_binding_ptr bind1 = new_test_binding();
	pp_register_ptr reg1 = new_pp_register(bind1, 1, BITS16);
	pp_direct_field_ptr field1 = new_pp_direct_field(type1);
	field1->add_regbits(reg1, 0, pp_value(0xffff), 0);
	dev->add_field("field1", field1);
	pp_const_field_ptr field2 = pp_field_from_dirent(dev->dirents()["field1"]);
	if (field2 != field1) {
		PP_TEST_ERROR("pp_device::add_field()");
		ret++;
	}

	/* test spaces */
	pp_space_ptr space1 = new_pp_space(bind1);
	dev->add_space("space1", space1);

	/* test sub-devices */
	pp_device_ptr dev2 = new_pp_device();
	dev->add_device("subdevice", dev2);

	//display_tree(dev);

	return ret;
}

int
main()
{
	int r;

	r = test_pp_device();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
