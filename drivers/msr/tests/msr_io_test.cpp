#include "pp.h"
#include "msr_driver.h"
#include "msr_binding.h"
#include "pp_test.h"

#define NULL_MAJOR	1
#define NULL_MINOR	3

#define ZERO_MAJOR	1
#define ZERO_MINOR	5

TEST(test_msr_io)
{
	system("mkdir -p test_data/93");
	system("echo -n \"01234567\" > test_data/93/msr");

	try {
		/* test ctors (for a dev file that exists) and address() */
		msr_io io1(msr_address(93), "test_data",
				NULL_MAJOR, NULL_MINOR);
		if (io1.address().cpu != 93) {
			TEST_FAIL("msr_io::msr_io(msr_address)");
		}

		/* test the read() method */
		if (io1.read(0, BITS64) != pp_value("0x3736353433323130")) {
			TEST_FAIL("msr_io::read()");
		}
		/* test read() around the bounds */
		try {
			io1.read(0xffffffff, BITS64);
			TEST_FAIL("msr_io::read(BITS64)");
		} catch (std::exception &e) {
		}
		try {
			io1.read(0x100000000ULL, BITS64);
			TEST_FAIL("msr_io::read(BITS64)");
		} catch (std::exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, pp_value("0xffeeddccbbaa9988"));
		if (io1.read(0, BITS64) != pp_value("0xffeeddccbbaa9988")) {
			TEST_FAIL("msr_io::write()");
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			msr_io io2(msr_address(76), "test_data",
					ZERO_MAJOR, ZERO_MINOR);
			TEST_ASSERT(io2.read(0x0, BITS64) == pp_value(0x0),
			    "msr_io::msr_io()");
		} catch (syserr::operation_not_permitted &e) {
			TEST_WARN("must be root to call fs::device::mkdev()");
		}
	} catch (std::exception &e) {
		system("rm -rf test_data");
		throw;
	}

	system("rm -rf test_data");
}
