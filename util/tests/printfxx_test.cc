#include "util/printfxx.h"
#include <cstdio>
#include "util/test.h"

using printfxx::sprintfxx;

TEST(test_sprintf)
{
	std::string s;

	s = sprintfxx("foo");
	TEST_ASSERT_EQ(s, "foo")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %d", 1);
	TEST_ASSERT_EQ(s, "foo 1")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %d %d", 1, 2);
	TEST_ASSERT_EQ(s, "foo 1 2")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %d %d %d", 1, 2, 3);
	TEST_ASSERT_EQ(s, "foo 1 2 3")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %d %d %d %d", 1, 2, 3, 4);
	TEST_ASSERT_EQ(s, "foo 1 2 3 4")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %d %d %d %d %d", 1, 2, 3, 4, 5);
	TEST_ASSERT_EQ(s, "foo 1 2 3 4 5")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %s", "a");
	TEST_ASSERT_EQ(s, "foo a")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %s %s", "a", "b");
	TEST_ASSERT_EQ(s, "foo a b")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %s %s %s", "a", "b", "c");
	TEST_ASSERT_EQ(s, "foo a b c")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %x", 10);
	TEST_ASSERT_EQ(s, "foo a")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %x %x", 10, 11);
	TEST_ASSERT_EQ(s, "foo a b")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %x %x %x", 10, 11, 12);
	TEST_ASSERT_EQ(s, "foo a b c")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %#x", 10);
	TEST_ASSERT_EQ(s, "foo 0xa")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %#x %#x", 10, 11);
	TEST_ASSERT_EQ(s, "foo 0xa 0xb")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %#x %#x %#x", 10, 11, 12);
	TEST_ASSERT_EQ(s, "foo 0xa 0xb 0xc")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %02x", 10);
	TEST_ASSERT_EQ(s, "foo 0a")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %02x %04x", 10, 11);
	TEST_ASSERT_EQ(s, "foo 0a 000b")
	    << "sprintfxx(): got '" << s << "'";

	s = sprintfxx("foo %02x %04x %08x", 10, 11, 12);
	TEST_ASSERT_EQ(s, "foo 0a 000b 0000000c")
	    << "sprintfxx(): got '" << s << "'";
}

struct FooBar {
	FooBar(int f, int b) : foo(f), bar(b) {}
	int foo;
	int bar;
};
inline std::ostream &
operator<<(std::ostream& o, const FooBar &foobar)
{
	return o << "{" << foobar.foo << "," << foobar.bar << "}";
}

TEST(test_user_defined_types)
{
	std::string s;

	FooBar fb(93, 76);
	s = sprintfxx("fb %s", fb);
	TEST_ASSERT_EQ(s, "fb {93,76}")
	    << "sprintfxx(): got '" << s << "'";
}
