/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_H__
#define PP_PP_H__

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <string>
using std::string;
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>

/* convert any stream-ready object to a string */
#include <sstream>
template <typename T>
string
convert_to_string(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}
#define to_string(x) convert_to_string<typeof(x)>(x)

/* enable simple string manipulations of boost::format */
string &
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

/*
 * pp_bitwidth - how wide something is, in bits.
 */
typedef enum pp_bitwidth {
	BITS0  = 0,
	BITS8  = 8,
	BITS16 = 16,
	BITS32 = 32,
	BITS64 = 64,
} pp_bitwidth;

/* generate a bitmask of n bits */
#define PP_MASK(n) ((((1ULL<<(n)/2)<<(n)/2)<<(n)%2)-1)

/*
 * pp_regaddr - the address of a register.
 */
typedef uint32_t pp_regaddr;

#endif // PP_PP_H__
