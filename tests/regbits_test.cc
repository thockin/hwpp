#include "hwpp.h"
#include "register_types.h"
#include "regbits.h"
#include "test_binding.h"
#include "util/test.h"

TEST(test_simple_regbits)
{
	// test ctors
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb;
		TEST_ASSERT(rb.width() == 0,
			"hwpp::RegBits::RegBits()");
	}
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg);
		TEST_ASSERT(rb.read() == 0xffff,
			"hwpp::RegBits::RegBits(Register)");
		TEST_ASSERT(rb.width() == 16, "hwpp::RegBits::width()");
	}
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 7);
		TEST_ASSERT(rb.read() == 0x1,
			"hwpp::RegBits::RegBits(hwpp::Register, int)");
		TEST_ASSERT(rb.width() == 1, "hwpp::RegBits::width()");
	}
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 3, 0);
		TEST_ASSERT(rb.read() == 0xf,
			"hwpp::RegBits::RegBits(hwpp::Register, int, int)");
		TEST_ASSERT(rb.width() == 4, "hwpp::RegBits::width()");
	}

	// test an overly-large shift
	try {
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 32);
		TEST_FAIL("hwpp::RegBits::RegBits()");
	} catch (std::exception &e) {
	}

	// test write
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg);
		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x98,
		    "hwpp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0x98,
		    "hwpp::RegBits::write()");
	}
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 7);
		rb.write(0x00);
		TEST_ASSERT(rb.read() == 0x0,
		    "hwpp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0xff7f,
		    "hwpp::RegBits::write()");
	}
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 3, 0);
		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x8,
		    "hwpp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0xfff8,
		    "hwpp::RegBits::write()");
	}
}

TEST(test_complex_regbits)
{
	// test operator+
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		reg->write(0x4321);
		TEST_ASSERT(reg->read() == 0x4321,
		    "hwpp::BoundRegister::write()");

		hwpp::RegBits rb = hwpp::RegBits(reg, 11, 8)
		    + hwpp::RegBits(reg, 3, 0);
		TEST_ASSERT(rb.width() == 8, "hwpp::RegBits::width()");
		TEST_ASSERT(rb.read() == 0x31, "hwpp::RegBits::write()");

		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x98, "hwpp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0x4928,
		    "hwpp::RegBits::write()");
	}

	// test operator+=
	{
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		reg->write(0x4321);
		TEST_ASSERT(reg->read() == 0x4321,
		    "hwpp::BoundRegister::write()");

		hwpp::RegBits rb(reg, 11, 8);
		TEST_ASSERT(rb.width() == 4, "hwpp::RegBits::width()");
		TEST_ASSERT(rb.read() == 0x3, "hwpp::RegBits::write()");
		rb += hwpp::RegBits(reg, 3, 0);
		TEST_ASSERT(rb.width() == 8, "hwpp::RegBits::width()");
		TEST_ASSERT(rb.read() == 0x31, "hwpp::RegBits::write()");

		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x98, "hwpp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0x4928,
		    "hwpp::RegBits::write()");
	}
}

TEST(test_exceptions)
{
	try {
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 0, 15);
		TEST_FAIL("hwpp::RegBits::RegBits(r, lo, hi)");
	} catch (hwpp::RegBits::range_error &e) {
	}
	try {
		hwpp::BindingPtr bind = new_test_binding();
		hwpp::RegisterPtr reg =
		    new_hwpp_bound_register(bind, 0, hwpp::BITS16);
		hwpp::RegBits rb(reg, 16);
		TEST_FAIL("hwpp::RegBits::RegBits(r, too_hi)");
	} catch (hwpp::RegBits::range_error &e) {
	}
}
