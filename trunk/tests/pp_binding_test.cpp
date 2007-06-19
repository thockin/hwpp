#include "pp_binding.h"
#include "test_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_pp_binding()
{
	int ret = 0;

	/* test the read() method */
	pp_binding_ptr sp = new_test_binding();
	if (sp->read(0, BITS8) != 0xff) {
		PP_TEST_ERROR("pp_binding::read()");
		ret++;
	}

	/* test the write() method */
	sp->write(0, BITS8, 0x11);
	if (sp->read(0, BITS16) != 0xff11) {
		PP_TEST_ERROR("pp_binding::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_binding();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
