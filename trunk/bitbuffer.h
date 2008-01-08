/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_BITBUFFER_H__
#define PP_BITBUFFER_H__

#include <iostream>
#include <boost/format.hpp>
#include <boost/shared_array.hpp>
#include <stdint.h>

/*
 * This is a simple buffer of a fixed bit width.  It is a suitable place
 * to store data for lower-level things like read() and write().  Once
 * initialized, it can not be resized except through a call to reset().
 *
 * Bitbuffers are always stored in little endian form as an array of
 * 'uint8_t'.  Bits beyond the specified bit width are initialized to 0,
 * but could be changed by the caller.
 */
class bitbuffer
{
    private:
	typedef boost::shared_array<uint8_t> uint8_array;
	uint8_array m_array;
	unsigned m_bits;
	unsigned m_bytes;

    public:
	bitbuffer(unsigned bits = 0)
	{
		reset(bits);
	}

	void
	reset(unsigned bits = 0)
	{
		m_bits = bits;
		m_bytes = (m_bits+(CHAR_BIT-1))/CHAR_BIT;

		uint8_array tmp(NULL);
		if (bits) {
			tmp = uint8_array(new uint8_t[m_bytes]);
			memset(tmp.get(), 0, m_bytes);
		}
		m_array = tmp;
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
	int i = bitbuf.size_bytes()-1;
	if (i >= 0) {
		while (i > 0 && bitbuf.get()[i] == 0) {
			i--;
		}
		o << boost::format("%x") %(int)bitbuf.get()[i];
		i--;
		for (i=i; i >= 0; i--) {
			o << boost::format("%02x") %(int)bitbuf.get()[i];
		}
	} else {
		o << "0";
	}

	return o;
}

#endif // PP_BITBUFFER_H__
