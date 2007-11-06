/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DEBUG_H__
#define PP_DEBUG_H__

#if DEBUG

#include <stdio.h>
#include <stdlib.h>

#define DASSERT(predicate) do { \
	if (predicate) { \
		printf("ERR: DASSERT() failed: %s:%d\n", __FILE__, __LINE__); \
		abort(); \
	} \
} while (0)

#else // !DEBUG

#define DASSERT(predicate)

#endif // DEBUG

#endif // PP_DEBUG_H__
