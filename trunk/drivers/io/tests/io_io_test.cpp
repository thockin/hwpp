#include "pp.h"
#include "io_driver.h"
#include "io_binding.h"
#include "pp_test.h"

TEST(test_io_io)
{
	system("mkdir -p test_data");
	system("echo -n \"01234567890123456789\" > test_data/dev_port");

	try {
		/* test ctors (for a dev file that exists) and address() */
		io_io io1(io_address(0, 20), "test_data/dev_port");
		if (io1.address().base != 0 || io1.address().size != 20) {
			TEST_ERROR("io_io::io_io(io_address)");
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			TEST_ERROR("io_io::read(BITS8)");
		}
		if (io1.read(0, BITS16) != 0x3130) {
			TEST_ERROR("io_io::read(BITS16)");
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			TEST_ERROR("io_io::read(BITS32)");
		}
		if (io1.read(0, BITS64) != pp_value("0x3736353433323130")) {
			TEST_ERROR("io_io::read(BITS64)");
		}
		/* test read() around the bounds */
		try {
			io1.read(19, BITS8);
		} catch (std::exception &e) {
			TEST_ERROR("io_io::read(BITS8)");
		}
		try {
			io1.read(19, BITS16);
			TEST_ERROR("io_io::read(BITS16)");
		} catch (std::exception &e) {
		}
		try {
			io1.read(20, BITS8);
			TEST_ERROR("io_io::read(BITS8)");
		} catch (std::exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, pp_value("0x0feeddccbbaa9988"));
		if (io1.read(0, BITS64) != pp_value("0x0feeddccbbaa9988")) {
			TEST_ERROR("io_io::write()");
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			io_io io2(io_address(0, 20), "test_data/bad_port");
			TEST_ERROR("io_io::io_io()");
		} catch (std::exception &e) {
		}
	} catch (std::exception &e) {
		system("rm -rf test_data");
		throw;
	}

	system("rm -rf test_data");
}
