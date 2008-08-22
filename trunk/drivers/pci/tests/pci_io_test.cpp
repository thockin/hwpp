#include "pci_driver.h"
#include "pci_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

void
test_pci_io()
{
	system("mkdir -p test_data/0000:01:02.3");
	system("echo -n \"01234567\" > test_data/0000:01:02.3/config");

	system("mkdir -p test_data/04");
	system("echo -n \"01234567\" > test_data/04/05.6");

	try {
		/* test ctors (for a dev file that exists) and address() */
		pci_io io1(pci_address(0, 1, 2, 3), "test_data");
		if (io1.address().segment != 0 || io1.address().bus != 1
		 || io1.address().device != 2 || io1.address().function != 3) {
			TEST_ERROR("pci_io::pci_io(pci_address)");
		}
		pci_io io2(pci_address(1, 2, 3), "test_data");
		if (io2.address().segment != 0 || io2.address().bus != 1
		 || io2.address().device != 2 || io2.address().function != 3) {
			TEST_ERROR("pci_io::pci_io(pci_address)");
		}
		pci_io io3(pci_address(4, 5, 6), "test_data");
		if (io3.address().segment != 0 || io3.address().bus != 4
		 || io3.address().device != 5 || io3.address().function != 6) {
			TEST_ERROR("pci_io::pci_io(pci_address)");
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			TEST_ERROR("pci_io::read(BITS8)");
		}
		if (io1.read(0, BITS16) != 0x3130) {
			TEST_ERROR("pci_io::read(BITS16)");
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			TEST_ERROR("pci_io::read(BITS32)");
		}
		if (io1.read(0, BITS64) != pp_value("0x3736353433323130")) {
			TEST_ERROR("pci_io::read(BITS64)");
		}
		/* test read() around EOF, but < 4 KB */
		if (io1.read(7, BITS16) != 0xff37) {
			TEST_ERROR("pci_io::read(BITS16)");
		}
		if (io1.read(7, BITS32) != 0xffffff37) {
			TEST_ERROR("pci_io::read(BITS32)");
		}
		if (io1.read(8, BITS16) != 0xffff) {
			TEST_ERROR("pci_io::read(BITS16)");
		}
		/* test read() around 4 KB */
		if (io1.read(4095, BITS8) != 0xff) {
			TEST_ERROR("pci_io::read(BITS8)");
		}
		try {
			io1.read(4095, BITS16);
			TEST_ERROR("pci_io::read(BITS16)");
		} catch (exception &e) {
		}
		try {
			io1.read(4096, BITS8);
			TEST_ERROR("pci_io::read(BITS8)");
		} catch (exception &e) {
		}

		/* test the write() method */
		io1.write(0, BITS64, pp_value("0xffeeddccbbaa9988"));
		if (io1.read(0, BITS64) != pp_value("0xffeeddccbbaa9988")) {
			TEST_ERROR("pci_io::write()");
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			pci_io io4(pci_address(1, 0, 0, 0), "test_data");
			TEST_ERROR("pci_io::pci_io()");
		} catch (exception &e) {
		}
	} catch (exception &e) {
		system("rm -rf test_data");
		throw;
	}

	system("rm -rf test_data");
}

TEST_LIST(
	TEST(test_pci_io),
);
