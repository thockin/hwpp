#include "hwpp.h"
#include "drivers/cpuid/cpuid_driver.h"
#include "drivers/cpuid/cpuid_binding.h"
#include "util/test.h"

namespace hwpp {

TEST(test_cpuid_io)
{
	/* test ctor and address() */
	CpuidIo io1(CpuidAddress(0));
	if (io1.address().cpu != 0) {
		TEST_FAIL("CpuidIo::CpuidIo(CpuidAddress)");
	}

	/* test the read() method */

	// FIXME: these two tests are now broken, since we are
	// using inline assembly instead of the cpuid file to
	// get cpuid information; this test needs to be rethought
#if 0
	if (io1.read(0, BITS32) != Value("0x33323130")) {
		TEST_FAIL("CpuidIo::read()");
	}
	if (io1.read(1, BITS32) != Value("0x37363534")) {
		TEST_FAIL("CpuidIo::read()");
	}
#endif
	try {
		io1.read(0, BITS8);
		TEST_FAIL("CpuidIo::read()");
	} catch (std::exception &e) {
	}
	try {
		io1.read(0, BITS16);
		TEST_FAIL("CpuidIo::read()");
	} catch (std::exception &e) {
	}
	try {
		io1.read(0, BITS32);
		TEST_FAIL("CpuidIo::read()");
	} catch (std::exception &e) {
	}
	try {
		io1.read(0, BITS64);
		TEST_FAIL("CpuidIo::read()");
	} catch (std::exception &e) {
	}
	/* test read() around the bounds */
	try {
		io1.read(0, BITS128);
	} catch (std::exception &e) {
		TEST_FAIL("CpuidIo::read()");
	}
	try {
		io1.read(1, BITS128);
	} catch (std::exception &e) {
		TEST_FAIL("CpuidIo::read()");
	}
	try {
		io1.read(MASK(64), BITS128);
	} catch (std::exception &e) {
		TEST_FAIL("CpuidIo::read()");
	}

	/* test read for a bad offset */
	try {
		io1.read(MASK(64) + 1, BITS128);
		TEST_FAIL("CpuidIo::read()");
	} catch (std::exception &e) {
	}

	/* test the write() method, which should be a no-op */
	io1.write(0, BITS128, Value("0xaaaaaaaabbbbbbbbccccccccdddddddd"));
	if (io1.read(0, BITS128)
	    == Value("0xaaaaaaaabbbbbbbbccccccccdddddddd")) {
		TEST_FAIL("CpuidIo::write()");
	}
}

//FIXME: test enumerate()

}  // namespace hwpp
