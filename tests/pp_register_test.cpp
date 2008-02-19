#include "pp_register.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
using namespace std;

int
test_pp_register()
{
	int ret = 0;

	/* test the binding constructor */
	pp_binding_ptr bind = new_test_binding();

	/* test the reg constructor */
	pp_register r0(bind.get(), 1, BITS8);

	/* set up test registers */
	pp_register r1(bind.get(), 1, BITS8);
	pp_register r2(bind.get(), 2, BITS16);
	pp_register r3(bind.get(), 3, BITS32);
	pp_register r4(bind.get(), 4, BITS64);
	pp_register r5(bind.get(), 5, BITS128);

	/* test the read() method */
	if (r1.read() != PP_MASK(BITS8)) {
		TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r2.read() != PP_MASK(BITS16)) {
		TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r3.read() != PP_MASK(BITS32)) {
		TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r4.read() != PP_MASK(BITS64)) {
		TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r5.read() != PP_MASK(BITS128)) {
		TEST_ERROR("pp_register::read()");
		ret++;
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_ERROR("pp_register::write()");
		ret++;
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_ERROR("pp_register::write()");
		ret++;
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_ERROR("pp_register::write()");
		ret++;
	}
	r4.write(pp_value("0x4444444444444444"));
	if (r4.read() != pp_value("0x4444444444444444")) {
		TEST_ERROR("pp_register::write()");
		ret++;
	}
	r5.write(pp_value("0x555555555555555555555555"));
	if (r5.read() != pp_value("0x555555555555555555555555")) {
		TEST_ERROR("pp_register::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_register();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
