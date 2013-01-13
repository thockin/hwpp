// Copyright (c) Tim Hockin, 2008
#ifndef HWPP_UTIL_BIT_BUFFER_H__
#define HWPP_UTIL_BIT_BUFFER_H__

#include <ostream>
#include <sstream>
#include <boost/format.hpp>
#include <boost/shared_array.hpp>
#include <limits.h>
#include <stdint.h>
#include <string.h>

namespace util {

// This is a simple buffer of a fixed bit width.  It is a suitable place
// to store data for lower-level things like read() and write().  Once
// initialized, it can not be resized except through a call to reset().
//
// Bitbuffers are always stored in little endian form as an array of
// 'uint8_t'.  Bits beyond the specified bit width are initialized to 0,
// but could be changed by the caller.
class BitBuffer
{
    private:
	typedef boost::shared_array<uint8_t> Uint8Array;
	Uint8Array m_array;
	std::size_t m_bits;
	std::size_t m_bytes;

    public:
	BitBuffer()
	{
		reset();
	}
	BitBuffer(std::size_t bits)
	{
		reset(bits);
	}
	BitBuffer(std::size_t bits, uint8_t pattern)
	{
		reset(bits, pattern);
	}
	BitBuffer(unsigned long bits, const uint8_t *data)
	{
		reset_from_data(bits, data);
	}

	// Initialize (or re-initialize) the buffer.
	void
	reset()
	{
		reset(0, 0);
	}
	// Initialize (or re-initialize) the fixed size buffer.
	void
	reset(std::size_t bits)
	{
		reset(bits, 0);
	}
	// Initialize (or re-initialize) the  fixed size buffer from a pattern.
	void
	reset(std::size_t bits, uint8_t pattern)
	{
		m_bits = bits;
		m_bytes = (m_bits+(CHAR_BIT-1))/CHAR_BIT;

		Uint8Array tmp(NULL);
		if (bits) {
			tmp = Uint8Array(new uint8_t[m_bytes]);
			memset(tmp.get(), pattern, m_bytes);
		}
		m_array = tmp;
	}

	// Initialize (or re-initialize) the buffer from a pointer to data.
	void
	reset_from_data(std::size_t bits, const uint8_t *data)
	{
		m_bits = bits;
		m_bytes = (m_bits+(CHAR_BIT-1))/CHAR_BIT;

		Uint8Array tmp(NULL);
		if (bits) {
			tmp = Uint8Array(new uint8_t[m_bytes]);
			memcpy(tmp.get(), data, m_bytes);
		}
		m_array = tmp;
	}

	// Fill the buffer with a pattern.
	void
	fill(uint8_t pattern)
	{
		memset(m_array.get(), pattern, m_bytes);
	}

	// Get a pointer to the raw data.
	uint8_t *
	get()
	{
		return m_array.get();
	}
	const uint8_t *
	get() const
	{
		return m_array.get();
	}

	// Get the size of the buffer.
	std::size_t
	size_bits() const
	{
		return m_bits;
	}
	std::size_t
	size_bytes() const
	{
		return m_bytes;
	}

	// Access the raw data.
	uint8_t &
	byte_at(int index)
	{
		return m_array[index];
	}
	const uint8_t &
	byte_at(int index) const
	{
		return m_array[index];
	}

	std::string
	to_string() const;
};

// stream output
inline std::ostream &
operator<<(std::ostream& o, const BitBuffer &bitbuf)
{
	o << "0x";
	// This is signed on purpose.
	signed long i = bitbuf.size_bytes()-1;
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

inline std::string
BitBuffer::to_string() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

} // namespace util

#endif // HWPP_UTIL_BIT_BUFFER_H__
