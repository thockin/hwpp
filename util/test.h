// Copyright (c) Tim Hockin, 2007-2008
//
// A basic unit testing framework.
//
// This should be familiar to anyone who has used other popular unit test
// frameworks, and should be easy to learn for anyone who hasn't.
//
// The code in this file is very C++ oriented, but could be made to work
// on plain C, if you hold your nose while adapting it (hint: use GCC's
// __attribute__((constructor))).  Some of the stuff in here is pretty
// hacky, but it is all to an end of making tests as easy as possible to
// write.
//
// First: Don't define main().  Just #include this file and write test
// functions.
//
// Second: A failed assertion causes an entire test function, and thereby
// the entire test file, to be marked as failing but DOES NOT halt
// execution.  If you need to terminate the test function, return.  If you
// need to terminate the test file, call TEST_EXIT().
//
// Third: The only way to make a test fail is to call TEST_ERROR() or
// TEST_FAIL() or to fail one of the TEST_ASSERT()s.  Simply returning early
// from a test function or calling TEST_EXIT() will not cause a test to
// fail.
//
// In order to keep this a single header file, the "public" API is mixed
// up with the "private" details.  In order to avoid name conflicts, all
// symbols which begin with "TEST" are reserved for use by this header, and
// all symbols which begin with "TEST_INTERNAL" or "TEST_" followed by a
// lower-case letter are considered internal and not for use by callers.
//
// Here is the public API:
//
// * TEST(name)
//
//   Define a single test function.  This function is essentially C code.
//   There is no 'this' object.
//
//   Example:
//   	TEST(test_name) {
//   		your_test_code();
//   		goes(here);
//   	}
//
// * TEST_WARN()
// * TEST_WARN(msg)
//
//   Issue a warning message.  This does not cause the current test to
//   fail.  The return value of this function can be used to stream
//   further details to the test output.
//
//   Example:
//   	TEST_WARN("something went badly");
//   	TEST_WARN("something went badly: ") << status;
//   	TEST_WARN() << something went badly: " << status;
//
// * TEST_FAIL()
// * TEST_FAIL(msg)
//
//   Issue a failure message.  This causes the current test to fail.  The
//   return value of this function can be used to stream further details
//   to the test output.
//
//   Example:
//   	TEST_FAIL("something went very badly");
//   	TEST_FAIL("something went very badly: ") << status;
//   	TEST_FAIL() << "something went very badly" << status;
//
// * TEST_ERROR()
// * TEST_ERROR(msg)
//
//   Issue an error message.  This causes the current test to fail.  A
//   TEST_ERROR() indicates that something in the test itself went wrong, as
//   opposed to the test simply failing.  The return value of this function
//   can be used to stream further details to the test output.
//
//   Example:
//   	TEST_ERROR("something went horribly badly");
//   	TEST_ERROR("something went horribly badly: ") << status;
//   	TEST_ERROR() << "something went horribly badly" << status;
//
// * TEST_ASSERT(pred)
// * TEST_ASSERT(pred, msg)
//
//   Assert that pred evaluates to boolean true (non-zero).  If pred
//   evaluates to boolean false (0), the msg is issued as through
//   TEST_FAIL(msg).  The return value of this function (and all of the
//   TEST_ASSERT_* functions) can be used to stream further details to the
//   test output.
//
//   Example:
//   	TEST_ASSERT(condition(), "condition() was false");
//   	TEST_ASSERT(condition(), "condition() was false: ") << status;
//   	TEST_ASSERT(condition()) << "condition() was false: " << status;
//
// * TEST_ASSERT_TRUE(pred)
// * TEST_ASSERT_TRUE(pred, msg)
//
//   Assert that pred evaluates to boolean true (non-zero).  This is a synonym
//   for TEST_ASSERT().
//
//   Example:
//   	TEST_ASSERT_TRUE(condition(), "condition() was false");
//   	TEST_ASSERT_TRUE(condition(), "condition() was false: ") << status;
//   	TEST_ASSERT_TRUE(condition()) << "condition() was false: " << status;
//
// * TEST_ASSERT_FALSE(pred)
// * TEST_ASSERT_FALSE(pred, msg)
//
//   Assert that pred evaluates to boolean false (zero).  This is the inverse
//   of TEST_ASSERT_TRUE().
//
//   Example:
//   	TEST_ASSERT_FALSE(condition(), "condition() was true");
//   	TEST_ASSERT_FALSE(condition(), "condition() was true: ") << status;
//   	TEST_ASSERT_FALSE(condition()) << "condition() was true: " << status;
//
// * TEST_ASSERT_EQ(lhs, rhs)
// * TEST_ASSERT_EQ(lhs, rhs, msg)
//
//   Assert that lhs equals rhs via the '==' operator.  This assertion is
//   tested in both directions (lhs == rhs && rhs == lhs).  If the
//   assertion evaluates to boolean false, the msg is issued as through
//   TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_EQ(ret, 0, "ret was not 0");
//
// * TEST_ASSERT_EQZ(lhs)
// * TEST_ASSERT_EQZ(lhs, msg)
//
//   Assert that lhs equals 0 via the '==' operator.  This assertion is
//   tested in both directions (lhs == 0 && 0 == lhs).  If the assertion
//   evaluates to boolean false, the msg is issued as through
//   TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_EQZ(ret, "ret was not 0");
//
// * TEST_ASSERT_NE(lhs, rhs)
// * TEST_ASSERT_NE(lhs, rhs, msg)
//
//   Assert that lhs does not equal rhs via the '!=' operator.  This
//   assertion is tested in both directions (lhs != rhs && rhs != lhs).
//   If the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_NE(ret, 0, "ret was 0");
//
// * TEST_ASSERT_NEZ(lhs)
// * TEST_ASSERT_NEZ(lhs, msg)
//
//   Assert that lhs does not equal 0 via the '!=' operator.  This
//   assertion is tested in both directions (lhs != 0 && 0 != lhs).  If
//   the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_NEZ(ret, "ret was 0");
//
// * TEST_ASSERT_LT(lhs, rhs)
// * TEST_ASSERT_LT(lhs, rhs, msg)
//
//   Assert that lhs is less than rhs via the '< operator.  This assertion
//   is tested in both directions (lhs < rhs && rhs >= lhs).  If the
//   assertion evaluates to boolean false, the msg is issued as through
//   TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_LT(ret, 0, "ret was not < 0");
//
// * TEST_ASSERT_LTZ(lhs)
// * TEST_ASSERT_LTZ(lhs, msg)
//
//   Assert that lhs is less than 0 via the '< operator.  This assertion
//   is tested in both directions (lhs < 0 && 0 >= lhs).  If the assertion
//   evaluates to boolean false, the msg is issued as through
//   TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_LTZ(ret, "ret was not < 0");
//
// * TEST_ASSERT_LE(lhs, rhs)
// * TEST_ASSERT_LE(lhs, rhs, msg)
//
//   Assert that lhs is less than or equal to rhs via the '<=' operator.
//   This assertion is tested in both directions (lhs <= rhs && rhs > lhs).
//   If the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_LE(ret, 0, "ret was not <= 0");
//
// * TEST_ASSERT_LEZ(lhs)
// * TEST_ASSERT_LEZ(lhs, msg)
//
//   Assert that lhs is less than or equal to 0 via the '<=' operator.
//   This assertion is tested in both directions (lhs <= 0 && 0 > lhs).
//   If the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_LEZ(ret, "ret was not <= 0");
//
// * TEST_ASSERT_GT(lhs, rhs)
// * TEST_ASSERT_GT(lhs, rhs, msg)
//
//   Assert that lhs is greater than rhs via the '> operator.  This
//   assertion is tested in both directions (lhs > rhs && rhs <= lhs).
//   If the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_GT(ret, 0, "ret was not > 0");
//
// * TEST_ASSERT_GTZ(lhs)
// * TEST_ASSERT_GTZ(lhs, msg)
//
//   Assert that lhs is greater than 0 via the '> operator.  This
//   assertion is tested in both directions (lhs > 0 && 0 <= lhs).  If the
//   assertion evaluates to boolean false, the msg is issued as through
//   TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_GTZ(ret, "ret was not > 0");
//
// * TEST_ASSERT_GE(lhs, rhs)
// * TEST_ASSERT_GE(lhs, rhs, msg)
//
//   Assert that lhs is greater than or equal to rhs via the '>=' operator.
//   This assertion is tested in both directions (lhs >= rhs && rhs < lhs).
//   If the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_GE(ret, 0, "ret was not >= 0");
//
// * TEST_ASSERT_GEZ(lhs)
// * TEST_ASSERT_GEZ(lhs, msg)
//
//   Assert that lhs is greater than or equal to 0 via the '>=' operator.
//   This assertion is tested in both directions (lhs >= 0 && 0 < lhs).
//   If the assertion evaluates to boolean false, the msg is issued as
//   through TEST_FAIL(msg).
//
//   Example:
//   	TEST_ASSERT_GEZ(ret, "ret was not >= 0");
//
// * TEST_EXIT()
//
//   Exit the current test.  If any assertions have failed, or if
//   TEST_FAIL() or TEST_ERROR() were called, the test will exit with a
//   failure code.  Otherwise it will exit with a success code.
//
//   Example:
//   	TEST_EXIT();
//
// * TEST_SETUP()
//
//   Define a one-time test setup function.  This is called once before
//   any test functions are run.  This function is optional.
//
//   Example:
//   	TEST_SETUP() {
//   		your_setup_code();
//   		goes(here);
//   	}
//
// * TEST_CLEANUP()
//
//   Define a one-time test cleanup function.  This is called once after
//   all tests functions are run.  This function is optional.
//
//   Example:
//   	TEST_CLEANUP() {
//   		your_cleanup_code();
//   		goes(here);
//   	}
//
// * TEST_SETUP_EACH()
//
//   Define a per-test setup function.  This is called once before
//   each test function is run.  This function is optional.
//
//   Example:
//   	TEST_SETUP_EACH() {
//   		your_setup_each_code();
//   		goes(here);
//   	}
//
// * TEST_CLEANUP_EACH()
//
//   Define a per-test cleanup function.  This is called once after
//   each test function is run.  This function is optional.
//
//   Example:
//   	TEST_CLEANUP_EACH() {
//   		your_cleanup_each_code();
//   		goes(here);
//   	}
//
// * TEST_PATH_PREFIX
//
//   If this macro is defined before this file is #include-ed, it will
//   become the prefix for all test-related paths.
//
// * TEST_TMP_DIR()
//
//   This function returns a const std::string & to the path to a usable
//   temporary directory under the TEST_PATH_PREFIX.  This directory will be
//   destroyed and created anew for each test function.

#ifndef HWPP_UTIL_TEST_H__
#define HWPP_UTIL_TEST_H__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/smart_ptr.hpp>
#include "util/execxx.h"

// How many failures have we had?
static int TEST_error_count;

// The global list of tests to run.
struct TEST_definition;
static std::vector<TEST_definition *> TEST_list;

// The currently running test.
static const TEST_definition *TEST_current;

// The main test definition class.
struct TEST_definition
{
	std::string test_name;
	void (*test_function)(void);

	TEST_definition(const std::string &name, void (*func)(void))
	: test_name(name), test_function(func)
	{
		TEST_list.push_back(this);
	}
};

// Define a test.
#define TEST(name_) \
	void name_(void); \
	static TEST_definition TEST_##name_##definition(#name_, name_); \
	void name_(void)

// Test paths, for use inside tests.
#ifndef TEST_PATH_PREFIX
#  define TEST_PATH_PREFIX "/tmp"
#endif

// Internal: Helper for initializing statics.
static const std::string *
TEST_init_tmpdir_string()
{
	std::string *s = new std::string(TEST_PATH_PREFIX);
	s->append("/");
	const char *user = getenv("USER");
	if (user) {
		s->append(user);
		s->append(".");
	}
	char pid[256] = {'\0'};
	snprintf(pid, sizeof(pid), "%d", getpid());
	s->append(pid);
	s->append("/test_temp/");
	return s;
}

// Get the path to the temp dir for this test.
static const std::string &
TEST_TMP_DIR()
{
	static const std::string *tmp_dir = TEST_init_tmpdir_string();
	return *tmp_dir;
}

// A helper to use ostreams for error message output while
// handling newline output sanely (on the client side, at least).
struct TEST_output_stream
{
	std::ostream &output_stream;
	bool is_error;

	TEST_output_stream(std::ostream &o, bool e)
	    : output_stream(o), is_error(e) {}
	~TEST_output_stream()
	{
		output_stream << std::endl;
	}
};

// A refcounted smart pointer ensures that the dtor only gets called once.
typedef boost::shared_ptr<TEST_output_stream> TEST_output_stream_ptr;

// Internal: Simplify code
inline TEST_output_stream_ptr
TEST_new_output_stream(std::ostream &output_stream, bool is_error)
{
	return TEST_output_stream_ptr(
	    new TEST_output_stream(output_stream, is_error));
}

// Provide a way to stream output through a TEST_output_stream.
template <typename Tdata>
inline const TEST_output_stream_ptr &
operator<<(const TEST_output_stream_ptr &output, const Tdata &data)
{
	output->output_stream << data;
	return output;
}

inline TEST_output_stream_ptr
TEST_start_msg(const std::string &prefix)
{
	static const TEST_definition *last_test;
	if (TEST_current != last_test) {
		last_test = TEST_current;
		std::cerr << TEST_current->test_name << "(): " << std::endl;
	}
	std::cerr << "  " << prefix << ": ";
	return TEST_new_output_stream(std::cerr, true);
}

// Generate a test warning message.
inline TEST_output_stream_ptr
TEST_warn_msg()
{
	return TEST_start_msg("WARN");
}
inline TEST_output_stream_ptr
TEST_warn_msg(const std::string &file, int line)
{
	return TEST_warn_msg() << "[" << file << ":" << line << "] ";
}

// Log a test warning.
#define TEST_WARN(...) TEST_warn(__FILE__, __LINE__, ##__VA_ARGS__)
inline TEST_output_stream_ptr
TEST_warn(const std::string &file, int line, const char *msg=NULL)
{
	return TEST_warn_msg(file, line) << msg;
}

// Generate a test failure message.
inline TEST_output_stream_ptr
TEST_fail_msg()
{
	return TEST_start_msg("FAIL");
}
inline TEST_output_stream_ptr
TEST_fail_msg(const std::string &file, int line)
{
	return TEST_fail_msg() << "[" << file << ":" << line << "] ";
}

// Log a test failure.
#define TEST_FAIL(...) TEST_fail(__FILE__, __LINE__, ##__VA_ARGS__)
inline TEST_output_stream_ptr
TEST_fail(const std::string &file, int line, const char *msg=NULL)
{
	return TEST_fail_msg(file, line) << msg;
}

// Generate a test error message.
inline TEST_output_stream_ptr
TEST_error_msg()
{
	return TEST_start_msg("ERROR");
}
inline TEST_output_stream_ptr
TEST_error_msg(const std::string &file, int line)
{
	return TEST_error_msg() << "[" << file << ":" << line << "] ";
}

// Log a test error.
#define TEST_ERROR(...) TEST_error(__FILE__, __LINE__, ##__VA_ARGS__)
inline TEST_output_stream_ptr
TEST_error(const std::string &file, int line, const char *msg=NULL)
{
	TEST_error_count++;
	return TEST_error_msg(file, line) << msg;
}

// Internal: Stringify an argument
#define TEST_INTERNAL_STRINGIFY(arg) TEST_INTERNAL_STRINGIFY_INNER(arg)
#define TEST_INTERNAL_STRINGIFY_INNER(arg) #arg

// Internal: Assert a condition with file and line numbers.
#define TEST_INTERNAL_ASSERT_FROM(file, line, pred, ...) \
    TEST_assert(file, line, pred, TEST_INTERNAL_STRINGIFY(pred), ##__VA_ARGS__)

// Assert a condition and fail if the condition is false.
#define TEST_ASSERT(pred, ...) TEST_INTERNAL_ASSERT_FROM(__FILE__, __LINE__, \
                                                         pred, ##__VA_ARGS__)
inline TEST_output_stream_ptr
TEST_assert(const std::string &file, int line,
            bool predicate, const char *pred_str,
            const char *msg=NULL)
{
	if (!msg) {
		msg = pred_str;
	}
	if (!predicate) {
		return TEST_fail(file, line, msg);
	} else {
		static std::ofstream null_stream("/dev/null");
		return TEST_new_output_stream(null_stream, false);
	}
}

// Assert a boolean condition.
#define TEST_ASSERT_TRUE(pred, ...)  TEST_ASSERT(pred, ##__VA_ARGS__)
#define TEST_ASSERT_FALSE(pred, ...) TEST_ASSERT(!pred, ##__VA_ARGS__)

// Internal: Define an assertion function which tests the binary expression in
// both directions.
#define TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(func_name, lr_op, rl_op)        \
	template <typename Tlhs, typename Trhs>                                \
	inline TEST_output_stream_ptr                                          \
	func_name (                                                            \
	      const std::string &file, int line,                               \
	      const Tlhs &lhs, const char *lhs_str,                            \
	      const Trhs &rhs, const char *rhs_str,                            \
	      const char *msg=NULL)                                            \
	{                                                                      \
		bool pred = (lhs lr_op rhs);                                   \
		std::string pred_str                                           \
		    = std::string(lhs_str) + " " + #lr_op + " " + rhs_str;     \
		const TEST_output_stream_ptr &out                              \
		    = TEST_assert(file, line, pred, pred_str.c_str(), msg);    \
		if (out->is_error) {                                           \
			return out;                                            \
		}                                                              \
		pred = (rhs rl_op lhs);                                        \
		pred_str                                                       \
		    = std::string(rhs_str) + " " + #rl_op + " " + lhs_str;     \
		return TEST_assert(file, line, pred, pred_str.c_str(), msg);   \
	}

// Assert binary expression conditions.  These all work reciprocally (testing
// A < B also tests that B >= A).  These have to bounce through functions to
// prevent double evaluation of macro arguments.
TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(TEST_assert_eq, ==, ==)
#define TEST_ASSERT_EQ(lhs, rhs, ...) TEST_assert_eq( \
    __FILE__, __LINE__, \
    lhs, TEST_INTERNAL_STRINGIFY(lhs), \
    rhs, TEST_INTERNAL_STRINGIFY(rhs), \
    ##__VA_ARGS__)
#define TEST_ASSERT_EQZ(lhs, ...) TEST_ASSERT_EQ(lhs, 0, ##__VA_ARGS__)

TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(TEST_assert_ne, !=, !=)
#define TEST_ASSERT_NE(lhs, rhs, ...) TEST_assert_ne( \
    __FILE__, __LINE__, \
    lhs, TEST_INTERNAL_STRINGIFY(lhs), \
    rhs, TEST_INTERNAL_STRINGIFY(rhs), \
    ##__VA_ARGS__)
#define TEST_ASSERT_NEZ(lhs, ...) TEST_ASSERT_NE(lhs, 0, ##__VA_ARGS__)

TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(TEST_assert_lt, <, >=)
#define TEST_ASSERT_LT(lhs, rhs, ...) TEST_assert_lt( \
    __FILE__, __LINE__, \
    lhs, TEST_INTERNAL_STRINGIFY(lhs), \
    rhs, TEST_INTERNAL_STRINGIFY(rhs), \
    ##__VA_ARGS__)
#define TEST_ASSERT_LTZ(lhs, ...) TEST_ASSERT_LT(lhs, 0, ##__VA_ARGS__)

TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(TEST_assert_gt, >, <=)
#define TEST_ASSERT_GT(lhs, rhs, ...) TEST_assert_gt( \
    __FILE__, __LINE__, \
    lhs, TEST_INTERNAL_STRINGIFY(lhs), \
    rhs, TEST_INTERNAL_STRINGIFY(rhs), \
    ##__VA_ARGS__)
#define TEST_ASSERT_GTZ(lhs, ...) TEST_ASSERT_GT(lhs, 0, ##__VA_ARGS__)

TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(TEST_assert_le, <, >=)
#define TEST_ASSERT_LE(lhs, rhs, ...) TEST_assert_le( \
    __FILE__, __LINE__, \
    lhs, TEST_INTERNAL_STRINGIFY(lhs), \
    rhs, TEST_INTERNAL_STRINGIFY(rhs), \
    ##__VA_ARGS__)
#define TEST_ASSERT_LEZ(lhs, ...) TEST_ASSERT_LE(lhs, 0, ##__VA_ARGS__)

TEST_INTERNAL_DEFINE_RECIPROCAL_ASSERT(TEST_assert_ge, >, <=)
#define TEST_ASSERT_GE(lhs, rhs, ...) TEST_assert_ge( \
    __FILE__, __LINE__, \
    lhs, TEST_INTERNAL_STRINGIFY(lhs), \
    rhs, TEST_INTERNAL_STRINGIFY(rhs), \
    ##__VA_ARGS__)
#define TEST_ASSERT_GEZ(lhs, ...) TEST_ASSERT_GE(lhs, 0, ##__VA_ARGS__)

// exit the test
#define TEST_EXIT() exit(TEST_error_count ?  EXIT_FAILURE : EXIT_SUCCESS)

// a helper class to assign function ptrs
struct TEST_funcptr_assigner
{
	TEST_funcptr_assigner(void (**ptr)(void), void (*val)(void))
	{
		*ptr = val;
	}
};

// optional functions, which can be provided by tests
static void (*TEST_setup_global_ptr)(void);
#define TEST_SETUP() \
	void TEST_SETUP(void); \
	static TEST_funcptr_assigner \
	  TEST_setup_assign(&TEST_setup_global_ptr, TEST_SETUP); \
	void TEST_SETUP(void)
static void (*TEST_cleanup_global_ptr)(void);
#define TEST_CLEANUP() \
	void TEST_CLEANUP(void); \
	static TEST_funcptr_assigner \
	  TEST_cleanup_assign(&TEST_cleanup_global_ptr, TEST_CLEANUP); \
	void TEST_CLEANUP(void)
static void (*TEST_setup_each_ptr)(void);
#define TEST_SETUP_EACH() \
	void TEST_SETUP_EACH(void); \
	static TEST_funcptr_assigner \
	  TEST_setup_each_assign(&TEST_setup_each_ptr, TEST_SETUP_EACH); \
	void TEST_SETUP_EACH(void)
static void (*TEST_cleanup_each_ptr)(void);
#define TEST_CLEANUP_EACH() \
	void TEST_CLEANUP_EACH(void); \
	static TEST_funcptr_assigner \
	  TEST_cleanup_each_assign(&TEST_cleanup_each_ptr, TEST_CLEANUP_EACH); \
	void TEST_CLEANUP_EACH(void)

// Internal: Helpers to call the setup/cleanup functions.
inline void
TEST_setup_global(void)
{
	if (TEST_setup_global_ptr)
		TEST_setup_global_ptr();
}
inline void
TEST_cleanup_global(void)
{
	if (TEST_cleanup_global_ptr)
		TEST_cleanup_global_ptr();
}
inline void
TEST_setup_each(void)
{
	execxx::systemxx("rm -rf " + TEST_TMP_DIR());
	execxx::systemxx("mkdir -p " + TEST_TMP_DIR());
	if (TEST_setup_each_ptr != NULL) {
		TEST_setup_each_ptr();
	}
}
inline void
TEST_cleanup_each(void)
{
	if (TEST_cleanup_each_ptr != NULL) {
		TEST_cleanup_each_ptr();
	}
	execxx::systemxx("rm -rf " + TEST_TMP_DIR());
}

// The test main() entry point.
int
main(void)
{
	TEST_setup_global();
	try {
		for (size_t i = 0; i < TEST_list.size(); i++) {
			TEST_current = TEST_list[i];
			TEST_setup_each();
			try {
				TEST_current->test_function();
			} catch (...) {
				TEST_error_msg() << "unhandled exception";
				TEST_cleanup_each();
				throw;
			}
			TEST_cleanup_each();
		}
	} catch (...) {
		TEST_cleanup_global();
		throw;
	}
	TEST_cleanup_global();
	TEST_EXIT();
}

#endif // HWPP_UTIL_TEST_H__
