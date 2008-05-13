/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_BIGNUM_H__
#define PP_BIGNUM_H__

#include <gmpxx.h>
#include <ostream>
#include "bitbuffer.h"

#define BITS_PER_LONG	(sizeof(long)*CHAR_BIT)

//
// All bignums are signed.  GMP will allow you to create a bignum from
// either signed or unsigned raw integers, and it will do "the right
// thing".  Unsigned raw integers will always create a positive bignum.  It
// is not clear exactly what happens if you read an out-of-bounds positive
// bignum as a signed value (e.g. ULONG_MAX read back as signed long).  It
// is also not clear what happens if you read a negative bignum as an
// unsigned value (e.g. -1 read back as unsigned int).  Just don't do those
// things, and we'll all be happy.
//
class bignum: public mpz_class
{
    public:
	// ctors are mostly pass-thru to GMP
	bignum() {}
	bignum(const bignum &that): mpz_class(that) {}
	template <class T, class U>
	bignum(const __gmp_expr<T, U> &expr): mpz_class(expr) {}
	explicit bignum(const char *str, int base=0): mpz_class(str, base) {}
	explicit bignum(const std::string &str, int base=0)
	    : mpz_class(str, base) {}
	bignum(signed char value): mpz_class(value) {}
	bignum(unsigned char value): mpz_class(value) {}
	bignum(signed short value): mpz_class(value) {}
	bignum(unsigned short value): mpz_class(value) {}
	bignum(signed int value): mpz_class(value) {}
	bignum(unsigned int value): mpz_class(value) {}
	bignum(signed long value): mpz_class(value) {}
	bignum(unsigned long value): mpz_class(value) {}
	bignum(float value): mpz_class(value) {}
	bignum(double value): mpz_class(value) {}
	// GMP only supports up to 'long' args for it's ctors.  We can
	// support 'long long' at a small cost on 32 bit systems, and for
	// free on 64 bit systems.
	bignum(signed long long value)
	{
		*this = value;
	}
	bignum(unsigned long long value)
	{
		*this = value;
	}
	bignum(const bitbuffer &bitbuf)
	{
		*this = bitbuf;
	}

	// assignment operators are mostly pass-thru to GMP
	bignum &
	operator=(const bignum &that)
	{ mpz_class::operator=(that); return *this; }
	template <class T, class U>
	bignum &
	operator=(const __gmp_expr<T, U> &that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(const std::string &that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(const char *that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(signed char that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(unsigned char that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(signed short that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(unsigned short that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(signed int that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(unsigned int that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(signed long that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(unsigned long that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(float that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
	operator=(double that)
	{ mpz_class::operator=(that); return *this; }
	bignum &
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
		unsigned long vhi = (that >> BITS_PER_LONG-1) >> 1;
		*this = vhi;
		*this <<= BITS_PER_LONG;
		*this += vlo;
		*this *= multiplier;

		return *this;
	}
	bignum &
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
		unsigned long vhi = (that >> BITS_PER_LONG-1) >> 1;
		*this = vhi;
		*this <<= BITS_PER_LONG;
		*this += vlo;

		return *this;
	}
	bignum &
	operator=(const bitbuffer &bitbuf)
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
		bignum myval(*this);
		int multiplier = 1;
		if (myval < 0) {
			multiplier = -1;
			myval *= -1;
		}

		unsigned long rlo = myval.mpz_class::get_ui();
		myval >>= BITS_PER_LONG;
		unsigned long long rhi = myval.mpz_class::get_ui();
		unsigned long long result =
		    (((rhi << BITS_PER_LONG-1)<<1) | rlo);
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
		bignum myval(*this);
		unsigned long rlo = myval.mpz_class::get_ui();
		myval >>= BITS_PER_LONG;
		unsigned long long rhi = myval.mpz_class::get_ui();
		unsigned long long result =
		    (((rhi << BITS_PER_LONG-1)<<1) | rlo);

		return result;
	}

	bitbuffer
	get_bitbuffer(unsigned long bits=0) const
	{
		// mpz_export() seems to not work.
		std::size_t bytes = 0;
		bignum tmp(*this);
		while (tmp != 0) {
			bytes++;
			tmp >>= CHAR_BIT;
		}
		if (bits) {
			bytes = (bits + (CHAR_BIT-1)) / CHAR_BIT;
		}
		bitbuffer bitbuf(bits ? bits : (bytes * CHAR_BIT));

		bignum myval(*this);
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
};

//
// Add some operators that the GMP implementation does not define.
// Note: These are defined on mpz_class, not on bignum.
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

#endif // PP_BIGNUM_H__
