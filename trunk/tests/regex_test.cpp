#include "pp.h"
#include "pp_test.h"
#include "bitbuffer.h"
#include "simple_regex.h"
using namespace std;


void
test_regex()
{
	// simple
	{
		regex re("foo.*");
		TEST_ASSERT(re.matches("foo"),
			"regex::regex()");
		TEST_ASSERT(re.matches("foobar"),
			"regex::regex()");
		TEST_ASSERT(re.matches(" foo"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("bar"),
			"regex::regex()");
	}
	{
		regex re("^foo.*");
		TEST_ASSERT(re.matches("foo"),
			"regex::regex()");
		TEST_ASSERT(re.matches("foobar"),
			"regex::regex()");
		TEST_ASSERT(!re.matches(" foo"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("bar"),
			"regex::regex()");
	}
	{
		regex re(".*foo$");
		TEST_ASSERT(re.matches("foo"),
			"regex::regex()");
		TEST_ASSERT(re.matches("barfoo"),
			"regex::regex()");
		TEST_ASSERT(re.matches(" foo"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("bar"),
			"regex::regex()");
	}
	{
		regex re("foo.*bar");
		TEST_ASSERT(re.matches("foobar"),
			"regex::regex()");
		TEST_ASSERT(re.matches("foo bar"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("bar foo"),
			"regex::regex()");
	}

	// with a back reference
	{
		regex re("^(foo).*\\1");
		TEST_ASSERT(re.matches("foofoo"),
			"regex::regex()");
		TEST_ASSERT(re.matches("foobarfoo"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("foo"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("foofo"),
			"regex::regex()");
		TEST_ASSERT(!re.matches("foobar"),
			"regex::regex()");
	}
	{
		try {
			regex re(".*\\1");
			TEST_ERROR("regex::regex()");
		} catch (std::invalid_argument &e) {
		}
	}

	// copy ctor
	{
		regex re("foo.");
		TEST_ASSERT(re.matches("foob"), "regex::regex()");
		TEST_ASSERT(!re.matches("foo"), "regex::regex()");
		regex re2(re);
		TEST_ASSERT(re2.matches("foob"), "regex::regex()");
		TEST_ASSERT(!re2.matches("foo"), "regex::regex()");
		TEST_ASSERT(re2.re_string() == re.re_string(),
			"regex::regex(regex)");
	}

	// assignment
	{
		regex re("foo.");
		TEST_ASSERT(re.matches("foob"), "regex::regex()");
		TEST_ASSERT(!re.matches("foo"), "regex::regex()");
		regex re2("foobar");
		re2 = re;
		TEST_ASSERT(re2.matches("foob"), "regex::regex()");
		TEST_ASSERT(!re2.matches("foo"), "regex::regex()");
		TEST_ASSERT(re2.re_string() == re.re_string(),
			"regex::regex(regex)");
	}

	// case insensitive
	{
		regex re("foo.*");
		TEST_ASSERT(re.matches("foo"), "regex::regex()");
		TEST_ASSERT(!re.matches("FOO"), "regex::regex()");
		re.set_ignore_case(true);
		TEST_ASSERT(re.matches("foo"), "regex::regex()");
		TEST_ASSERT(re.matches("FOO"), "regex::regex()");
		re.set_ignore_case(false);
		TEST_ASSERT(re.matches("foo"), "regex::regex()");
		TEST_ASSERT(!re.matches("FOO"), "regex::regex()");
	}
}

// Perform the tests
TEST_LIST(
	TEST(test_regex),
);
