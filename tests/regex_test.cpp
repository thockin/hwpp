#include "pp.h"
#include "pp_test.h"
#include "bitbuffer.h"
#include "simple_regex.h"
using namespace std;


int
test_regex()
{
	int ret = 0;

	// simple
	{
		regex re("foo.*");
		ret += TEST_ASSERT(re.matches("foo"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches("foobar"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches(" foo"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("bar"),
			"regex::regex()");
	}
	{
		regex re("^foo.*");
		ret += TEST_ASSERT(re.matches("foo"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches("foobar"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches(" foo"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("bar"),
			"regex::regex()");
	}
	{
		regex re(".*foo$");
		ret += TEST_ASSERT(re.matches("foo"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches("barfoo"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches(" foo"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("bar"),
			"regex::regex()");
	}
	{
		regex re("foo.*bar");
		ret += TEST_ASSERT(re.matches("foobar"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches("foo bar"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("bar foo"),
			"regex::regex()");
	}

	// with a back reference
	{
		regex re("^(foo).*\\1");
		ret += TEST_ASSERT(re.matches("foofoo"),
			"regex::regex()");
		ret += TEST_ASSERT(re.matches("foobarfoo"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("foo"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("foofo"),
			"regex::regex()");
		ret += TEST_ASSERT(!re.matches("foobar"),
			"regex::regex()");
	}
	{
		try {
			regex re(".*\\1");
			TEST_ERROR("regex::regex()");
			ret++;
		} catch (std::invalid_argument &e) {
		}
	}

	// copy ctor
	{
		regex re("foo.");
		ret += TEST_ASSERT(re.matches("foob"), "regex::regex()");
		ret += TEST_ASSERT(!re.matches("foo"), "regex::regex()");
		regex re2(re);
		ret += TEST_ASSERT(re2.matches("foob"), "regex::regex()");
		ret += TEST_ASSERT(!re2.matches("foo"), "regex::regex()");
		ret += TEST_ASSERT(re2.re_string() == re.re_string(),
			"regex::regex(regex)");
	}

	// assignment
	{
		regex re("foo.");
		ret += TEST_ASSERT(re.matches("foob"), "regex::regex()");
		ret += TEST_ASSERT(!re.matches("foo"), "regex::regex()");
		regex re2("foobar");
		re2 = re;
		ret += TEST_ASSERT(re2.matches("foob"), "regex::regex()");
		ret += TEST_ASSERT(!re2.matches("foo"), "regex::regex()");
		ret += TEST_ASSERT(re2.re_string() == re.re_string(),
			"regex::regex(regex)");
	}

	// case insensitive
	{
		regex re("foo.*");
		ret += TEST_ASSERT(re.matches("foo"), "regex::regex()");
		ret += TEST_ASSERT(!re.matches("FOO"), "regex::regex()");
		re.set_ignore_case(true);
		ret += TEST_ASSERT(re.matches("foo"), "regex::regex()");
		ret += TEST_ASSERT(re.matches("FOO"), "regex::regex()");
		re.set_ignore_case(false);
		ret += TEST_ASSERT(re.matches("foo"), "regex::regex()");
		ret += TEST_ASSERT(!re.matches("FOO"), "regex::regex()");
	}

	return 0;
}

// Perform the tests
TEST_LIST(
	TEST(test_regex),
);
