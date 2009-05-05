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
#include "printfxx.h"
#include "bignum.h"
#include "debug.h"

/*
 * pp_value - a numeric value, such as data from a register.
 */
typedef bignum::BigInt pp_value;

/*
 * pp_bitwidth - how wide something is, in bits.
 */
typedef unsigned long pp_bitwidth;
static const pp_bitwidth BITS0   = 0;
static const pp_bitwidth BITS4   = 4;
static const pp_bitwidth BITS8   = 8;
static const pp_bitwidth BITS12  = 12;
static const pp_bitwidth BITS16  = 16;
static const pp_bitwidth BITS20  = 20;
static const pp_bitwidth BITS32  = 32;
static const pp_bitwidth BITS64  = 64;
static const pp_bitwidth BITS128 = 128;
#define PP_BITWIDTH_MAX BITS128
#define BYTES_TO_BITS(bytes) pp_bitwidth((bytes) * CHAR_BIT)
#define BITS_TO_BYTES(bits)  size_t(((bits)+(CHAR_BIT-1)) / CHAR_BIT)

/* generate a bitmask of n bits */
static inline pp_value
PP_MASK(pp_bitwidth nbits)
{
	pp_value val(1);
	val <<= nbits;
	val--;
	return val;
}

//
// PP_WARN() - issue a runtime warning
//
#define PP_WARN(message) do { \
	std::cerr << "WARN: " << message << std::endl; \
} while (0)

//
// PP version constants
//
extern const uint8_t  pp_ver_major;
extern const uint8_t  pp_ver_minor;
extern const uint16_t pp_ver_micro;
extern const uint32_t pp_version;
extern const char *pp_version_string;

static inline uint32_t
PP_MKVERSION(uint8_t major, uint8_t minor, uint16_t micro)
{
	return ((major << 24) | (minor << 16) | (micro));
}

#endif // PP_PP_H__
