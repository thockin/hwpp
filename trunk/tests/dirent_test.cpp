#include "pp.h"
#include "dirent.h"
#include "field_types.h"
#include "register.h"
#include "scope.h"
#include "datatype_types.h"
#include "util/test.h"

class TestDirent: public pp::Dirent
{
    public:
	TestDirent(pp::DirentType type): pp::Dirent(type)
	{
	}
	virtual pp::Value
	read() const
	{
		return 0;
	}
	virtual void
	write(const pp::Value &val) const
	{
		(void)val;
	}
};

TEST(test_pp_dirent)
{
	/* test the field constructor */
	pp::DirentPtr dirent(new TestDirent(pp::DIRENT_TYPE_FIELD));

	/* test the dirent_type() method */
	if (dirent->dirent_type() != pp::DIRENT_TYPE_FIELD) {
		TEST_FAIL("pp::Dirent::dirent_type()");
	}
	if (!dirent->is_field()) {
		TEST_FAIL("pp::Dirent::is_field()");
	}
	if (dirent->is_register()) {
		TEST_FAIL("pp::Dirent::is_register()");
	}

	if (to_string(pp::DIRENT_TYPE_FIELD) != "Field") {
		TEST_FAIL("pp::Dirent::operator<<(ostream)");
	}
	if (to_string(pp::DIRENT_TYPE_REGISTER) != "Register") {
		TEST_FAIL("pp::Dirent::operator<<(ostream)");
	}
	if (to_string(pp::DIRENT_TYPE_SCOPE) != "Scope") {
		TEST_FAIL("pp::Dirent::operator<<(ostream)");
	}
	if (to_string(pp::DIRENT_TYPE_ARRAY) != "Array") {
		TEST_FAIL("pp::Dirent::operator<<(ostream)");
	}
	if (to_string(pp::DIRENT_TYPE_ALIAS) != "Alias") {
		TEST_ERROR("pp::Dirent::operator<<(ostream)");
	}

	/* test up-casting */
	try {
		pp::field_from_dirent(dirent);
	} catch (pp::Dirent::ConversionError &e) {
		TEST_FAIL("pp::field_from_dirent()");
	}
	try {
		pp::register_from_dirent(dirent);
		TEST_FAIL("pp::register_from_dirent()");
	} catch (pp::Dirent::ConversionError &e) {
	}
	try {
		pp::scope_from_dirent(dirent);
		TEST_FAIL("pp::scope_from_dirent()");
	} catch (pp::Dirent::ConversionError &e) {
	}
}
