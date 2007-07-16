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
	pp_dirent_ptr dirent = new_pp_dirent(PP_DIRENT_FIELD);

	/* test the dirent_type() method */
	if (dirent->dirent_type() != PP_DIRENT_FIELD) {
		PP_TEST_ERROR("pp_dirent::dirent_type()");
		ret++;
	}
	if (!dirent->is_field()) {
		PP_TEST_ERROR("pp_dirent::is_field()");
		ret++;
	}
	if (dirent->is_register()) {
		PP_TEST_ERROR("pp_dirent::is_register()");
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
