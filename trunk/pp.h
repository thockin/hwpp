/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_H__
#define PP_PP_H__

#define __STDC_LIMIT_MACROS
#define _FILE_OFFSET_BITS 64
#include <stdint.h>
#include <stdlib.h>
#include <string>
using std::string;
#include <iostream>
#include <limits>
#include <climits>
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
#include "bignum.h"
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
 * pp_value - data from a register.
 */
typedef bignum pp_value;

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
	BITS128 = 128,
} pp_bitwidth;
#define PP_BITWIDTH_MAX BITS128

/* generate a bitmask of n bits */
static inline pp_value
PP_MASK(int nbits)
{
	pp_value val(1);
	val <<= nbits;
	val--;
	return val;
}

/*
 * WARN() - issue a runtime warning
 */
#define WARN(message) do { \
	std::cerr << "WARN: " << message << std::endl; \
} while (0)

#endif // PP_PP_H__
