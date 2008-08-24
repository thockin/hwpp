#include "pp.h"
#include "pp_path.h"
#include <iostream>
#include "pp_test.h"

TEST(test_element)
{
	{
		pp_path::element e("foo");
		TEST_ASSERT(e.to_string() == "foo",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("%foo");
		TEST_ASSERT(e.to_string() == "%foo",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("%foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("%bar")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo_123");
		TEST_ASSERT(e.to_string() == "foo_123",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo_123")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("foo_456")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[]");
		TEST_ASSERT(e.to_string() == "foo[]",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_APPEND,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[$]");
		TEST_ASSERT(e.to_string() == "foo[$]",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[$]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[$]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_TAIL,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[0]");
		TEST_ASSERT(e.to_string() == "foo[0]",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[0]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[0]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_index() == 0,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[010]");
		TEST_ASSERT(e.to_string() == "foo[8]",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[010]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[010]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[8]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[8]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_index() == 8,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[0x10]");
		TEST_ASSERT(e.to_string() == "foo[16]",
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[0x10]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[0x10]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.equals(pp_path::element("foo[16]")),
			"pp_path::element::element()");
		TEST_ASSERT(!e.equals(pp_path::element("bar[16]")),
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_mode() == e.ARRAY_INDEX,
			"pp_path::element::element()");
		TEST_ASSERT(e.array_index() == 16,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("_");
		TEST_ASSERT(e.to_string() == "_",
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("$foo");
		TEST_ASSERT(e.to_string() == "$foo",
			"pp_path::element::element()");
		TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
		TEST_ASSERT(e.is_bookmark() == true,
			"pp_path::element::element()");
	}
	{
		try {
			pp_path::element e("foo$bar");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("$123");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("$");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("$$");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("123");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}
}

TEST(test_ctors)
{
	// test the default ctor
	{
		pp_path path;
		TEST_ASSERT(path == "",
		    "pp_path::pp_path()");
		TEST_ASSERT(!path.is_initialized(),
		    "pp_path::pp_path()");
		TEST_ASSERT(!path.is_absolute(),
		    "pp_path::pp_path()");
	}

	// test construction from a string
	{
		pp_path path("");
		TEST_ASSERT(path == "",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_initialized(),
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string(""));
		TEST_ASSERT(path == "",
			"pp_path::pp_path(string)");
		TEST_ASSERT(!path.is_initialized(),
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/");
		TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/"));
		TEST_ASSERT(path == "/",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	{
		pp_path path("a");
		TEST_ASSERT(path == "a",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("a"));
		TEST_ASSERT(path == "a",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/a");
		TEST_ASSERT(path == "/a",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/a"));
		TEST_ASSERT(path == "/a",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	{
		pp_path path("a/b");
		TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("a/b"));
		TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/a/b");
		TEST_ASSERT(path == "/a/b",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/a/b"));
		TEST_ASSERT(path == "/a/b",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	{
		pp_path path("a/b/c");
		TEST_ASSERT(path == "a/b/c",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("a/b/c"));
		TEST_ASSERT(path == "a/b/c",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(string)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/a/b/c");
		TEST_ASSERT(path == "/a/b/c",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/a/b/c"));
		TEST_ASSERT(path == "/a/b/c",
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(string)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	// test whitespace chomping
	{
		pp_path path("   ");
		TEST_ASSERT(path == "",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("/  ");
		TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("  /  ");
		TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("  /");
		TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("  a/b  ");
		TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}

	// test delimiter collapsing
	{
		pp_path path("////");
		TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("a////b");
		TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("////a////b");
		TEST_ASSERT(path == "/a/b",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		try {
			pp_path path("123");
			TEST_ERROR("pp_path::pp_path()");
		} catch (pp_path::invalid_error &e) {
		}
	}

	// test carats
	{
		pp_path path("$foo");
		TEST_ASSERT(path == "$foo",
			"pp_path::element::element()");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::is_absolute()");
	}
	{
		pp_path path("$foo/foo");
		TEST_ASSERT(path == "$foo/foo",
			"pp_path::element::element()");
		TEST_ASSERT(path != pp_path("$foo/bar"),
			"pp_path::element::element()");
		TEST_ASSERT(path != pp_path("$bar/foo"),
			"pp_path::element::element()");
		TEST_ASSERT(!path.is_absolute(),
			"pp_path::is_absolute()");
	}
	{
		pp_path path("/$foo/foo");
		TEST_ASSERT(path == "/$foo/foo",
			"pp_path::pp_path()");
		TEST_ASSERT(path != pp_path("/$foo/bar"),
			"pp_path::pp_path()");
		TEST_ASSERT(path != pp_path("/$bar/foo"),
			"pp_path::pp_path()");
		TEST_ASSERT(path.is_absolute(),
			"pp_path::is_absolute()");
	}

	// test copy construction
	{
		pp_path path("a/b");
		pp_path path2(path);
		TEST_ASSERT(path2 == "a/b",
			"pp_path::pp_path(char *)");
		TEST_ASSERT(path2.size() == 2,
			"pp_path::pp_path(char *)");
		TEST_ASSERT(!path2.is_absolute(),
			"pp_path::pp_path(char *)");
	}
}

TEST(test_comparisons)
{
	{
		pp_path left("a/b/c");
		pp_path right("a/b/c");
		TEST_ASSERT(left.equals(right), "pp_path::equals()");
		TEST_ASSERT(left == right, "pp_path::operator==()");
	}
	{
		pp_path left("a/b/c");
		pp_path right("d/e/f");
		TEST_ASSERT(!left.equals(right), "pp_path::equals()");
		TEST_ASSERT(left != right, "pp_path::operator!=()");
	}
	{
		pp_path left("a/b/c");
		string right("a/b/c");
		TEST_ASSERT(left == right, "pp_path::operator==()");
	}
	{
		pp_path left("a/b/c");
		string right("d/e/f");
		TEST_ASSERT(left != right, "pp_path::operator!=()");
	}
	{
		string left("a/b/c");
		pp_path right("a/b/c");
		TEST_ASSERT(left == right, "pp_path::operator==()");
	}
	{
		string left("a/b/c");
		pp_path right("d/e/f");
		TEST_ASSERT(left != right, "pp_path::operator!=()");
	}
}

TEST(test_iters)
{
	pp_path path("a/b/c");

	pp_path::iterator it = path.begin();
	TEST_ASSERT(it != path.end(), "pp_path::end()");
	TEST_ASSERT(*it == "a", "pp_path::begin()");
	it++;
	TEST_ASSERT(it != path.end(), "pp_path::end()");
	TEST_ASSERT(*it == "b", "pp_path::iterator::operator++()");
	it++;
	TEST_ASSERT(it != path.end(), "pp_path::end()");
	TEST_ASSERT(*it == "c", "pp_path::iterator::operator++()");
	it++;
	TEST_ASSERT(it == path.end(), "pp_path::end()");
	it--;
	TEST_ASSERT(it != path.end(), "pp_path::end()");
	TEST_ASSERT(*it == "c", "pp_path::iterator::operator--()");
	it--;
	TEST_ASSERT(it != path.end(), "pp_path::end()");
	TEST_ASSERT(*it == "b", "pp_path::iterator::operator--()");
	it--;
	TEST_ASSERT(it != path.end(), "pp_path::end()");
	TEST_ASSERT(it == path.begin(), "pp_path::end()");
	TEST_ASSERT(*it == "a", "pp_path::iterator::operator--()");

	pp_path::reverse_iterator rit = path.rbegin();
	TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	TEST_ASSERT(*rit == "c", "pp_path::rbegin()");
	rit++;
	TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	TEST_ASSERT(*rit == "b", "pp_path::iterator::operator++()");
	rit++;
	TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	TEST_ASSERT(*rit == "a", "pp_path::iterator::operator++()");
	rit++;
	TEST_ASSERT(rit == path.rend(), "pp_path::rend()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	TEST_ASSERT(*rit == "a", "pp_path::iterator::operator--()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	TEST_ASSERT(*rit == "b", "pp_path::iterator::operator--()");
	rit--;
	TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	TEST_ASSERT(rit == path.rbegin(), "pp_path::rend()");
	TEST_ASSERT(*rit == "c", "pp_path::iterator::operator--()");
}

TEST(test_contents)
{
	pp_path path;

	// create a path
	path.push_back("a");
	path.push_back("b");
	path.push_back("c");
	TEST_ASSERT(path.size() == 3, "pp_path::size()");

	pp_path::iterator it = path.begin();
	TEST_ASSERT(*it == "a", "pp_path::begin()");
	it++;
	TEST_ASSERT(*it == "b", "pp_path::iterator::operator++()");
	it++;
	TEST_ASSERT(*it == "c", "pp_path::iterator::operator++()");
	it++;
	TEST_ASSERT(it == path.end(), "pp_path::end()");

	// test front() and back()
	TEST_ASSERT(path.front() == "a", "pp_path::front()");
	TEST_ASSERT(path.back() == "c", "pp_path::back()");

	// pop items
	TEST_ASSERT(path.pop_front() == "a", "pp_path::pop_front()");
	TEST_ASSERT(path.pop_back() == "c", "pp_path::pop_back()");
	TEST_ASSERT(path.size() == 1, "pp_path::size()");
	TEST_ASSERT(path.front() == "b", "pp_path::front()");
	TEST_ASSERT(path.back() == "b", "pp_path::back()");
	path.pop_back();
	TEST_ASSERT(path.size() == 0, "pp_path::size()");

	// cause exceptions
	try {
		path.pop_front();
		TEST_ERROR("pp_path::pop_front()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.pop_back();
		TEST_ERROR("pp_path::pop_back()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.front();
		TEST_ERROR("pp_path::front()");
	} catch (std::out_of_range &e) {
	}
	try {
		path.back();
		TEST_ERROR("pp_path::back()");
	} catch (std::out_of_range &e) {
	}

	// test clear()
	path = pp_path("a/b/c");
	TEST_ASSERT(path.size() == 3, "pp_path::size()");
	path.clear();
	TEST_ASSERT(path.size() == 0, "pp_path::size()");
	TEST_ASSERT(path.is_initialized() == 0, "pp_path::size()");

	//test to_string()
	path = pp_path("a/b/c");
	TEST_ASSERT(path.to_string() == "a/b/c",
	    "pp_path::to_string()");
	path = pp_path("/a/b/c");
	TEST_ASSERT(path.to_string() == "/a/b/c",
	    "pp_path::to_string()");

	// test catenation operators
	// path += path
	path = pp_path("a/b/c");
	path += pp_path("d/e/f");
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+=(pp_path, pp_path)");
	// path += string
	path = pp_path("a/b/c");
	path += "d";
	TEST_ASSERT(path == "a/b/c/d",
	    "pp_path::operator+=(pp_path, string)");
	path = pp_path("a/b/c");
	path += "d/e/f";
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+=(pp_path, string)");
	// path + path
	path = pp_path("a/b/c") + pp_path("d/e/f");
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+=(pp_path, pp_path)");
	// path + string
	path = pp_path("a/b/c") + "d";
	TEST_ASSERT(path == "a/b/c/d",
	    "pp_path::operator+(pp_path, string)");
	path = pp_path("a/b/c") + "d/e/f";
	TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+(pp_path, string)");
}

TEST(test_const)
{
	pp_path path("a/b/c");
	const pp_path &const_path = path;
	TEST_ASSERT(const_path.size() == 3, "pp_path::size()");

	pp_path::iterator it = path.begin();
	pp_path::const_iterator cit = const_path.begin();
	TEST_ASSERT(*it == *cit, "pp_path::begin()");
	it++;
	cit++;
	TEST_ASSERT(*it == *cit, "pp_path::iterator::operator++()");
	it++;
	cit++;
	TEST_ASSERT(*it == *cit, "pp_path::iterator::operator++()");
}
