// A simple way to hold bits.
// Copyright (c) Tim Hockin, 2008

#include "util/bit_buffer.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include "util/assert.h"

namespace util {

void
BitBuffer::reset(std::size_t bits, uint8_t pattern)
{
	m_bits = bits;
	m_bytes = (m_bits+(CHAR_BIT-1))/CHAR_BIT;
	if (m_bytes > 0) {
		m_data.resize(m_bytes);
		fill(pattern);
	} else {
		m_data.clear();
	}
}

void
BitBuffer::reset_from_data(std::size_t bits, const uint8_t *data)
{
	m_bits = bits;
	m_bytes = (m_bits+(CHAR_BIT-1))/CHAR_BIT;
	if (m_bytes) {
		ASSERT(data != NULL);
		m_data.resize(m_bytes);
		fill_from_data(data);
	} else {
		m_data.clear();
	}
}

void
BitBuffer::fill(uint8_t pattern)
{
	ASSERT(m_bytes > 0);
	memset(get(), pattern, m_bytes);
}

void
BitBuffer::fill_from_data(const uint8_t *data)
{
	ASSERT(data != NULL);
	ASSERT(m_bytes > 0);
	memcpy(get(), data, m_bytes);
}

std::string
BitBuffer::to_string() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

std::ostream &
operator<<(std::ostream &o, const BitBuffer &bitbuf)
{
	o << "0x";
	// This is signed on purpose.
	long i = bitbuf.size_bytes()-1;
	if (i >= 0) {
		// Skip leading zeros.
		while (i > 0 && bitbuf.get()[i] == 0) {
			i--;
		}
		// Print the most-significant non-zero byte.
		o << boost::format("%x") %(int)bitbuf.get()[i];
		i--;
		// Print the rest.
		for (i=i; i >= 0; i--) {
			o << boost::format("%02x") %(int)bitbuf.get()[i];
		}
	} else {
		o << "0";
	}

	return o;
}

} // namespace util
