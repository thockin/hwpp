/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * A File that contains functions for searching through a tree and 
 * performing one of three actions, Display the value, Set the value or
 * tell me if this path exists in our tree.
 */

#include "pp.h"
#include "pp_test.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "utils.h"
#include "test_binding.h"
#include <iostream>
using namespace std;

int get_field_test(const pp_device_ptr tree)
{
	int return_value = 0;
	pp_path path1 ("Colour/Value");
	pp_path path2 ("ID");
	pp_path path3 ("");

	/* Search for an item that is not a field */
	if (get_field(tree, path1) != NULL) {
		return_value ++;
		PP_TEST_ERROR("Found Colour/Value which is not a Field");
	}

	/* Search for an item that is in the tree and is a field */
	if (get_field(tree, path2) == NULL) {
		return_value++;
		PP_TEST_ERROR("Couldn't Find ID");
	}

	/* Search for an item that is not a field */
	if (get_field(tree, path3) != NULL) {
		return_value++;
		PP_TEST_ERROR("Found empty path, which is not a Field");
	}
	return return_value;
}

int get_register_test(const pp_device_ptr tree)
{
	int return_value = 0;
	pp_path path1 ("Colour/Value");
	pp_path path2 ("ID");
	pp_path path3 ("");

	if (get_register(tree, path1) == NULL) {
		return_value ++;
		PP_TEST_ERROR("Couldn't Find Colour/Value");
	}

	if (get_register(tree, path2) != NULL) {
		return_value++;
		PP_TEST_ERROR("ID is not a Register");
	}

	if (get_field(tree, path3) != NULL) {
		return_value++;
		PP_TEST_ERROR("Found empty path, which is not a Register");
	}

	return return_value;
}

int main()
{
	int return_value = 0;

	pp_device_ptr root_device = new_pp_device();
	pp_datatype_ptr my_datatype = new_pp_int();

	/* add datatype to device */
	root_device->add_datatype("INT", my_datatype);

	/* create a field and add it to the device */
	pp_direct_field_ptr field1 = new_pp_direct_field(my_datatype);
	root_device->add_field("ID", field1);

	/* create a second field and add it to the device */
	pp_direct_field_ptr field2 = new_pp_direct_field(my_datatype);
	root_device->add_field("Lambda", field2);

	/* create a scope and add it to the device */
	pp_binding_ptr bind1 = new_test_binding();
	pp_space_ptr space1 = new_pp_space(bind1);
	root_device->add_space("Colour", space1);

	/* create a register and add it to the scope we just created */
	pp_binding_ptr bind2 = new_test_binding();
	pp_register_ptr reg1 = new_pp_register(bind2, 1, BITS16);
	space1->add_register("Value", reg1);

	/* test the search tool */
	return_value += get_field_test(root_device);
	return_value += get_register_test(root_device);
	//cout << "Error Count " << return_value << endl;
	return return_value;
}
