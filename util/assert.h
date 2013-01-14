// Rudimentary support for assertions.
//
// This does not layer on top of util/log.h because logging can be disabled,
// but assertions can not.
//
// Copyright (c) Tim Hockin, 2007

#ifndef HWPP_UTIL_ASSERT_H__
#define HWPP_UTIL_ASSERT_H__

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

#define ASSERTM(predicate, message) do { \
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

#define DASSERT(predicate)            ASSERT(predicate)
#define DASSERTM(predicate, message)  ASSERTM(predicate, message)

#else // !DEBUG

#define DASSERT(predicate)
#define DASSERTM(predicate, message)

#endif // DEBUG

#endif // HWPP_UTIL_ASSERT_H__
