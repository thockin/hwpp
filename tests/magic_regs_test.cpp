/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 */

#include "pp.h"
#include "pp_register.h"
#include "pp_test.h"

extern pp_register_ptr magic_zeros;
extern pp_register_ptr magic_ones;

TEST(magic_regs_test)
{
	if (magic_zeros->read() != 0) {
		TEST_ERROR("magic_zeros->read()");
	}
	magic_zeros->write(1);
	if (magic_zeros->read() != 0) {
		TEST_ERROR("magic_zeros->write()");
	}

	pp_value ones = pp_value("0xffffffffffffffffffffffffffffffff");
	if (magic_ones->read() != ones) {
		TEST_ERROR("magic_ones->read()");
	}
	magic_ones->write(0);
	if (magic_ones->read() != ones) {
		TEST_ERROR("magic_ones->write()");
	}
}
