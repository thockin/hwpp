#include "bit_buffer.h"
#include "pp_test.h"

TEST(test_ctors)
{
	{
		bitbuffer bb;
		TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::bitbuffer()");
		TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::bitbuffer()");
	}
	{
		bitbuffer bb(0);
		TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::bitbuffer(0)");
		TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::bitbuffer(0)");
	}
	{
		bitbuffer bb(1);
		TEST_ASSERT(bb.size_bits() == 1,
			"bitbuffer::bitbuffer(int)");
		TEST_ASSERT(bb.size_bytes() == 1,
			"bitbuffer::bitbuffer(int)");
	}
	{
		bitbuffer bb(9);
		TEST_ASSERT(bb.size_bits() == 9,
			"bitbuffer::bitbuffer(int)");
		TEST_ASSERT(bb.size_bytes() == 2,
			"bitbuffer::bitbuffer(int)");
	}
	{
		bitbuffer bb(65);
		TEST_ASSERT(bb.size_bits() == 65,
			"bitbuffer::bitbuffer(int)");
		TEST_ASSERT(bb.size_bytes() == 9,
			"bitbuffer::bitbuffer(int)");
	}
}

TEST(test_reset)
{
	{
		bitbuffer bb(1);
		bb.reset();
		TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::reset()");
		TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::reset()");
	}
	{
		bitbuffer bb(1);
		bb.reset(0);
		TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::reset(int)");
		TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::reset(int)");
	}
	{
		bitbuffer bb(1);
		bb.reset(2);
		TEST_ASSERT(bb.size_bits() == 2,
			"bitbuffer::reset(int)");
		TEST_ASSERT(bb.size_bytes() == 1,
			"bitbuffer::reset(int)");
	}
}

TEST(test_get)
{
	// test get()
	{
		bitbuffer bb(1024 * CHAR_BIT);
		TEST_ASSERT(bb.get()[0] == 0,
			"bitbuffer::get(int)");
		memset(bb.get(), 0x5a, bb.size_bytes());
		TEST_ASSERT(bb.get()[0] == 0x5a,
			"bitbuffer::get(int)");
	}
}

TEST(test_fills)
{
	{
		bitbuffer bb(8, 0x5a);
		TEST_ASSERT(bb.size_bits() == 8,
			"bitbuffer::bitbuffer(int, unsigned char)");
		TEST_ASSERT(bb.size_bytes() == 1,
			"bitbuffer::bitbuffer(int, unsigned char)");
		TEST_ASSERT(bb.get()[0] == 0x5a,
			"bitbuffer::bitbuffer(int, unsigned char)");
		bb.fill(0x96);
		TEST_ASSERT(bb.get()[0] == 0x96,
			"bitbuffer::fill(unsigned char)");
		bb.reset(16, 0x81);
		TEST_ASSERT(bb.get()[0] == 0x81,
			"bitbuffer::reset(int, unsigned char)");
		TEST_ASSERT(bb.get()[1] == 0x81,
			"bitbuffer::reset(int, unsigned char)");
	}
}

TEST(test_byte_at)
{
	{
		bitbuffer bb(2);
		bb.get()[0] = 0x12;
		bb.get()[1] = 0x34;
		TEST_ASSERT(bb.byte_at(0) == 0x12,
			"bitbuffer::byte_at(int)");
		TEST_ASSERT(bb.byte_at(1) == 0x34,
			"bitbuffer::byte_at(int)");
		bb.byte_at(0) = 0x56;
		TEST_ASSERT(bb.byte_at(0) == 0x56,
			"bitbuffer::byte_at(int)");
	}
}

TEST(test_io)
{
	{
		bitbuffer bb(0);
		TEST_ASSERT(bb.to_string() == "0x0",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
	{
		bitbuffer bb(1);
		TEST_ASSERT(bb.to_string() == "0x0",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
	{
		bitbuffer bb(8);
		bb.get()[0] = 0x5a;
		TEST_ASSERT(bb.to_string() == "0x5a",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
	{
		bitbuffer bb(65);
		bb.get()[0] = 0x5a;
		bb.get()[1] = 0x96;
		TEST_ASSERT(bb.to_string() == "0x965a",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
}
