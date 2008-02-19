/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include "pp.h"
#include <iostream>

#define TEST_ERROR_ON_LINE(msg, file, line) \
	std::cerr << "ERROR: [" << file << ":" << line << "] " \
		<< msg << std::endl
#define TEST_ERROR(msg) TEST_ERROR_ON_LINE(msg, __FILE__, __LINE__)

#define TEST_ASSERT(pred, msg) TEST_ASSERT_(pred, msg, __FILE__, __LINE__)
inline int
TEST_ASSERT_(bool predicate, const string &msg, const string &file, int line)
{
	if (!predicate) {
		TEST_ERROR_ON_LINE(msg, file, line);
		return 1;
	}
	return 0;
}

#define TEST_WARNING(msg) \
	std::cerr << "WARNING: [" __FILE__ << ":" << __LINE__ << "] " \
		<< msg << std::endl

#endif // PP_TEST_H__
