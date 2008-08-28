/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include "pp.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <stdlib.h>

// how many failures have we had?
static int TEST_error_count;

// a helper to use ostreams for error message output while
// handling newline output sanely (on the client side, at least)
struct TEST_output_helper
{
	std::ostream &output_stream;

	explicit
	TEST_output_helper(std::ostream &o): output_stream(o)
	{
	}
	~TEST_output_helper()
	{
		output_stream << std::endl;
	}
};

// a refcounted smart pointer ensures that the dtor only gets called onc
typedef boost::shared_ptr<TEST_output_helper> TEST_output_helper_ptr;

// a helper to simplify cod
inline TEST_output_helper_ptr
TEST_new_output_helper(std::ostream &output_stream)
{
	return TEST_output_helper_ptr(new TEST_output_helper(output_stream));
}

// provide a way to stream output through a TEST_output_helper
template <typename Tdata>
const TEST_output_helper_ptr &
operator<<(const TEST_output_helper_ptr &output, const Tdata &data)
{
	output->output_stream << data;
	return output;
}

// generate a test failure
#define TEST_ERROR(...) TEST_do_error(__FILE__, __LINE__, ##__VA_ARGS__)
inline TEST_output_helper_ptr
TEST_do_error(const string &file, int line)
{
	TEST_error_count++;
	std::cerr << "FAIL: [" << file << ":" << line << "] ";
	return TEST_new_output_helper(std::cerr);
}
inline TEST_output_helper_ptr
TEST_do_error(const string &file, int line, const string &msg)
{
	TEST_error_count++;
	std::cerr << "FAIL: [" << file << ":" << line << "] "
		<< msg;
	return TEST_new_output_helper(std::cerr);
}

// assert a condition and fail if the condition is false
#define TEST_ASSERT(...) TEST_do_assert(__FILE__, __LINE__, ##__VA_ARGS__)
inline TEST_output_helper_ptr
TEST_do_assert(const string &file, int line, bool predicate)
{
	if (!predicate) {
		return TEST_do_error(file, line);
	} else {
		static std::ofstream null_stream("/dev/null");
		return TEST_new_output_helper(null_stream);
	}
}
inline TEST_output_helper_ptr
TEST_do_assert(const string &file, int line, bool predicate, const string &msg)
{
	if (!predicate) {
		return TEST_do_error(file, line, msg);
	} else {
		static std::ofstream null_stream("/dev/null");
		return TEST_new_output_helper(null_stream);
	}
}

// helpers for testing comparisons reciprocally
#define TEST_ASSERT_EQ(...) \
	TEST_do_assert_eq(__FILE__, __LINE__, ##__VA_ARGS__)
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_eq(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs == rhs);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs == lhs);
	}
}
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_eq(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs, const string &msg)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs == rhs, msg);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs == lhs, msg);
	}
}
#define TEST_ASSERT_NE(...) \
	TEST_do_assert_ne(__FILE__, __LINE__, ##__VA_ARGS__)
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_ne(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs != rhs);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs != lhs);
	}
}
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_ne(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs, const string &msg)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs != rhs, msg);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs != lhs, msg);
	}
}
#define TEST_ASSERT_LT(...) \
	TEST_do_assert_lt(__FILE__, __LINE__, ##__VA_ARGS__)
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_lt(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs < rhs);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs >= lhs);
	}
}
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_lt(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs, const string &msg)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs < rhs, msg);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs >= lhs, msg);
	}
}
#define TEST_ASSERT_GT(...) \
	TEST_do_assert_gt(__FILE__, __LINE__, ##__VA_ARGS__)
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_gt(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs > rhs);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs <= lhs);
	}
}
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_gt(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs, const string &msg)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs > rhs, msg);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs <= lhs, msg);
	}
}
#define TEST_ASSERT_LE(...) \
	TEST_do_assert_le(__FILE__, __LINE__, ##__VA_ARGS__)
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_le(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs <= rhs);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs > lhs);
	}
}
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_le(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs, const string &msg)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs <= rhs, msg);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs > lhs, msg);
	}
}
#define TEST_ASSERT_GE(...) \
	TEST_do_assert_ge(__FILE__, __LINE__, ##__VA_ARGS__)
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_ge(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs >= rhs);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs < lhs);
	}
}
template <typename Tlhs, typename Trhs>
inline TEST_output_helper_ptr
TEST_do_assert_ge(const string &file, int line,
		  const Tlhs &lhs, const Trhs &rhs, const string &msg)
{
	const TEST_output_helper_ptr &ret =
		TEST_do_assert(file, line, lhs >= rhs, msg);
	if (ret->output_stream == std::cerr) {
		return ret;
	} else {
		return TEST_do_assert(file, line, rhs < lhs, msg);
	}
}

// generate a warning message
#define TEST_WARNING(...) TEST_do_warning(__FILE__, __LINE__, ##__VA_ARGS__)
inline TEST_output_helper_ptr
TEST_do_warning(const string &file, int line)
{
	std::cerr << "WARN: [" << file << ":" << line << "] ";
	return TEST_new_output_helper(std::cerr);
}
inline TEST_output_helper_ptr
TEST_do_warning(const string &file, int line, const string &msg)
{
	std::cerr << "WARN: [" << file << ":" << line << "] "
		<< msg;
	return TEST_new_output_helper(std::cerr);
}

// the global list of tests to run
static std::vector<void (*)(void)> TEST_list;

// a helper class to register tests
struct TEST_definition
{
	TEST_definition(void (*func)(void))
	{
		TEST_list.push_back(func);
	}
};

// define a test
#define TEST(name_) \
	void name_(void); \
	static TEST_definition TEST_##name_##definition(name_);\
	void name_(void)

// test paths, for use inside tests
#ifndef TEST_PATH_PREFIX
#  define TEST_PATH_PREFIX "./"
#endif
#define TEST_TMP_DIR TEST_PATH_PREFIX "test_tmp"

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
	    TEST_setup(&TEST_setup_global_ptr, TEST_SETUP); \
	void TEST_SETUP(void)
static void (*TEST_cleanup_global_ptr)(void);
#define TEST_CLEANUP() \
	void TEST_CLEANUP(void); \
	static TEST_funcptr_assigner \
	    TEST_cleanup(&TEST_cleanup_global_ptr, TEST_CLEANUP); \
	void TEST_CLEANUP(void)
static void (*TEST_setup_each_ptr)(void);
#define TEST_SETUP_EACH() \
	void TEST_SETUP_EACH(void); \
	static TEST_funcptr_assigner \
	    TEST_setup_each(&TEST_setup_each_ptr, TEST_SETUP_EACH); \
	void TEST_SETUP_EACH(void)
static void (*TEST_cleanup_each_ptr)(void);
#define TEST_CLEANUP_EACH() \
	void TEST_CLEANUP_EACH(void); \
	static TEST_funcptr_assigner \
	    TEST_cleanup_each(&TEST_cleanup_each_ptr, TEST_CLEANUP_EACH); \
	void TEST_CLEANUP_EACH(void)

// helpers to call the setup/cleanup functions
inline void
TEST_do_setup_global(void)
{
	if (TEST_setup_global_ptr)
		TEST_setup_global_ptr();
}
inline void
TEST_do_cleanup_global(void)
{
	if (TEST_cleanup_global_ptr)
		TEST_cleanup_global_ptr();
}
inline void
TEST_do_setup_each(void)
{
	system("rm -rf " TEST_TMP_DIR);
	system("mkdir -p " TEST_TMP_DIR);
	if (TEST_setup_each_ptr)
		TEST_setup_each_ptr();
}
inline void
TEST_do_cleanup_each(void)
{
	if (TEST_cleanup_each_ptr)
		TEST_cleanup_each_ptr();
	system("rm -rf " TEST_TMP_DIR);
}

// the test main() entry point
int
main(void)
{
	TEST_do_setup_global();
	try {
		for (size_t i = 0; i < TEST_list.size(); i++) {
			TEST_do_setup_each();
			try {
				TEST_list[i]();
			} catch (...) {
				TEST_do_cleanup_each();
				throw;
			}
			TEST_do_cleanup_each();
		}
	} catch (...) {
		TEST_do_cleanup_global();
		throw;
	}
	TEST_do_cleanup_global();
	int exit_code = TEST_error_count ?  EXIT_FAILURE : EXIT_SUCCESS;
	exit(exit_code);
}

#endif // PP_TEST_H__
