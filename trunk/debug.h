/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DEBUG_H__
#define PP_DEBUG_H__

#if DEBUG

#include <iostream>

#define DASSERT_PRINT_ERR__(predicate) std::cerr \
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

#define DASSERT_ABORT__() abort()

#define DASSERT(predicate) do { \
	if (!(predicate)) { \
		DASSERT_PRINT_ERR__(predicate) \
			<< std::endl; \
		DASSERT_ABORT__(); \
	} \
} while (0)

#define DASSERT_MSG(predicate, message) do { \
	if (!(predicate)) { \
		DASSERT_PRINT_ERR__(predicate) \
			<< ":" \
			<< std::endl \
			<< "        " \
			<< message \
			<< std::endl; \
		DASSERT_ABORT__(); \
	} \
} while (0)

#define DWARN(message) do { \
	std::cerr \
		<< "DBG: warning at " \
		<< __FILE__ \
		<< ":" \
		<< __LINE__ \
		<< " " \
		<< __func__ \
		<< "(): " \
		<< message \
		<< std::endl; \
} while (0)

#define DTRACE(condition, message) do { \
	if (condition) { \
		std::cerr \
			<< "DBG: " \
			<< message \
			<< std::endl; \
	} \
} while (0)

// trace dirents?
#ifndef TRACE_DIRENTS
#  define TRACE_DIRENTS 0
#endif
// trace register creation?
#ifndef TRACE_REGS
#  define TRACE_REGS 0
#endif
// trace field creation?
#ifndef TRACE_FIELDS
#  define TRACE_FIELDS 0
#endif
#ifndef TRACE_FIELD_BITS
#  define TRACE_FIELD_BITS 0
#endif
// trace scope creation?
#ifndef TRACE_SCOPES
#  define TRACE_SCOPES 0
#endif
// trace datatype creation?
#ifndef TRACE_TYPES
#  define TRACE_TYPES 0
#endif
// trace driver utils?
#ifndef TRACE_DRIVER_UTILS
#  define TRACE_DRIVER_UTILS 0
#endif
// trace driver utils?
#ifndef TRACE_DISCOVERY
#  define TRACE_DISCOVERY 0
#endif
// trace object lifetimes?
#ifndef TRACE_LIFETIMES
#  define TRACE_LIFETIMES 0
#endif

#else // !DEBUG

#define DASSERT(predicate)
#define DASSERT_MSG(predicate, message)
#define DWARN(message)
#define DTRACE(condition, message)

#endif // DEBUG

#endif // PP_DEBUG_H__
