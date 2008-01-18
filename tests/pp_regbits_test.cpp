#include "pp.h"
#include "pp_test.h"
#include "pp_register.h"
#include "pp_regbits.h"
#include "test_binding.h"
using namespace std;

int
test_simple_regbits()
{
	int ret = 0;

	// test ctors
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb;
		ret += PP_TEST_ASSERT(rb.width() == 0,
			"pp_regbits::pp_regbits()");
	}
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get());
		ret += PP_TEST_ASSERT(rb.read() == 0xffff,
			"pp_regbits::pp_regbits(pp_register)");
		ret+= PP_TEST_ASSERT(rb.width() == 16, "pp_regbits::width()");
	}
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get(), 7);
		ret += PP_TEST_ASSERT(rb.read() == 0x1,
			"pp_regbits::pp_regbits(pp_register, int)");
		ret+= PP_TEST_ASSERT(rb.width() == 1, "pp_regbits::width()");
	}
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get(), 3, 0);
		ret += PP_TEST_ASSERT(rb.read() == 0xf,
			"pp_regbits::pp_regbits(pp_register, int, int)");
		ret+= PP_TEST_ASSERT(rb.width() == 4, "pp_regbits::width()");
	}

	// test an overly-large shift
	try {
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get(), 32);
		PP_TEST_ERROR("pp_regbits::pp_regbits()");
		ret++;
	} catch (exception &e) {
	}

	// test write
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get());
		rb.write(0x98);
		ret += PP_TEST_ASSERT(rb.read() == 0x98,
		    "pp_regbits::write()");
		ret += PP_TEST_ASSERT(reg->read() == 0x98,
		    "pp_regbits::write()");
	}
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get(), 7);
		rb.write(0x00);
		ret += PP_TEST_ASSERT(rb.read() == 0x0,
		    "pp_regbits::write()");
		ret += PP_TEST_ASSERT(reg->read() == 0xff7f,
		    "pp_regbits::write()");
	}
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		pp_regbits rb(reg.get(), 3, 0);
		rb.write(0x98);
		ret += PP_TEST_ASSERT(rb.read() == 0x8,
		    "pp_regbits::write()");
		ret += PP_TEST_ASSERT(reg->read() == 0xfff8,
		    "pp_regbits::write()");
	}

	return ret;
}

int
test_complex_regbits()
{
	int ret = 0;

	// test operator+
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		reg->write(0x4321);
		ret += PP_TEST_ASSERT(reg->read() == 0x4321,
		    "pp_register::write()");

		pp_regbits rb = pp_regbits(reg.get(), 11, 8)
		    + pp_regbits(reg.get(), 3, 0);
		ret += PP_TEST_ASSERT(rb.width() == 8, "pp_regbits::width()");
		ret += PP_TEST_ASSERT(rb.read() == 0x31, "pp_regbits::write()");

		rb.write(0x98);
		ret += PP_TEST_ASSERT(rb.read() == 0x98, "pp_regbits::write()");
		ret += PP_TEST_ASSERT(reg->read() == 0x4928,
		    "pp_regbits::write()");
	}

	// test operator+=
	{
		pp_binding_ptr bind = new_test_binding();
		pp_register_ptr reg = new_pp_register(bind.get(), 0, BITS16);
		reg->write(0x4321);
		ret += PP_TEST_ASSERT(reg->read() == 0x4321,
		    "pp_register::write()");

		pp_regbits rb(reg.get(), 11, 8);
		ret += PP_TEST_ASSERT(rb.width() == 4, "pp_regbits::width()");
		ret += PP_TEST_ASSERT(rb.read() == 0x3, "pp_regbits::write()");
		rb += pp_regbits(reg.get(), 3, 0);
		ret += PP_TEST_ASSERT(rb.width() == 8, "pp_regbits::width()");
		ret += PP_TEST_ASSERT(rb.read() == 0x31, "pp_regbits::write()");

		rb.write(0x98);
		ret += PP_TEST_ASSERT(rb.read() == 0x98, "pp_regbits::write()");
		ret += PP_TEST_ASSERT(reg->read() == 0x4928,
		    "pp_regbits::write()");
	}

	return ret;
}

int
main()
{
	int r = 0;

	r += test_simple_regbits();
	r += test_complex_regbits();

	if (r) return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
