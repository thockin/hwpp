#include "pp_binding.h"
#include "test_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

void
test_pp_binding()
{
	/* test the read() method */
	pp_binding_ptr sp = new_test_binding();
	if (sp->read(0, BITS8) != 0xff) {
		TEST_ERROR("pp_binding::read()");
	}

	/* test the write() method */
	sp->write(0, BITS8, 0x11);
	if (sp->read(0, BITS16) != 0xff11) {
		TEST_ERROR("pp_binding::write()");
	}
}

TEST_LIST(
	TEST(test_pp_binding),
);
