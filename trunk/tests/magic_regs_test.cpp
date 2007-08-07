/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 *
 * A File that contains functions for searching through a tree and 
 * performing one of three actions, Display the value, Set the value or
 * tell me if this path exists in our tree.
 */

#include "pp.h"
#include "pp_test.h"
#include "pp_fields.h"
#include <iostream>
using namespace std;

int
magic_regs_test()
{
	int ret = 0;

	if (magic_zeros->read() != 0) {
		PP_TEST_ERROR("magic_zeros->read()");
		ret++;
	}
	magic_zeros->write(1);
	if (magic_zeros->read() != 0) {
		PP_TEST_ERROR("magic_zeros->write()");
		ret++;
	}

	if (magic_ones->read() != 0xffffffffffffffffULL) {
		PP_TEST_ERROR("magic_ones->read()");
		ret++;
	}
	magic_ones->write(0);
	if (magic_ones->read() != 0xffffffffffffffffULL) {
		PP_TEST_ERROR("magic_ones->write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int ret = 0;

	ret += magic_regs_test();

	return ret;
}
