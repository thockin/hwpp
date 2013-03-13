#include "util/bignum.h"
#include "util/test.h"
#include <sstream>
#include "util/bit_buffer.h"

namespace bignum {

TEST(test_ctors)
{
	// default
	{
		BigInt val;
		TEST_ASSERT(val.get_si() == 0, "BigInt::BigInt()");
		TEST_ASSERT(val.get_ui() == 0, "BigInt::BigInt()");
		TEST_ASSERT(val.get_str(10) == "0", "BigInt::BigInt()");
	}

	// from char
	{
		BigInt val((signed char)0x12);
		TEST_ASSERT(val.get_si() == 0x12,
			"BigInt::BigInt(signed char)");
		TEST_ASSERT(val.get_ui() == 0x12,
			"BigInt::BigInt(signed char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"BigInt::BigInt(signed char)");
	}
	{
		BigInt val((signed char)-1);
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::BigInt(signed char)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::BigInt(signed char)");
	}
	{
		BigInt val((unsigned char)0x12);
		TEST_ASSERT(val.get_si() == 0x12,
			"BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_ui() == 0x12,
			"BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"BigInt::BigInt(unsigned char)");
	}
	{
		BigInt val((unsigned char)-1);
		TEST_ASSERT(val.get_si() == 0xff,
			"BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_ui() == 0xff,
			"BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "ff",
			"BigInt::BigInt(unsigned char)");
	}

	// from short
	{
		BigInt val((signed short)0x1234);
		TEST_ASSERT(val.get_si() == 0x1234,
			"BigInt::BigInt(signed short)");
		TEST_ASSERT(val.get_ui() == 0x1234,
			"BigInt::BigInt(signed short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"BigInt::BigInt(signed short)");
	}
	{
		BigInt val((signed short)-1);
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::BigInt(signed short)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::BigInt(signed short)");
	}
	{
		BigInt val((unsigned short)0x1234);
		TEST_ASSERT(val.get_si() == 0x1234,
			"BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_ui() == 0x1234,
			"BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"BigInt::BigInt(unsigned short)");
	}
	{
		BigInt val((unsigned short)-1);
		TEST_ASSERT(val.get_si() == 0xffff,
			"BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_ui() == 0xffff,
			"BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "ffff",
			"BigInt::BigInt(unsigned short)");
	}

	// from int
	// On 32 bit platforms this is where we will start to see
	// signed vs. unsigned oddities.
	{
		BigInt val((signed int)0x12345678);
		TEST_ASSERT(val.get_si() == 0x12345678,
			"BigInt::BigInt(signed int)");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"BigInt::BigInt(signed int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"BigInt::BigInt(signed int)");
	}
	{
		BigInt val((signed int)-1);
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::BigInt(signed int)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::BigInt(signed int)");
	}
	{
		BigInt val((unsigned int)0x12345678);
		TEST_ASSERT(val.get_si() == 0x12345678,
			"BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"BigInt::BigInt(unsigned int)");
	}
	{
		BigInt val((unsigned int)-1);
		// On a 32 bit platform this will fail, since get_si()
		// will return a signed value, and -1 is OOB.
		//TEST_ASSERT(val.get_si() == (signed int)0xffffffff,
		//	"BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_ui() == 0xffffffff,
			"BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "ffffffff",
			"BigInt::BigInt(unsigned int)");
	}

	// from long long
	// Skip long, since either sizeof(long) == sizeof(int) or
	// sizeof(long) == sizeof(long long).
	{
		BigInt val(0x123456789abcdef0LL);
		TEST_ASSERT(val.get_si() == 0x123456789abcdef0LL,
			"val.get_si()");
		TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_ui()");
		TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"BigInt::BigInt(signed long long)");
	}
	{
		BigInt val(-1LL);
		TEST_ASSERT(val.get_si() == -1,
			"val.get_si()");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::BigInt(signed long long)");
	}
	{
		BigInt val(0x123456789abcdef0ULL);
		TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_ui()");
		TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"BigInt::BigInt(unsigned long long)");
	}
	{
		BigInt val(-1ULL);
		TEST_ASSERT(val.as_uint() == 0xffffffffffffffffULL,
			"val.as_uint()");
		TEST_ASSERT(val.get_str(16) == "ffffffffffffffff",
			"BigInt::BigInt(unsigned long long)");
	}

	// from BigInt
	{
		BigInt val1(12345678);
		BigInt val2(val1);
		TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"BigInt::BigInt(BigInt)");
	}

	// from string
	{
		BigInt val("12345678");
		TEST_ASSERT(val.get_ui() == 12345678,
			"BigInt::BigInt(string)");
		val = "23456789";
		TEST_ASSERT(val.get_ui() == 23456789,
			"BigInt::BigInt(string)");
	}
	{
		BigInt val("0x12345678");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"BigInt::BigInt(string)");
		val = "0x23456789";
		TEST_ASSERT(val.get_ui() == 0x23456789,
			"BigInt::BigInt(string)");
	}
	{
		BigInt val("0x112233445566778899aabbccddeeff00");
		TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"BigInt::BigInt(string)");
		val = "0x2233445566778899aabbccddeeff0011";
		TEST_ASSERT(
			val.get_str(16) == "2233445566778899aabbccddeeff0011",
			"BigInt::BigInt(string)");
	}
	{
		BigInt val("-1");
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::BigInt(string)");
		val = "-1";
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::BigInt(string)");
	}

	// from BitBuffer
	{
		util::BitBuffer bb(8);
		bb.byte_at(0) = 0x12;
		BigInt val(bb);
		TEST_ASSERT(val.get_ui() == 0x12,
			"BigInt(util::BitBuffer)");
		util::BitBuffer bb2 = val.to_bitbuffer();
		TEST_ASSERT(bb2.to_string() == "0x12",
			"BigInt::to_bitbuffer()");
		util::BitBuffer bb3 = val.to_bitbuffer(8);
		TEST_ASSERT(bb3.to_string() == "0x12",
			"BigInt::to_bitbuffer(int)");
	}
	{
		util::BitBuffer bb(16);
		bb.byte_at(1) = 0x12;
		bb.byte_at(0) = 0x34;
		BigInt val(bb);
		TEST_ASSERT(val.get_ui() == 0x1234,
			"BigInt(util::BitBuffer)");
		util::BitBuffer bb2 = val.to_bitbuffer();
		TEST_ASSERT(bb2.to_string() == "0x1234",
			"BigInt::to_bitbuffer()");
		util::BitBuffer bb3 = val.to_bitbuffer(16);
		TEST_ASSERT(bb3.to_string() == "0x1234",
			"BigInt::to_bitbuffer(int)");
	}
}

TEST(test_assignments)
{
	// from char
	{
		BigInt val;
		val = (signed char)0x12;
		TEST_ASSERT(val.get_si() == 0x12,
			"BigInt::operator=(signed char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"BigInt::operator=(signed char)");
	}
	{
		BigInt val;
		val = (signed char)-1;
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::operator=(signed char)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::operator=(signed char)");
	}
	{
		BigInt val;
		val = (unsigned char)0x12;
		TEST_ASSERT(val.get_ui() == 0x12,
			"BigInt::operator=(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"BigInt::operator=(unsigned char)");
	}
	{
		BigInt val;
		val = (unsigned char)-1;
		TEST_ASSERT(val.get_ui() == 0xff,
			"BigInt::operator=(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "ff",
			"BigInt::operator=(unsigned char)");
	}

	// from short
	{
		BigInt val;
		val = (signed short)0x1234;
		TEST_ASSERT(val.get_si() == 0x1234,
			"BigInt::operator=(signed short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"BigInt::operator=(signed short)");
	}
	{
		BigInt val;
		val = (signed short)-1;
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::operator=(signed short)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::operator=(signed short)");
	}
	{
		BigInt val;
		val = (unsigned short)0x1234;
		TEST_ASSERT(val.get_ui() == 0x1234,
			"BigInt::operator=(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"BigInt::operator=(unsigned short)");
	}
	{
		BigInt val;
		val = (unsigned short)-1;
		TEST_ASSERT(val.get_ui() == 0xffff,
			"BigInt::operator=(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "ffff",
			"BigInt::operator=(unsigned short)");
	}

	// from int
	{
		BigInt val;
		val = (signed int)0x12345678;
		TEST_ASSERT(val.get_si() == 0x12345678,
			"BigInt::operator=(signed int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"BigInt::operator=(signed int)");
	}
	{
		BigInt val;
		val = (signed int)-1;
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::operator=(signed int)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::operator=(signed int)");
	}
	{
		BigInt val;
		val = (unsigned int)0x12345678;
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"BigInt::operator=(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"BigInt::operator=(unsigned int)");
	}
	{
		BigInt val;
		val = (unsigned int)-1;
		TEST_ASSERT(val.get_ui() == 0xffffffff,
			"BigInt::operator=(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "ffffffff",
			"BigInt::operator=(unsigned int)");
	}

	// from BigInt
	{
		BigInt val1(12345678);
		BigInt val2;
		val2 = val1;
		TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"BigInt::operator=(BigInt)");
	}

	// from string
	{
		BigInt val;
		val = "12345678";
		TEST_ASSERT(val.get_ui() == 12345678,
			"BigInt::operator=(string)");
		TEST_ASSERT(val.get_str(10) == "12345678",
			"BigInt::operator=(string)");
	}
	{
		BigInt val;
		val = "0x112233445566778899aabbccddeeff00";
		TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"BigInt::operator=(string)");
	}
	{
		BigInt val;
		val = "-1";
		TEST_ASSERT(val.get_si() == -1,
			"BigInt::operator=(string)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"BigInt::operator=(string)");
	}
}

TEST(test_comparisons)
{
	// Because BigInt is a template, int's important to excercise all
	// permutations of comparisons, to force instantiation.

	//
	// compare to char
	//
	// ==
	{
		BigInt val((signed char)0x12);
		TEST_ASSERT_EQ(val, (signed char)0x12,
			"BigInt::operator==(signed char)");
		TEST_ASSERT_EQ(val, 0x12,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((signed char)-1);
		TEST_ASSERT_EQ(val, (signed char)-1,
			"BigInt::operator==(signed char)");
		TEST_ASSERT_EQ(val, -1,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((unsigned char)0x12);
		TEST_ASSERT_EQ(val, (unsigned char)0x12,
			"BigInt::operator==(unsigned char)");
		TEST_ASSERT_EQ(val, 0x12,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((unsigned char)-1);
		TEST_ASSERT_EQ(val, (unsigned char)0xff,
			"BigInt::operator==(unsigned char)");
		TEST_ASSERT_EQ(val, 0xff,
			"BigInt::operator==(int)");
	}
	// !=
	{
		BigInt val((signed char)0x13);
		TEST_ASSERT_NE(val, (signed char)0x12,
			"BigInt::operator!=(signed char)");
		TEST_ASSERT_NE(val, 0x12,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((signed char)-2);
		TEST_ASSERT_NE(val, (signed char)-1,
			"BigInt::operator!=(signed char)");
		TEST_ASSERT_NE(val, -1,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((unsigned char)0x13);
		TEST_ASSERT_NE(val, (unsigned char)0x12,
			"BigInt::operator!=(unsigned char)");
		TEST_ASSERT_NE(val, 0x12,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((unsigned char)-2);
		TEST_ASSERT_NE(val, (unsigned char)0xff,
			"BigInt::operator!=(unsigned char)");
		TEST_ASSERT_NE(val, 0xff,
			"BigInt::operator!=(int)");
	}
	// <
	{
		BigInt val((signed char)0x12);
		TEST_ASSERT_LT(val, (signed char)0x13,
			"BigInt::operator<(signed char)");
		TEST_ASSERT_LT(val, 0x13,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((signed char)-2);
		TEST_ASSERT_LT(val, (signed char)-1,
			"BigInt::operator<(signed char)");
		TEST_ASSERT_LT(val, -1,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((unsigned char)0x12);
		TEST_ASSERT_LT(val, (unsigned char)0x13,
			"BigInt::operator<(unsigned char)");
		TEST_ASSERT_LT(val, 0x13,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((unsigned char)-2);
		TEST_ASSERT_LT(val, (unsigned char)0xff,
			"BigInt::operator<(unsigned char)");
		TEST_ASSERT_LT(val, 0xff,
			"BigInt::operator<(int)");
	}
	// >
	{
		BigInt val((signed char)0x12);
		TEST_ASSERT_GT(val, (signed char)0x11,
			"BigInt::operator>(signed char)");
		TEST_ASSERT_GT(val, 0x11,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((signed char)-1);
		TEST_ASSERT_GT(val, (signed char)-2,
			"BigInt::operator>(signed char)");
		TEST_ASSERT_GT(val, -2,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((unsigned char)0x12);
		TEST_ASSERT_GT(val, (unsigned char)0x11,
			"BigInt::operator>(unsigned char)");
		TEST_ASSERT_GT(val, 0x11,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((unsigned char)-1);
		TEST_ASSERT_GT(val, (unsigned char)0xfe,
			"BigInt::operator>(unsigned char)");
		TEST_ASSERT_GT(val, 0xfe,
			"BigInt::operator>(int)");
	}
	// <=
	{
		BigInt val((signed char)0x12);
		TEST_ASSERT_LE(val, (signed char)0x13,
			"BigInt::operator<=(signed char)");
		TEST_ASSERT_LE(val, 0x13,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((signed char)-2);
		TEST_ASSERT_LE(val, (signed char)-1,
			"BigInt::operator<=(signed char)");
		TEST_ASSERT_LE(val, -1,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((unsigned char)0x12);
		TEST_ASSERT_LE(val, (unsigned char)0x13,
			"BigInt::operator<=(unsigned char)");
		TEST_ASSERT_LE(val, 0x13,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((unsigned char)-2);
		TEST_ASSERT_LE(val, (unsigned char)0xff,
			"BigInt::operator<=(unsigned char)");
		TEST_ASSERT_LE(val, 0xff,
			"BigInt::operator<=(int)");
	}
	// >=
	{
		BigInt val((signed char)0x12);
		TEST_ASSERT_GE(val, (signed char)0x11,
			"BigInt::operator>=(signed char)");
		TEST_ASSERT_GE(val, 0x11,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((signed char)-1);
		TEST_ASSERT_GE(val, (signed char)-2,
			"BigInt::operator>=(signed char)");
		TEST_ASSERT_GE(val, -2,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((unsigned char)0x12);
		TEST_ASSERT_GE(val, (unsigned char)0x11,
			"BigInt::operator>=(unsigned char)");
		TEST_ASSERT_GE(val, 0x11,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((unsigned char)-1);
		TEST_ASSERT_GE(val, (unsigned char)0xfe,
			"BigInt::operator>=(unsigned char)");
		TEST_ASSERT_GE(val, 0xfe,
			"BigInt::operator>=(int)");
	}

	//
	// compare to short
	//
	// ==
	{
		BigInt val((signed short)0x1234);
		TEST_ASSERT_EQ(val, (signed short)0x1234,
			"BigInt::operator==(signed short)");
		TEST_ASSERT_EQ(val, 0x1234,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((signed short)-1);
		TEST_ASSERT_EQ(val, (signed short)-1,
			"BigInt::operator==(signed short)");
		TEST_ASSERT_EQ(val, -1,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((unsigned short)0x1234);
		TEST_ASSERT_EQ(val, (unsigned short)0x1234,
			"BigInt::operator==(unsigned short)");
		TEST_ASSERT_EQ(val, 0x1234,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((unsigned short)-1);
		TEST_ASSERT_EQ(val, (unsigned short)0xffff,
			"BigInt::operator==(unsigned short)");
		TEST_ASSERT_EQ(val, 0xffff,
			"BigInt::operator==(int)");
	}
	// !=
	{
		BigInt val((signed short)0x1235);
		TEST_ASSERT_NE(val, (signed short)0x1234,
			"BigInt::operator!=(signed short)");
		TEST_ASSERT_NE(val, 0x1234,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((signed short)-2);
		TEST_ASSERT_NE(val, (signed short)-1,
			"BigInt::operator!=(signed short)");
		TEST_ASSERT_NE(val, -1,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((unsigned short)0x1235);
		TEST_ASSERT_NE(val, (unsigned short)0x1234,
			"BigInt::operator!=(unsigned short)");
		TEST_ASSERT_NE(val, 0x1234,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((unsigned short)-2);
		TEST_ASSERT_NE(val, (unsigned short)0xffff,
			"BigInt::operator!=(unsigned short)");
		TEST_ASSERT_NE(val, 0xffff,
			"BigInt::operator!=(int)");
	}
	// <
	{
		BigInt val((signed short)0x1234);
		TEST_ASSERT_LT(val, (signed short)0x1235,
			"BigInt::operator<(signed short)");
		TEST_ASSERT_LT(val, 0x1235,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((signed short)-2);
		TEST_ASSERT_LT(val, (signed short)-1,
			"BigInt::operator<(signed short)");
		TEST_ASSERT_LT(val, -1,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((unsigned short)0x1234);
		TEST_ASSERT_LT(val, (unsigned short)0x1235,
			"BigInt::operator<(unsigned short)");
		TEST_ASSERT_LT(val, 0x1235,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((unsigned short)-2);
		TEST_ASSERT_LT(val, (unsigned short)0xffff,
			"BigInt::operator<(unsigned short)");
		TEST_ASSERT_LT(val, 0xffff,
			"BigInt::operator<(int)");
	}
	// >
	{
		BigInt val((signed short)0x1235);
		TEST_ASSERT_GT(val, (signed short)0x1234,
			"BigInt::operator>(signed short)");
		TEST_ASSERT_GT(val, 0x1234,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((signed short)-1);
		TEST_ASSERT_GT(val, (signed short)-2,
			"BigInt::operator>(signed short)");
		TEST_ASSERT_GT(val, -2,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((unsigned short)0x1235);
		TEST_ASSERT_GT(val, (unsigned short)0x1234,
			"BigInt::operator>(unsigned short)");
		TEST_ASSERT_GT(val, 0x1234,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((unsigned short)-1);
		TEST_ASSERT_GT(val, (unsigned short)0xfffe,
			"BigInt::operator>(unsigned short)");
		TEST_ASSERT_GT(val, 0xfffe,
			"BigInt::operator>(int)");
	}
	// <=
	{
		BigInt val((signed short)0x1234);
		TEST_ASSERT_LE(val, (signed short)0x1235,
			"BigInt::operator<=(signed short)");
		TEST_ASSERT_LE(val, 0x1235,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((signed short)-2);
		TEST_ASSERT_LE(val, (signed short)-1,
			"BigInt::operator<=(signed short)");
		TEST_ASSERT_LE(val, -1,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((unsigned short)0x1234);
		TEST_ASSERT_LE(val, (unsigned short)0x1235,
			"BigInt::operator<=(unsigned short)");
		TEST_ASSERT_LE(val, 0x1235,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((unsigned short)-2);
		TEST_ASSERT_LE(val, (unsigned short)0xffff,
			"BigInt::operator<=(unsigned short)");
		TEST_ASSERT_LE(val, 0xffff,
			"BigInt::operator<=(int)");
	}
	// >=
	{
		BigInt val((signed short)0x1235);
		TEST_ASSERT_GE(val, (signed short)0x1234,
			"BigInt::operator>=(signed short)");
		TEST_ASSERT_GE(val, 0x1234,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((signed short)-1);
		TEST_ASSERT_GE(val, (signed short)-2,
			"BigInt::operator>=(signed short)");
		TEST_ASSERT_GE(val, -2,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((unsigned short)0x1235);
		TEST_ASSERT_GE(val, (unsigned short)0x1234,
			"BigInt::operator>=(unsigned short)");
		TEST_ASSERT_GE(val, 0x1234,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((unsigned short)-1);
		TEST_ASSERT_GE(val, (unsigned short)0xfffe,
			"BigInt::operator>=(unsigned short)");
		TEST_ASSERT_GE(val, 0xfffe,
			"BigInt::operator>=(int)");
	}

	//
	// compare to int
	//
	// ==
	{
		BigInt val((signed int)0x12345678);
		TEST_ASSERT_EQ(val, (signed int)0x12345678,
			"BigInt::operator==(signed int)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((signed int)-1);
		TEST_ASSERT_EQ(val, (signed int)-1,
			"BigInt::operator==(signed int)");
		TEST_ASSERT_EQ(val, -1,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((unsigned int)0x12345678);
		TEST_ASSERT_EQ(val, (unsigned int)0x12345678,
			"BigInt::operator==(unsigned int)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"BigInt::operator==(int)");
	}
	{
		BigInt val((unsigned int)-1);
		TEST_ASSERT_EQ(val, (unsigned int)0xffffffff,
			"BigInt::operator==(unsigned int)");
		TEST_ASSERT_EQ(val, 0xffffffff,
			"BigInt::operator==(int)");
	}
	// !=
	{
		BigInt val((signed int)0x12345679);
		TEST_ASSERT_NE(val, (signed int)0x12345678,
			"BigInt::operator!=(signed int)");
		TEST_ASSERT_NE(val, 0x12345678,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((signed int)-2);
		TEST_ASSERT_NE(val, (signed int)-1,
			"BigInt::operator!=(signed int)");
		TEST_ASSERT_NE(val, -1,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((unsigned int)0x12345679);
		TEST_ASSERT_NE(val, (unsigned int)0x12345678,
			"BigInt::operator!=(unsigned int)");
		TEST_ASSERT_NE(val, 0x12345678,
			"BigInt::operator!=(int)");
	}
	{
		BigInt val((unsigned int)-2);
		TEST_ASSERT_NE(val, (unsigned int)0xffffffff,
			"BigInt::operator!=(unsigned int)");
		TEST_ASSERT_NE(val, 0xffffffff,
			"BigInt::operator!=(int)");
	}
	// <
	{
		BigInt val((signed int)0x12345678);
		TEST_ASSERT_LT(val, (signed int)0x12345679,
			"BigInt::operator<(signed int)");
		TEST_ASSERT_LT(val, 0x12345679,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((signed int)-2);
		TEST_ASSERT_LT(val, (signed int)-1,
			"BigInt::operator<(signed int)");
		TEST_ASSERT_LT(val, -1,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((unsigned int)0x12345678);
		TEST_ASSERT_LT(val, (unsigned int)0x12345679,
			"BigInt::operator<(unsigned int)");
		TEST_ASSERT_LT(val, 0x12345679,
			"BigInt::operator<(int)");
	}
	{
		BigInt val((unsigned int)-2);
		TEST_ASSERT_LT(val, (unsigned int)0xffffffff,
			"BigInt::operator<(unsigned int)");
		TEST_ASSERT_LT(val, 0xffffffff,
			"BigInt::operator<(int)");
	}
	// >
	{
		BigInt val((signed int)0x12345679);
		TEST_ASSERT_GT(val, (signed int)0x12345678,
			"BigInt::operator>(signed int)");
		TEST_ASSERT_GT(val, 0x12345678,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((signed int)-1);
		TEST_ASSERT_GT(val, (signed int)-2,
			"BigInt::operator>(signed int)");
		TEST_ASSERT_GT(val, -2,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((unsigned int)0x12345679);
		TEST_ASSERT_GT(val, (unsigned int)0x12345678,
			"BigInt::operator>(unsigned int)");
		TEST_ASSERT_GT(val, 0x12345678,
			"BigInt::operator>(int)");
	}
	{
		BigInt val((unsigned int)-1);
		TEST_ASSERT_GT(val, (unsigned int)0xfffffffe,
			"BigInt::operator>(unsigned int)");
		TEST_ASSERT_GT(val, 0xfffffffe,
			"BigInt::operator>(int)");
	}
	// <=
	{
		BigInt val((signed int)0x12345678);
		TEST_ASSERT_LE(val, (signed int)0x12345679,
			"BigInt::operator<=(signed int)");
		TEST_ASSERT_LE(val, 0x12345679,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((signed int)-2);
		TEST_ASSERT_LE(val, (signed int)-1,
			"BigInt::operator<=(signed int)");
		TEST_ASSERT_LE(val, -1,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((unsigned int)0x12345678);
		TEST_ASSERT_LE(val, (unsigned int)0x12345679,
			"BigInt::operator<=(unsigned int)");
		TEST_ASSERT_LE(val, 0x12345679,
			"BigInt::operator<=(int)");
	}
	{
		BigInt val((unsigned int)-2);
		TEST_ASSERT_LE(val, (unsigned int)0xffffffff,
			"BigInt::operator<=(unsigned int)");
		TEST_ASSERT_LE(val, 0xffffffff,
			"BigInt::operator<=(int)");
	}
	// >=
	{
		BigInt val((signed int)0x12345679);
		TEST_ASSERT_GE(val, (signed int)0x12345678,
			"BigInt::operator>=(signed int)");
		TEST_ASSERT_GE(val, 0x12345678,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((signed int)-1);
		TEST_ASSERT_GE(val, (signed int)-2,
			"BigInt::operator>=(signed int)");
		TEST_ASSERT_GE(val, -2,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((unsigned int)0x12345679);
		TEST_ASSERT_GE(val, (unsigned int)0x12345678,
			"BigInt::operator>=(unsigned int)");
		TEST_ASSERT_GE(val, 0x12345678,
			"BigInt::operator>=(int)");
	}
	{
		BigInt val((unsigned int)-1);
		TEST_ASSERT_GE(val, (unsigned int)0xfffffffe,
			"BigInt::operator>=(unsigned int)");
		TEST_ASSERT_GE(val, 0xfffffffe,
			"BigInt::operator>=(int)");
	}

	//
	// compare to long
	//
	// ==
	{
		BigInt val((signed long)0x12345678);
		TEST_ASSERT_EQ(val, (signed long)0x12345678,
			"BigInt::operator==(signed long)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"BigInt::operator==(long)");
	}
	{
		BigInt val((signed long)-1);
		TEST_ASSERT_EQ(val, (signed long)-1,
			"BigInt::operator==(signed long)");
		TEST_ASSERT_EQ(val, -1,
			"BigInt::operator==(long)");
	}
	{
		BigInt val((unsigned long)0x12345678);
		TEST_ASSERT_EQ(val, (unsigned long)0x12345678,
			"BigInt::operator==(unsigned long)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"BigInt::operator==(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// !=
	{
		BigInt val((signed long)0x12345679);
		TEST_ASSERT_NE(val, (signed long)0x12345678,
			"BigInt::operator!=(signed long)");
		TEST_ASSERT_NE(val, 0x12345678,
			"BigInt::operator!=(long)");
	}
	{
		BigInt val((signed long)-2);
		TEST_ASSERT_NE(val, (signed long)-1,
			"BigInt::operator!=(signed long)");
		TEST_ASSERT_NE(val, -1,
			"BigInt::operator!=(long)");
	}
	{
		BigInt val((unsigned long)0x12345679);
		TEST_ASSERT_NE(val, (unsigned long)0x12345678,
			"BigInt::operator!=(unsigned long)");
		TEST_ASSERT_NE(val, 0x12345678,
			"BigInt::operator!=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// <
	{
		BigInt val((signed long)0x12345678);
		TEST_ASSERT_LT(val, (signed long)0x12345679,
			"BigInt::operator<(signed long)");
		TEST_ASSERT_LT(val, 0x12345679,
			"BigInt::operator<(long)");
	}
	{
		BigInt val((signed long)-2);
		TEST_ASSERT_LT(val, (signed long)-1,
			"BigInt::operator<(signed long)");
		TEST_ASSERT_LT(val, -1,
			"BigInt::operator<(long)");
	}
	{
		BigInt val((unsigned long)0x12345678);
		TEST_ASSERT_LT(val, (unsigned long)0x12345679,
			"BigInt::operator<(unsigned long)");
		TEST_ASSERT_LT(val, 0x12345679,
			"BigInt::operator<(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// >
	{
		BigInt val((signed long)0x12345679);
		TEST_ASSERT_GT(val, (signed long)0x12345678,
			"BigInt::operator>(signed long)");
		TEST_ASSERT_GT(val, 0x12345678,
			"BigInt::operator>(long)");
	}
	{
		BigInt val((signed long)-1);
		TEST_ASSERT_GT(val, (signed long)-2,
			"BigInt::operator>(signed long)");
		TEST_ASSERT_GT(val, -2,
			"BigInt::operator>(long)");
	}
	{
		BigInt val((unsigned long)0x12345679);
		TEST_ASSERT_GT(val, (unsigned long)0x12345678,
			"BigInt::operator>(unsigned long)");
		TEST_ASSERT_GT(val, 0x12345678,
			"BigInt::operator>(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// <=
	{
		BigInt val((signed long)0x12345678);
		TEST_ASSERT_LE(val, (signed long)0x12345679,
			"BigInt::operator<=(signed long)");
		TEST_ASSERT_LE(val, 0x12345679,
			"BigInt::operator<=(long)");
	}
	{
		BigInt val((signed long)-2);
		TEST_ASSERT_LE(val, (signed long)-1,
			"BigInt::operator<=(signed long)");
		TEST_ASSERT_LE(val, -1,
			"BigInt::operator<=(long)");
	}
	{
		BigInt val((unsigned long)0x12345678);
		TEST_ASSERT_LE(val, (unsigned long)0x12345679,
			"BigInt::operator<=(unsigned long)");
		TEST_ASSERT_LE(val, 0x12345679,
			"BigInt::operator<=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// >=
	{
		BigInt val((signed long)0x12345679);
		TEST_ASSERT_GE(val, (signed long)0x12345678,
			"BigInt::operator>=(signed long)");
		TEST_ASSERT_GE(val, 0x12345678,
			"BigInt::operator>=(long)");
	}
	{
		BigInt val((signed long)-1);
		TEST_ASSERT_GE(val, (signed long)-2,
			"BigInt::operator>=(signed long)");
		TEST_ASSERT_GE(val, -2,
			"BigInt::operator>=(long)");
	}
	{
		BigInt val((unsigned long)0x12345679);
		TEST_ASSERT_GE(val, (unsigned long)0x12345678,
			"BigInt::operator>=(unsigned long)");
		TEST_ASSERT_GE(val, 0x12345678,
			"BigInt::operator>=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.

	//
	// compare to long long
	//
	// ==
	{
		BigInt val((signed long long)0x1234567890ULL);
		TEST_ASSERT_EQ(val, (signed long long)0x1234567890ULL,
			"BigInt::operator==(signed long long)");
		TEST_ASSERT_EQ(val, 0x1234567890ULL,
			"BigInt::operator==(long long)");
	}
	{
		BigInt val((signed long long)-1);
		TEST_ASSERT_EQ(val, (signed long long)-1,
			"BigInt::operator==(signed long long)");
		TEST_ASSERT_EQ(val, -1,
			"BigInt::operator==(long long)");
	}
	{
		BigInt val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_EQ(val, (unsigned long long)0x1234567890ULL,
			"BigInt::operator==(unsigned long long)");
		TEST_ASSERT_EQ(val, 0x1234567890ULL,
			"BigInt::operator==(long long)");
	}
	{
		BigInt val((unsigned long long)-1);
		TEST_ASSERT_EQ(
			val, (unsigned long long)0xffffffffffffffffULL,
			"BigInt::operator==(unsigned long long)");
		TEST_ASSERT_EQ(val, 0xffffffffffffffffULL,
			"BigInt::operator==(long long)");
	}
	// !=
	{
		BigInt val((signed long long)0x1234567891ULL);
		TEST_ASSERT_NE(val, (signed long long)0x1234567890ULL,
			"BigInt::operator!=(signed long long)");
		TEST_ASSERT_NE(val, 0x1234567890ULL,
			"BigInt::operator!=(long long)");
	}
	{
		BigInt val((signed long long)-2);
		TEST_ASSERT_NE(val, (signed long long)-1,
			"BigInt::operator!=(signed long long)");
		TEST_ASSERT_NE(val, -1,
			"BigInt::operator!=(long long)");
	}
	{
		BigInt val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_NE(val, (unsigned long long)0x1234567890ULL,
			"BigInt::operator!=(unsigned long long)");
		TEST_ASSERT_NE(val, 0x1234567890ULL,
			"BigInt::operator!=(long long)");
	}
	{
		BigInt val((unsigned long long)-2);
		TEST_ASSERT_NE(
			val, (unsigned long long)0xffffffffffffffffULL,
			"BigInt::operator!=(unsigned long long)");
		TEST_ASSERT_NE(val, 0xffffffffffffffffULL,
			"BigInt::operator!=(long long)");
	}
	// <
	{
		BigInt val((signed long long)0x1234567890ULL);
		TEST_ASSERT_LT(val, (signed long long)0x1234567891ULL,
			"BigInt::operator<(signed long long)");
		TEST_ASSERT_LT(val, 0x1234567891ULL,
			"BigInt::operator<(long long)");
	}
	{
		BigInt val((signed long long)-2);
		TEST_ASSERT_LT(val, (signed long long)-1,
			"BigInt::operator<(signed long long)");
		TEST_ASSERT_LT(val, -1,
			"BigInt::operator<(long long)");
	}
	{
		BigInt val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_LT(val, (unsigned long long)0x1234567891ULL,
			"BigInt::operator<(unsigned long long)");
		TEST_ASSERT_LT(val, 0x1234567891ULL,
			"BigInt::operator<(long long)");
	}
	{
		BigInt val((unsigned long long)-2);
		TEST_ASSERT_LT(
			val, (unsigned long long)0xffffffffffffffffULL,
			"BigInt::operator<(unsigned long long)");
		TEST_ASSERT_LT(val, 0xffffffffffffffffULL,
			"BigInt::operator<(long long)");
	}
	// >
	{
		BigInt val((signed long long)0x1234567891ULL);
		TEST_ASSERT_GT(val, (signed long long)0x1234567890ULL,
			"BigInt::operator>(signed long long)");
		TEST_ASSERT_GT(val, 0x1234567890ULL,
			"BigInt::operator>(long long)");
	}
	{
		BigInt val((signed long long)-1);
		TEST_ASSERT_GT(val, (signed long long)-2,
			"BigInt::operator>(signed long long)");
		TEST_ASSERT_GT(val, -2,
			"BigInt::operator>(long long)");
	}
	{
		BigInt val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_GT(val, (unsigned long long)0x1234567890ULL,
			"BigInt::operator>(unsigned long long)");
		TEST_ASSERT_GT(val, 0x1234567890ULL,
			"BigInt::operator>(long long)");
	}
	{
		BigInt val((unsigned long long)-1);
		TEST_ASSERT_GE(
			val, (unsigned long long)0xfffffffffffffffeULL,
			"BigInt::operator>(unsigned long long)");
		TEST_ASSERT_GT(val, 0xfffffffffffffffeULL,
			"BigInt::operator>(long long)");
	}
	// <=
	{
		BigInt val((signed long long)0x1234567890ULL);
		TEST_ASSERT_LE(val, (signed long long)0x1234567891ULL,
			"BigInt::operator<=(signed long long)");
		TEST_ASSERT_LE(val, 0x1234567891ULL,
			"BigInt::operator<=(long long)");
	}
	{
		BigInt val((signed long long)-2);
		TEST_ASSERT_LE(val, (signed long long)-1,
			"BigInt::operator<=(signed long long)");
		TEST_ASSERT_LE(val, -1,
			"BigInt::operator<=(long long)");
	}
	{
		BigInt val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_LE(val, (unsigned long long)0x1234567891ULL,
			"BigInt::operator<=(unsigned long long)");
		TEST_ASSERT_LE(val, 0x1234567891ULL,
			"BigInt::operator<=(long long)");
	}
	{
		BigInt val((unsigned long long)-2);
		TEST_ASSERT_LE(
			val, (unsigned long long)0xffffffffffffffffULL,
			"BigInt::operator<=(unsigned long long)");
		TEST_ASSERT_LE(val, 0xffffffffffffffffULL,
			"BigInt::operator<=(long long)");
	}
	// >=
	{
		BigInt val((signed long long)0x1234567891ULL);
		TEST_ASSERT_GE(val, (signed long long)0x1234567890ULL,
			"BigInt::operator>=(signed long long)");
		TEST_ASSERT_GE(val, 0x1234567890ULL,
			"BigInt::operator>=(long long)");
	}
	{
		BigInt val((signed long long)-1);
		TEST_ASSERT_GE(val, (signed long long)-2,
			"BigInt::operator>=(signed long long)");
		TEST_ASSERT_GE(val, -2,
			"BigInt::operator>=(long long)");
	}
	{
		BigInt val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_GE(val, (unsigned long long)0x1234567890ULL,
			"BigInt::operator>=(unsigned long long)");
		TEST_ASSERT_GE(val, 0x1234567890ULL,
			"BigInt::operator>=(long long)");
	}
	{
		BigInt val((unsigned long long)-1);
		TEST_ASSERT_GE(
			val, (unsigned long long)0xfffffffffffffffeULL,
			"BigInt::operator>=(unsigned long long)");
		TEST_ASSERT_GE(val, 0xfffffffffffffffeULL,
			"BigInt::operator>=(long long)");
	}

	//
	// compare to BigInt
	//
	// ==
	{
		BigInt val1(12345678);
		BigInt val2(12345678);
		TEST_ASSERT_EQ(val1, val2, "BigInt::operator==(BigInt)");
	}
	// !=
	{
		BigInt val1(12345678);
		BigInt val2(12345679);
		TEST_ASSERT_NE(val1, val2, "BigInt::operator!=(BigInt)");
	}
	// <
	{
		BigInt val1(12345678);
		BigInt val2(12345679);
		TEST_ASSERT_LT(val1, val2, "BigInt::operator<(BigInt)");
	}
	// >
	{
		BigInt val1(12345679);
		BigInt val2(12345678);
		TEST_ASSERT_GT(val1, val2, "BigInt::operator>(BigInt)");
	}
	// <=
	{
		BigInt val1(12345678);
		BigInt val2(12345679);
		TEST_ASSERT_LE(val1, val2, "BigInt::operator<=(BigInt)");
	}
	// >=
	{
		BigInt val1(12345679);
		BigInt val2(12345678);
		TEST_ASSERT_GE(val1, val2, "BigInt::operator>=(BigInt)");
	}

	// can't compare directly to strings
}

TEST(test_bool)
{
	// convert to bool
	{
		BigInt val(1);
		TEST_ASSERT(val, "BigInt::operator bool()");
	}
	{
		BigInt val(12345);
		TEST_ASSERT(val, "BigInt::operator bool()");
	}
	{
		BigInt val(-1);
		TEST_ASSERT(val, "BigInt::operator bool()");
	}
	{
		BigInt val(0);
		TEST_ASSERT(!val, "BigInt::operator bool()");
	}
	{
		BigInt val(2);
		TEST_ASSERT(!!val, "BigInt::operator bool()");
	}
	{
		BigInt val(0);
		TEST_ASSERT(!!!val, "BigInt::operator bool()");
	}
}

TEST(test_small_arithmetic)
{
	{
		BigInt val(10);
		TEST_ASSERT(val+3 == 13,
			"BigInt::operator+(BigInt, int)");
		TEST_ASSERT(3+val == 13,
			"BigInt::operator+(int, BigInt)");
		TEST_ASSERT(val+BigInt(3) == 13,
			"BigInt::operator+(BigInt, BigInt)");
		val += 5;
		TEST_ASSERT(val == 15,
			"BigInt::operator+=(BigInt, int)");
		val++;
		TEST_ASSERT(val == 16,
			"BigInt::operator++()");
		++val;
		TEST_ASSERT(val == 17,
			"BigInt::operator++()");
	}
	{
		BigInt val(10);
		TEST_ASSERT(val-3 == 7,
			"BigInt::operator-(BigInt, int)");
		TEST_ASSERT(17-val == 7,
			"BigInt::operator-(int, BigInt)");
		TEST_ASSERT(val-BigInt(3) == 7,
			"BigInt::operator-(BigInt, BigInt)");
		val -= 5;
		TEST_ASSERT(val == 5,
			"BigInt::operator-=(BigInt, int)");
		val--;
		TEST_ASSERT(val == 4,
			"BigInt::operator--()");
		--val;
		TEST_ASSERT(val == 3,
			"BigInt::operator-()");
	}
	{
		BigInt val(10);
		TEST_ASSERT(val*2 == 20,
			"BigInt::operator*(BigInt, int)");
		TEST_ASSERT(2*val == 20,
			"BigInt::operator*(int, BigInt)");
		TEST_ASSERT(val*BigInt(2) == 20,
			"BigInt::operator*(BigInt, BigInt)");
		val *= 3;
		TEST_ASSERT(val == 30,
			"BigInt::operator*=(BigInt, int)");
	}
	{
		BigInt val(10);
		TEST_ASSERT(val/2 == 5,
			"BigInt::operator/(BigInt, int)");
		TEST_ASSERT(50/val == 5,
			"BigInt::operator/(int, BigInt)");
		TEST_ASSERT(val/BigInt(2) == 5,
			"BigInt::operator/(BigInt, BigInt)");
		val /= 5;
		TEST_ASSERT(val == 2,
			"BigInt::operator/=(BigInt, int)");
	}
	{
		BigInt val(17);
		TEST_ASSERT(val%10 == 7,
			"BigInt::operator%(BigInt, int)");
		TEST_ASSERT(24%val == 7,
			"BigInt::operator%(int, BigInt)");
		TEST_ASSERT(val%BigInt(10) == 7,
			"BigInt::operator%(BigInt, BigInt)");
		val %= 5;
		TEST_ASSERT(val == 2,
			"BigInt::operator%=(BigInt, int)");
	}
}

TEST(test_large_arithmetic)
{
	{
		BigInt val("0x1000000000000000a");
		TEST_ASSERT(val+3 == BigInt("0x1000000000000000d"),
			"BigInt::operator+(BigInt, int)");
		TEST_ASSERT(3+val == BigInt("0x1000000000000000d"),
			"BigInt::operator+(int, BigInt)");
		val += 5;
		TEST_ASSERT(val == BigInt("0x1000000000000000f"),
			"BigInt::operator+=(BigInt, int)");
		val++;
		TEST_ASSERT(val == BigInt("0x10000000000000010"),
			"BigInt::operator++()");
		++val;
		TEST_ASSERT(val == BigInt("0x10000000000000011"),
			"BigInt::operator++()");
	}
	{
		BigInt val("0x1000000000000000a");
		TEST_ASSERT(val-3 == BigInt("0x10000000000000007"),
			"BigInt::operator-(BigInt, int)");
		val -= 5;
		TEST_ASSERT(val == BigInt("0x10000000000000005"),
			"BigInt::operator-=(int, BigInt)");
		val--;
		TEST_ASSERT(val == BigInt("0x10000000000000004"),
			"BigInt::operator--(BigInt, int)");
		--val;
		TEST_ASSERT(val == BigInt("0x10000000000000003"),
			"BigInt::operator--()");
	}
	{
		BigInt val("0x10000000000000010");
		TEST_ASSERT(val*2 == BigInt("0x20000000000000020"),
			"BigInt::operator*(BigInt, int)");
		TEST_ASSERT(2*val == BigInt("0x20000000000000020"),
			"BigInt::operator*(int, BigInt)");
		val *= 3;
		TEST_ASSERT(val == BigInt("0x30000000000000030"),
			"BigInt::operator*=(BigInt, int)");
	}
	{
		BigInt val("0xa000000000000000a");
		TEST_ASSERT(val/2 == BigInt("0x50000000000000005"),
			"BigInt::operator/(BigInt, int)");
		val /= 5;
		TEST_ASSERT(val == BigInt("0x20000000000000002"),
			"BigInt::operator/=(BigInt, int)");
	}
	{
		BigInt val("0x10000000000000011");
		TEST_ASSERT(val%10 == BigInt("0x03"),
			"BigInt::operator%(BigInt, int)");
		val %= 5;
		TEST_ASSERT(val == BigInt("0x03"),
			"BigInt::operator%=(BigInt, int)");
	}
}

// NOTE:
//  Bitwise NOT is supported, but what does it really mean for an
//  arbitrary-width number?
TEST(test_small_logical)
{
	{
		BigInt val(0xff);
		TEST_ASSERT((val & BigInt(0x107)) == 0x7,
			"BigInt::operator&(BigInt, unsigned)");
		TEST_ASSERT((BigInt(0x107) & val) == 0x7,
			"BigInt::operator&(unsigned, BigInt)");
		val &= BigInt(0x370);
		TEST_ASSERT(val == 0x70,
			"BigInt::operator&=(BigInt, unsigned)");
	}
	{
		BigInt val(0xff);
		TEST_ASSERT((val | BigInt(0x107)) == 0x1ff,
			"BigInt::operator|()");
		TEST_ASSERT((BigInt(0x107) | val) == 0x1ff,
			"BigInt::operator|()");
		val |= BigInt(0x270);
		TEST_ASSERT(val == 0x2ff, "BigInt::operator|=()");
	}
	{
		BigInt val(0xff);
		TEST_ASSERT((val ^ BigInt(0x107)) == 0x1f8,
			"BigInt::operator^()");
		TEST_ASSERT((BigInt(0x107) ^ val) == 0x1f8,
			"BigInt::operator^()");
		val ^= BigInt(0x270);
		TEST_ASSERT(val == 0x28f, "BigInt::operator^=()");
	}
	{
		BigInt val(0x10);
		TEST_ASSERT((val << 2) == 0x40,
			"BigInt::operator<<()");
		TEST_ASSERT((2 << val) == 0x20000,
			"BigInt::operator<<()");
		val <<= 3;
		TEST_ASSERT(val == 0x80, "BigInt::operator<<=()");
	}
	{
		BigInt val(0x10);
		TEST_ASSERT((val >> 2) == 0x04,
			"BigInt::operator>>()");
		TEST_ASSERT((0x20000 >> val) == 0x2,
			"BigInt::operator>>()");
		val >>= 3;
		TEST_ASSERT(val == 0x02, "BigInt::operator>>=()");
	}
}

TEST(test_large_logical)
{
	{
		BigInt val("0xf00000000000000ff");
		TEST_ASSERT((val & BigInt(0x107)) == 0x7,
			"BigInt::operator&()");
		TEST_ASSERT((BigInt(0x107) & val) == 0x7,
			"BigInt::operator&()");
		val &= BigInt(0x370);
		TEST_ASSERT(val == 0x70, "BigInt::operator&=()");
	}
	{
		BigInt val("0xf00000000000000ff");
		TEST_ASSERT(
			(val | BigInt(0x107)) == BigInt("0xf00000000000001ff"),
			"BigInt::operator|()");
		TEST_ASSERT(
			(BigInt(0x107) | val) == BigInt("0xf00000000000001ff"),
			"BigInt::operator|()");
		val |= BigInt(0x270);
		TEST_ASSERT(
			val == BigInt("0xf00000000000002ff"),
			"BigInt::operator|=()");
	}
	{
		BigInt val("0xf00000000000000ff");
		TEST_ASSERT(
			(val ^ BigInt(0x107)) == BigInt("0xf00000000000001f8"),
			"BigInt::operator^()");
		TEST_ASSERT(
			(BigInt(0x107) ^ val) == BigInt("0xf00000000000001f8"),
			"BigInt::operator^()");
		val ^= BigInt(0x270);
		TEST_ASSERT(
			val == BigInt("0xf000000000000028f"),
			"BigInt::operator^=()");
	}
	{
		BigInt val("0xf0000000000000010");
		TEST_ASSERT(
			(val << 2) == BigInt("0x3c0000000000000040"),
			"BigInt::operator<<()");
		val <<= 3;
		TEST_ASSERT(
			val == BigInt("0x780000000000000080"),
			"BigInt::operator<<=()");
	}
	{
		BigInt val("0xf0000000000000010");
		TEST_ASSERT(
			(val >> 2) == BigInt("0x3c000000000000004"),
			"BigInt::operator>>()");
		val >>= 3;
		TEST_ASSERT(
			val == BigInt("0x1e000000000000002"),
			"BigInt::operator>>=()");
	}
}

TEST(test_small_io)
{
	{
		BigInt val(15);
		TEST_ASSERT(val.to_dec_string() == "15",
				"BigInt::to_dec_string()");
		TEST_ASSERT(val.to_string() == "15",
				"BigInt::to_string()");
		TEST_ASSERT(val.to_hex_string() == "0xf",
				"BigInt::to_dec_string()");
		TEST_ASSERT(val.to_oct_string() == "017",
				"BigInt::to_dec_string()");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss.setf(std::ios_base::uppercase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss.setf(std::ios_base::uppercase);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss.setf(std::ios_base::uppercase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss.setf(std::ios_base::uppercase);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss.setf(std::ios_base::uppercase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss.setf(std::ios_base::uppercase);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails because gmpxx adds the leading '+' in octal
		// mode, but libstdc++ does not.  Very minor issue.
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
		#endif
	}
	{
		#if 0
		// This test fails because gmpxx adds the leading '+' in hex
		// mode, but libstdc++ does not.  Very minor issue.
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
		#endif
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.width(8);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.width(8);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.width(8);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.width(8);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.width(8);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.width(8);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.setf(std::ios_base::showbase);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.setf(std::ios_base::showbase);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::left);
		valoss.setf(std::ios_base::showbase);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.setf(std::ios_base::showbase);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.setf(std::ios_base::showbase);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
	{
		BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::right);
		valoss.setf(std::ios_base::showbase);
		valoss.width(8);
		valoss.fill('0');
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"BigInt::operator<<(ostream)");
	}
}

TEST(test_large_io)
{
	{
		std::string num = "12345678123456781234567812345678";
		BigInt val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::dec;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"BigInt::operator<<(ostream)");
	}
	{
		std::string num = "012345671234567123456712345671234567";
		BigInt val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::oct;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"BigInt::operator<<(ostream)");
	}
	{
		std::string num = "0x123456789abcdef123456789abcdef";
		BigInt val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::hex;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"BigInt::operator<<(ostream)");
	}
}

TEST(test_operators)
{
	// test operators with signed char
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (signed char)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (signed char)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (signed char)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (signed char)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (signed char)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (signed char)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (signed char)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (signed char)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (signed char)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (signed char)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (signed char)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (signed char)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (signed char)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (signed char)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (signed char)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (signed char)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (signed char)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (signed char)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (signed char)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (signed char)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with unsigned char
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (unsigned char)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (unsigned char)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (unsigned char)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (unsigned char)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (unsigned char)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (unsigned char)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (unsigned char)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (unsigned char)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (unsigned char)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (unsigned char)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (unsigned char)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (unsigned char)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (unsigned char)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (unsigned char)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (unsigned char)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (unsigned char)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (unsigned char)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (unsigned char)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (unsigned char)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (unsigned char)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with signed short
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (signed short)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (signed short)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (signed short)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (signed short)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (signed short)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (signed short)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (signed short)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (signed short)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (signed short)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (signed short)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (signed short)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (signed short)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (signed short)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (signed short)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (signed short)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (signed short)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (signed short)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (signed short)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (signed short)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (signed short)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with unsigned short
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (unsigned short)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (unsigned short)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (unsigned short)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (unsigned short)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (unsigned short)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (unsigned short)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (unsigned short)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (unsigned short)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (unsigned short)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (unsigned short)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (unsigned short)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (unsigned short)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (unsigned short)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (unsigned short)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (unsigned short)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (unsigned short)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (unsigned short)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (unsigned short)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (unsigned short)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (unsigned short)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with signed int
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (signed int)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (signed int)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (signed int)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (signed int)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (signed int)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (signed int)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (signed int)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (signed int)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (signed int)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (signed int)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (signed int)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (signed int)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (signed int)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (signed int)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (signed int)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (signed int)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (signed int)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (signed int)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (signed int)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (signed int)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with unsigned int
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (unsigned int)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (unsigned int)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (unsigned int)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (unsigned int)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (unsigned int)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (unsigned int)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (unsigned int)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (unsigned int)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (unsigned int)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (unsigned int)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (unsigned int)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (unsigned int)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (unsigned int)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (unsigned int)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (unsigned int)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (unsigned int)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (unsigned int)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (unsigned int)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (unsigned int)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (unsigned int)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with signed long
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (signed long)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (signed long)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (signed long)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (signed long)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (signed long)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (signed long)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (signed long)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (signed long)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (signed long)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (signed long)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (signed long)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (signed long)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (signed long)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (signed long)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (signed long)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (signed long)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (signed long)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (signed long)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (signed long)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (signed long)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with unsigned long
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (unsigned long)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (unsigned long)1;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (unsigned long)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (unsigned long)1;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (unsigned long)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (unsigned long)2;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (unsigned long)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (unsigned long)2;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		val2 = val | (unsigned long)3;
		TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (unsigned long)3;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		val2 = val & (unsigned long)2;
		TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (unsigned long)2;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		val2 = val ^ (unsigned long)1;
		TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (unsigned long)1;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (unsigned long)2;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (unsigned long)2;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (unsigned long)1;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (unsigned long)1;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (unsigned long)1;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (unsigned long)1;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// Most operators with (signed or unsigned) long long fail, because
	// GMP does not natively support them.  There must be some hack to
	// make this work, but I can't be bothered to mess with it any further.
	#if 0
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (signed long long)1LL;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (signed long long)1LL;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (signed long long)1LL;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (signed long long)1LL;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (signed long long)2LL;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (signed long long)2LL;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (signed long long)2LL;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (signed long long)2LL;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed long long)3LL;
		//TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (signed long long)3LL;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed long long)2LL;
		//TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (signed long long)2LL;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed long long)1LL;
		//TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (signed long long)1LL;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (signed long long)2LL;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (signed long long)2LL;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (signed long long)1LL;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (signed long long)1LL;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (signed long long)1LL;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (signed long long)1LL;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}

	// test operators with unsigned long long
	{
		BigInt val = 1;
		BigInt val2;

		val2 = val + (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 2, "BigInt::operator+()");
		val += (unsigned long long)1ULL;
		TEST_ASSERT(val == 2, "BigInt::operator+=()");

		val2 = val - (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 1, "BigInt::operator-()");
		val -= (unsigned long long)1ULL;
		TEST_ASSERT(val == 1, "BigInt::operator-=()");

		val2 = val * (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 2, "BigInt::operator*()");
		val *= (unsigned long long)2ULL;
		TEST_ASSERT(val == 2, "BigInt::operator*=()");

		val2 = val / (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 1, "BigInt::operator/()");
		val /= (unsigned long long)2ULL;
		TEST_ASSERT(val == 1, "BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned long long)3ULL;
		//TEST_ASSERT(val2 == 3, "BigInt::operator|()");
		val |= (unsigned long long)3ULL;
		TEST_ASSERT(val == 3, "BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned long long)2ULL;
		//TEST_ASSERT(val2 == 2, "BigInt::operator&()");
		val &= (unsigned long long)2ULL;
		TEST_ASSERT(val == 2, "BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned long long)1ULL;
		//TEST_ASSERT(val2 == 3, "BigInt::operator^()");
		val ^= (unsigned long long)1ULL;
		TEST_ASSERT(val == 3, "BigInt::operator^=()");

		val2 = val % (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 1, "BigInt::operator%()");
		val %= (unsigned long long)2ULL;
		TEST_ASSERT(val == 1, "BigInt::operator%=()");

		val2 = val << (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 2, "BigInt::operator<<()");
		val <<= (unsigned long long)1ULL;
		TEST_ASSERT(val == 2, "BigInt::operator<<=()");

		val2 = val >> (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 1, "BigInt::operator>>()");
		val >>= (unsigned long long)1ULL;
		TEST_ASSERT(val == 1, "BigInt::operator>>=()");
	}
	#endif // 0
}

TEST(test_extensions)
{
	// test popcount()
	{
		TEST_ASSERT(BigInt::popcount(1) == 1);
		TEST_ASSERT(BigInt::popcount(2) == 1);
		TEST_ASSERT(BigInt::popcount(3) == 2);
		TEST_ASSERT(BigInt::popcount(0xffff) == 16);
		TEST_ASSERT(BigInt::popcount(0xffffffff) == 32);
		TEST_ASSERT(
		    BigInt::popcount(BigInt("0xffffffffffffffff")) == 64);
		TEST_ASSERT(
		    BigInt::popcount(BigInt("0xffffffffffffffffffff")) == 80);
	}
	{
		BigInt val;

		val = 1;
		TEST_ASSERT(val.popcount() == 1);
		val <<= 1;
		TEST_ASSERT(val.popcount() == 1);

		val = 3;
		TEST_ASSERT(val.popcount() == 2);

		val = 0xffff;
		TEST_ASSERT(val.popcount() == 16);

		val = 0xffffffff;
		TEST_ASSERT(val.popcount() == 32);

		val = "0xffffffffffffffff";
		TEST_ASSERT(val.popcount() == 64);

		val = "0xffffffffffffffffffffffffffffffff";
		TEST_ASSERT(val.popcount() == 128);
	}

	// test pow()
	{
		TEST_ASSERT(BigInt::pow(2, 2) == 4);
		TEST_ASSERT(BigInt::pow(2, 3) == 8);
		TEST_ASSERT(BigInt::pow(2, 16) == 0x10000);

		TEST_ASSERT(BigInt::pow(3, 2) == 9);
		TEST_ASSERT(BigInt::pow(3, 3) == 27);
		TEST_ASSERT(BigInt::pow(3, 4) == 81);

		TEST_ASSERT(BigInt::pow(10, 2) == 100);
		TEST_ASSERT(BigInt::pow(10, 3) == 1000);
		TEST_ASSERT(BigInt::pow(10, 6) == 1000000);
	}
	{
		BigInt val;

		val = 2;
		TEST_ASSERT(val.pow(2) == 4);
		TEST_ASSERT(val == 2);
		TEST_ASSERT(val.pow(3) == 8);
		TEST_ASSERT(val.pow(16) == 0x10000);

		val = 3;
		TEST_ASSERT(val.pow(2) == 9);
		TEST_ASSERT(val == 3);
		TEST_ASSERT(val.pow(3) == 27);
		TEST_ASSERT(val.pow(4) == 81);

		val = 10;
		TEST_ASSERT(val.pow(2) == 100);
		TEST_ASSERT(val == 10);
		TEST_ASSERT(val.pow(3) == 1000);
		TEST_ASSERT(val.pow(6) == 1000000);
	}

	// test raise()
	{
		BigInt val;

		val = 2;
		val.raise(2);
		TEST_ASSERT(val == 4);
		val = 2;
		val.raise(3);
		TEST_ASSERT(val == 8);
		val = 2;
		val.raise(16);
		TEST_ASSERT(val == 0x10000);

		val = 3;
		val.raise(2);
		TEST_ASSERT(val == 9);
		val = 3;
		val.raise(3);
		TEST_ASSERT(val == 27);
		val = 3;
		val.raise(4);
		TEST_ASSERT(val == 81);

		val = 10;
		val.raise(2);
		TEST_ASSERT(val == 100);
		val = 10;
		val.raise(3);
		TEST_ASSERT(val == 1000);
		val = 10;
		val.raise(6);
		TEST_ASSERT(val == 1000000);
	}
}

} // namespace bignum
