// Copyright (c) Tim Hockin, 2008
#ifndef PP_BITBUFFER_H__
#define PP_BITBUFFER_H__

#include <ostream>
#include <boost/format.hpp>
#include <boost/shared_array.hpp>
#include <stdint.h>

//
// This is a simple buffer of a fixed bit width.  It is a suitable place
// to store data for lower-level things like read() and write().  Once
// initialized, it can not be resized except through a call to reset().
//
// Bitbuffers are always stored in little endian form as an array of
// 'uint8_t'.  Bits beyond the specified bit width are initialized to 0,
// but could be changed by the caller.
//
class bitbuffer
{
    private:
	typedef boost::shared_array<uint8_t> uint8_array;
	uint8_array m_array;
	std::size_t m_bits;
	std::size_t m_bytes;

    public:
	bitbuffer(std::size_t bits = 0, uint8_t pattern = 0)
	{
		reset(bits, pattern);
	}

	void
	reset(std::size_t bits = 0, uint8_t pattern = 0)
	{
		m_bits = bits;
		m_bytes = (m_bits+(CHAR_BIT-1))/CHAR_BIT;

		uint8_array tmp(NULL);
		if (bits) {
			tmp = uint8_array(new uint8_t[m_bytes]);
			memset(tmp.get(), pattern, m_bytes);
		}
		m_array = tmp;
	}

	void
	fill(uint8_t pattern)
	{
		memset(m_array.get(), pattern, m_bytes);
	}

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
};

// stream output
inline std::ostream &
operator<<(std::ostream& o, const bitbuffer &bitbuf)
{
	o << "0x";
	// this is signed on purpose
	signed long i = bitbuf.size_bytes()-1;
	if (i >= 0) {
		// skip leading zeros
		while (i > 0 && bitbuf.get()[i] == 0) {
			i--;
		}
		// print the most-significant non-zero byte
		o << boost::format("%x") %(int)bitbuf.get()[i];
		i--;
		// print the rest
		for (i=i; i >= 0; i--) {
			o << boost::format("%02x") %(int)bitbuf.get()[i];
		}
	} else {
		o << "0";
	}

	return o;
}

#endif // PP_BITBUFFER_H__
