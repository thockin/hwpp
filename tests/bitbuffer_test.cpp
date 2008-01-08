#include "pp.h"
#include "pp_test.h"
#include "bitbuffer.h"
using namespace std;

int
main(void)
{
	int ret = 0;

	// test ctor
	{
		bitbuffer bb;
		ret += PP_TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::bitbuffer()");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::bitbuffer()");
	}
	{
		bitbuffer bb(0);
		ret += PP_TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::bitbuffer(0)");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::bitbuffer(0)");
	}
	{
		bitbuffer bb(1);
		ret += PP_TEST_ASSERT(bb.size_bits() == 1,
			"bitbuffer::bitbuffer(int)");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 1,
			"bitbuffer::bitbuffer(int)");
	}
	{
		bitbuffer bb(9);
		ret += PP_TEST_ASSERT(bb.size_bits() == 9,
			"bitbuffer::bitbuffer(int)");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 2,
			"bitbuffer::bitbuffer(int)");
	}
	{
		bitbuffer bb(65);
		ret += PP_TEST_ASSERT(bb.size_bits() == 65,
			"bitbuffer::bitbuffer(int)");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 9,
			"bitbuffer::bitbuffer(int)");
	}

	// test reset()
	{
		bitbuffer bb(1);
		bb.reset();
		ret += PP_TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::reset()");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::reset()");
	}
	{
		bitbuffer bb(1);
		bb.reset(0);
		ret += PP_TEST_ASSERT(bb.size_bits() == 0,
			"bitbuffer::reset(int)");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 0,
			"bitbuffer::reset(int)");
	}
	{
		bitbuffer bb(1);
		bb.reset(2);
		ret += PP_TEST_ASSERT(bb.size_bits() == 2,
			"bitbuffer::reset(int)");
		ret += PP_TEST_ASSERT(bb.size_bytes() == 1,
			"bitbuffer::reset(int)");
	}

	// test get()
	{
		bitbuffer bb(1024 * CHAR_BIT);
		ret += PP_TEST_ASSERT(bb.get()[0] == 0,
			"bitbuffer::get(int)");
		memset(bb.get(), 0x5a, bb.size_bytes());
		ret += PP_TEST_ASSERT(bb.get()[0] == 0x5a,
			"bitbuffer::get(int)");
	}

	// test byte_at()
	{
		bitbuffer bb(2);
		bb.get()[0] = 0x12;
		bb.get()[1] = 0x34;
		ret += PP_TEST_ASSERT(bb.byte_at(0) == 0x12,
			"bitbuffer::byte_at(int)");
		ret += PP_TEST_ASSERT(bb.byte_at(1) == 0x34,
			"bitbuffer::byte_at(int)");
		bb.byte_at(0) = 0x56;
		ret += PP_TEST_ASSERT(bb.byte_at(0) == 0x56,
			"bitbuffer::byte_at(int)");
	}

	// test IO
	{
		bitbuffer bb(0);
		ret += PP_TEST_ASSERT(to_string(bb) == "0x0",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
	{
		bitbuffer bb(1);
		ret += PP_TEST_ASSERT(to_string(bb) == "0x0",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
	{
		bitbuffer bb(8);
		bb.get()[0] = 0x5a;
		ret += PP_TEST_ASSERT(to_string(bb) == "0x5a",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}
	{
		bitbuffer bb(65);
		bb.get()[0] = 0x5a;
		bb.get()[1] = 0x96;
		ret += PP_TEST_ASSERT(to_string(bb) == "0x965a",
			"bitbuffer::operator<<(ostream, bitbuffer)");
	}

	return ret;
}
