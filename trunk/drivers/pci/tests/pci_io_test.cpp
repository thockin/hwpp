#include "pci_driver.h"
#include "pci_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_pci_io()
{
	int ret = 0;

	system("mkdir -p test_data/0000:01:02.3");
	system("echo -n \"01234567\" > test_data/0000:01:02.3/config");

	system("mkdir -p test_data/04");
	system("echo -n \"01234567\" > test_data/04/05.6");

	try {
		/* test ctors (for a dev file that exists) and address() */
		pci_io io1(pci_address(0, 1, 2, 3), "test_data");
		if (io1.address().segment != 0 || io1.address().bus != 1
		 || io1.address().device != 2 || io1.address().function != 3) {
			PP_TEST_ERROR("pci_io::pci_io(pci_address)");
			ret++;
		}
		pci_io io2(pci_address(1, 2, 3), "test_data");
		if (io2.address().segment != 0 || io2.address().bus != 1
		 || io2.address().device != 2 || io2.address().function != 3) {
			PP_TEST_ERROR("pci_io::pci_io(pci_address)");
			ret++;
		}
		pci_io io3(pci_address(4, 5, 6), "test_data");
		if (io3.address().segment != 0 || io3.address().bus != 4
		 || io3.address().device != 5 || io3.address().function != 6) {
			PP_TEST_ERROR("pci_io::pci_io(pci_address)");
			ret++;
		}

		/* test the read() method */
		if (io1.read(0, BITS8) != 0x30) {
			PP_TEST_ERROR("pci_io::read(BITS8)");
			ret++;
		}
		if (io1.read(0, BITS16) != 0x3130) {
			PP_TEST_ERROR("pci_io::read(BITS16)");
			ret++;
		}
		if (io1.read(0, BITS32) != 0x33323130) {
			PP_TEST_ERROR("pci_io::read(BITS32)");
			ret++;
		}
		if (io1.read(0, BITS64) != pp_value("0x3736353433323130")) {
			PP_TEST_ERROR("pci_io::read(BITS64)");
			ret++;
		}

		/* test the write() method */
		io1.write(0, BITS64, pp_value("0xffeeddccbbaa9988"));
		if (io1.read(0, BITS64) != pp_value("0xffeeddccbbaa9988")) {
			PP_TEST_ERROR("pci_io::write()");
			ret++;
		}

		/* test ctors (for a dev file that does not exist) */
		try {
			pci_io io4(pci_address(1, 0, 0, 0), "test_data");
			PP_TEST_ERROR("pci_io::pci_io()");
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

	r = test_pci_io();
	//FIXME: could test pci_io::enumerate() with a fake devdir
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
