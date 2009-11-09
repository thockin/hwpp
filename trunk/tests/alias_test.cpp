#include "pp/pp.h"
#include "pp/alias.h"
#include "pp/dirent.h"
#include "test_helpers.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// test the basic constructor
	{
		pp::AliasPtr alias = new_pp_alias("foo");
		if (alias->dirent_type() != pp::DIRENT_TYPE_ALIAS) {
			TEST_ERROR("pp::Alias::Alias(string)");
		}
		if (alias->link_path() != "foo") {
			TEST_ERROR("pp::Alias::Alias(string)");
		}
	}
	{
		pp::AliasPtr alias = new_pp_alias("foo/bar");
		if (alias->dirent_type() != pp::DIRENT_TYPE_ALIAS) {
			TEST_ERROR("pp::Alias::Alias(string)");
		}
		if (alias->link_path() != "foo/bar") {
			TEST_ERROR("pp::Alias::Alias(string)");
		}
	}
}
