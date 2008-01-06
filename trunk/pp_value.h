/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_VALUE_H__
#define PP_PP_VALUE_H__

#include <gmpxx.h>

// All pp_values are signed.  GMP will allow you to create a pp_value from
// either signed or unsigned raw integers, and it will do "the right
// thing".  Unsigned raw integers will create a positive pp_value.  It is
// not clear exactly what happens if you read an out-of-bounds positive
// pp_value as a signed value (e.g. ULONG_MAX read back as signed long).  It
// is also not clear what happens if you read a negative pp_value as an
// unsigned value (e.g. -1 read back as unsigned int).  Just don't do
// those things, and we'll all be happy.
typedef mpz_class pp_value;

//
// Since I can not add constructors to GMP's class, let's define some
// friendly converter functions for pp_values.
//

#define BITS_PER_LONG	(sizeof(long)*CHAR_BIT)

template<typename Tdata>
inline pp_value
pp_value_from(Tdata value)
{
	return pp_value(value);
}

// GMP only supports up to 'long' args for it's ctors.  We can support
// 'long long' at a small cost on 32 bit systems, and for free on 64 bit
// systems.
template<>
inline pp_value
pp_value_from<int64_t>(int64_t value)
{
	if ((sizeof(long) == sizeof(long long))
	 || (value >= LONG_MIN && value <= LONG_MAX)) {
		// simple
		return pp_value((long)value);
	}

	// not so simple
	int negative = 0;
	if (value < 0) {
		negative = 1;
		value *= -1;
	}

	unsigned long vlo = value;
	unsigned long vhi = value >> BITS_PER_LONG;
	pp_value ret(vhi);
	ret <<= BITS_PER_LONG;
	ret += vlo;

	if (negative) {
		ret *= -1;
	}

	return ret;
}
template<>
inline pp_value
pp_value_from<uint64_t>(uint64_t value)
{
	if ((sizeof(long) == sizeof(long long)) || (value <= ULONG_MAX)) {
		// simple
		return pp_value((unsigned long)value);
	}

	// not so simple
	unsigned long vlo = value;
	unsigned long vhi = value >> BITS_PER_LONG;
	pp_value ret(vhi);
	ret <<= BITS_PER_LONG;
	ret += vlo;
	return ret;
}

template<typename Tdata>
inline Tdata
pp_value_to(const pp_value &value)
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
pp_value_to<int64_t>(const pp_value &value)
{
	if (sizeof(long) == sizeof(long long)
	 || (value >= LONG_MIN && value <= LONG_MAX)) {
		// simple
		return value.get_si();
	}

	// not so simple
	pp_value myval(value);
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
pp_value_to<uint64_t>(const pp_value &value)
{
	if ((sizeof(long) == sizeof(long long)) || (value <= ULONG_MAX)) {
		// simple
		return value.get_ui();
	}

	// not so simple
	pp_value myval(value);
	unsigned long rlo = myval.get_ui();
	myval >>= BITS_PER_LONG;
	unsigned long long rhi = myval.get_ui();
	unsigned long long uresult = ((rhi << BITS_PER_LONG) | rlo);

	return uresult;
}
template<>
inline string
pp_value_to<string>(const pp_value &value)
{
	return value.get_str();
}

//
// add some operators that the standard implementation does not define
//

inline pp_value
operator<<(const pp_value &lhs, const pp_value &rhs)
{
	return (lhs << rhs.get_ui());
}
inline pp_value
operator>>(const pp_value &lhs, const pp_value &rhs)
{
	return (lhs >> rhs.get_ui());
}

#endif // PP_PP_VALUE_H__
