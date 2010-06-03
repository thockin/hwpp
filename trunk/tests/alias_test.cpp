#include "pp.h"
#include "alias.h"
#include "dirent.h"
#include "util/test.h"

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
