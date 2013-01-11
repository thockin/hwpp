#include "hwpp.h"
#include "alias.h"
#include "dirent.h"
#include "util/test.h"

TEST(test_ctors)
{
	// test the basic constructor
	{
		hwpp::AliasPtr alias = new_hwpp_alias("foo");
		if (alias->dirent_type() != hwpp::DIRENT_TYPE_ALIAS) {
			TEST_ERROR("hwpp::Alias::Alias(string)");
		}
		if (alias->link_path() != "foo") {
			TEST_ERROR("hwpp::Alias::Alias(string)");
		}
	}
	{
		hwpp::AliasPtr alias = new_hwpp_alias("foo/bar");
		if (alias->dirent_type() != hwpp::DIRENT_TYPE_ALIAS) {
			TEST_ERROR("hwpp::Alias::Alias(string)");
		}
		if (alias->link_path() != "foo/bar") {
			TEST_ERROR("hwpp::Alias::Alias(string)");
		}
	}
}
