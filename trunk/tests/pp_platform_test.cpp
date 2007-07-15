#include "pp_platform.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "keyed_vector.h"
#include <iostream>

using namespace std;

void dump_scope(const pp_scope_ptr &scope)
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
	for (size_t i = 0; i < scope->dirents.size(); i++) {
		cout << "dirent:   "
		     << scope->dirents.key_at(i) << endl;
		if (scope->dirents[i].is_scope()) {
			pp_scope_ptr sub = scope->dirents[i].as_scope();
			dump_scope(sub);
		}
	}
}
void
dump_device(const pp_device_ptr &dev)
{
	for (size_t i = 0; i < dev->constants.size(); i++) {
		cout << "constant: ";
		cout << dev->constants.key_at(i) << " = "
		     << dev->constants[i] << endl;
	}
	for (size_t i = 0; i < dev->datatypes.size(); i++) {
		cout << "datatype: "
		     << dev->datatypes.key_at(i) << endl;
	}
	for (size_t i = 0; i < dev->dirents.size(); i++) {
		cout << "dirent:   "
		     << dev->dirents.key_at(i) << endl;
		if (dev->dirents[i].is_space()) {
			pp_space_ptr sub = dev->dirents[i].as_space();
			dump_scope(sub);
		} else if (dev->dirents[i].is_device()) {
			pp_device_ptr sub = dev->dirents[i].as_device();
			dump_device(sub);
		}
	}
}

void
dump_device(const pp_platform_ptr &dev)
{
	for (size_t i = 0; i < dev->constants.size(); i++) {
		cout << "constant: ";
		cout << dev->constants.key_at(i) << " = "
		     << dev->constants[i] << endl;
	}
	for (size_t i = 0; i < dev->datatypes.size(); i++) {
		cout << "datatype: "
		     << dev->datatypes.key_at(i) << endl;
	}
	for (size_t i = 0; i < dev->dirents.size(); i++) {
		cout << "dirent:   "
		     << dev->dirents.key_at(i) << endl;
		if (dev->dirents[i].is_space()) {
			pp_space_ptr sub = dev->dirents[i].as_space();
			dump_scope(sub);
		} else if (dev->dirents[i].is_device()) {
			pp_device_ptr sub = dev->dirents[i].as_device();
			dump_device(sub);
		}
	}
}


int
test_pp_platform()
{
	int ret = 0;

	/* test the basic constructor */
	pp_platform_ptr dev = new_pp_platform();


	/* define a constant */
	pp_value val1(1);
	dev->add_constant("const1", val1);
	pp_value val2 = dev->constants["const1"];
	if (val2 != val1) {
		PP_TEST_ERROR("pp_platform::add_constant()");
		ret++;
	}

	/* define a datatype */
	pp_datatype_ptr type1 = new_pp_int();
	dev->add_datatype("type1", type1); //FIXME: handle errors?
	pp_datatype_ptr type2 = dev->datatypes["type1"];
	if (type2 != type1) {
		PP_TEST_ERROR("pp_platform::add_datatype()");
		ret++;
	}
	pp_datatype_ptr type3 = dev->datatypes[0];
	if (type3 != type1) {
		PP_TEST_ERROR("pp_platform::add_datatype()");
		ret++;
	}

	/* test adding a field */

	pp_binding_ptr bind1 = new_test_binding();
	pp_register_ptr reg1 = new_pp_register(bind1, 1, BITS16);
	pp_direct_field_ptr field1 = new_pp_direct_field(type1);
	field1->add_regbits(reg1, 0, pp_value(0xffff), 0);
	dev->add_field("field1", field1);
	pp_field_ptr field2 = dev->dirents["field1"].as_field();
	if (field2 != field1) {
		PP_TEST_ERROR("pp_platform::add_field()");
		ret++;
	}

	/* test spaces */
	pp_space_ptr space1 = new_pp_space(bind1);
	space1->add_constant("one", 1);
	space1->add_constant("two", 2);
	space1->add_constant("three", 3);
	dev->add_space("space1", space1);

	/* test sub-devices */
	pp_platform_ptr dev2 = new_pp_platform();
	dev2->add_constant("one", 1);
	dev2->add_constant("two", 2);
	dev2->add_constant("three", 3);
	dev->add_device("subdevice", dev2);

	//dump_device(dev);

	return ret;
}

int
main()
{
	int r;

	r = test_pp_platform();
        cout << "Error count " << r << endl;
	if (r) return EXIT_FAILURE;
	return EXIT_SUCCESS;
}