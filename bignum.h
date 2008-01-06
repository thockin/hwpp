/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_BIGNUM_H__
#define PP_BIGNUM_H__

#include <gmpxx.h>

// All bignums are signed.  GMP will allow you to create a bignum from
// either signed or unsigned raw integers, and it will do "the right
// thing".  Unsigned raw integers will create a positive bignum.  It is
// not clear exactly what happens if you read an out-of-bounds positive
// bignum as a signed value (e.g. ULONG_MAX read back as signed long).  It
// is also not clear what happens if you read a negative bignum as an
// unsigned value (e.g. -1 read back as unsigned int).  Just don't do
// those things, and we'll all be happy.
typedef mpz_class bignum;

//
// Since I can not add constructors to GMP's class, let's define some
// friendly converter functions for bignums.
//

#define BITS_PER_LONG	(sizeof(long)*CHAR_BIT)

template<typename Tdata>
inline bignum
bignum_from(Tdata value)
{
	return bignum(value);
}

// GMP only supports up to 'long' args for it's ctors.  We can support
// 'long long' at a small cost on 32 bit systems, and for free on 64 bit
// systems.
template<>
inline bignum
bignum_from<int64_t>(int64_t value)
{
	if ((sizeof(long) == sizeof(long long))
	 || (value >= LONG_MIN && value <= LONG_MAX)) {
		// simple
		return bignum((long)value);
	}

	// not so simple
	int negative = 0;
	if (value < 0) {
		negative = 1;
		value *= -1;
	}

	unsigned long vlo = value;
	unsigned long vhi = value >> BITS_PER_LONG;
	bignum ret(vhi);
	ret <<= BITS_PER_LONG;
	ret += vlo;

	if (negative) {
		ret *= -1;
	}

	return ret;
}
template<>
inline bignum
bignum_from<uint64_t>(uint64_t value)
{
	if ((sizeof(long) == sizeof(long long)) || (value <= ULONG_MAX)) {
		// simple
		return bignum((unsigned long)value);
	}

	// not so simple
	unsigned long vlo = value;
	unsigned long vhi = value >> BITS_PER_LONG;
	bignum ret(vhi);
	ret <<= BITS_PER_LONG;
	ret += vlo;
	return ret;
}

template<typename Tdata>
inline Tdata
bignum_to(const bignum &value)
{
	if (std::numeric_limits<Tdata>::is_signed) {
		return value.get_si();
	}
	return value.get_ui();
}

// GMP only supports up to 'long' args for it's get methods.  We can support
// 'long long' at a small cost on 32 bit systems, and for free on 64 bit
// systems.
template<>
inline int64_t
bignum_to<int64_t>(const bignum &value)
{
	if (sizeof(long) == sizeof(long long)
	 || (value >= LONG_MIN && value <= LONG_MAX)) {
		// simple
		return value.get_si();
	}

	// not so simple
	bignum myval(value);
	int negative = 0;
	if (myval < 0) {
		negative = 1;
		myval *= -1;
	}

	unsigned long rlo = myval.get_ui();
	myval >>= BITS_PER_LONG;
	unsigned long long rhi = myval.get_ui();
	unsigned long long uresult = ((rhi << BITS_PER_LONG) | rlo);

	if (negative) {
		uresult *= -1;
	}

	return uresult;
}
template<>
inline uint64_t
bignum_to<uint64_t>(const bignum &value)
{
	if ((sizeof(long) == sizeof(long long)) || (value <= ULONG_MAX)) {
		// simple
		return value.get_ui();
	}

	// not so simple
	bignum myval(value);
	unsigned long rlo = myval.get_ui();
	myval >>= BITS_PER_LONG;
	unsigned long long rhi = myval.get_ui();
	unsigned long long uresult = ((rhi << BITS_PER_LONG) | rlo);

	return uresult;
}
template<>
inline string
bignum_to<string>(const bignum &value)
{
	return value.get_str();
}

//
// add some operators that the standard implementation does not define
//

inline bignum
operator<<(const bignum &lhs, const bignum &rhs)
{
	return (lhs << rhs.get_ui());
}
inline bignum
operator>>(const bignum &lhs, const bignum &rhs)
{
	return (lhs >> rhs.get_ui());
}

#endif // PP_BIGNUM_H__
