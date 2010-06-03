#include "pp.h"
#include "drivers/pci/pci_driver.h"
#include "drivers/pci/pci_binding.h"
#include "util/test.h"

namespace pp {

TEST(test_pci_io)
{
	system("mkdir -p test_data/0000:01:02.3");
	system("echo -n \"01234567\" > test_data/0000:01:02.3/config");

	system("mkdir -p test_data/04");
	system("echo -n \"01234567\" > test_data/04/05.6");

	try {
		/* test ctors (for a dev file that exists) and address() */
		PciIo io1(PciAddress(0, 1, 2, 3), "test_data");
		if (io1.address().segment != 0 || io1.address().bus != 1
		 || io1.address().device != 2 || io1.address().function != 3) {
			TEST_FAIL("PciIo::PciIo(PciAddress)");
		}
		PciIo io2(PciAddress(1, 2, 3), "test_data");
		if (io2.address().segment != 0 || io2.address().bus != 1
		 || io2.address().device != 2 || io2.address().function != 3) {
			TEST_FAIL("PciIo::PciIo(PciAddress)");
		}
		PciIo io3(PciAddress(4, 5, 6), "test_data");
		if (io3.address().segment != 0 || io3.address().bus != 4
		 || io3.address().device != 5 || io3.address().function != 6) {
			TEST_FAIL("PciIo::PciIo(PciAddress)");
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			TEST_FAIL("PciIo::read(BITS8)");
		}
		if (io1.read(0, BITS16) != 0x3130) {
			TEST_FAIL("PciIo::read(BITS16)");
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			TEST_FAIL("PciIo::read(BITS32)");
		}
		if (io1.read(0, BITS64) != Value("0x3736353433323130")) {
			TEST_FAIL("PciIo::read(BITS64)");
		}
		/* test read() around EOF, but < 4 KB */
		if (io1.read(7, BITS16) != 0xff37) {
			TEST_FAIL("PciIo::read(BITS16)");
		}
		if (io1.read(7, BITS32) != 0xffffff37) {
			TEST_FAIL("PciIo::read(BITS32)");
		}
		if (io1.read(8, BITS16) != 0xffff) {
			TEST_FAIL("PciIo::read(BITS16)");
		}
		/* test read() around 4 KB */
		if (io1.read(4095, BITS8) != 0xff) {
			TEST_FAIL("PciIo::read(BITS8)");
		}
		try {
			io1.read(4095, BITS16);
			TEST_FAIL("PciIo::read(BITS16)");
		} catch (std::exception &e) {
		}
		try {
			io1.read(4096, BITS8);
			TEST_FAIL("PciIo::read(BITS8)");
		} catch (std::exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, Value("0xffeeddccbbaa9988"));
		if (io1.read(0, BITS64) != Value("0xffeeddccbbaa9988")) {
			TEST_FAIL("PciIo::write()");
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			PciIo io4(PciAddress(1, 0, 0, 0), "test_data");
			TEST_FAIL("PciIo::PciIo()");
		} catch (std::exception &e) {
		}
	} catch (std::exception &e) {
		system("rm -rf test_data");
		throw;
	}

	system("rm -rf test_data");
}

}  // namespace pp
