#include "pp.h"
#include "simple_regex.h"
#include "pp_test.h"

namespace util {

TEST(test_simple_regex)
{
	// simple
	{
		SimpleRegex re("foo.*");
		TEST_ASSERT(re.matches("foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches("foobar"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches(" foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("bar"),
			"SimpleRegex::SimpleRegex()");
	}
	{
		SimpleRegex re("^foo.*");
		TEST_ASSERT(re.matches("foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches("foobar"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches(" foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("bar"),
			"SimpleRegex::SimpleRegex()");
	}
	{
		SimpleRegex re(".*foo$");
		TEST_ASSERT(re.matches("foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches("barfoo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches(" foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("bar"),
			"SimpleRegex::SimpleRegex()");
	}
	{
		SimpleRegex re("foo.*bar");
		TEST_ASSERT(re.matches("foobar"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches("foo bar"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("bar foo"),
			"SimpleRegex::SimpleRegex()");
	}

	// with a back reference
	{
		SimpleRegex re("^(foo).*\\1");
		TEST_ASSERT(re.matches("foofoo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches("foobarfoo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("foo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("foofo"),
			"SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("foobar"),
			"SimpleRegex::SimpleRegex()");
	}
	{
		try {
			SimpleRegex re(".*\\1");
			TEST_FAIL("SimpleRegex::SimpleRegex()");
		} catch (std::invalid_argument &e) {
		}
	}

	// copy ctor
	{
		SimpleRegex re("foo.");
		TEST_ASSERT(re.matches("foob"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("foo"), "SimpleRegex::SimpleRegex()");
		SimpleRegex re2(re);
		TEST_ASSERT(re2.matches("foob"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re2.matches("foo"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re2.re_string() == re.re_string(),
			"SimpleRegex::SimpleRegex(SimpleRegex)");
	}

	// assignment
	{
		SimpleRegex re("foo.");
		TEST_ASSERT(re.matches("foob"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("foo"), "SimpleRegex::SimpleRegex()");
		SimpleRegex re2("foobar");
		re2 = re;
		TEST_ASSERT(re2.matches("foob"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re2.matches("foo"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re2.re_string() == re.re_string(),
			"SimpleRegex::SimpleRegex(SimpleRegex)");
	}

	// case insensitive
	{
		SimpleRegex re("foo.*");
		TEST_ASSERT(re.matches("foo"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("FOO"), "SimpleRegex::SimpleRegex()");
		re.set_ignore_case(true);
		TEST_ASSERT(re.matches("foo"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(re.matches("FOO"), "SimpleRegex::SimpleRegex()");
		re.set_ignore_case(false);
		TEST_ASSERT(re.matches("foo"), "SimpleRegex::SimpleRegex()");
		TEST_ASSERT(!re.matches("FOO"), "SimpleRegex::SimpleRegex()");
	}
}

} // namespace util
