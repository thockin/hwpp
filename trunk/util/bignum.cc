// More full-featured support for big-int values than GMPXX provides natively.
// Copyright (c) Tim Hockin, 2007

#include "util/bignum.h"

#include <limits.h>
#include <ostream>
#include <sstream>
#include "util/assert.h"
#include "util/bit_buffer.h"

namespace bignum {

static const int BITS_PER_LONG = (sizeof(long) * CHAR_BIT);

BigInt &
BigInt::operator=(signed long long that)
{
	// The simple case: 'that' is in bounds for the GMP
	// assignment operator.
	if ((sizeof(long) == sizeof(long long))
	 || in_bounds_for<signed long>(that)) {
		*this = (signed long)that;
		return *this;
	}

	// The not-so-simple case: break it into pieces and initialize
	// piece-by-piece.
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
BigInt::operator=(unsigned long long that)
{
	// The simple case: 'that' is in bounds for the GMP
	// assignment operator.
	if ((sizeof(long) == sizeof(long long))
	 || in_bounds_for<unsigned long>(that)) {
		*this = (unsigned long)that;
		return *this;
	}

	// The not-so-simple case: break it into pieces and initialize
	// piece-by-piece.
	unsigned long vlo = that;
	unsigned long vhi = (that >> (BITS_PER_LONG-1)) >> 1;
	*this = vhi;
	*this <<= BITS_PER_LONG;
	*this += vlo;

	return *this;
}

BigInt &
BigInt::operator=(const ::util::BitBuffer &bitbuf)
{
	// Sadly, mpz_import() seems to not work.
	*this = 0;
	for (std::size_t i = bitbuf.size_bytes(); i > 0; i--) {
		*this <<= CHAR_BIT;
		*this += bitbuf.byte_at(i-1);
	}
	return *this;
}

long long
BigInt::as_int() const
{
	// The simple case: 'that' is in bounds for the GMP
	// get_si() function.
	if (sizeof(long) == sizeof(long long)
	 || in_bounds_for<signed long>(*this)) {
		return mpz_class::get_si();
	}

	// The not-so-simple case: break it into pieces and extract it
	// piece-by-piece.
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

	return result * multiplier;
}

unsigned long long
BigInt::as_uint() const
{
	// The simple case: 'that' is in bounds for the GMP
	// get_ui() function.
	if ((sizeof(long) == sizeof(long long))
	 || in_bounds_for<unsigned long>(*this)) {
		return mpz_class::get_ui();
	}

	// The not-so-simple case: break it into pieces and extract it
	// piece-by-piece.
	BigInt myval(*this);
	unsigned long rlo = myval.mpz_class::get_ui();
	myval >>= BITS_PER_LONG;
	unsigned long long rhi = myval.mpz_class::get_ui();
	unsigned long long result =
	    (((rhi << (BITS_PER_LONG-1))<<1) | rlo);

	return result;
}

::util::BitBuffer
BigInt::to_bitbuffer(std::size_t bits) const
{
	ASSERT(*this >= 0);

	// Sadly, mpz_export() seems to not work.
	std::size_t bytes = 0;
	if (bits) {
		bytes = (bits + (CHAR_BIT-1)) / CHAR_BIT;
	} else {
		BigInt tmp(*this);
		while (tmp != 0) {
			bytes++;
			tmp >>= CHAR_BIT;
		}
	}
	::util::BitBuffer bitbuf(bytes * CHAR_BIT);

	BigInt myval(*this);
	for (std::size_t i = 0; i < bytes; i++) {
		bitbuf.byte_at(i) = myval.as_uint() & 0xff;
		myval >>= CHAR_BIT;
	}

	return bitbuf;
}

std::string
BigInt::to_dec_string() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

std::string
BigInt::to_hex_string() const
{
	std::ostringstream oss;
	oss << "0x" << std::hex << *this;
	return oss.str();
}

std::string
BigInt::to_oct_string() const
{
	std::ostringstream oss;
	oss << "0" << std::oct << *this;
	return oss.str();
}

} // namespace bignum
