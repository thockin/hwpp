// A simple way to hold bits.
// Copyright (c) Tim Hockin, 2008

#ifndef HWPP_UTIL_BIT_BUFFER_H__
#define HWPP_UTIL_BIT_BUFFER_H__

#include <stdint.h>
#include <ostream>
#include <string>
#include <vector>

namespace util {

// This is a simple buffer of a fixed bit width.  It is a suitable place
// to store data for lower-level things like read() and write().  Once
// initialized, it can not be resized except through a call to reset().
//
// BitBuffers are always stored in little endian form as a vector of
// 'uint8_t'.  Bits beyond the specified bit width, up to the next byte
// alignment, are ignored.
class BitBuffer
{
    private:
	std::vector<uint8_t> m_data;
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
	BitBuffer(std::size_t bits, const uint8_t *data)
	{
		reset_from_data(bits, data);
	}

	// Initialize (or re-initialize) the buffer.
	void
	reset()
	{
		reset(0, 0);
	}
	// Initialize (or re-initialize) the buffer.
	void
	reset(std::size_t bits)
	{
		reset(bits, 0);
	}
	// Initialize (or re-initialize) the buffer from a pattern.
	void
	reset(std::size_t bits, uint8_t pattern);

	// Initialize (or re-initialize) the buffer from a pointer to data.
	void
	reset_from_data(std::size_t bits, const uint8_t *data);

	// Fill the buffer with a pattern.
	void
	fill(uint8_t pattern);
	void
	fill_from_data(const uint8_t *data);

	// Get a pointer to the raw data array.
	uint8_t *
	get()
	{
		const BitBuffer *const_this = this;
		return const_cast<uint8_t*>(const_this->get());
	}
	const uint8_t *
	get() const
	{
		if (m_bytes > 0) {
			return &m_data.front();
		}
		return NULL;
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
		const BitBuffer *const_this = this;
		return const_cast<uint8_t&>(const_this->byte_at(index));
	}
	const uint8_t &
	byte_at(int index) const
	{
		return m_data[index];
	}

	std::string
	to_string() const;
};

// stream output
std::ostream &
operator<<(std::ostream &o, const BitBuffer &bitbuf);

} // namespace util

#endif // HWPP_UTIL_BIT_BUFFER_H__
