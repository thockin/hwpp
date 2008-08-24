/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include "pp.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <stdlib.h>

// how many failures have we had?
static int TEST_error_count;

// generate a test failure
#define TEST_ERROR(msg) TEST_do_error(msg, __FILE__, __LINE__)
inline void
TEST_do_error(const string &msg, const string &file, int line)
{
	TEST_error_count++;
	std::cerr << "FAIL: [" << file << ":" << line << "] "
		<< msg << std::endl;
}

// assert a condition and fail if the condition is false
#define TEST_ASSERT(pred, msg) TEST_do_assert(pred, msg, __FILE__, __LINE__)
inline void
TEST_do_assert(bool predicate, const string &msg, const string &file, int line)
{
	if (!predicate) {
		TEST_do_error(msg, file, line);
	}
}

// helpers for testing comparisons reciprocally
#define TEST_ASSERT_EQ(lhs, rhs, msg) \
	TEST_ASSERT(lhs == rhs, msg), TEST_ASSERT(rhs == lhs, msg)
#define TEST_ASSERT_NE(lhs, rhs, msg) \
	TEST_ASSERT(lhs != rhs, msg), TEST_ASSERT(rhs != lhs, msg)
#define TEST_ASSERT_LT(lhs, rhs, msg) \
	TEST_ASSERT(lhs < rhs, msg), TEST_ASSERT(rhs >= lhs, msg)
#define TEST_ASSERT_GT(lhs, rhs, msg) \
	TEST_ASSERT(lhs > rhs, msg), TEST_ASSERT(rhs <= lhs, msg)
#define TEST_ASSERT_LE(lhs, rhs, msg) \
	TEST_ASSERT(lhs <= rhs, msg), TEST_ASSERT(rhs > lhs, msg)
#define TEST_ASSERT_GE(lhs, rhs, msg) \
	TEST_ASSERT(lhs >= rhs, msg), TEST_ASSERT(rhs < lhs, msg)

// generate a warning message
#define TEST_WARNING(msg) TEST_do_warning(msg, __FILE__, __LINE__)
inline void
TEST_do_warning(const string &msg, const string &file, int line)
{
	std::cerr << "WARN: [" << file << ":" << line << "] "
		<< msg << std::endl;
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
