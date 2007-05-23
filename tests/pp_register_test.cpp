#include "pp_register.h"
#include "pp.h"
#include "pp_test.h"
#include "pp_test_binding.h"
using namespace std;

int
test_pp_register()
{
	int ret = 0;

	/* test the binding constructor */
	pp_binding_ptr ts(new pp_test_binding);

	/* test the reg constructor */
	pp_register r0(ts, 1, BITS8);

	/* set up test registers */
	pp_register r1(ts, 1, BITS8);
	pp_register r2(ts, 2, BITS16);
	pp_register r3(ts, 3, BITS32);
	pp_register r4(ts, 4, BITS64);

	/* test the read() method */
	if (r1.read() != 0xff) {
		PP_TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r2.read() != 0xffff) {
		PP_TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r3.read() != 0xffffffff) {
		PP_TEST_ERROR("pp_register::read()");
		ret++;
	}
	if (r4.read() != 0xffffffffffffffffULL) {
		PP_TEST_ERROR("pp_register::read()");
		ret++;
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		PP_TEST_ERROR("pp_register::write()");
		ret++;
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		PP_TEST_ERROR("pp_register::write()");
		ret++;
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		PP_TEST_ERROR("pp_register::write()");
		ret++;
	}
	r4.write(0x4444444444444444ULL);
	if (r4.read() != 0x4444444444444444ULL) {
		PP_TEST_ERROR("pp_register::write()");
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
