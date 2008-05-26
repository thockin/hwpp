/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include "pp.h"
#include <iostream>
#include <stdlib.h>

#define TEST_ERROR(msg) TEST_ERROR_(msg, __FILE__, __LINE__)
inline int
TEST_ERROR_(const string &msg, const string &file, int line)
{
	std::cerr << "ERROR: [" << file << ":" << line << "] "
		<< msg << std::endl;
	return 1;
}

#define TEST_ASSERT(pred, msg) TEST_ASSERT_(pred, msg, __FILE__, __LINE__)
inline int
TEST_ASSERT_(bool predicate, const string &msg, const string &file, int line)
{
	if (!predicate) {
		return TEST_ERROR_(msg, file, line);
	}
	return 0;
}

#define TEST_WARNING(msg) \
	std::cerr << "WARNING: [" __FILE__ << ":" << __LINE__ << "] " \
		<< msg << std::endl

#define TEST_EXIT(result_) \
	do {  \
		int exit_code_ = (result_) ? EXIT_FAILURE : EXIT_SUCCESS; \
		exit(exit_code_); \
	} while(0)

struct test_component
{
	int (*entry_point)(void);
};

#define TEST(name_) \
	{ \
		name_ \
	}

#define TEST_LIST(...) \
	int main(void) \
	{ \
		static struct test_component __t[] = { \
			__VA_ARGS__ \
		}; \
		int ret = 0; \
		int num = sizeof(__t)/sizeof(__t[0]); \
		struct test_component* current = &__t[0]; \
		while (num--) { \
			ret += current->entry_point(); \
			current++; \
		} \
		TEST_EXIT(ret); \
	}

#endif // PP_TEST_H__
