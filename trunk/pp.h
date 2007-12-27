/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_H__
#define PP_PP_H__

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdlib.h>
#include <string>
using std::string;
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
#include "debug.h"

/* convert any stream-ready object to a string */
#include <sstream>
template <typename T>
inline string
to_string(const T &val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

/* enable simple string manipulations of boost::format */
inline string &
operator+=(string &str, const boost::format &fmt)
{
	/* return the original lhs */
	str += to_string(fmt);
	return str;
}
inline string
operator+(const string &str, const boost::format &fmt)
{
	/* return a new string */
	return string(str + to_string(fmt));
}
inline string
operator+(const boost::format &fmt, const string &str)
{
	/* return a new string */
	return string(to_string(fmt) + str);
}
inline bool
operator==(const string &str, const boost::format &fmt)
{
	return (str == to_string(fmt));
}
inline bool
operator==(const boost::format &fmt, const string &str)
{
	return (str == fmt);
}
inline bool
operator!=(const string &str, const boost::format &fmt)
{
	return !(str == fmt);
}
inline bool
operator!=(const boost::format &fmt, const string &str)
{
	return !(str == fmt);
}

/*
 * pp_value - the fundamental container for data values.
 */
 //FIXME: these should probably become opaque bignum types
typedef uint64_t pp_value;
typedef int64_t  pp_svalue;

inline pp_value
pp_value_from_string(const string &str)
{
	return strtoull(str.c_str(), NULL, 0);
}

/*
 * pp_bitwidth - how wide something is, in bits.
 */
typedef enum pp_bitwidth {
	BITS0  = 0,
	BITS4  = 4,
	BITS8  = 8,
	BITS16 = 16,
	BITS32 = 32,
	BITS64 = 64,
} pp_bitwidth;
#define PP_BITWIDTH_MAX BITS64

/* generate a bitmask of n bits */
#define PP_MASK(n) ((((1ULL<<(n)/2)<<(n)/2)<<(n)%2)-1)

/*
 * pp_regaddr - the address of a register.
 */
typedef pp_value pp_regaddr;

/*
 * WARN() - issue a runtime warning
 */
#define WARN(message) do { \
	std::cerr << "WARN: " << message << std::endl; \
} while (0)

#endif // PP_PP_H__
