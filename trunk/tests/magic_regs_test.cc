/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 */

#include "hwpp.h"
#include "register.h"
#include "util/test.h"

namespace hwpp {
extern RegisterPtr magic_zeros;
extern RegisterPtr magic_ones;
}  // namespace hwpp

TEST(magic_regs_test)
{
	if (hwpp::magic_zeros->read() != 0) {
		TEST_FAIL("magic_zeros->read()");
	}
	hwpp::magic_zeros->write(1);
	if (hwpp::magic_zeros->read() != 0) {
		TEST_FAIL("magic_zeros->write()");
	}

	hwpp::Value ones = hwpp::Value("0xffffffffffffffffffffffffffffffff");
	if (hwpp::magic_ones->read() != ones) {
		TEST_FAIL("magic_ones->read()");
	}
	hwpp::magic_ones->write(0);
	if (hwpp::magic_ones->read() != ones) {
		TEST_FAIL("magic_ones->write()");
	}
}
