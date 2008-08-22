/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include "pp.h"
#include <iostream>
#include <stdlib.h>

static int TEST_ERROR_COUNT;

#define TEST_ERROR(msg) TEST_ERROR_(msg, __FILE__, __LINE__)
inline void
TEST_ERROR_(const string &msg, const string &file, int line)
{
	TEST_ERROR_COUNT++;
	std::cerr << "ERROR: [" << file << ":" << line << "] "
		<< msg << std::endl;
}

#define TEST_ASSERT(pred, msg) TEST_ASSERT_(pred, msg, __FILE__, __LINE__)
inline void
TEST_ASSERT_(bool predicate, const string &msg, const string &file, int line)
{
	if (!predicate) {
		TEST_ERROR_(msg, file, line);
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

#define TEST_WARNING(msg) \
	std::cerr << "WARNING: [" __FILE__ << ":" << __LINE__ << "] " \
		<< msg << std::endl

struct test_component
{
	void (*entry_point)(void);
};

#define TEST(name_) \
	{ \
		name_ \
	}

#define TEST_TMP_DIR "test_tmp"

inline void
TEST_SETUP()
{
	system("rm -rf " TEST_TMP_DIR);
	system("mkdir -p " TEST_TMP_DIR);
}

inline void
TEST_CLEANUP()
{
	system("rm -rf " TEST_TMP_DIR);
}

#define TEST_LIST(...) \
	int main(void) \
	{ \
		static struct test_component __t[] = { \
			__VA_ARGS__ \
		}; \
		int num = sizeof(__t)/sizeof(__t[0]); \
		struct test_component* current = &__t[0]; \
		while (num--) { \
			TEST_SETUP(); \
			current->entry_point(); \
			current++; \
			TEST_CLEANUP(); \
		} \
		int exit_code = TEST_ERROR_COUNT ? \
				EXIT_FAILURE : EXIT_SUCCESS; \
		exit(exit_code); \
	}

#endif // PP_TEST_H__
