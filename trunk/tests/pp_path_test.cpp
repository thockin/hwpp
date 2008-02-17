/*
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp.h"
#include "pp_test.h"
#include "pp_path.h"
#include <iostream>

using namespace std;

int
test_element()
{
	int ret = 0;

	{
		pp_path::element e("foo");
		ret += PP_TEST_ASSERT(e.to_string() == "foo",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("%foo");
		ret += PP_TEST_ASSERT(e.to_string() == "%foo",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("%foo")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("%bar")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo_123");
		ret += PP_TEST_ASSERT(e.to_string() == "foo_123",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo_123")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("foo_456")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[]");
		ret += PP_TEST_ASSERT(e.to_string() == "foo[]",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo[]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar[]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.array_index() == -1,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[0]");
		ret += PP_TEST_ASSERT(e.to_string() == "foo[0]",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo[0]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar[0]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.array_index() == 0,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[010]");
		ret += PP_TEST_ASSERT(e.to_string() == "foo[8]",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo[010]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar[010]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo[8]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar[8]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.array_index() == 8,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("foo[0x10]");
		ret += PP_TEST_ASSERT(e.to_string() == "foo[16]",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo[0x10]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("foo")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar[0x10]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.equals(pp_path::element("foo[16]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!e.equals(pp_path::element("bar[16]")),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == true,
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.array_index() == 16,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("_");
		ret += PP_TEST_ASSERT(e.to_string() == "_",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
	}
	{
		pp_path::element e("^");
		ret += PP_TEST_ASSERT(e.to_string() == "^",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(e.is_array() == false,
			"pp_path::element::element()");
	}
	{
		try {
			pp_path::element e("^foo");
			PP_TEST_ERROR("pp_path::pp_path()");
			ret++;
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("^^");
			PP_TEST_ERROR("pp_path::pp_path()");
			ret++;
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("123");
			PP_TEST_ERROR("pp_path::pp_path()");
			ret++;
		} catch (pp_path::invalid_error &e) {
		}
	}
	{
		try {
			pp_path::element e("");
			PP_TEST_ERROR("pp_path::pp_path()");
			ret++;
		} catch (pp_path::invalid_error &e) {
		}
	}

	return ret;
}

int
test_ctors()
{
	int ret = 0;

	// test the default ctor
	{
		pp_path path;
		ret += PP_TEST_ASSERT(path == "",
		    "pp_path::pp_path()");
		ret += PP_TEST_ASSERT(!path.is_initialized(),
		    "pp_path::pp_path()");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
		    "pp_path::pp_path()");
	}

	// test construction from a string
	{
		pp_path path("");
		ret += PP_TEST_ASSERT(path == "",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_initialized(),
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string(""));
		ret += PP_TEST_ASSERT(path == "",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(!path.is_initialized(),
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/");
		ret += PP_TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/"));
		ret += PP_TEST_ASSERT(path == "/",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	{
		pp_path path("a");
		ret += PP_TEST_ASSERT(path == "a",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("a"));
		ret += PP_TEST_ASSERT(path == "a",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/a");
		ret += PP_TEST_ASSERT(path == "/a",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/a"));
		ret += PP_TEST_ASSERT(path == "/a",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_initialized(),
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 1,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	{
		pp_path path("a/b");
		ret += PP_TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("a/b"));
		ret += PP_TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/a/b");
		ret += PP_TEST_ASSERT(path == "/a/b",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/a/b"));
		ret += PP_TEST_ASSERT(path == "/a/b",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	{
		pp_path path("a/b/c");
		ret += PP_TEST_ASSERT(path == "a/b/c",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("a/b/c"));
		ret += PP_TEST_ASSERT(path == "a/b/c",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(string)");
	}
	{
		pp_path path("/a/b/c");
		ret += PP_TEST_ASSERT(path == "/a/b/c",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path(string("/a/b/c"));
		ret += PP_TEST_ASSERT(path == "/a/b/c",
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.size() == 3,
			"pp_path::pp_path(string)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(string)");
	}

	// test whitespace chomping
	{
		pp_path path("   ");
		ret += PP_TEST_ASSERT(path == "",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("/  ");
		ret += PP_TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("  /  ");
		ret += PP_TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("  /");
		ret += PP_TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("  a/b  ");
		ret += PP_TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}

	// test delimiter collapsing
	{
		pp_path path("////");
		ret += PP_TEST_ASSERT(path == "/",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 0,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("a////b");
		ret += PP_TEST_ASSERT(path == "a/b",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		pp_path path("////a////b");
		ret += PP_TEST_ASSERT(path == "/a/b",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.size() == 2,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::pp_path(char *)");
	}
	{
		try {
			pp_path path("123");
			PP_TEST_ERROR("pp_path::pp_path()");
			ret++;
		} catch (pp_path::invalid_error &e) {
		}
	}

	// test carats
	{
		pp_path path("^");
		ret += PP_TEST_ASSERT(path == "^",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::is_absolute()");
	}
	{
		pp_path path("^/foo");
		ret += PP_TEST_ASSERT(path == "^/foo",
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(path != pp_path("^/bar"),
			"pp_path::element::element()");
		ret += PP_TEST_ASSERT(!path.is_absolute(),
			"pp_path::is_absolute()");
	}
	{
		pp_path path("/^/foo");
		ret += PP_TEST_ASSERT(path == "/^/foo",
			"pp_path::pp_path()");
		ret += PP_TEST_ASSERT(path != pp_path("/^/bar"),
			"pp_path::pp_path()");
		ret += PP_TEST_ASSERT(path.is_absolute(),
			"pp_path::is_absolute()");
	}

	// test copy construction
	{
		pp_path path("a/b");
		pp_path path2(path);
		ret += PP_TEST_ASSERT(path2 == "a/b",
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(path2.size() == 2,
			"pp_path::pp_path(char *)");
		ret += PP_TEST_ASSERT(!path2.is_absolute(),
			"pp_path::pp_path(char *)");
	}

	return ret;
}

int
test_comparisons()
{
	int ret = 0;

	{
		pp_path left("a/b/c");
		pp_path right("a/b/c");
		ret += PP_TEST_ASSERT(left.equals(right), "pp_path::equals()");
		ret += PP_TEST_ASSERT(left == right, "pp_path::operator==()");
	}
	{
		pp_path left("a/b/c");
		pp_path right("d/e/f");
		ret += PP_TEST_ASSERT(!left.equals(right), "pp_path::equals()");
		ret += PP_TEST_ASSERT(left != right, "pp_path::operator!=()");
	}
	{
		pp_path left("a/b/c");
		string right("a/b/c");
		ret += PP_TEST_ASSERT(left == right, "pp_path::operator==()");
	}
	{
		pp_path left("a/b/c");
		string right("d/e/f");
		ret += PP_TEST_ASSERT(left != right, "pp_path::operator!=()");
	}
	{
		string left("a/b/c");
		pp_path right("a/b/c");
		ret += PP_TEST_ASSERT(left == right, "pp_path::operator==()");
	}
	{
		string left("a/b/c");
		pp_path right("d/e/f");
		ret += PP_TEST_ASSERT(left != right, "pp_path::operator!=()");
	}

	return ret;
}

int
test_iters()
{
	int ret = 0;

	pp_path path("a/b/c");

	pp_path::iterator it = path.begin();
	ret += PP_TEST_ASSERT(it != path.end(), "pp_path::end()");
	ret += PP_TEST_ASSERT(*it == "a", "pp_path::begin()");
	it++;
	ret += PP_TEST_ASSERT(it != path.end(), "pp_path::end()");
	ret += PP_TEST_ASSERT(*it == "b", "pp_path::iterator::operator++()");
	it++;
	ret += PP_TEST_ASSERT(it != path.end(), "pp_path::end()");
	ret += PP_TEST_ASSERT(*it == "c", "pp_path::iterator::operator++()");
	it++;
	ret += PP_TEST_ASSERT(it == path.end(), "pp_path::end()");
	it--;
	ret += PP_TEST_ASSERT(it != path.end(), "pp_path::end()");
	ret += PP_TEST_ASSERT(*it == "c", "pp_path::iterator::operator--()");
	it--;
	ret += PP_TEST_ASSERT(it != path.end(), "pp_path::end()");
	ret += PP_TEST_ASSERT(*it == "b", "pp_path::iterator::operator--()");
	it--;
	ret += PP_TEST_ASSERT(it != path.end(), "pp_path::end()");
	ret += PP_TEST_ASSERT(it == path.begin(), "pp_path::end()");
	ret += PP_TEST_ASSERT(*it == "a", "pp_path::iterator::operator--()");

	pp_path::reverse_iterator rit = path.rbegin();
	ret += PP_TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(*rit == "c", "pp_path::rbegin()");
	rit++;
	ret += PP_TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(*rit == "b", "pp_path::iterator::operator++()");
	rit++;
	ret += PP_TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(*rit == "a", "pp_path::iterator::operator++()");
	rit++;
	ret += PP_TEST_ASSERT(rit == path.rend(), "pp_path::rend()");
	rit--;
	ret += PP_TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(*rit == "a", "pp_path::iterator::operator--()");
	rit--;
	ret += PP_TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(*rit == "b", "pp_path::iterator::operator--()");
	rit--;
	ret += PP_TEST_ASSERT(rit != path.rend(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(rit == path.rbegin(), "pp_path::rend()");
	ret += PP_TEST_ASSERT(*rit == "c", "pp_path::iterator::operator--()");

	return ret;
}

int
test_contents()
{
	int ret = 0;
	pp_path path;

	// create a path
	path.push_back("a");
	path.push_back("b");
	path.push_back("c");
	ret += PP_TEST_ASSERT(path.size() == 3, "pp_path::size()");

	pp_path::iterator it = path.begin();
	ret += PP_TEST_ASSERT(*it == "a", "pp_path::begin()");
	it++;
	ret += PP_TEST_ASSERT(*it == "b", "pp_path::iterator::operator++()");
	it++;
	ret += PP_TEST_ASSERT(*it == "c", "pp_path::iterator::operator++()");
	it++;
	ret += PP_TEST_ASSERT(it == path.end(), "pp_path::end()");

	// test front() and back()
	ret += PP_TEST_ASSERT(path.front() == "a", "pp_path::front()");
	ret += PP_TEST_ASSERT(path.back() == "c", "pp_path::back()");

	// pop items
	ret += PP_TEST_ASSERT(path.pop_front() == "a", "pp_path::pop_front()");
	ret += PP_TEST_ASSERT(path.pop_back() == "c", "pp_path::pop_back()");
	ret += PP_TEST_ASSERT(path.size() == 1, "pp_path::size()");
	ret += PP_TEST_ASSERT(path.front() == "b", "pp_path::front()");
	ret += PP_TEST_ASSERT(path.back() == "b", "pp_path::back()");
	path.pop_back();
	ret += PP_TEST_ASSERT(path.size() == 0, "pp_path::size()");

	// cause exceptions
	try {
		path.pop_front();
		PP_TEST_ERROR("pp_path::pop_front()");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		path.pop_back();
		PP_TEST_ERROR("pp_path::pop_back()");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		path.front();
		PP_TEST_ERROR("pp_path::front()");
		ret++;
	} catch (std::out_of_range &e) {
	}
	try {
		path.back();
		PP_TEST_ERROR("pp_path::back()");
		ret++;
	} catch (std::out_of_range &e) {
	}

	// test clear()
	path = pp_path("a/b/c");
	ret += PP_TEST_ASSERT(path.size() == 3, "pp_path::size()");
	path.clear();
	ret += PP_TEST_ASSERT(path.size() == 0, "pp_path::size()");
	ret += PP_TEST_ASSERT(path.is_initialized() == 0, "pp_path::size()");

	//test to_string()
	path = pp_path("a/b/c");
	ret += PP_TEST_ASSERT(path.to_string() == "a/b/c",
	    "pp_path::to_string()");
	path = pp_path("/a/b/c");
	ret += PP_TEST_ASSERT(path.to_string() == "/a/b/c",
	    "pp_path::to_string()");

	// test catenation operators
	// path += path
	path = pp_path("a/b/c");
	path += pp_path("d/e/f");
	ret += PP_TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+=(pp_path, pp_path)");
	// path += string
	path = pp_path("a/b/c");
	path += "d";
	ret += PP_TEST_ASSERT(path == "a/b/c/d",
	    "pp_path::operator+=(pp_path, string)");
	path = pp_path("a/b/c");
	path += "d/e/f";
	ret += PP_TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+=(pp_path, string)");
	// path + path
	path = pp_path("a/b/c") + pp_path("d/e/f");
	ret += PP_TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+=(pp_path, pp_path)");
	// path + string
	path = pp_path("a/b/c") + "d";
	ret += PP_TEST_ASSERT(path == "a/b/c/d",
	    "pp_path::operator+(pp_path, string)");
	path = pp_path("a/b/c") + "d/e/f";
	ret += PP_TEST_ASSERT(path == "a/b/c/d/e/f",
	    "pp_path::operator+(pp_path, string)");

	return ret;
}

int
test_const()
{
	int ret = 0;

	pp_path path("a/b/c");
	const pp_path &const_path = path;
	ret += PP_TEST_ASSERT(const_path.size() == 3, "pp_path::size()");

	pp_path::iterator it = path.begin();
	pp_path::const_iterator cit = const_path.begin();
	ret += PP_TEST_ASSERT(*it == *cit, "pp_path::begin()");
	it++;
	cit++;
	ret += PP_TEST_ASSERT(*it == *cit, "pp_path::iterator::operator++()");
	it++;
	cit++;
	ret += PP_TEST_ASSERT(*it == *cit, "pp_path::iterator::operator++()");

	return ret;
}

int
main()
{
	int error_count = 0;
	error_count += test_element();
	error_count += test_ctors();
	error_count += test_comparisons();
	error_count += test_iters();
	error_count += test_contents();
	error_count += test_const();
	return error_count;
}
