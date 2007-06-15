/*
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp_path.h"
#include <iostream>

using namespace std;

#define ERROR(msg) \
	cerr << "ERROR: [" __FILE__ << ":" << __LINE__ << "] " << msg << endl

int
test_push_pop()
{
	int return_value = 0;
	pp_path path;

	/* Generate pp_path Information */
	path.push_back("One");
	path.push_back("Two");
	path.push_back("Three");

	/* create a path iterator */
	pp_path::iterator my_iterator;
	my_iterator = path.begin ();

	/* test created path */
	if ((*my_iterator).compare("One") != 0) {
		ERROR("pp_path::compare()");
		return_value++;
	}

	my_iterator++;

	if ((*my_iterator).compare("Two") != 0) {
		ERROR("pp_path::compare()");
		return_value++;
	}

	my_iterator++;

	if ((*my_iterator).compare("Three") != 0) {
		ERROR("pp_path::compare()");
		return_value++;
	}

	if (path.size() != 3) {
		ERROR("pp_path::size()");
		return_value++;
	}

	/* pop items */
	path.pop_back();
	path.pop_back();
	path.pop_back();

	if (!path.empty()) {
		ERROR("pp_path::empty()");
		return_value++;
	}

	return return_value;
}

int
test_const()
{
	int return_value = 0;
	pp_path path;
	const pp_path &const_path = path;
	path.push_back("One");
	path.push_back("Two");
	path.push_back("Three");

	if (const_path.empty()) {
		ERROR("pp_path::empty()");
		return_value++;
	}

	if (path.size() != 3) {
		ERROR("pp_path::size()");
		return_value++;
	}

	if (const_path.size() != 3) {
		ERROR("pp_path::size()");
		return_value++;
	}

	pp_path::iterator my_iterator;
	pp_path::const_iterator const_iterator;
	my_iterator = path.begin();
	const_iterator = const_path.begin();

	if (my_iterator->compare(*const_iterator) != 0) {
		ERROR("pp_path::iterator::compare()");
		ERROR("first item incorrect");
		return_value++;
	}

	my_iterator++;
	const_iterator++;

	if (my_iterator->compare(*const_iterator) != 0) {
		ERROR("pp_path::iterator::compare()");
		return_value++;
	}

	my_iterator++;
	const_iterator++;

	if (my_iterator->compare(*const_iterator) != 0) {
		ERROR("pp_path::iterator::compare()");
		return_value++;
	}

	path.pop_front();

	if (const_path.size() != 2) {
		ERROR("pp_path::size()");
		return_value++;
	}

	return return_value;
}

int
test_comparisons()
{
	pp_path pathA, pathB, pathC, pathD, pathE;

	int return_value = 0;

	pathA.push_back("One");
	pathA.push_back("Two");
	pathA.push_back("Three");

	pathB.push_back("One");
	pathB.push_back("Two");

	pathC.push_back("One");
	pathC.push_back("Two");
	pathC.push_back("Three");
	pathC.push_back("Four");

	pathD.push_back("One");
	pathD.push_back("Seven");
	pathD.push_back("Three");

	pathE.push_back("One");
	pathE.push_back("Two");
	pathE.push_back("Three");

	if (pathA.equals(pathB) == false) {
	} else {
		ERROR("patha == pathb");
		return_value++;
	}

	if (pathA.equals(pathC) == false) {
	} else {
		ERROR("patha == pathc");
		return_value++;
	}

	if (pathA.equals(pathD) == false) {
	} else {
		ERROR("patha == pathd");
		return_value++;
	}

	if (pathA.equals(pathE) == true) {
	} else {
		ERROR("patha != pathe");
		return_value++;
	}

	return return_value;
}

int test_ctors()
{
	int return_value = 0;
	pp_path path;

	/* test construction from a string */
	path = pp_path("a/b/c");
	if (path != "a/b/c" || path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("  a/b/c  ");
	if (path != "  a/b/c  " || path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("/a/b/c/");
	if (path != "/a/b/c" || !path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("////a////b////c////");
	if (path != "/a/b/c" || !path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("");
	if (path != "" || path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("   ");
	if (path != "   " || path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("/");
	if (path != "/" || !path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("   /");
	if (path != "   " || path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}
	path = pp_path("////");
	if (path != "/" || !path.is_absolute()) {
		ERROR("pp_path::pp_path(string)");
		return_value++;
	}

	/* test default constructor */
	path = pp_path();
	if (path != "" || path.is_absolute()) {
		ERROR("pp_path::pp_path()");
		return_value++;
	}

	/* test copy construction */
	path = pp_path("a/b/c");
	path = pp_path(path);
	if (path != "a/b/c" || path.is_absolute()) {
		ERROR("pp_path::operator=(pp_path)");
		return_value++;
	}

	return return_value;
}

int
test_operators()
{
	int return_value = 0;
	string str;
	pp_path path;

	// <<
	str = to_string(pp_path("a/b/c"));
	if (str != "a/b/c") {
		ERROR("pp_path::operator<<()");
		return_value++;
	}
	str = to_string(pp_path("/a/b/c"));
	if (str != "/a/b/c") {
		ERROR("pp_path::operator<<()");
		return_value++;
	}

	// path == path
	if (pp_path("a/b/c") == pp_path("d/e/f")) {
		ERROR("pp_path::operator==(pp_path, pp_path)");
		return_value++;
	}
	if (!(pp_path("a/b/c") == pp_path("a/b/c"))) {
		ERROR("pp_path::operator==(pp_path, pp_path)");
		return_value++;
	}

	// path == string
	if (!(pp_path("a/b/c") == "a/b/c")) {
		ERROR("pp_path::operator==(pp_path, string)");
		return_value++;
	}
	if (pp_path("a/b/c") == "d/e/f") {
		ERROR("pp_path::operator==(pp_path, string)");
		return_value++;
	}
	if (!(pp_path("/a/b/c") == "/a/b/c")) {
		ERROR("pp_path::operator==(pp_path, string)");
		return_value++;
	}
	if (pp_path("/a/b/c") == "/d/e/f") {
		ERROR("pp_path::operator==(pp_path, string)");
		return_value++;
	}

	// string == path
	if (!("a/b/c" == pp_path("a/b/c"))) {
		ERROR("pp_path::operator==(string, pp_path)");
		return_value++;
	}
	if ("a/b/c" == pp_path("d/e/f")) {
		ERROR("pp_path::operator==(string, pp_path)");
		return_value++;
	}
	if (!("/a/b/c" == pp_path("/a/b/c"))) {
		ERROR("pp_path::operator==(string, pp_path)");
		return_value++;
	}
	if ("/a/b/c" == pp_path("/d/e/f")) {
		ERROR("pp_path::operator==(string, pp_path)");
		return_value++;
	}

	// path != path
	if (pp_path("a/b/c") != pp_path("a/b/c")) {
		ERROR("pp_path::operator!=(pp_path, pp_path)");
		return_value++;
	}
	if (!(pp_path("a/b/c") != pp_path("d/e/f"))) {
		ERROR("pp_path::operator!=(pp_path, pp_path)");
		return_value++;
	}

	// path != string
	if (pp_path("a/b/c") != "a/b/c") {
		ERROR("pp_path::operator!=(pp_path, string)");
		return_value++;
	}
	if (!(pp_path("a/b/c") != "d/e/f")) {
		ERROR("pp_path::operator!=(pp_path, string)");
		return_value++;
	}
	if (pp_path("/a/b/c") != "/a/b/c") {
		ERROR("pp_path::operator!=(pp_path, string)");
		return_value++;
	}
	if (!(pp_path("/a/b/c") != "/d/e/f")) {
		ERROR("pp_path::operator!=(pp_path, string)");
		return_value++;
	}

	// string != path
	if ("a/b/c" != pp_path("a/b/c")) {
		ERROR("pp_path::operator!=(string, pp_path)");
		return_value++;
	}
	if (!("a/b/c" != pp_path("d/e/f"))) {
		ERROR("pp_path::operator!=(string, pp_path)");
		return_value++;
	}
	if ("/a/b/c" != pp_path("/a/b/c")) {
		ERROR("pp_path::operator!=(string, pp_path)");
		return_value++;
	}
	if (!("/a/b/c" != pp_path("/d/e/f"))) {
		ERROR("pp_path::operator!=(string, pp_path)");
		return_value++;
	}

	// path += path
	path = "a/b/c";
	path += pp_path("d/e/f");
	if (path != "a/b/c/d/e/f") {
		ERROR("pp_path::operator+=(pp_path, pp_path)");
		return_value++;
	}

	// path += string
	path = "a/b/c";
	path += "d";
	if (path != "a/b/c/d") {
		ERROR("pp_path::operator+=(pp_path, string)");
		return_value++;
	}

	// path + path
	path = pp_path("a/b/c") + pp_path("d/e/f");
	if (path != "a/b/c/d/e/f") {
		ERROR("pp_path::operator+=(pp_path, pp_path)");
		return_value++;
	}

	// path + string
	path = pp_path("a/b/c") + "d";
	if (path != "a/b/c/d") {
		ERROR("pp_path::operator+(pp_path, string)");
		return_value++;
	}

	// path + path w/ a '\' as a delimiter
	path = pp_path("a\\b\\c", "\\") + pp_path("d\\e\\f", "\\");
	if (path != "a\\b\\c\\d\\e\\f") {
		ERROR("pp_path::operator+=(pp_path, pp_path) with delim");
		return_value++;
	}

	// path + string w/ a '\' as a delimiter
	path = pp_path("a\\b\\c", "\\") + "d";
	if (path != "a\\b\\c\\d") {
		ERROR("pp_path::operator+(pp_path, string) with delim");
		return_value++;
	}

	return return_value;
}

int
main()
{
	int error_count = 0;
	error_count += test_push_pop();
	error_count += test_const();
	error_count += test_comparisons();
	error_count += test_ctors();
	error_count += test_operators();
	return error_count;
}
