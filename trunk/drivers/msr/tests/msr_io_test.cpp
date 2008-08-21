#include "msr_driver.h"
#include "msr_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

#define NULL_MAJOR	1
#define NULL_MINOR	3

#define ZERO_MAJOR	1
#define ZERO_MINOR	5

int
test_msr_io()
{
	int ret = 0;

	system("mkdir -p test_data/93");
	system("echo -n \"01234567\" > test_data/93/msr");

	try {
		/* test ctors (for a dev file that exists) and address() */
		msr_io io1(msr_address(93), "test_data",
				NULL_MAJOR, NULL_MINOR);
		if (io1.address().cpu != 93) {
			TEST_ERROR("msr_io::msr_io(msr_address)");
			ret++;
		}

		/* test the read() method */
		if (io1.read(0, BITS64) != pp_value("0x3736353433323130")) {
			TEST_ERROR("msr_io::read()");
			ret++;
		}
		/* test read() around the bounds */
		try {
			io1.read(0xffffffff, BITS64);
			TEST_ERROR("msr_io::read(BITS64)");
			ret++;
		} catch (exception &e) {
		}
		try {
			io1.read(0x100000000ULL, BITS64);
			TEST_ERROR("msr_io::read(BITS64)");
			ret++;
		} catch (exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, pp_value("0xffeeddccbbaa9988"));
		if (io1.read(0, BITS64) != pp_value("0xffeeddccbbaa9988")) {
			TEST_ERROR("msr_io::write()");
			ret++;
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			msr_io io2(msr_address(76), "test_data",
					ZERO_MAJOR, ZERO_MINOR);
			ret += TEST_ASSERT(io2.read(0x0, BITS64)
					   == pp_value(0x0),
					   "msr_io::msr_io()");
		} catch (exception &e) {
			TEST_ERROR("msr_io::msr_io() "
				   "[note: must be root to call "
				   "fs::device::mkdev()]");
			ret++;
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

	r = test_msr_io();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
