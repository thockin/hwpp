#include "cpuid_driver.h"
#include "cpuid_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

#define NULL_MAJOR	1
#define NULL_MINOR	3

int
test_cpuid_io()
{
	int ret = 0;

	system("mkdir -p test_data/93");
	system("echo -n \"0123456789012345\" > test_data/93/cpuid");

	try {
		/* test ctors (for a dev file that exists) and address() */
		cpuid_io io1(cpuid_address(93, 0), "test_data",
				NULL_MAJOR, NULL_MINOR);
		if (io1.address().cpu != 93) {
			PP_TEST_ERROR("cpuid_io::cpuid_io(cpuid_address)");
			ret++;
		}

		/* test the read() method */
		if (io1.read(0, BITS32) != pp_value("0x33323130")) {
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		}
		if (io1.read(1, BITS32) != pp_value("0x37363534")) {
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		}
		try {
			io1.read(0, BITS8);
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		} catch (exception &e) {
		}
		try {
			io1.read(0, BITS16);
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		} catch (exception &e) {
		}
		try {
			io1.read(0, BITS64);
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		} catch (exception &e) {
		}
		try {
			io1.read(0, BITS128);
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		} catch (exception &e) {
		}
		/* test read() around the bounds */
		try {
			io1.read(3, BITS32);
		} catch (exception &e) {
			PP_TEST_ERROR("cpuid_io::read(BITS8)");
			ret++;
		}
		try {
			io1.read(4, BITS32);
			PP_TEST_ERROR("cpuid_io::read(BITS16)");
			ret++;
		} catch (exception &e) {
		}

		/* test read for a bad offset */
		try {
			io1.read(5, BITS32);
			PP_TEST_ERROR("cpuid_io::read()");
			ret++;
		} catch (exception &e) {
		}

		/* test the write() method, which should be a no-op */
		io1.write(0, BITS32, 0xbbaa9988);
		if (io1.read(0, BITS32) != 0x33323130) {
			PP_TEST_ERROR("cpuid_io::write()");
			ret++;
		}

		/* test ctor for a dev file that does not exist */
		try {
			cpuid_io io2(cpuid_address(76, 0),
					"test_data", NULL_MAJOR, NULL_MINOR);
			PP_TEST_ERROR("cpuid_io::cpuid_io()");
			ret++;
		} catch (exception &e) {
		}
	} catch (exception &e) {
		system("rm -rf test_data");
		throw;
	}

	system("rm -rf test_data");
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
