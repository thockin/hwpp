#include "io_driver.h"
#include "io_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_io_io()
{
	int ret = 0;

	system("mkdir -p test_data");
	system("echo -n \"01234567890123456789\" > test_data/dev_port");

	try {
		/* test ctors (for a dev file that exists) and address() */
		io_io io1(io_address(0, 20), "test_data/dev_port");
		if (io1.address().base != 0 || io1.address().size != 20) {
			PP_TEST_ERROR("io_io::io_io(io_address)");
			ret++;
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			PP_TEST_ERROR("io_io::read(BITS8)");
			ret++;
		}
		if (io1.read(0, BITS16) != 0x3130) {
			PP_TEST_ERROR("io_io::read(BITS16)");
			ret++;
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			PP_TEST_ERROR("io_io::read(BITS32)");
			ret++;
		}
		if (io1.read(0, BITS64) != pp_value("0x3736353433323130")) {
			PP_TEST_ERROR("io_io::read(BITS64)");
			ret++;
		}
		/* test read() around the bounds */
		try {
			io1.read(19, BITS8);
		} catch (exception &e) {
			PP_TEST_ERROR("io_io::read(BITS8)");
			ret++;
		}
		try {
			io1.read(19, BITS16);
			PP_TEST_ERROR("io_io::read(BITS16)");
			ret++;
		} catch (exception &e) {
		}
		try {
			io1.read(20, BITS8);
			PP_TEST_ERROR("io_io::read(BITS8)");
			ret++;
		} catch (exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, pp_value("0x0feeddccbbaa9988"));
		if (io1.read(0, BITS64) != pp_value("0x0feeddccbbaa9988")) {
			PP_TEST_ERROR("io_io::write()");
			ret++;
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			io_io io2(io_address(0, 20), "test_data/bad_port");
			PP_TEST_ERROR("io_io::io_io()");
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

	r = test_io_io();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
