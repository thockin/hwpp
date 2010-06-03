#include "pp.h"
#include "binding.h"
#include "test_binding.h"
#include "util/test.h"

TEST(test_pp_binding)
{
	/* test the read() method */
	pp::BindingPtr sp = new_test_binding();
	if (sp->read(0, pp::BITS8) != 0xff) {
		TEST_FAIL("pp::Binding::read()");
	}

	/* test the write() method */
	sp->write(0, pp::BITS8, 0x11);
	if (sp->read(0, pp::BITS16) != 0xff11) {
		TEST_FAIL("pp::Binding::write()");
	}
}
