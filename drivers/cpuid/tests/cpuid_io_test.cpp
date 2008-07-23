#include "cpuid_driver.h"
#include "cpuid_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_cpuid_io()
{
	int ret = 0;

	/* test ctor and address() */
	cpuid_io io1(cpuid_address(0));
	if (io1.address().cpu != 0) {
		TEST_ERROR("cpuid_io::cpuid_io(cpuid_address)");
		ret++;
	}

	/* test the read() method */

	// FIXME: these two tests are now broken, since we are
	// using inline assembly instead of the cpuid file to
	// get cpuid information; this test needs to be rethought
#if 0
	if (io1.read(0, BITS32) != pp_value("0x33323130")) {
		TEST_ERROR("cpuid_io::read()");
		ret++;
	}
	if (io1.read(1, BITS32) != pp_value("0x37363534")) {
		TEST_ERROR("cpuid_io::read()");
		ret++;
	}
#endif
	try {
		io1.read(0, BITS8);
		TEST_ERROR("cpuid_io::read()");
		ret++;
	} catch (exception &e) {
	}
	try {
		io1.read(0, BITS16);
		TEST_ERROR("cpuid_io::read()");
		ret++;
	} catch (exception &e) {
	}
	try {
		io1.read(0, BITS32);
		TEST_ERROR("cpuid_io::read()");
		ret++;
	} catch (exception &e) {
	}
	try {
		io1.read(0, BITS64);
		TEST_ERROR("cpuid_io::read()");
		ret++;
	} catch (exception &e) {
	}
	/* test read() around the bounds */
	try {
		io1.read(0, BITS128);
	} catch (exception &e) {
		TEST_ERROR("cpuid_io::read()");
		ret++;
	}
	try {
		io1.read(1, BITS128);
	} catch (exception &e) {
		TEST_ERROR("cpuid_io::read()");
		ret++;
	}
	try {
		io1.read(PP_MASK(64), BITS128);
	} catch (exception &e) {
		TEST_ERROR("cpuid_io::read()");
		ret++;
	}

	/* test read for a bad offset */
	try {
		io1.read(PP_MASK(64) + 1, BITS128);
		TEST_ERROR("cpuid_io::read()");
		ret++;
	} catch (exception &e) {
	}

	/* test the write() method, which should be a no-op */
	io1.write(0, BITS128, pp_value("0xaaaaaaaabbbbbbbbccccccccdddddddd"));
	if (io1.read(0, BITS128)
	    == pp_value("0xaaaaaaaabbbbbbbbccccccccdddddddd")) {
		TEST_ERROR("cpuid_io::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_cpuid_io();
	if (r) return EXIT_FAILURE;
	//FIXME: could test enumerate()

	return EXIT_SUCCESS;
}
