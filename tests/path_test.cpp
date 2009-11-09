#include "pp/pp.h"
#include "pp/path.h"
#include <iostream>
#include "pp_test.h"

TEST(test_element)
{
	{
		pp::Path::Element e("foo");
		TEST_ASSERT(e.to_string() == "foo",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("%foo");
		TEST_ASSERT(e.to_string() == "%foo",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("%foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("%bar")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo_123");
		TEST_ASSERT(e.to_string() == "foo_123",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo_123")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo_456")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[]");
		TEST_ASSERT(e.to_string() == "foo[]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_APPEND,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[0]");
		TEST_ASSERT(e.to_string() == "foo[0]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[0]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[0]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == 0,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[010]");
		TEST_ASSERT(e.to_string() == "foo[8]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[010]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[010]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[8]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[8]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == 8,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[0x10]");
		TEST_ASSERT(e.to_string() == "foo[16]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[0x10]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[0x10]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[16]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[16]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == 16,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[-1]");
		TEST_ASSERT(e.to_string() == "foo[-1]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[-1]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[-1]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -1,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[-2]");
		TEST_ASSERT(e.to_string() == "foo[-2]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[-2]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[-2]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -2,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[-010]");
		TEST_ASSERT(e.to_string() == "foo[-8]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[-8]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[-8]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -8,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("foo[-0x10]");
		TEST_ASSERT(e.to_string() == "foo[-16]",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.equals(pp::Path::Element("foo[-16]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("foo")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!e.equals(pp::Path::Element("bar[-16]")),
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == true,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.array_index() == -16,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("_");
		TEST_ASSERT(e.to_string() == "_",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp::Path::Element::Element()");
	}
	{
		pp::Path::Element e("$foo");
		TEST_ASSERT(e.to_string() == "$foo",
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_array() == false,
			"pp::Path::Element::Element()");
		TEST_ASSERT(e.is_bookmark() == true,
			"pp::Path::Element::Element()");
	}
	{
		try {
			pp::Path::Element e("foo$bar");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}
	{
		try {
			pp::Path::Element e("$123");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}
	{
		try {
			pp::Path::Element e("$");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}
	{
		try {
			pp::Path::Element e("$$");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}
	{
		try {
			pp::Path::Element e("123");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}
	{
		try {
			pp::Path::Element e("");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}
}

TEST(test_ctors)
{
	// test the default ctor
	{
		pp::Path path;
		TEST_ASSERT(path == "",
		    "pp::Path::pp::Path()");
		TEST_ASSERT(!path.is_initialized(),
		    "pp::Path::pp::Path()");
		TEST_ASSERT(!path.is_absolute(),
		    "pp::Path::pp::Path()");
	}

	// test construction from a string
	{
		pp::Path path("");
		TEST_ASSERT(path == "",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_initialized(),
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string(""));
		TEST_ASSERT(path == "",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(!path.is_initialized(),
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}
	{
		pp::Path path("/");
		TEST_ASSERT(path == "/",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("/"));
		TEST_ASSERT(path == "/",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_initialized(),
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}

	{
		pp::Path path("a");
		TEST_ASSERT(path == "a",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 1,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("a"));
		TEST_ASSERT(path == "a",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_initialized(),
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 1,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}
	{
		pp::Path path("/a");
		TEST_ASSERT(path == "/a",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 1,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("/a"));
		TEST_ASSERT(path == "/a",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_initialized(),
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 1,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}

	{
		pp::Path path("a/b");
		TEST_ASSERT(path == "a/b",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("a/b"));
		TEST_ASSERT(path == "a/b",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}
	{
		pp::Path path("/a/b");
		TEST_ASSERT(path == "/a/b",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("/a/b"));
		TEST_ASSERT(path == "/a/b",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}

	{
		pp::Path path("a/b/c");
		TEST_ASSERT(path == "a/b/c",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 3,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("a/b/c"));
		TEST_ASSERT(path == "a/b/c",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 3,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}
	{
		pp::Path path("/a/b/c");
		TEST_ASSERT(path == "/a/b/c",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 3,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path(string("/a/b/c"));
		TEST_ASSERT(path == "/a/b/c",
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.size() == 3,
			"pp::Path::pp::Path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(string)");
	}

	// test whitespace chomping
	{
		pp::Path path("   ");
		TEST_ASSERT(path == "",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path("/  ");
		TEST_ASSERT(path == "/",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path("  /  ");
		TEST_ASSERT(path == "/",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path("  /");
		TEST_ASSERT(path == "/",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path("  a/b  ");
		TEST_ASSERT(path == "a/b",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}

	// test delimiter collapsing
	{
		pp::Path path("////");
		TEST_ASSERT(path == "/",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path("a////b");
		TEST_ASSERT(path == "a/b",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		pp::Path path("////a////b");
		TEST_ASSERT(path == "/a/b",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
	{
		try {
			pp::Path path("123");
			TEST_FAIL("pp::Path::pp::Path()");
		} catch (pp::Path::InvalidError &e) {
		}
	}

	// test carats
	{
		pp::Path path("$foo");
		TEST_ASSERT(path == "$foo",
			"pp::Path::Element::Element()");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::is_absolute()");
	}
	{
		pp::Path path("$foo/foo");
		TEST_ASSERT(path == "$foo/foo",
			"pp::Path::Element::Element()");
		TEST_ASSERT(path != pp::Path("$foo/bar"),
			"pp::Path::Element::Element()");
		TEST_ASSERT(path != pp::Path("$bar/foo"),
			"pp::Path::Element::Element()");
		TEST_ASSERT(!path.is_absolute(),
			"pp::Path::is_absolute()");
	}
	{
		pp::Path path("/$foo/foo");
		TEST_ASSERT(path == "/$foo/foo",
			"pp::Path::pp::Path()");
		TEST_ASSERT(path != pp::Path("/$foo/bar"),
			"pp::Path::pp::Path()");
		TEST_ASSERT(path != pp::Path("/$bar/foo"),
			"pp::Path::pp::Path()");
		TEST_ASSERT(path.is_absolute(),
			"pp::Path::is_absolute()");
	}

	// test copy construction
	{
		pp::Path path("a/b");
		pp::Path path2(path);
		TEST_ASSERT(path2 == "a/b",
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(path2.size() == 2,
			"pp::Path::pp::Path(char *)");
		TEST_ASSERT(!path2.is_absolute(),
			"pp::Path::pp::Path(char *)");
	}
}

TEST(test_comparisons)
{
	{
		pp::Path left("a/b/c");
		pp::Path right("a/b/c");
		TEST_ASSERT(left.equals(right), "pp::Path::equals()");
		TEST_ASSERT(left == right, "pp::Path::operator==()");
	}
	{
		pp::Path left("a/b/c");
		pp::Path right("d/e/f");
		TEST_ASSERT(!left.equals(right), "pp::Path::equals()");
		TEST_ASSERT(left != right, "pp::Path::operator!=()");
	}
	{
		pp::Path left("a/b/c");
		string right("a/b/c");
		TEST_ASSERT(left == right, "pp::Path::operator==()");
	}
	{
		pp::Path left("a/b/c");
		string right("d/e/f");
		TEST_ASSERT(left != right, "pp::Path::operator!=()");
	}
	{
		string left("a/b/c");
		pp::Path right("a/b/c");
		TEST_ASSERT(left == right, "pp::Path::operator==()");
	}
	{
		string left("a/b/c");
		pp::Path right("d/e/f");
		TEST_ASSERT(left != right, "pp::Path::operator!=()");
	}
}

TEST(test_iters)
{
	pp::Path path("a/b/c");

	pp::Path::iterator it = path.begin();
	TEST_ASSERT(it != path.end(), "pp::Path::end()");
	TEST_ASSERT(*it == "a", "pp::Path::begin()");
	it++;
	TEST_ASSERT(it != path.end(), "pp::Path::end()");
	TEST_ASSERT(*it == "b", "pp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(it != path.end(), "pp::Path::end()");
	TEST_ASSERT(*it == "c", "pp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(it == path.end(), "pp::Path::end()");
	it--;
	TEST_ASSERT(it != path.end(), "pp::Path::end()");
	TEST_ASSERT(*it == "c", "pp::Path::iterator::operator--()");
	it--;
	TEST_ASSERT(it != path.end(), "pp::Path::end()");
	TEST_ASSERT(*it == "b", "pp::Path::iterator::operator--()");
	it--;
	TEST_ASSERT(it != path.end(), "pp::Path::end()");
	TEST_ASSERT(it == path.begin(), "pp::Path::end()");
	TEST_ASSERT(*it == "a", "pp::Path::iterator::operator--()");

	pp::Path::reverse_iterator rit = path.rbegin();
	TEST_ASSERT(rit != path.rend(), "pp::Path::rend()");
	TEST_ASSERT(*rit == "c", "pp::Path::rbegin()");
	rit++;
	TEST_ASSERT(rit != path.rend(), "pp::Path::rend()");
	TEST_ASSERT(*rit == "b", "pp::Path::iterator::operator++()");
	rit++;
	TEST_ASSERT(rit != path.rend(), "pp::Path::rend()");
	TEST_ASSERT(*rit == "a", "pp::Path::iterator::operator++()");
	rit++;
	TEST_ASSERT(rit == path.rend(), "pp::Path::rend()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "pp::Path::rend()");
	TEST_ASSERT(*rit == "a", "pp::Path::iterator::operator--()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "pp::Path::rend()");
	TEST_ASSERT(*rit == "b", "pp::Path::iterator::operator--()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "pp::Path::rend()");
	TEST_ASSERT(rit == path.rbegin(), "pp::Path::rend()");
	TEST_ASSERT(*rit == "c", "pp::Path::iterator::operator--()");
}

TEST(test_contents)
{
	pp::Path path;
	pp::Path path2;
	pp::Path::iterator it;

	// create a path
	path.push_back("a");
	path.push_back("b");
	path.push_back("c");
	TEST_ASSERT(path.size() == 3, "pp::Path::size()");

	it = path.begin();
	TEST_ASSERT(*it == "a", "pp::Path::begin()");
	it++;
	TEST_ASSERT(*it == "b", "pp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(*it == "c", "pp::Path::iterator::operator++()");
	it++;
	TEST_ASSERT(it == path.end(), "pp::Path::end()");

	// test front() and back()
	TEST_ASSERT(path.front() == "a", "pp::Path::front()");
	TEST_ASSERT(path.back() == "c", "pp::Path::back()");

	// pop items
	TEST_ASSERT(path.pop_front() == "a", "pp::Path::pop_front()");
	TEST_ASSERT(path.pop_back() == "c", "pp::Path::pop_back()");
	TEST_ASSERT(path.size() == 1, "pp::Path::size()");
	TEST_ASSERT(path.front() == "b", "pp::Path::front()");
	TEST_ASSERT(path.back() == "b", "pp::Path::back()");
	path.pop_back();
	TEST_ASSERT(path.size() == 0, "pp::Path::size()");

	// cause exceptions
	try {
		path.pop_front();
		TEST_FAIL("pp::Path::pop_front()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.pop_back();
		TEST_FAIL("pp::Path::pop_back()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.front();
		TEST_FAIL("pp::Path::front()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.back();
		TEST_FAIL("pp::Path::back()");
	} catch (std::out_of_range &e) {
	}

	// test clear()
	path = pp::Path("a/b/c");
	TEST_ASSERT(path.size() == 3, "pp::Path::size()");
	path.clear();
	TEST_ASSERT(path.size() == 0, "pp::Path::size()");
	TEST_ASSERT(path.is_initialized() == 0, "pp::Path::size()");

	//test to_string()
	path = pp::Path("a/b/c");
	TEST_ASSERT(path.to_string() == "a/b/c",
	    "pp::Path::to_string()");
	path = pp::Path("/a/b/c");
	TEST_ASSERT(path.to_string() == "/a/b/c",
	    "pp::Path::to_string()");

	// test catenation operators
	// path += path
	path = pp::Path("a/b/c");
	path += pp::Path("d/e/f");
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp::Path::operator+=(pp::Path, pp::Path)");
	// path += string
	path = pp::Path("a/b/c");
	path += "d";
	TEST_ASSERT(path == "a/b/c/d",
	    "pp::Path::operator+=(pp::Path, string)");
	path = pp::Path("a/b/c");
	path += "d/e/f";
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp::Path::operator+=(pp::Path, string)");
	// path + path
	path = pp::Path("a/b/c") + pp::Path("d/e/f");
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp::Path::operator+=(pp::Path, pp::Path)");
	// path + string
	path = pp::Path("a/b/c") + "d";
	TEST_ASSERT(path == "a/b/c/d",
	    "pp::Path::operator+(pp::Path, string)");
	path = pp::Path("a/b/c") + "d/e/f";
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp::Path::operator+(pp::Path, string)");

	// test insert
	path = "a/b";
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "b", "pp::Path::iterator::operator++()");
	path.insert(it, pp::Path::Element("c"));
	TEST_ASSERT(path == "a/c/b", "pp::Path::insert(iterator, element)");
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "c", "pp::Path::iterator::operator++()");
	path2 = "d/e/f";
	path.insert(it, path2.begin(), path2.end());
	TEST_ASSERT(path == "a/d/e/f/c/b",
	    "pp::Path::insert(iterator, iterator, iterator)");

	// test erase()
	path = pp::Path("a/b/c/d/e");
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "b", "pp::Path::iterator::operator++()");
	path.erase(it);
	TEST_ASSERT(path == "a/c/d/e", "pp::Path::erase(iterator)");
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "c", "pp::Path::iterator::operator++()");
	path.erase(it, path.end());
	TEST_ASSERT(path == "a", "pp::Path::erase(iterator, iterator)");

	// test splice()
	path = "a/b";
	path2 = "c/d/e";
	it = path.begin();
	it++;
	TEST_ASSERT(*it == "b", "pp::Path::iterator::operator++()");
	path.splice(it, path2);
	TEST_ASSERT(path == "a/c/d/e/b", "pp::Path::splice(iterator, pp::Path)");
}

TEST(test_const)
{
	pp::Path path("a/b/c");
	const pp::Path &const_path = path;
	TEST_ASSERT(const_path.size() == 3, "pp::Path::size()");

	pp::Path::iterator it = path.begin();
	pp::Path::const_iterator cit = const_path.begin();
	TEST_ASSERT(*it == *cit, "pp::Path::begin()");
	it++;
	cit++;
	TEST_ASSERT(*it == *cit, "pp::Path::iterator::operator++()");
	it++;
	cit++;
	TEST_ASSERT(*it == *cit, "pp::Path::iterator::operator++()");
}
