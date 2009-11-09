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
using std::size_t;
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
using boost::static_pointer_cast;
using boost::const_pointer_cast;
#include "pp/util/printfxx.h"
#include "pp/util/bignum.h"
#include "pp/debug.h"
#include "pp/version.h"

namespace pp {

/*
 * Value - a numeric value, such as data from a register.
 */
typedef ::bignum::BigInt Value;

/*
 * BitWidth - how wide something is, in bits.
 */
typedef unsigned int BitWidth;
static const BitWidth BITS0   = 0;
static const BitWidth BITS4   = 4;
static const BitWidth BITS8   = 8;
static const BitWidth BITS12  = 12;
static const BitWidth BITS16  = 16;
static const BitWidth BITS20  = 20;
static const BitWidth BITS32  = 32;
static const BitWidth BITS64  = 64;
static const BitWidth BITS128 = 128;
static const BitWidth BIT_WIDTH_MAX = BITS128;

inline BitWidth
BYTES_TO_BITS(size_t bytes)
{
	return BitWidth((bytes) * CHAR_BIT);
}

inline size_t
BITS_TO_BYTES(BitWidth bits)
{
	return size_t(((bits)+(CHAR_BIT-1)) / CHAR_BIT);
}

/* generate a bitmask of n bits */
inline Value
MASK(BitWidth nbits)
{
	Value val(1);
	val <<= nbits;
	val--;
	return val;
}

//
// WARN() - issue a runtime warning
//
inline void
WARN(const string &message)
{
	std::cerr << "WARN: " << message << std::endl;
}

}  // namespace pp

#endif // PP_PP_H__
