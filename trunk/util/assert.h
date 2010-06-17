/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_UTIL_ASSERT_H__
#define PP_UTIL_ASSERT_H__

#include <stdlib.h>
#include <iostream>

#define ASSERT_PRINT_ERR__(predicate) std::cerr \
		<< "ERROR: assertion failed at " \
		<< __FILE__ \
		<< ":" \
		<< __LINE__ \
		<< " " \
		<< __func__ \
		<< "():" \
		<< std::endl \
		<< "    assert(" \
		<< #predicate \
		<< ")"

#define ASSERT_ABORT__() abort()

#define ASSERT(predicate) do { \
	if (!(predicate)) { \
		ASSERT_PRINT_ERR__(predicate) \
			<< std::endl; \
		ASSERT_ABORT__(); \
	} \
} while (0)

#define ASSERT_MSG(predicate, message) do { \
	if (!(predicate)) { \
		ASSERT_PRINT_ERR__(predicate) \
			<< ":" \
			<< std::endl \
			<< "        " \
			<< message \
			<< std::endl; \
		ASSERT_ABORT__(); \
	} \
} while (0)

#if DEBUG

#define DASSERT(predicate)               ASSERT(predicate)
#define DASSERT_MSG(predicate, message)  ASSERT_MSG(predicate, message)

#else // !DEBUG

#define DASSERT(predicate)
#define DASSERT_MSG(predicate, message)

#endif // DEBUG

#endif // PP_UTIL_ASSERT_H__
