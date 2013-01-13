/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_HWPP_H__
#define HWPP_HWPP_H__

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
#include "util/printfxx.h"
#include "util/bignum.h"
#include "debug.h"
#include "version.h"

namespace hwpp {

// Value - a numeric value, such as data from a register.
typedef ::bignum::BigInt Value;

// BitWidth - how wide something is, in bits.
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

// Generate a bitmask of N bits.
inline Value
MASK(BitWidth nbits)
{
	Value val(1);
	val <<= nbits;
	val--;
	return val;
}

// Initialize HWPP.
//FIXME: pass an options struct?
void
hwpp_init(bool enable_logging, uint8_t log_verbosity);

}  // namespace hwpp

#endif // HWPP_HWPP_H__
