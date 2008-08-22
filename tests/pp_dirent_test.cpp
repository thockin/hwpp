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
		(void)val;
	}
};

void
test_pp_dirent()
{
	/* test the field constructor */
	pp_dirent_ptr dirent(new test_dirent(PP_DIRENT_FIELD));

	/* test the dirent_type() method */
	if (dirent->dirent_type() != PP_DIRENT_FIELD) {
		TEST_ERROR("pp_dirent::dirent_type()");
	}
	if (!dirent->is_field()) {
		TEST_ERROR("pp_dirent::is_field()");
	}
	if (dirent->is_register()) {
		TEST_ERROR("pp_dirent::is_register()");
	}

	if (to_string(PP_DIRENT_FIELD) != "pp_field") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
	}
	if (to_string(PP_DIRENT_REGISTER) != "pp_register") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
	}
	if (to_string(PP_DIRENT_SCOPE) != "pp_scope") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
	}
	if (to_string(PP_DIRENT_ARRAY) != "pp_array") {
		TEST_ERROR("pp_dirent::operator<<(ostream)");
	}

	/* test up-casting */
	try {
		pp_field_from_dirent(dirent);
	} catch (pp_dirent::conversion_error &e) {
		TEST_ERROR("pp_field_from_dirent()");
	}
	try {
		pp_register_from_dirent(dirent);
		TEST_ERROR("pp_register_from_dirent()");
	} catch (pp_dirent::conversion_error &e) {
	}
	try {
		pp_scope_from_dirent(dirent);
		TEST_ERROR("pp_scope_from_dirent()");
	} catch (pp_dirent::conversion_error &e) {
	}
}

TEST_LIST(
	TEST(test_pp_dirent),
);
