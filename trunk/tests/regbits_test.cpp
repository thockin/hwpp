#include "pp/pp.h"
#include "pp/register_types.h"
#include "pp/regbits.h"
#include "test_binding.h"
#include "pp_test.h"

TEST(test_simple_regbits)
{
	// test ctors
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb;
		TEST_ASSERT(rb.width() == 0,
			"pp::RegBits::RegBits()");
	}
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg);
		TEST_ASSERT(rb.read() == 0xffff,
			"pp::RegBits::RegBits(Register)");
		TEST_ASSERT(rb.width() == 16, "pp::RegBits::width()");
	}
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 7);
		TEST_ASSERT(rb.read() == 0x1,
			"pp::RegBits::RegBits(pp::Register, int)");
		TEST_ASSERT(rb.width() == 1, "pp::RegBits::width()");
	}
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 3, 0);
		TEST_ASSERT(rb.read() == 0xf,
			"pp::RegBits::RegBits(pp::Register, int, int)");
		TEST_ASSERT(rb.width() == 4, "pp::RegBits::width()");
	}

	// test an overly-large shift
	try {
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 32);
		TEST_FAIL("pp::RegBits::RegBits()");
	} catch (std::exception &e) {
	}

	// test write
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg);
		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x98,
		    "pp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0x98,
		    "pp::RegBits::write()");
	}
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 7);
		rb.write(0x00);
		TEST_ASSERT(rb.read() == 0x0,
		    "pp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0xff7f,
		    "pp::RegBits::write()");
	}
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 3, 0);
		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x8,
		    "pp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0xfff8,
		    "pp::RegBits::write()");
	}
}

TEST(test_complex_regbits)
{
	// test operator+
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		reg->write(0x4321);
		TEST_ASSERT(reg->read() == 0x4321,
		    "pp::BoundRegister::write()");

		pp::RegBits rb = pp::RegBits(reg, 11, 8)
		    + pp::RegBits(reg, 3, 0);
		TEST_ASSERT(rb.width() == 8, "pp::RegBits::width()");
		TEST_ASSERT(rb.read() == 0x31, "pp::RegBits::write()");

		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x98, "pp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0x4928,
		    "pp::RegBits::write()");
	}

	// test operator+=
	{
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		reg->write(0x4321);
		TEST_ASSERT(reg->read() == 0x4321,
		    "pp::BoundRegister::write()");

		pp::RegBits rb(reg, 11, 8);
		TEST_ASSERT(rb.width() == 4, "pp::RegBits::width()");
		TEST_ASSERT(rb.read() == 0x3, "pp::RegBits::write()");
		rb += pp::RegBits(reg, 3, 0);
		TEST_ASSERT(rb.width() == 8, "pp::RegBits::width()");
		TEST_ASSERT(rb.read() == 0x31, "pp::RegBits::write()");

		rb.write(0x98);
		TEST_ASSERT(rb.read() == 0x98, "pp::RegBits::write()");
		TEST_ASSERT(reg->read() == 0x4928,
		    "pp::RegBits::write()");
	}
}

TEST(test_exceptions)
{
	try {
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 0, 15);
		TEST_FAIL("pp::RegBits::RegBits(r, lo, hi)");
	} catch (pp::RegBits::range_error &e) {
	}
	try {
		pp::BindingPtr bind = new_test_binding();
		pp::RegisterPtr reg =
		    new_pp_bound_register(bind, 0, pp::BITS16);
		pp::RegBits rb(reg, 16);
		TEST_FAIL("pp::RegBits::RegBits(r, too_hi)");
	} catch (pp::RegBits::range_error &e) {
	}
}
