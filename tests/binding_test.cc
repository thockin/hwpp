#include "hwpp.h"
#include "binding.h"
#include "test_binding.h"
#include "util/test.h"

TEST(test_hwpp_binding)
{
	/* test the read() method */
	hwpp::BindingPtr sp = new_test_binding();
	if (sp->read(0, hwpp::BITS8) != 0xff) {
		TEST_FAIL("hwpp::Binding::read()");
	}

	/* test the write() method */
	sp->write(0, hwpp::BITS8, 0x11);
	if (sp->read(0, hwpp::BITS16) != 0xff11) {
		TEST_FAIL("hwpp::Binding::write()");
	}
}
