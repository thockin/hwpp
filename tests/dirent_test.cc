#include "hwpp.h"
#include "dirent.h"
#include "field_types.h"
#include "register.h"
#include "scope.h"
#include "datatype_types.h"
#include "util/test.h"

class TestDirent: public hwpp::Dirent
{
    public:
	TestDirent(hwpp::DirentType type): hwpp::Dirent(type)
	{
	}
	virtual hwpp::Value
	read() const
	{
		return 0;
	}
	virtual void
	write(const hwpp::Value &val) const
	{
		(void)val;
	}
};

TEST(test_hwpp_dirent)
{
	/* test the field constructor */
	hwpp::DirentPtr dirent(new TestDirent(hwpp::DIRENT_TYPE_FIELD));

	/* test the dirent_type() method */
	if (dirent->dirent_type() != hwpp::DIRENT_TYPE_FIELD) {
		TEST_FAIL("hwpp::Dirent::dirent_type()");
	}
	if (!dirent->is_field()) {
		TEST_FAIL("hwpp::Dirent::is_field()");
	}
	if (dirent->is_register()) {
		TEST_FAIL("hwpp::Dirent::is_register()");
	}

	if (to_string(hwpp::DIRENT_TYPE_FIELD) != "Field") {
		TEST_FAIL("hwpp::Dirent::operator<<(ostream)");
	}
	if (to_string(hwpp::DIRENT_TYPE_REGISTER) != "Register") {
		TEST_FAIL("hwpp::Dirent::operator<<(ostream)");
	}
	if (to_string(hwpp::DIRENT_TYPE_SCOPE) != "Scope") {
		TEST_FAIL("hwpp::Dirent::operator<<(ostream)");
	}
	if (to_string(hwpp::DIRENT_TYPE_ARRAY) != "Array") {
		TEST_FAIL("hwpp::Dirent::operator<<(ostream)");
	}
	if (to_string(hwpp::DIRENT_TYPE_ALIAS) != "Alias") {
		TEST_ERROR("hwpp::Dirent::operator<<(ostream)");
	}

	/* test up-casting */
	try {
		hwpp::field_from_dirent(dirent);
	} catch (hwpp::Dirent::ConversionError &e) {
		TEST_FAIL("hwpp::field_from_dirent()");
	}
	try {
		hwpp::register_from_dirent(dirent);
		TEST_FAIL("hwpp::register_from_dirent()");
	} catch (hwpp::Dirent::ConversionError &e) {
	}
	try {
		hwpp::scope_from_dirent(dirent);
		TEST_FAIL("hwpp::scope_from_dirent()");
	} catch (hwpp::Dirent::ConversionError &e) {
	}
}
