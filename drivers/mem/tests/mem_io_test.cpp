#include "mem_driver.h"
#include "mem_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_mem_io()
{
	int ret = 0;

	system("mkdir -p test_data");
	system("echo -n \"01234567\" > test_data/dev_mem");

	try {
		/* test ctors (for a dev file that exists) and address() */
		mem_io io1(mem_address(0, 20), "test_data/dev_mem");
		if (io1.address().base != 0 || io1.address().size != 20) {
			PP_TEST_ERROR("mem_io::mem_io(mem_address)");
			ret++;
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			PP_TEST_ERROR("mem_io::read(BITS8)");
			ret++;
		}
		if (io1.read(0, BITS16) != 0x3130) {
			PP_TEST_ERROR("mem_io::read(BITS16)");
			ret++;
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			PP_TEST_ERROR("mem_io::read(BITS32)");
			ret++;
		}
		if (io1.read(0, BITS64) != 0x3736353433323130ULL) {
			PP_TEST_ERROR("mem_io::read(BITS64)");
			ret++;
		}

		/* test the write() method */
		io1.write(0, BITS64, 0xffeeddccbbaa9988ULL);
		if (io1.read(0, BITS64) != 0xffeeddccbbaa9988ULL) {
			PP_TEST_ERROR("mem_io::write()");
			ret++;
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			mem_io io2(mem_address(0, 20), "test_data/bad_mem");
			PP_TEST_ERROR("mem_io::mem_io()");
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

	r = test_mem_io();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
