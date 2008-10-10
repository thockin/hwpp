#include "pp.h"
#include "pp_alias.h"
#include "pp_dirent.h"
#include "test_helpers.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// test the basic constructor
	{
		pp_alias_ptr alias = new_pp_alias("foo");
		if (alias->dirent_type() != PP_DIRENT_ALIAS) {
			TEST_ERROR("pp_alias::pp_alias(string)");
		}
		if (alias->link_path() != "foo") {
			TEST_ERROR("pp_alias::pp_alias(string)");
		}
	}
	{
		pp_alias_ptr alias = new_pp_alias("foo/bar");
		if (alias->dirent_type() != PP_DIRENT_ALIAS) {
			TEST_ERROR("pp_alias::pp_alias(string)");
		}
		if (alias->link_path() != "foo/bar") {
			TEST_ERROR("pp_alias::pp_alias(string)");
		}
	}
}
