// More full-featured support for big-int values than GMPXX provides natively.
// Copyright (c) Tim Hockin, 2007

#ifndef HWPP_UTIL_BIGNUM_H__
#define HWPP_UTIL_BIGNUM_H__

#include <cstdio>  // Some versions of gmpxx.h need but don't include this
#include <gmpxx.h>
#include <limits>
#include "util/bit_buffer.h"

namespace bignum {

// BigInt
//
// All BigInts are signed.  This class allows you to create a BigInt from
// either signed or unsigned raw integers, and it will try to do "the right
// thing".  Unsigned raw integers will always create a positive BigInt.  It
// is not clear exactly what happens if you read an out-of-bounds positive
// BigInt as a signed value (e.g. ULONG_MAX read back as signed long).  It
// is also not clear what happens if you read a negative BigInt as an
// unsigned value (e.g. -1 read back as unsigned int).  Just don't do those
// things, and we'll all be happy.
class BigInt: public mpz_class
{
    public:
	// The ctors are mostly pass-thru to GMP.
	BigInt() {}
	BigInt(const BigInt &that): mpz_class(that) {}
	template<typename T, typename U>
	BigInt(const __gmp_expr<T, U> &expr): mpz_class(expr) {}
	explicit BigInt(const char *str): mpz_class(str, 0) {}
	BigInt(const char *str, int base): mpz_class(str, base) {}
	explicit BigInt(const std::string &str) : mpz_class(str, 0) {}
	BigInt(const std::string &str, int base) : mpz_class(str, base) {}
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
	// free on 64 bit systems.  Consolidate the logic into operator=().
	BigInt(signed long long value) { *this = value; }
	BigInt(unsigned long long value) { *this = value; }
	BigInt(const ::util::BitBuffer &bitbuf) { *this = bitbuf; }

	// The assignment operators are mostly pass-thru to GMP.
	BigInt &
	operator=(const BigInt &that)
	{ mpz_class::operator=(that); return *this; }
	template<typename T, typename U>
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
	operator=(signed long long that);
	BigInt &
	operator=(unsigned long long that);
	BigInt &
	operator=(const ::util::BitBuffer &bitbuf);

	// GMP only supports up to 'long' args for it's get methods.  We
	// can support 'long long' at a small cost on 32 bit systems, and
	// for free on 64 bit systems.
	long long
	get_si() const
	{
		return as_int();
	}
	long long
	as_int() const;

	unsigned long long
	get_ui() const
	{
		return as_uint();
	}
	unsigned long long
	as_uint() const;

	// Don't call this for negative numbers, which effectively have an
	// infinite number of bits.
	::util::BitBuffer
	to_bitbuffer() const
	{
		return to_bitbuffer(0);
	}
	::util::BitBuffer
	to_bitbuffer(std::size_t bits) const;

	// Stay consistent with the mpz_class API.
	bool
	fits_slonglong_p() const
	{
		return in_bounds_for<signed long long>(*this);
	}
	bool
	fits_ulonglong_p() const
	{
		return in_bounds_for<unsigned long long>(*this);
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
	to_dec_string() const;

	// convert to a hex string
	std::string
	to_hex_string() const;

	// convert to an octal string
	std::string
	to_oct_string() const;

	// convert to a string
	std::string
	to_string() const
	{
		return to_dec_string();
	}

    private:
	// Helper function: test whether val can be represented by a Tdest.
	// Calling this with different signedness would be bad, but it's not
	// worth fixing. Don't do that.
	template<typename Tdest, typename Tsrc>
	static bool
	in_bounds_for(const Tsrc &val)
	{
		return (val >= std::numeric_limits<Tdest>::min()
		     && val <= std::numeric_limits<Tdest>::max());
	}

    // This strangeness allows BigInt to safely be treated as bools.
    // It's commonly known as the "safe bool idiom".
    private:
	typedef void (BigInt::*BoolType)() const;
	void convert_to_bool() const {}
    public:
	operator BoolType() const
	{
		if (*this == 0) {
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

#endif // HWPP_UTIL_BIGNUM_H__
