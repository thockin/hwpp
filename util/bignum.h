/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_UTIL_BIGNUM_H__
#define PP_UTIL_BIGNUM_H__

#include <cstdio>  // Some versions of gmpxx.h need but don't include this
#include <gmpxx.h>
#include <ostream>
#include <sstream>
#include "assert.h"
#include "bit_buffer.h"

#define BITS_PER_LONG	(sizeof(long)*CHAR_BIT)

namespace bignum {

//
// All BigInts are signed.  GMP will allow you to create a BigInt from
// either signed or unsigned raw integers, and it will do "the right
// thing".  Unsigned raw integers will always create a positive BigInt.  It
// is not clear exactly what happens if you read an out-of-bounds positive
// BigInt as a signed value (e.g. ULONG_MAX read back as signed long).  It
// is also not clear what happens if you read a negative BigInt as an
// unsigned value (e.g. -1 read back as unsigned int).  Just don't do those
// things, and we'll all be happy.
//
class BigInt: public mpz_class
{
    public:
	// ctors are mostly pass-thru to GMP
	BigInt() {}
	BigInt(const BigInt &that): mpz_class(that) {}
	template <class T, class U>
	BigInt(const __gmp_expr<T, U> &expr): mpz_class(expr) {}
	explicit BigInt(const char *str, int base=0): mpz_class(str, base) {}
	explicit BigInt(const std::string &str, int base=0)
	    : mpz_class(str, base) {}
	BigInt(signed char value): mpz_class(value) {}
	BigInt(unsigned char value): mpz_class(value) {}
	BigInt(signed short value): mpz_class(value) {}
	BigInt(unsigned short value): mpz_class(value) {}
	BigInt(signed int value): mpz_class(value) {}
	BigInt(unsigned int value): mpz_class(value) {}
	BigInt(signed long value): mpz_class(value) {}
	BigInt(unsigned long value): mpz_class(value) {}
	BigInt(float value): mpz_class(value) {}
	BigInt(double value): mpz_class(value) {}
	// GMP only supports up to 'long' args for it's ctors.  We can
	// support 'long long' at a small cost on 32 bit systems, and for
	// free on 64 bit systems.
	BigInt(signed long long value)
	{
		*this = value;
	}
	BigInt(unsigned long long value)
	{
		*this = value;
	}
	BigInt(const util::BitBuffer &bitbuf)
	{
		*this = bitbuf;
	}

	// assignment operators are mostly pass-thru to GMP
	BigInt &
	operator=(const BigInt &that)
	{ mpz_class::operator=(that); return *this; }
	template <class T, class U>
	BigInt &
	operator=(const __gmp_expr<T, U> &that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(const std::string &that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(const char *that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(signed char that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(unsigned char that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(signed short that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(unsigned short that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(signed int that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(unsigned int that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(signed long that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(unsigned long that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(float that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(double that)
	{ mpz_class::operator=(that); return *this; }
	BigInt &
	operator=(signed long long that)
	{
		if ((sizeof(long) == sizeof(long long))
		 || (that >= LONG_MIN && that <= LONG_MAX)) {
			// simple
			*this = (signed long)that;
			return *this;
		}

		// not so simple
		int multiplier = 1;
		if (that < 0) {
			multiplier = -1;
			that *= -1;
		}

		unsigned long vlo = that;
		unsigned long vhi = (that >> (BITS_PER_LONG-1)) >> 1;
		*this = vhi;
		*this <<= BITS_PER_LONG;
		*this += vlo;
		*this *= multiplier;

		return *this;
	}
	BigInt &
	operator=(unsigned long long that)
	{
		if ((sizeof(long) == sizeof(long long))
		 || (that <= ULONG_MAX)) {
			// simple
			*this = (unsigned long)that;
			return *this;
		}

		// not so simple
		unsigned long vlo = that;
		unsigned long vhi = (that >> (BITS_PER_LONG-1)) >> 1;
		*this = vhi;
		*this <<= BITS_PER_LONG;
		*this += vlo;

		return *this;
	}
	BigInt &
	operator=(const util::BitBuffer &bitbuf)
	{
		// mpz_import() seems to not work.
		*this = 0;
		for (std::size_t i = bitbuf.size_bytes(); i > 0; i--) {
			*this <<= CHAR_BIT;
			*this += bitbuf.byte_at(i-1);
		}
		return *this;
	}

	// GMP only supports up to 'long' args for it's get methods.  We
	// can support 'long long' at a small cost on 32 bit systems, and
	// for free on 64 bit systems.
	long long
	get_si() const
	{
		return get_int();
	}
	long long
	get_int() const
	{
		if (sizeof(long) == sizeof(long long)
		 || (*this >= LONG_MIN && *this <= LONG_MAX)) {
			// simple
			return mpz_class::get_si();
		}

		// not so simple
		BigInt myval(*this);
		int multiplier = 1;
		if (myval < 0) {
			multiplier = -1;
			myval *= -1;
		}

		unsigned long rlo = myval.mpz_class::get_ui();
		myval >>= BITS_PER_LONG;
		unsigned long long rhi = myval.mpz_class::get_ui();
		unsigned long long result =
		    (((rhi << (BITS_PER_LONG-1))<<1) | rlo);
		result *= multiplier;

		return result;
	}

	unsigned long long
	get_ui() const
	{
		return get_int();
	}
	unsigned long long
	get_uint() const
	{
		if ((sizeof(long) == sizeof(long long))
		 || (*this >= 0 && *this <= ULONG_MAX)) {
			// simple
			return mpz_class::get_ui();
		}

		// not so simple
		BigInt myval(*this);
		unsigned long rlo = myval.mpz_class::get_ui();
		myval >>= BITS_PER_LONG;
		unsigned long long rhi = myval.mpz_class::get_ui();
		unsigned long long result =
		    (((rhi << (BITS_PER_LONG-1))<<1) | rlo);

		return result;
	}

	// Don't call this for negative numbers, which effectively have an
	// infinite number of bits.
	util::BitBuffer 
	get_bitbuffer(std::size_t bits=0) const
	{
		ASSERT(*this >= 0);

		// mpz_export() seems to not work.
		std::size_t bytes = 0;
		BigInt tmp(*this);
		while (tmp != 0) {
			bytes++;
			tmp >>= CHAR_BIT;
		}
		if (bits) {
			bytes = (bits + (CHAR_BIT-1)) / CHAR_BIT;
		}
		util::BitBuffer bitbuf(bits ? bits : (bytes * CHAR_BIT));

		BigInt myval(*this);
		for (std::size_t i = 0; i < bytes; i++) {
			bitbuf.byte_at(i) = myval.get_uint() & 0xff;
			myval >>= CHAR_BIT;
		}

		return bitbuf;
	}

	// Stay consistent with the mpz_class API.
	bool
	fits_slonglong_p() const
	{
		return (*this >= LONG_MIN && *this <= LONG_MAX);
	}
	bool
	fits_ulonglong_p() const
	{
		return (*this >= 0 && *this <= ULONG_MAX);
	}

	// Count the number of set bits.
	unsigned long
	popcount() const
	{
		return mpz_popcount(get_mpz_t());
	}
	static unsigned long
	popcount(const BigInt &val)
	{
		return val.popcount();
	}

	// Exponentiate this BigInt and return the result.
	BigInt
	pow(unsigned long exponent) const
	{
		BigInt bn(*this);
		mpz_pow_ui(bn.get_mpz_t(), get_mpz_t(), exponent);
		return bn;
	}
	static BigInt
	pow(const BigInt &val, unsigned long exponent)
	{
		return val.pow(exponent);
	}

	// Exponentiate this BigInt in place.
	void
	raise(unsigned long exponent)
	{
		mpz_pow_ui(get_mpz_t(), get_mpz_t(), exponent);
	}

	// convert to a decimal string
	std::string
	to_dec_string() const
	{
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}

	// convert to a hex string
	std::string
	to_hex_string() const
	{
		std::ostringstream oss;
		oss << "0x" << std::hex << *this;
		return oss.str();
	}

	// convert to an octal string
	std::string
	to_oct_string() const
	{
		std::ostringstream oss;
		oss << "0" << std::oct << *this;
		return oss.str();
	}

	// convert to a string
	std::string
	to_string() const
	{
		return to_dec_string();
	}

    // This strangeness allows BigInt to safely be treated as bools.
    // It's commonly known as the "safe bool idiom".
    private:
	typedef void (BigInt::*BoolType)() const;
	void convert_to_bool() const {}
    public:
	operator BoolType() const
	{
		if  (*this == 0) {
			return NULL;
		}
		return &BigInt::convert_to_bool;
	}
};

//
// Add some operators that the GMP implementation does not define.
// Note: These are defined on mpz_class, not on BigInt.
//

inline mpz_class
operator<<(const mpz_class &lhs, const mpz_class &rhs)
{
	return (lhs << rhs.get_ui());
}
inline mpz_class
operator>>(const mpz_class &lhs, const mpz_class &rhs)
{
	return (lhs >> rhs.get_ui());
}
inline mpz_class &
operator|=(mpz_class &lhs, const mpz_class &rhs)
{
	lhs = lhs | rhs;
	return lhs;
}
inline mpz_class &
operator&=(mpz_class &lhs, const mpz_class &rhs)
{
	lhs = lhs & rhs;
	return lhs;
}
inline mpz_class &
operator^=(mpz_class &lhs, const mpz_class &rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

//
// Because BigInt supports 'long long' types, but GMP doesn't, we need
// explicit operators for comparisons.
// Note: These are defined on mpz_class, not on BigInt.
//

// ==
inline bool
operator==(const mpz_class &lhs, signed long long rhs)
{
	return (lhs == BigInt(rhs));
}
inline bool
operator==(const mpz_class &lhs, unsigned long long rhs)
{
	return (lhs == BigInt(rhs));
}
inline bool
operator==(signed long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) == rhs);
}
inline bool
operator==(unsigned long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) == rhs);
}
// !=
inline bool
operator!=(const mpz_class &lhs, signed long long rhs)
{
	return (lhs != BigInt(rhs));
}
inline bool
operator!=(const mpz_class &lhs, unsigned long long rhs)
{
	return (lhs != BigInt(rhs));
}
inline bool
operator!=(signed long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) != rhs);
}
inline bool
operator!=(unsigned long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) != rhs);
}
// <
inline bool
operator<(const mpz_class &lhs, signed long long rhs)
{
	return (lhs < BigInt(rhs));
}
inline bool
operator<(const mpz_class &lhs, unsigned long long rhs)
{
	return (lhs < BigInt(rhs));
}
inline bool
operator<(signed long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) < rhs);
}
inline bool
operator<(unsigned long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) < rhs);
}
// >
inline bool
operator>(const mpz_class &lhs, signed long long rhs)
{
	return (lhs > BigInt(rhs));
}
inline bool
operator>(const mpz_class &lhs, unsigned long long rhs)
{
	return (lhs > BigInt(rhs));
}
inline bool
operator>(signed long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) > rhs);
}
inline bool
operator>(unsigned long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) > rhs);
}
// <=
inline bool
operator<=(const mpz_class &lhs, signed long long rhs)
{
	return (lhs <= BigInt(rhs));
}
inline bool
operator<=(const mpz_class &lhs, unsigned long long rhs)
{
	return (lhs <= BigInt(rhs));
}
inline bool
operator<=(signed long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) <= rhs);
}
inline bool
operator<=(unsigned long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) <= rhs);
}
// >=
inline bool
operator>=(const mpz_class &lhs, signed long long rhs)
{
	return (lhs >= BigInt(rhs));
}
inline bool
operator>=(const mpz_class &lhs, unsigned long long rhs)
{
	return (lhs >= BigInt(rhs));
}
inline bool
operator>=(signed long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) >= rhs);
}
inline bool
operator>=(unsigned long long lhs, const mpz_class &rhs)
{
	return (BigInt(lhs) >= rhs);
}

} // namespace bignum

#endif // PP_UTIL_BIGNUM_H__