#include "msr_driver.h"
#include "msr_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

#define NULL_MAJOR	1
#define NULL_MINOR	3

int
test_msr_io()
{
	int ret = 0;

	system("mkdir -p test_data/93");
	system("echo -n \"01234567\" > test_data/93/msr");

	try {
		/* test ctors (for a dev file that exists) and address() */
		msr_io io1(93, "test_data", NULL_MAJOR, NULL_MINOR);
		if (io1.address().cpu != 93) {
			PP_TEST_ERROR("msr_io::msr_io(int)");
			ret++;
		}
		msr_io io2(msr_address(93), "test_data",
				NULL_MAJOR, NULL_MINOR);
		if (io2.address().cpu != 93) {
			PP_TEST_ERROR("msr_io::msr_io(msr_address)");
			ret++;
		}

		/* test the read() method */
		if (io1.read(0, BITS64) != 0x3736353433323130ULL) {
			PP_TEST_ERROR("msr_io::read()");
			ret++;
		}

		/* test the write() method */
		io1.write(0, BITS64, 0xffeeddccbbaa9988ULL);
		if (io1.read(0, BITS64) != 0xffeeddccbbaa9988ULL) {
			PP_TEST_ERROR("msr_io::write()");
			ret++;
		}

		/* test ctors (for a dev file that does not exist) */
		msr_io io3(76, "test_data", NULL_MAJOR, NULL_MINOR);
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
