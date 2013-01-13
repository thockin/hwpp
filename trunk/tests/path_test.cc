#include "hwpp.h"
#include "path.h"
#include <iostream>
#include "util/test.h"

TEST(test_element)
{
	{
		hwpp::Path::Element e("foo");
		TEST_ASSERT(e.to_string() == "foo",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("%foo");
		TEST_ASSERT(e.to_string() == "%foo",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("%foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("%bar")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo_123");
		TEST_ASSERT(e.to_string() == "foo_123",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo_123")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo_456")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[]");
		TEST_ASSERT(e.to_string() == "foo[]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_APPEND,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[0]");
		TEST_ASSERT(e.to_string() == "foo[0]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[0]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[0]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == 0,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[010]");
		TEST_ASSERT(e.to_string() == "foo[8]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[010]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[010]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[8]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[8]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == 8,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[0x10]");
		TEST_ASSERT(e.to_string() == "foo[16]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[0x10]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[0x10]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[16]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[16]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == 16,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[-1]");
		TEST_ASSERT(e.to_string() == "foo[-1]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[-1]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[-1]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -1,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[-2]");
		TEST_ASSERT(e.to_string() == "foo[-2]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[-2]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[-2]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -2,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[-010]");
		TEST_ASSERT(e.to_string() == "foo[-8]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[-8]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[-8]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -8,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("foo[-0x10]");
		TEST_ASSERT(e.to_string() == "foo[-16]",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.equals(hwpp::Path::Element("foo[-16]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("foo")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(hwpp::Path::Element("bar[-16]")),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -16,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("_");
		TEST_ASSERT(e.to_string() == "_",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"hwpp::Path::Element::Element()");
	}
	{
		hwpp::Path::Element e("$foo");
		TEST_ASSERT(e.to_string() == "$foo",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == true,
			"hwpp::Path::Element::Element()");
	}
	{
		try {
			hwpp::Path::Element e("foo$bar");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}
	{
		try {
			hwpp::Path::Element e("$123");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}
	{
		try {
			hwpp::Path::Element e("$");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}
	{
		try {
			hwpp::Path::Element e("$$");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}
	{
		try {
			hwpp::Path::Element e("123");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}
	{
		try {
			hwpp::Path::Element e("");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}
}

TEST(test_ctors)
{
	// test the default ctor
	{
		hwpp::Path path;
		TEST_ASSERT(path == "",
		    "hwpp::Path::hwpp::Path()");
		TEST_ASSERT(!path.is_initialized(),
		    "hwpp::Path::hwpp::Path()");
		TEST_ASSERT(!path.is_absolute(),
		    "hwpp::Path::hwpp::Path()");
	}

	// test construction from a string
	{
		hwpp::Path path("");
		TEST_ASSERT(path == "",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_initialized(),
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string(""));
		TEST_ASSERT(path == "",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(!path.is_initialized(),
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}
	{
		hwpp::Path path("/");
		TEST_ASSERT(path == "/",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("/"));
		TEST_ASSERT(path == "/",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_initialized(),
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}

	{
		hwpp::Path path("a");
		TEST_ASSERT(path == "a",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 1,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("a"));
		TEST_ASSERT(path == "a",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_initialized(),
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 1,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}
	{
		hwpp::Path path("/a");
		TEST_ASSERT(path == "/a",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 1,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("/a"));
		TEST_ASSERT(path == "/a",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_initialized(),
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 1,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}

	{
		hwpp::Path path("a/b");
		TEST_ASSERT(path == "a/b",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("a/b"));
		TEST_ASSERT(path == "a/b",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}
	{
		hwpp::Path path("/a/b");
		TEST_ASSERT(path == "/a/b",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("/a/b"));
		TEST_ASSERT(path == "/a/b",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}

	{
		hwpp::Path path("a/b/c");
		TEST_ASSERT(path == "a/b/c",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 3,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("a/b/c"));
		TEST_ASSERT(path == "a/b/c",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 3,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}
	{
		hwpp::Path path("/a/b/c");
		TEST_ASSERT(path == "/a/b/c",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 3,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path(string("/a/b/c"));
		TEST_ASSERT(path == "/a/b/c",
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.size() == 3,
			"hwpp::Path::hwpp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(string)");
	}

	// test whitespace chomping
	{
		hwpp::Path path("   ");
		TEST_ASSERT(path == "",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path("/  ");
		TEST_ASSERT(path == "/",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path("  /  ");
		TEST_ASSERT(path == "/",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path("  /");
		TEST_ASSERT(path == "/",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path("  a/b  ");
		TEST_ASSERT(path == "a/b",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}

	// test delimiter collapsing
	{
		hwpp::Path path("////");
		TEST_ASSERT(path == "/",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path("a////b");
		TEST_ASSERT(path == "a/b",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		hwpp::Path path("////a////b");
		TEST_ASSERT(path == "/a/b",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
	{
		try {
			hwpp::Path path("123");
			TEST_FAIL("hwpp::Path::hwpp::Path()");
		} catch (hwpp::Path::InvalidError &e) {
		}
	}

	// test carats
	{
		hwpp::Path path("$foo");
		TEST_ASSERT(path == "$foo",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::is_absolute()");
	}
	{
		hwpp::Path path("$foo/foo");
		TEST_ASSERT(path == "$foo/foo",
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(path != hwpp::Path("$foo/bar"),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(path != hwpp::Path("$bar/foo"),
			"hwpp::Path::Element::Element()");
		TEST_ASSERT(!path.is_absolute(),
			"hwpp::Path::is_absolute()");
	}
	{
		hwpp::Path path("/$foo/foo");
		TEST_ASSERT(path == "/$foo/foo",
			"hwpp::Path::hwpp::Path()");
		TEST_ASSERT(path != hwpp::Path("/$foo/bar"),
			"hwpp::Path::hwpp::Path()");
		TEST_ASSERT(path != hwpp::Path("/$bar/foo"),
			"hwpp::Path::hwpp::Path()");
		TEST_ASSERT(path.is_absolute(),
			"hwpp::Path::is_absolute()");
	}

	// test copy construction
	{
		hwpp::Path path("a/b");
		hwpp::Path path2(path);
		TEST_ASSERT(path2 == "a/b",
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(path2.size() == 2,
			"hwpp::Path::hwpp::Path(char *)");
		TEST_ASSERT(!path2.is_absolute(),
			"hwpp::Path::hwpp::Path(char *)");
	}
}

TEST(test_comparisons)
{
	{
		hwpp::Path left("a/b/c");
		hwpp::Path right("a/b/c");
		TEST_ASSERT(left.equals(right), "hwpp::Path::equals()");
		TEST_ASSERT(left == right, "hwpp::Path::operator==()");
	}
	{
		hwpp::Path left("a/b/c");
		hwpp::Path right("d/e/f");
		TEST_ASSERT(!left.equals(right), "hwpp::Path::equals()");
		TEST_ASSERT(left != right, "hwpp::Path::operator!=()");
	}
	{
		hwpp::Path left("a/b/c");
		string right("a/b/c");
		TEST_ASSERT(left == right, "hwpp::Path::operator==()");
	}
	{
		hwpp::Path left("a/b/c");
		string right("d/e/f");
		TEST_ASSERT(left != right, "hwpp::Path::operator!=()");
	}
	{
		string left("a/b/c");
		hwpp::Path right("a/b/c");
		TEST_ASSERT(left == right, "hwpp::Path::operator==()");
	}
	{
		string left("a/b/c");
		hwpp::Path right("d/e/f");
		TEST_ASSERT(left != right, "hwpp::Path::operator!=()");
	}
}

TEST(test_iters)
{
	hwpp::Path path("a/b/c");

	hwpp::Path::iterator it = path.begin();
	TEST_ASSERT(it != path.end(), "hwpp::Path::end()");
	TEST_ASSERT(*it == "a", "hwpp::Path::begin()");
	it++;
	TEST_ASSERT(it != path.end(), "hwpp::Path::end()");
	TEST_ASSERT(*it == "b", "hwpp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(it != path.end(), "hwpp::Path::end()");
	TEST_ASSERT(*it == "c", "hwpp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(it == path.end(), "hwpp::Path::end()");
	it--;
	TEST_ASSERT(it != path.end(), "hwpp::Path::end()");
	TEST_ASSERT(*it == "c", "hwpp::Path::iterator::operator--()");
	it--;
	TEST_ASSERT(it != path.end(), "hwpp::Path::end()");
	TEST_ASSERT(*it == "b", "hwpp::Path::iterator::operator--()");
	it--;
	TEST_ASSERT(it != path.end(), "hwpp::Path::end()");
	TEST_ASSERT(it == path.begin(), "hwpp::Path::end()");
	TEST_ASSERT(*it == "a", "hwpp::Path::iterator::operator--()");

	hwpp::Path::reverse_iterator rit = path.rbegin();
	TEST_ASSERT(rit != path.rend(), "hwpp::Path::rend()");
	TEST_ASSERT(*rit == "c", "hwpp::Path::rbegin()");
	rit++;
	TEST_ASSERT(rit != path.rend(), "hwpp::Path::rend()");
	TEST_ASSERT(*rit == "b", "hwpp::Path::iterator::operator++()");
	rit++;
	TEST_ASSERT(rit != path.rend(), "hwpp::Path::rend()");
	TEST_ASSERT(*rit == "a", "hwpp::Path::iterator::operator++()");
	rit++;
	TEST_ASSERT(rit == path.rend(), "hwpp::Path::rend()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "hwpp::Path::rend()");
	TEST_ASSERT(*rit == "a", "hwpp::Path::iterator::operator--()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "hwpp::Path::rend()");
	TEST_ASSERT(*rit == "b", "hwpp::Path::iterator::operator--()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "hwpp::Path::rend()");
	TEST_ASSERT(rit == path.rbegin(), "hwpp::Path::rend()");
	TEST_ASSERT(*rit == "c", "hwpp::Path::iterator::operator--()");
}

TEST(test_contents)
{
	hwpp::Path path;
	hwpp::Path path2;
	hwpp::Path::iterator it;

	// create a path
	path.push_back("a");
	path.push_back("b");
	path.push_back("c");
	TEST_ASSERT(path.size() == 3, "hwpp::Path::size()");

	it = path.begin();
	TEST_ASSERT(*it == "a", "hwpp::Path::begin()");
	it++;
	TEST_ASSERT(*it == "b", "hwpp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(*it == "c", "hwpp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(it == path.end(), "hwpp::Path::end()");

	// test front() and back()
	TEST_ASSERT(path.front() == "a", "hwpp::Path::front()");
	TEST_ASSERT(path.back() == "c", "hwpp::Path::back()");

	// pop items
	TEST_ASSERT(path.pop_front() == "a", "hwpp::Path::pop_front()");
	TEST_ASSERT(path.pop_back() == "c", "hwpp::Path::pop_back()");
	TEST_ASSERT(path.size() == 1, "hwpp::Path::size()");
	TEST_ASSERT(path.front() == "b", "hwpp::Path::front()");
	TEST_ASSERT(path.back() == "b", "hwpp::Path::back()");
	path.pop_back();
	TEST_ASSERT(path.size() == 0, "hwpp::Path::size()");

	// cause exceptions
	try {
		path.pop_front();
		TEST_FAIL("hwpp::Path::pop_front()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.pop_back();
		TEST_FAIL("hwpp::Path::pop_back()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.front();
		TEST_FAIL("hwpp::Path::front()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.back();
		TEST_FAIL("hwpp::Path::back()");
	} catch (std::out_of_range &e) {
	}

	// test clear()
	path = hwpp::Path("a/b/c");
	TEST_ASSERT(path.size() == 3, "hwpp::Path::size()");
	path.clear();
	TEST_ASSERT(path.size() == 0, "hwpp::Path::size()");
	TEST_ASSERT(path.is_initialized() == 0, "hwpp::Path::size()");

	//test to_string()
	path = hwpp::Path("a/b/c");
	TEST_ASSERT(path.to_string() == "a/b/c",
	    "hwpp::Path::to_string()");
	path = hwpp::Path("/a/b/c");
	TEST_ASSERT(path.to_string() == "/a/b/c",
	    "hwpp::Path::to_string()");

	// test catenation operators
	// path += path
	path = hwpp::Path("a/b/c");
	path += hwpp::Path("d/e/f");
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "hwpp::Path::operator+=(hwpp::Path, hwpp::Path)");
	// path += string
	path = hwpp::Path("a/b/c");
	path += "d";
	TEST_ASSERT(path == "a/b/c/d",
	    "hwpp::Path::operator+=(hwpp::Path, string)");
	path = hwpp::Path("a/b/c");
	path += "d/e/f";
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "hwpp::Path::operator+=(hwpp::Path, string)");
	// path + path
	path = hwpp::Path("a/b/c") + hwpp::Path("d/e/f");
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "hwpp::Path::operator+=(hwpp::Path, hwpp::Path)");
	// path + string
	path = hwpp::Path("a/b/c") + "d";
	TEST_ASSERT(path == "a/b/c/d",
	    "hwpp::Path::operator+(hwpp::Path, string)");
	path = hwpp::Path("a/b/c") + "d/e/f";
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "hwpp::Path::operator+(hwpp::Path, string)");

	// test insert
	path = "a/b";
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "b", "hwpp::Path::iterator::operator++()");
	path.insert(it, hwpp::Path::Element("c"));
	TEST_ASSERT(path == "a/c/b", "hwpp::Path::insert(iterator, element)");
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "c", "hwpp::Path::iterator::operator++()");
	path2 = "d/e/f";
	path.insert(it, path2.begin(), path2.end());
	TEST_ASSERT(path == "a/d/e/f/c/b",
	    "hwpp::Path::insert(iterator, iterator, iterator)");

	// test erase()
	path = hwpp::Path("a/b/c/d/e");
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "b", "hwpp::Path::iterator::operator++()");
	path.erase(it);
	TEST_ASSERT(path == "a/c/d/e", "hwpp::Path::erase(iterator)");
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "c", "hwpp::Path::iterator::operator++()");
	path.erase(it, path.end());
	TEST_ASSERT(path == "a", "hwpp::Path::erase(iterator, iterator)");

	// test splice()
	path = "a/b";
	path2 = "c/d/e";
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "b", "hwpp::Path::iterator::operator++()");
	path.splice(it, path2);
	TEST_ASSERT(path == "a/c/d/e/b", "hwpp::Path::splice(iterator, hwpp::Path)");
}

TEST(test_const)
{
	hwpp::Path path("a/b/c");
	const hwpp::Path &const_path = path;
	TEST_ASSERT(const_path.size() == 3, "hwpp::Path::size()");

	hwpp::Path::iterator it = path.begin();
	hwpp::Path::const_iterator cit = const_path.begin();
	TEST_ASSERT(*it == *cit, "hwpp::Path::begin()");
	it++;
	cit++;
	TEST_ASSERT(*it == *cit, "hwpp::Path::iterator::operator++()");
	it++;
	cit++;
	TEST_ASSERT(*it == *cit, "hwpp::Path::iterator::operator++()");
}
