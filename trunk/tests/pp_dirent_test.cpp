#include "pp_dirent.h"
#include "pp.h"
#include "pp_fields.h"
#include "pp_datatypes.h"
#include "pp_test.h"
using namespace std;

int
test_pp_dirent()
{
	int ret = 0;

	/* test the field constructor */
	pp_dirent_ptr dp1(new pp_dirent(PP_DIRENT_FIELD));

	/* test the type() method */
	if (dp1->type() != PP_DIRENT_FIELD) {
		PP_TEST_ERROR("pp_dirent::type()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_dirent();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
