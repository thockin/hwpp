#include "util/bit_buffer.h"
#include "util/test.h"

#include <string.h>

namespace util {

TEST(test_ctors)
{
	{
		BitBuffer bb;
		TEST_ASSERT(bb.size_bits() == 0,
			"BitBuffer::BitBuffer()");
		TEST_ASSERT(bb.size_bytes() == 0,
			"BitBuffer::BitBuffer()");
	}
	{
		BitBuffer bb(0);
		TEST_ASSERT(bb.size_bits() == 0,
			"BitBuffer::BitBuffer(0)");
		TEST_ASSERT(bb.size_bytes() == 0,
			"BitBuffer::BitBuffer(0)");
	}
	{
		BitBuffer bb(1);
		TEST_ASSERT(bb.size_bits() == 1,
			"BitBuffer::BitBuffer(int)");
		TEST_ASSERT(bb.size_bytes() == 1,
			"BitBuffer::BitBuffer(int)");
	}
	{
		BitBuffer bb(9);
		TEST_ASSERT(bb.size_bits() == 9,
			"BitBuffer::BitBuffer(int)");
		TEST_ASSERT(bb.size_bytes() == 2,
			"BitBuffer::BitBuffer(int)");
	}
	{
		BitBuffer bb(65);
		TEST_ASSERT(bb.size_bits() == 65,
			"BitBuffer::BitBuffer(int)");
		TEST_ASSERT(bb.size_bytes() == 9,
			"BitBuffer::BitBuffer(int)");
	}
}

TEST(test_reset)
{
	{
		BitBuffer bb(1);
		bb.reset();
		TEST_ASSERT(bb.size_bits() == 0,
			"BitBuffer::reset()");
		TEST_ASSERT(bb.size_bytes() == 0,
			"BitBuffer::reset()");
	}
	{
		BitBuffer bb(1);
		bb.reset(0);
		TEST_ASSERT(bb.size_bits() == 0,
			"BitBuffer::reset(int)");
		TEST_ASSERT(bb.size_bytes() == 0,
			"BitBuffer::reset(int)");
	}
	{
		BitBuffer bb(1);
		bb.reset(2);
		TEST_ASSERT(bb.size_bits() == 2,
			"BitBuffer::reset(int)");
		TEST_ASSERT(bb.size_bytes() == 1,
			"BitBuffer::reset(int)");
	}
}

TEST(test_get)
{
	// test get()
	{
		BitBuffer bb(1024);
		TEST_ASSERT(bb.get()[0] == 0,
			"BitBuffer::get(int)");
		memset(bb.get(), 0x5a, bb.size_bytes());
		TEST_ASSERT(bb.get()[0] == 0x5a,
			"BitBuffer::get(int)");
	}
}

TEST(test_fills)
{
	{
		BitBuffer bb(8, 0x5a);
		TEST_ASSERT(bb.size_bits() == 8,
			"BitBuffer::BitBuffer(int, unsigned char)");
		TEST_ASSERT(bb.size_bytes() == 1,
			"BitBuffer::BitBuffer(int, unsigned char)");
		TEST_ASSERT(bb.get()[0] == 0x5a,
			"BitBuffer::BitBuffer(int, unsigned char)");
		bb.fill(0x96);
		TEST_ASSERT(bb.get()[0] == 0x96,
			"BitBuffer::fill(unsigned char)");
		bb.reset(16, 0x81);
		TEST_ASSERT(bb.get()[0] == 0x81,
			"BitBuffer::reset(int, unsigned char)");
		TEST_ASSERT(bb.get()[1] == 0x81,
			"BitBuffer::reset(int, unsigned char)");
	}
}

TEST(test_byte_at)
{
	{
		BitBuffer bb(2);
		bb.get()[0] = 0x12;
		bb.get()[1] = 0x34;
		TEST_ASSERT(bb.byte_at(0) == 0x12,
			"BitBuffer::byte_at(int)");
		TEST_ASSERT(bb.byte_at(1) == 0x34,
			"BitBuffer::byte_at(int)");
		bb.byte_at(0) = 0x56;
		TEST_ASSERT(bb.byte_at(0) == 0x56,
			"BitBuffer::byte_at(int)");
	}
}

TEST(test_io)
{
	{
		BitBuffer bb(0);
		TEST_ASSERT(bb.to_string() == "0x0",
			"BitBuffer::operator<<(ostream, BitBuffer)");
	}
	{
		BitBuffer bb(1);
		TEST_ASSERT(bb.to_string() == "0x0",
			"BitBuffer::operator<<(ostream, BitBuffer)");
	}
	{
		BitBuffer bb(8);
		bb.get()[0] = 0x5a;
		TEST_ASSERT(bb.to_string() == "0x5a",
			"BitBuffer::operator<<(ostream, BitBuffer)");
	}
	{
		BitBuffer bb(65);
		bb.get()[0] = 0x5a;
		bb.get()[1] = 0x96;
		TEST_ASSERT(bb.to_string() == "0x965a",
			"BitBuffer::operator<<(ostream, BitBuffer)");
	}
}

} // namespace util
