#include "pp/pp.h"
#include "pp/drivers/io/io_driver.h"
#include "pp/drivers/io/io_binding.h"
#include "pp/util/test.h"

namespace pp {

TEST(test_io_io)
{
	system("mkdir -p test_data");
	system("echo -n \"01234567890123456789\" > test_data/dev_port");

	try {
		/* test ctors (for a dev file that exists) and address() */
		IoIo io1(IoAddress(0, 20), "test_data/dev_port");
		if (io1.address().base != 0 || io1.address().size != 20) {
			TEST_FAIL("IoIo::IoIo(IoAddress)");
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			TEST_FAIL("IoIo::read(BITS8)");
		}
		if (io1.read(0, BITS16) != 0x3130) {
			TEST_FAIL("IoIo::read(BITS16)");
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			TEST_FAIL("IoIo::read(BITS32)");
		}
		if (io1.read(0, BITS64) != Value("0x3736353433323130")) {
			TEST_FAIL("IoIo::read(BITS64)");
		}
		/* test read() around the bounds */
		try {
			io1.read(19, BITS8);
		} catch (std::exception &e) {
			TEST_FAIL("IoIo::read(BITS8)");
		}
		try {
			io1.read(19, BITS16);
			TEST_FAIL("IoIo::read(BITS16)");
		} catch (std::exception &e) {
		}
		try {
			io1.read(20, BITS8);
			TEST_FAIL("IoIo::read(BITS8)");
		} catch (std::exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, Value("0x0feeddccbbaa9988"));
		if (io1.read(0, BITS64) != Value("0x0feeddccbbaa9988")) {
			TEST_FAIL("IoIo::write()");
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			IoIo io2(IoAddress(0, 20), "test_data/bad_port");
			TEST_FAIL("IoIo::IoIo()");
		} catch (std::exception &e) {
		}
	} catch (std::exception &e) {
		system("rm -rf test_data");
		throw;
	}

	system("rm -rf test_data");
}

}  // namespace pp
