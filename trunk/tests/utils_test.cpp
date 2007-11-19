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

int
get_field_test(const pp_scope *tree)
{
	int return_value = 0;

	/* search for a field */
	try {
		get_field(tree, pp_path("space/field"));
	} catch (exception &e) {
		PP_TEST_ERROR("get_field()");
		return_value++;
	}

	/* search for an item that is not a field */
	try {
		get_field(tree, pp_path("space/register"));
		PP_TEST_ERROR("get_field()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item that is not a leaf node */
	try {
		get_field(tree, pp_path("space/scope"));
		PP_TEST_ERROR("get_field()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item that does not exist */
	try {
		get_field(tree, pp_path("space/garbage"));
		PP_TEST_ERROR("get_field()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item through a path with ".." */
	try {
		get_field(tree, pp_path("space/scope/../field"));
	} catch (exception &e) {
		PP_TEST_ERROR("get_field()");
		return_value++;
	}

	return return_value;
}

int
get_register_test(const pp_scope *tree)
{
	int return_value = 0;

	/* search for a register */
	try {
		get_register(tree, pp_path("space/register"));
	} catch (exception &e) {
		return_value++;
		PP_TEST_ERROR("get_register()");
	}

	/* search for an item that is not a register */
	try {
		get_register(tree, pp_path("space/field"));
		PP_TEST_ERROR("get_register()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item that is not a leaf node */
	try {
		get_register(tree, pp_path("space/scope"));
		PP_TEST_ERROR("get_register()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item that does not exist */
	try {
		get_register(tree, pp_path("space/garbage"));
		PP_TEST_ERROR("get_register()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item through a path with ".." */
	try {
		get_register(tree, pp_path("space/scope/../register"));
	} catch (exception &e) {
		PP_TEST_ERROR("get_register()");
		return_value++;
	}

	return return_value;
}

int
get_dirent_test(const pp_scope *tree)
{
	int return_value = 0;

	/* search for a register */
	try {
		get_dirent(tree, pp_path("space/register"));
	} catch (exception &e) {
		return_value++;
		PP_TEST_ERROR("get_dirent()");
	}

	/* search for a field */
	try {
		get_dirent(tree, pp_path("space/field"));
	} catch (exception &e) {
		return_value++;
		PP_TEST_ERROR("get_dirent()");
	}

	/* search for a scope */
	try {
		get_dirent(tree, pp_path("space/scope"));
	} catch (exception &e) {
		return_value++;
		PP_TEST_ERROR("get_dirent()");
	}

	/* search for an item that does not exist */
	try {
		get_dirent(tree, pp_path("space/garbage"));
		PP_TEST_ERROR("get_dirent()");
		return_value++;
	} catch (exception &e) {
	}

	/* search for an item through a path with ".." */
	try {
		get_dirent(tree, pp_path("space/scope/../register"));
	} catch (exception &e) {
		PP_TEST_ERROR("get_dirent()");
		return_value++;
	}

	return return_value;
}

int
dirent_defined_test(const pp_scope *tree)
{
	int return_value = 0;

	/* search for a register */
	if (!dirent_defined(tree, pp_path("space/register"))) {
		return_value++;
		PP_TEST_ERROR("get_dirent()");
	}

	/* search for a field */
	if (!dirent_defined(tree, pp_path("space/field"))) {
		return_value++;
		PP_TEST_ERROR("get_dirent()");
	}

	/* search for a scope */
	if (!dirent_defined(tree, pp_path("space/scope"))) {
		return_value++;
		PP_TEST_ERROR("get_dirent()");
	}

	/* search for an item that does not exist */
	if (dirent_defined(tree, pp_path("space/garbage"))) {
		PP_TEST_ERROR("get_dirent()");
		return_value++;
	}

	/* search for an item through a path with ".." */
	try {
		dirent_defined(tree, pp_path("space/scope/../register"));
	} catch (exception &e) {
		PP_TEST_ERROR("dirent_defined()");
		return_value++;
	}

	return return_value;
}

int
main()
{
	int return_value = 0;

	pp_scope_ptr root_device = new_pp_scope();
	pp_datatype_ptr my_datatype = new_pp_int();

	/* create a space and add it to the device */
	pp_binding_ptr bind1 = new_test_binding();
	pp_scope_ptr space = new_pp_scope(bind1);
	root_device->add_dirent("space", space);

	/* create a field and add it to the space */
	pp_regbits_field_ptr field1 = new_pp_regbits_field(my_datatype.get());
	space->add_dirent("field", field1);

	/* create a register and add it to the space */
	pp_register_ptr reg1 = new_pp_register(bind1.get(), 1, BITS16);
	space->add_dirent("register", reg1);

	/* create a scope and add it to the space */
	pp_scope_ptr scope1 = new_pp_scope();
	space->add_dirent("scope", scope1);

	/* test the search tool */
	return_value += get_field_test(root_device.get());
	return_value += get_register_test(root_device.get());
	return_value += get_dirent_test(root_device.get());
	return_value += dirent_defined_test(root_device.get());
	//cout << "Error Count " << return_value << endl;

	return return_value;
}
