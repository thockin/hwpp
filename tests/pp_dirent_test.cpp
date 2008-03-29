#include "pp_dirent.h"
#include "pp.h"
#include "pp_fields.h"
#include "pp_register.h"
#include "pp_scope.h"
#include "pp_datatypes.h"
#include "pp_test.h"
using namespace std;

class test_dirent: public pp_dirent
{
    public:
	test_dirent(pp_dirent_type type): pp_dirent(type)
	{
	}
	virtual pp_value
	read() const
	{
		return 0;
	}
	virtual void
	write(const pp_value &val) const
	{
	}
};

int
test_pp_dirent()
{
	int ret = 0;

	/* test the field constructor */
	pp_dirent_ptr dirent(new test_dirent(PP_DIRENT_FIELD));

	/* test the dirent_type() method */
	if (dirent->dirent_type() != PP_DIRENT_FIELD) {
		TEST_ERROR("pp_dirent::dirent_type()");
		ret++;
	}
	if (!dirent->is_field()) {
		TEST_ERROR("pp_dirent::is_field()");
		ret++;
	}
	if (dirent->is_register()) {
		TEST_ERROR("pp_dirent::is_register()");
		ret++;
	}

	if (to_string(PP_DIRENT_FIELD) != "pp_field") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
		ret++;
	}
	if (to_string(PP_DIRENT_REGISTER) != "pp_register") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
		ret++;
	}
	if (to_string(PP_DIRENT_SCOPE) != "pp_scope") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
		ret++;
	}
	if (to_string(PP_DIRENT_ARRAY) != "pp_array") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
		ret++;
	}

	/* test up-casting */
	try {
		pp_field_from_dirent(dirent.get());
	} catch (pp_dirent::conversion_error &e) {
		TEST_ERROR("pp_field_from_dirent()");
		ret++;
	}
	try {
		pp_register_from_dirent(dirent.get());
		TEST_ERROR("pp_register_from_dirent()");
		ret++;
	} catch (pp_dirent::conversion_error &e) {
	}
	try {
		pp_scope_from_dirent(dirent.get());
		TEST_ERROR("pp_scope_from_dirent()");
		ret++;
	} catch (pp_dirent::conversion_error &e) {
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_dirent();

	TEST_EXIT(r);
}
