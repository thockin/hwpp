/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TEST_H__
#define PP_TEST_H__

#include "pp.h"
#include <iostream>

#define PP_TEST_ERROR_ON_LINE(msg, file, line) \
	std::cerr << "ERROR: [" << file << ":" << line << "] " \
		<< msg << std::endl
#define PP_TEST_ERROR(msg) PP_TEST_ERROR_ON_LINE(msg, __FILE__, __LINE__)

#define PP_TEST_ASSERT(pred, msg) PP_TEST_ASSERT_(pred, msg, __FILE__, __LINE__)
inline int
PP_TEST_ASSERT_(bool predicate, const string &msg, const string &file, int line)
{
	if (!predicate) {
		PP_TEST_ERROR_ON_LINE(msg, file, line);
		return 1;
	}
	return 0;
}

#define PP_TEST_WARNING(msg) \
	std::cerr << "WARNING: [" __FILE__ << ":" << __LINE__ << "] " \
		<< msg << std::endl

#endif // PP_TEST_H__
