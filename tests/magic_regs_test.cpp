/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 */

#include "pp/pp.h"
#include "pp/register.h"
#include "pp_test.h"

namespace pp {
extern RegisterPtr magic_zeros;
extern RegisterPtr magic_ones;
}  // namespace pp

TEST(magic_regs_test)
{
	if (pp::magic_zeros->read() != 0) {
		TEST_FAIL("magic_zeros->read()");
	}
	pp::magic_zeros->write(1);
	if (pp::magic_zeros->read() != 0) {
		TEST_FAIL("magic_zeros->write()");
	}

	pp::Value ones = pp::Value("0xffffffffffffffffffffffffffffffff");
	if (pp::magic_ones->read() != ones) {
		TEST_FAIL("magic_ones->read()");
	}
	pp::magic_ones->write(0);
	if (pp::magic_ones->read() != ones) {
		TEST_FAIL("magic_ones->write()");
	}
}
