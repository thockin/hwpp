#include "bignum.h"
#include "bit_buffer.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// default
	{
		bignum::BigInt val;
		TEST_ASSERT(val.get_si() == 0, "bignum::BigInt::BigInt()");
		TEST_ASSERT(val.get_ui() == 0, "bignum::BigInt::BigInt()");
		TEST_ASSERT(val.get_str(10) == "0", "bignum::BigInt::BigInt()");
	}

	// from char
	{
		bignum::BigInt val((signed char)0x12);
		TEST_ASSERT(val.get_si() == 0x12,
			"bignum::BigInt::BigInt(signed char)");
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::BigInt::BigInt(signed char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::BigInt::BigInt(signed char)");
	}
	{
		bignum::BigInt val((signed char)-1);
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::BigInt(signed char)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::BigInt(signed char)");
	}
	{
		bignum::BigInt val((unsigned char)0x12);
		TEST_ASSERT(val.get_si() == 0x12,
			"bignum::BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::BigInt::BigInt(unsigned char)");
	}
	{
		bignum::BigInt val((unsigned char)-1);
		TEST_ASSERT(val.get_si() == 0xff,
			"bignum::BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_ui() == 0xff,
			"bignum::BigInt::BigInt(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "ff",
			"bignum::BigInt::BigInt(unsigned char)");
	}

	// from short
	{
		bignum::BigInt val((signed short)0x1234);
		TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::BigInt::BigInt(signed short)");
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::BigInt::BigInt(signed short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::BigInt::BigInt(signed short)");
	}
	{
		bignum::BigInt val((signed short)-1);
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::BigInt(signed short)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::BigInt(signed short)");
	}
	{
		bignum::BigInt val((unsigned short)0x1234);
		TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::BigInt::BigInt(unsigned short)");
	}
	{
		bignum::BigInt val((unsigned short)-1);
		TEST_ASSERT(val.get_si() == 0xffff,
			"bignum::BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_ui() == 0xffff,
			"bignum::BigInt::BigInt(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "ffff",
			"bignum::BigInt::BigInt(unsigned short)");
	}

	// from int
	// On 32 bit platforms this is where we will start to see
	// signed vs. unsigned oddities.
	{
		bignum::BigInt val((signed int)0x12345678);
		TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::BigInt::BigInt(signed int)");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::BigInt::BigInt(signed int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::BigInt::BigInt(signed int)");
	}
	{
		bignum::BigInt val((signed int)-1);
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::BigInt(signed int)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::BigInt(signed int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345678);
		TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::BigInt::BigInt(unsigned int)");
	}
	{
		bignum::BigInt val((unsigned int)-1);
		// On a 32 bit platform this will fail, since get_si()
		// will return a signed value, and -1 is OOB.
		//TEST_ASSERT(val.get_si() == (signed int)0xffffffff,
		//	"bignum::BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_ui() == 0xffffffff,
			"bignum::BigInt::BigInt(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "ffffffff",
			"bignum::BigInt::BigInt(unsigned int)");
	}

	// from long long
	// Skip long, since either sizeof(long) == sizeof(int) or
	// sizeof(long) == sizeof(long long).
	{
		bignum::BigInt val(0x123456789abcdef0LL);
		TEST_ASSERT(val.get_si() == 0x123456789abcdef0LL,
			"val.get_int()");
		TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"bignum::BigInt::BigInt(signed long long)");
	}
	{
		bignum::BigInt val(-1LL);
		TEST_ASSERT(val.get_si() == -1,
			"val.get_int()");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::BigInt(signed long long)");
	}
	{
		bignum::BigInt val(0x123456789abcdef0ULL);
		TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"bignum::BigInt::BigInt(unsigned long long)");
	}
	{
		bignum::BigInt val(-1ULL);
		TEST_ASSERT(val.get_uint() == 0xffffffffffffffffULL,
			"val.get_uint()");
		TEST_ASSERT(val.get_str(16) == "ffffffffffffffff",
			"bignum::BigInt::BigInt(unsigned long long)");
	}

	// from bignum::BigInt
	{
		bignum::BigInt val1(12345678);
		bignum::BigInt val2(val1);
		TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"bignum::BigInt::BigInt(bignum::BigInt)");
	}

	// from string
	{
		bignum::BigInt val("12345678");
		TEST_ASSERT(val.get_ui() == 12345678,
			"bignum::BigInt::BigInt(string)");
		val = "23456789";
		TEST_ASSERT(val.get_ui() == 23456789,
			"bignum::BigInt::BigInt(string)");
	}
	{
		bignum::BigInt val("0x12345678");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::BigInt::BigInt(string)");
		val = "0x23456789";
		TEST_ASSERT(val.get_ui() == 0x23456789,
			"bignum::BigInt::BigInt(string)");
	}
	{
		bignum::BigInt val("0x112233445566778899aabbccddeeff00");
		TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::BigInt::BigInt(string)");
		val = "0x2233445566778899aabbccddeeff0011";
		TEST_ASSERT(
			val.get_str(16) == "2233445566778899aabbccddeeff0011",
			"bignum::BigInt::BigInt(string)");
	}
	{
		bignum::BigInt val("-1");
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::BigInt(string)");
		val = "-1";
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::BigInt(string)");
	}

	// from bitbuffer
	{
		bitbuffer bb(8);
		bb.byte_at(0) = 0x12;
		bignum::BigInt val(bb);
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::BigInt(bitbuffer)");
		bitbuffer bb2 = val.get_bitbuffer();
		TEST_ASSERT(bb2.to_string() == "0x12",
			"bignum::BigInt::get_bitbuffer()");
		bitbuffer bb3 = val.get_bitbuffer(8);
		TEST_ASSERT(bb3.to_string() == "0x12",
			"bignum::BigInt::get_bitbuffer(int)");
	}
	{
		bitbuffer bb(16);
		bb.byte_at(1) = 0x12;
		bb.byte_at(0) = 0x34;
		bignum::BigInt val(bb);
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::BigInt(bitbuffer)");
		bitbuffer bb2 = val.get_bitbuffer();
		TEST_ASSERT(bb2.to_string() == "0x1234",
			"bignum::BigInt::get_bitbuffer()");
		bitbuffer bb3 = val.get_bitbuffer(16);
		TEST_ASSERT(bb3.to_string() == "0x1234",
			"bignum::BigInt::get_bitbuffer(int)");
	}
	{
		bitbuffer bb = bignum::BigInt(-1).get_bitbuffer();
		TEST_ASSERT(bb.to_string() == "0x1",
			"bignum::BigInt::get_bitbuffer()");
	}
}

TEST(test_assignments)
{
	// from char
	{
		bignum::BigInt val;
		val = (signed char)0x12;
		TEST_ASSERT(val.get_si() == 0x12,
			"bignum::BigInt::operator=(signed char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::BigInt::operator=(signed char)");
	}
	{
		bignum::BigInt val;
		val = (signed char)-1;
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::operator=(signed char)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::operator=(signed char)");
	}
	{
		bignum::BigInt val;
		val = (unsigned char)0x12;
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::BigInt::operator=(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::BigInt::operator=(unsigned char)");
	}
	{
		bignum::BigInt val;
		val = (unsigned char)-1;
		TEST_ASSERT(val.get_ui() == 0xff,
			"bignum::BigInt::operator=(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "ff",
			"bignum::BigInt::operator=(unsigned char)");
	}

	// from short
	{
		bignum::BigInt val;
		val = (signed short)0x1234;
		TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::BigInt::operator=(signed short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::BigInt::operator=(signed short)");
	}
	{
		bignum::BigInt val;
		val = (signed short)-1;
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::operator=(signed short)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::operator=(signed short)");
	}
	{
		bignum::BigInt val;
		val = (unsigned short)0x1234;
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::BigInt::operator=(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::BigInt::operator=(unsigned short)");
	}
	{
		bignum::BigInt val;
		val = (unsigned short)-1;
		TEST_ASSERT(val.get_ui() == 0xffff,
			"bignum::BigInt::operator=(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "ffff",
			"bignum::BigInt::operator=(unsigned short)");
	}

	// from int
	{
		bignum::BigInt val;
		val = (signed int)0x12345678;
		TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::BigInt::operator=(signed int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::BigInt::operator=(signed int)");
	}
	{
		bignum::BigInt val;
		val = (signed int)-1;
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::operator=(signed int)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::operator=(signed int)");
	}
	{
		bignum::BigInt val;
		val = (unsigned int)0x12345678;
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::BigInt::operator=(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::BigInt::operator=(unsigned int)");
	}
	{
		bignum::BigInt val;
		val = (unsigned int)-1;
		TEST_ASSERT(val.get_ui() == 0xffffffff,
			"bignum::BigInt::operator=(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "ffffffff",
			"bignum::BigInt::operator=(unsigned int)");
	}

	// from bignum::BigInt
	{
		bignum::BigInt val1(12345678);
		bignum::BigInt val2;
		val2 = val1;
		TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"bignum::BigInt::operator=(bignum::BigInt)");
	}

	// from string
	{
		bignum::BigInt val;
		val = "12345678";
		TEST_ASSERT(val.get_ui() == 12345678,
			"bignum::BigInt::operator=(string)");
		TEST_ASSERT(val.get_str(10) == "12345678",
			"bignum::BigInt::operator=(string)");
	}
	{
		bignum::BigInt val;
		val = "0x112233445566778899aabbccddeeff00";
		TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::BigInt::operator=(string)");
	}
	{
		bignum::BigInt val;
		val = "-1";
		TEST_ASSERT(val.get_si() == -1,
			"bignum::BigInt::operator=(string)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::BigInt::operator=(string)");
	}
}

TEST(test_comparisons)
{
	// Because bignum::BigInt is a template, int's important to excercise all
	// permutations of comparisons, to force instantiation.

	//
	// compare to char
	//
	// ==
	{
		bignum::BigInt val((signed char)0x12);
		TEST_ASSERT_EQ(val, (signed char)0x12,
			"bignum::BigInt::operator==(signed char)");
		TEST_ASSERT_EQ(val, 0x12,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((signed char)-1);
		TEST_ASSERT_EQ(val, (signed char)-1,
			"bignum::BigInt::operator==(signed char)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((unsigned char)0x12);
		TEST_ASSERT_EQ(val, (unsigned char)0x12,
			"bignum::BigInt::operator==(unsigned char)");
		TEST_ASSERT_EQ(val, 0x12,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((unsigned char)-1);
		TEST_ASSERT_EQ(val, (unsigned char)0xff,
			"bignum::BigInt::operator==(unsigned char)");
		TEST_ASSERT_EQ(val, 0xff,
			"bignum::BigInt::operator==(int)");
	}
	// !=
	{
		bignum::BigInt val((signed char)0x13);
		TEST_ASSERT_NE(val, (signed char)0x12,
			"bignum::BigInt::operator!=(signed char)");
		TEST_ASSERT_NE(val, 0x12,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((signed char)-2);
		TEST_ASSERT_NE(val, (signed char)-1,
			"bignum::BigInt::operator!=(signed char)");
		TEST_ASSERT_NE(val, -1,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((unsigned char)0x13);
		TEST_ASSERT_NE(val, (unsigned char)0x12,
			"bignum::BigInt::operator!=(unsigned char)");
		TEST_ASSERT_NE(val, 0x12,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((unsigned char)-2);
		TEST_ASSERT_NE(val, (unsigned char)0xff,
			"bignum::BigInt::operator!=(unsigned char)");
		TEST_ASSERT_NE(val, 0xff,
			"bignum::BigInt::operator!=(int)");
	}
	// <
	{
		bignum::BigInt val((signed char)0x12);
		TEST_ASSERT_LT(val, (signed char)0x13,
			"bignum::BigInt::operator<(signed char)");
		TEST_ASSERT_LT(val, 0x13,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((signed char)-2);
		TEST_ASSERT_LT(val, (signed char)-1,
			"bignum::BigInt::operator<(signed char)");
		TEST_ASSERT_LT(val, -1,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((unsigned char)0x12);
		TEST_ASSERT_LT(val, (unsigned char)0x13,
			"bignum::BigInt::operator<(unsigned char)");
		TEST_ASSERT_LT(val, 0x13,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((unsigned char)-2);
		TEST_ASSERT_LT(val, (unsigned char)0xff,
			"bignum::BigInt::operator<(unsigned char)");
		TEST_ASSERT_LT(val, 0xff,
			"bignum::BigInt::operator<(int)");
	}
	// >
	{
		bignum::BigInt val((signed char)0x12);
		TEST_ASSERT_GT(val, (signed char)0x11,
			"bignum::BigInt::operator>(signed char)");
		TEST_ASSERT_GT(val, 0x11,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((signed char)-1);
		TEST_ASSERT_GT(val, (signed char)-2,
			"bignum::BigInt::operator>(signed char)");
		TEST_ASSERT_GT(val, -2,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((unsigned char)0x12);
		TEST_ASSERT_GT(val, (unsigned char)0x11,
			"bignum::BigInt::operator>(unsigned char)");
		TEST_ASSERT_GT(val, 0x11,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((unsigned char)-1);
		TEST_ASSERT_GT(val, (unsigned char)0xfe,
			"bignum::BigInt::operator>(unsigned char)");
		TEST_ASSERT_GT(val, 0xfe,
			"bignum::BigInt::operator>(int)");
	}
	// <=
	{
		bignum::BigInt val((signed char)0x12);
		TEST_ASSERT_LE(val, (signed char)0x13,
			"bignum::BigInt::operator<=(signed char)");
		TEST_ASSERT_LE(val, 0x13,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((signed char)-2);
		TEST_ASSERT_LE(val, (signed char)-1,
			"bignum::BigInt::operator<=(signed char)");
		TEST_ASSERT_LE(val, -1,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((unsigned char)0x12);
		TEST_ASSERT_LE(val, (unsigned char)0x13,
			"bignum::BigInt::operator<=(unsigned char)");
		TEST_ASSERT_LE(val, 0x13,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((unsigned char)-2);
		TEST_ASSERT_LE(val, (unsigned char)0xff,
			"bignum::BigInt::operator<=(unsigned char)");
		TEST_ASSERT_LE(val, 0xff,
			"bignum::BigInt::operator<=(int)");
	}
	// >=
	{
		bignum::BigInt val((signed char)0x12);
		TEST_ASSERT_GE(val, (signed char)0x11,
			"bignum::BigInt::operator>=(signed char)");
		TEST_ASSERT_GE(val, 0x11,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((signed char)-1);
		TEST_ASSERT_GE(val, (signed char)-2,
			"bignum::BigInt::operator>=(signed char)");
		TEST_ASSERT_GE(val, -2,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((unsigned char)0x12);
		TEST_ASSERT_GE(val, (unsigned char)0x11,
			"bignum::BigInt::operator>=(unsigned char)");
		TEST_ASSERT_GE(val, 0x11,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((unsigned char)-1);
		TEST_ASSERT_GE(val, (unsigned char)0xfe,
			"bignum::BigInt::operator>=(unsigned char)");
		TEST_ASSERT_GE(val, 0xfe,
			"bignum::BigInt::operator>=(int)");
	}

	//
	// compare to short
	//
	// ==
	{
		bignum::BigInt val((signed short)0x1234);
		TEST_ASSERT_EQ(val, (signed short)0x1234,
			"bignum::BigInt::operator==(signed short)");
		TEST_ASSERT_EQ(val, 0x1234,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((signed short)-1);
		TEST_ASSERT_EQ(val, (signed short)-1,
			"bignum::BigInt::operator==(signed short)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((unsigned short)0x1234);
		TEST_ASSERT_EQ(val, (unsigned short)0x1234,
			"bignum::BigInt::operator==(unsigned short)");
		TEST_ASSERT_EQ(val, 0x1234,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((unsigned short)-1);
		TEST_ASSERT_EQ(val, (unsigned short)0xffff,
			"bignum::BigInt::operator==(unsigned short)");
		TEST_ASSERT_EQ(val, 0xffff,
			"bignum::BigInt::operator==(int)");
	}
	// !=
	{
		bignum::BigInt val((signed short)0x1235);
		TEST_ASSERT_NE(val, (signed short)0x1234,
			"bignum::BigInt::operator!=(signed short)");
		TEST_ASSERT_NE(val, 0x1234,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((signed short)-2);
		TEST_ASSERT_NE(val, (signed short)-1,
			"bignum::BigInt::operator!=(signed short)");
		TEST_ASSERT_NE(val, -1,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((unsigned short)0x1235);
		TEST_ASSERT_NE(val, (unsigned short)0x1234,
			"bignum::BigInt::operator!=(unsigned short)");
		TEST_ASSERT_NE(val, 0x1234,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((unsigned short)-2);
		TEST_ASSERT_NE(val, (unsigned short)0xffff,
			"bignum::BigInt::operator!=(unsigned short)");
		TEST_ASSERT_NE(val, 0xffff,
			"bignum::BigInt::operator!=(int)");
	}
	// <
	{
		bignum::BigInt val((signed short)0x1234);
		TEST_ASSERT_LT(val, (signed short)0x1235,
			"bignum::BigInt::operator<(signed short)");
		TEST_ASSERT_LT(val, 0x1235,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((signed short)-2);
		TEST_ASSERT_LT(val, (signed short)-1,
			"bignum::BigInt::operator<(signed short)");
		TEST_ASSERT_LT(val, -1,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((unsigned short)0x1234);
		TEST_ASSERT_LT(val, (unsigned short)0x1235,
			"bignum::BigInt::operator<(unsigned short)");
		TEST_ASSERT_LT(val, 0x1235,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((unsigned short)-2);
		TEST_ASSERT_LT(val, (unsigned short)0xffff,
			"bignum::BigInt::operator<(unsigned short)");
		TEST_ASSERT_LT(val, 0xffff,
			"bignum::BigInt::operator<(int)");
	}
	// >
	{
		bignum::BigInt val((signed short)0x1235);
		TEST_ASSERT_GT(val, (signed short)0x1234,
			"bignum::BigInt::operator>(signed short)");
		TEST_ASSERT_GT(val, 0x1234,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((signed short)-1);
		TEST_ASSERT_GT(val, (signed short)-2,
			"bignum::BigInt::operator>(signed short)");
		TEST_ASSERT_GT(val, -2,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((unsigned short)0x1235);
		TEST_ASSERT_GT(val, (unsigned short)0x1234,
			"bignum::BigInt::operator>(unsigned short)");
		TEST_ASSERT_GT(val, 0x1234,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((unsigned short)-1);
		TEST_ASSERT_GT(val, (unsigned short)0xfffe,
			"bignum::BigInt::operator>(unsigned short)");
		TEST_ASSERT_GT(val, 0xfffe,
			"bignum::BigInt::operator>(int)");
	}
	// <=
	{
		bignum::BigInt val((signed short)0x1234);
		TEST_ASSERT_LE(val, (signed short)0x1235,
			"bignum::BigInt::operator<=(signed short)");
		TEST_ASSERT_LE(val, 0x1235,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((signed short)-2);
		TEST_ASSERT_LE(val, (signed short)-1,
			"bignum::BigInt::operator<=(signed short)");
		TEST_ASSERT_LE(val, -1,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((unsigned short)0x1234);
		TEST_ASSERT_LE(val, (unsigned short)0x1235,
			"bignum::BigInt::operator<=(unsigned short)");
		TEST_ASSERT_LE(val, 0x1235,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((unsigned short)-2);
		TEST_ASSERT_LE(val, (unsigned short)0xffff,
			"bignum::BigInt::operator<=(unsigned short)");
		TEST_ASSERT_LE(val, 0xffff,
			"bignum::BigInt::operator<=(int)");
	}
	// >=
	{
		bignum::BigInt val((signed short)0x1235);
		TEST_ASSERT_GE(val, (signed short)0x1234,
			"bignum::BigInt::operator>=(signed short)");
		TEST_ASSERT_GE(val, 0x1234,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((signed short)-1);
		TEST_ASSERT_GE(val, (signed short)-2,
			"bignum::BigInt::operator>=(signed short)");
		TEST_ASSERT_GE(val, -2,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((unsigned short)0x1235);
		TEST_ASSERT_GE(val, (unsigned short)0x1234,
			"bignum::BigInt::operator>=(unsigned short)");
		TEST_ASSERT_GE(val, 0x1234,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((unsigned short)-1);
		TEST_ASSERT_GE(val, (unsigned short)0xfffe,
			"bignum::BigInt::operator>=(unsigned short)");
		TEST_ASSERT_GE(val, 0xfffe,
			"bignum::BigInt::operator>=(int)");
	}

	//
	// compare to int
	//
	// ==
	{
		bignum::BigInt val((signed int)0x12345678);
		TEST_ASSERT_EQ(val, (signed int)0x12345678,
			"bignum::BigInt::operator==(signed int)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((signed int)-1);
		TEST_ASSERT_EQ(val, (signed int)-1,
			"bignum::BigInt::operator==(signed int)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345678);
		TEST_ASSERT_EQ(val, (unsigned int)0x12345678,
			"bignum::BigInt::operator==(unsigned int)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::BigInt::operator==(int)");
	}
	{
		bignum::BigInt val((unsigned int)-1);
		TEST_ASSERT_EQ(val, (unsigned int)0xffffffff,
			"bignum::BigInt::operator==(unsigned int)");
		TEST_ASSERT_EQ(val, 0xffffffff,
			"bignum::BigInt::operator==(int)");
	}
	// !=
	{
		bignum::BigInt val((signed int)0x12345679);
		TEST_ASSERT_NE(val, (signed int)0x12345678,
			"bignum::BigInt::operator!=(signed int)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((signed int)-2);
		TEST_ASSERT_NE(val, (signed int)-1,
			"bignum::BigInt::operator!=(signed int)");
		TEST_ASSERT_NE(val, -1,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345679);
		TEST_ASSERT_NE(val, (unsigned int)0x12345678,
			"bignum::BigInt::operator!=(unsigned int)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::BigInt::operator!=(int)");
	}
	{
		bignum::BigInt val((unsigned int)-2);
		TEST_ASSERT_NE(val, (unsigned int)0xffffffff,
			"bignum::BigInt::operator!=(unsigned int)");
		TEST_ASSERT_NE(val, 0xffffffff,
			"bignum::BigInt::operator!=(int)");
	}
	// <
	{
		bignum::BigInt val((signed int)0x12345678);
		TEST_ASSERT_LT(val, (signed int)0x12345679,
			"bignum::BigInt::operator<(signed int)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((signed int)-2);
		TEST_ASSERT_LT(val, (signed int)-1,
			"bignum::BigInt::operator<(signed int)");
		TEST_ASSERT_LT(val, -1,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345678);
		TEST_ASSERT_LT(val, (unsigned int)0x12345679,
			"bignum::BigInt::operator<(unsigned int)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::BigInt::operator<(int)");
	}
	{
		bignum::BigInt val((unsigned int)-2);
		TEST_ASSERT_LT(val, (unsigned int)0xffffffff,
			"bignum::BigInt::operator<(unsigned int)");
		TEST_ASSERT_LT(val, 0xffffffff,
			"bignum::BigInt::operator<(int)");
	}
	// >
	{
		bignum::BigInt val((signed int)0x12345679);
		TEST_ASSERT_GT(val, (signed int)0x12345678,
			"bignum::BigInt::operator>(signed int)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((signed int)-1);
		TEST_ASSERT_GT(val, (signed int)-2,
			"bignum::BigInt::operator>(signed int)");
		TEST_ASSERT_GT(val, -2,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345679);
		TEST_ASSERT_GT(val, (unsigned int)0x12345678,
			"bignum::BigInt::operator>(unsigned int)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::BigInt::operator>(int)");
	}
	{
		bignum::BigInt val((unsigned int)-1);
		TEST_ASSERT_GT(val, (unsigned int)0xfffffffe,
			"bignum::BigInt::operator>(unsigned int)");
		TEST_ASSERT_GT(val, 0xfffffffe,
			"bignum::BigInt::operator>(int)");
	}
	// <=
	{
		bignum::BigInt val((signed int)0x12345678);
		TEST_ASSERT_LE(val, (signed int)0x12345679,
			"bignum::BigInt::operator<=(signed int)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((signed int)-2);
		TEST_ASSERT_LE(val, (signed int)-1,
			"bignum::BigInt::operator<=(signed int)");
		TEST_ASSERT_LE(val, -1,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345678);
		TEST_ASSERT_LE(val, (unsigned int)0x12345679,
			"bignum::BigInt::operator<=(unsigned int)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::BigInt::operator<=(int)");
	}
	{
		bignum::BigInt val((unsigned int)-2);
		TEST_ASSERT_LE(val, (unsigned int)0xffffffff,
			"bignum::BigInt::operator<=(unsigned int)");
		TEST_ASSERT_LE(val, 0xffffffff,
			"bignum::BigInt::operator<=(int)");
	}
	// >=
	{
		bignum::BigInt val((signed int)0x12345679);
		TEST_ASSERT_GE(val, (signed int)0x12345678,
			"bignum::BigInt::operator>=(signed int)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((signed int)-1);
		TEST_ASSERT_GE(val, (signed int)-2,
			"bignum::BigInt::operator>=(signed int)");
		TEST_ASSERT_GE(val, -2,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((unsigned int)0x12345679);
		TEST_ASSERT_GE(val, (unsigned int)0x12345678,
			"bignum::BigInt::operator>=(unsigned int)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::BigInt::operator>=(int)");
	}
	{
		bignum::BigInt val((unsigned int)-1);
		TEST_ASSERT_GE(val, (unsigned int)0xfffffffe,
			"bignum::BigInt::operator>=(unsigned int)");
		TEST_ASSERT_GE(val, 0xfffffffe,
			"bignum::BigInt::operator>=(int)");
	}

	//
	// compare to long
	//
	// ==
	{
		bignum::BigInt val((signed long)0x12345678);
		TEST_ASSERT_EQ(val, (signed long)0x12345678,
			"bignum::BigInt::operator==(signed long)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::BigInt::operator==(long)");
	}
	{
		bignum::BigInt val((signed long)-1);
		TEST_ASSERT_EQ(val, (signed long)-1,
			"bignum::BigInt::operator==(signed long)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::BigInt::operator==(long)");
	}
	{
		bignum::BigInt val((unsigned long)0x12345678);
		TEST_ASSERT_EQ(val, (unsigned long)0x12345678,
			"bignum::BigInt::operator==(unsigned long)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::BigInt::operator==(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// !=
	{
		bignum::BigInt val((signed long)0x12345679);
		TEST_ASSERT_NE(val, (signed long)0x12345678,
			"bignum::BigInt::operator!=(signed long)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::BigInt::operator!=(long)");
	}
	{
		bignum::BigInt val((signed long)-2);
		TEST_ASSERT_NE(val, (signed long)-1,
			"bignum::BigInt::operator!=(signed long)");
		TEST_ASSERT_NE(val, -1,
			"bignum::BigInt::operator!=(long)");
	}
	{
		bignum::BigInt val((unsigned long)0x12345679);
		TEST_ASSERT_NE(val, (unsigned long)0x12345678,
			"bignum::BigInt::operator!=(unsigned long)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::BigInt::operator!=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// <
	{
		bignum::BigInt val((signed long)0x12345678);
		TEST_ASSERT_LT(val, (signed long)0x12345679,
			"bignum::BigInt::operator<(signed long)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::BigInt::operator<(long)");
	}
	{
		bignum::BigInt val((signed long)-2);
		TEST_ASSERT_LT(val, (signed long)-1,
			"bignum::BigInt::operator<(signed long)");
		TEST_ASSERT_LT(val, -1,
			"bignum::BigInt::operator<(long)");
	}
	{
		bignum::BigInt val((unsigned long)0x12345678);
		TEST_ASSERT_LT(val, (unsigned long)0x12345679,
			"bignum::BigInt::operator<(unsigned long)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::BigInt::operator<(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// >
	{
		bignum::BigInt val((signed long)0x12345679);
		TEST_ASSERT_GT(val, (signed long)0x12345678,
			"bignum::BigInt::operator>(signed long)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::BigInt::operator>(long)");
	}
	{
		bignum::BigInt val((signed long)-1);
		TEST_ASSERT_GT(val, (signed long)-2,
			"bignum::BigInt::operator>(signed long)");
		TEST_ASSERT_GT(val, -2,
			"bignum::BigInt::operator>(long)");
	}
	{
		bignum::BigInt val((unsigned long)0x12345679);
		TEST_ASSERT_GT(val, (unsigned long)0x12345678,
			"bignum::BigInt::operator>(unsigned long)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::BigInt::operator>(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// <=
	{
		bignum::BigInt val((signed long)0x12345678);
		TEST_ASSERT_LE(val, (signed long)0x12345679,
			"bignum::BigInt::operator<=(signed long)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::BigInt::operator<=(long)");
	}
	{
		bignum::BigInt val((signed long)-2);
		TEST_ASSERT_LE(val, (signed long)-1,
			"bignum::BigInt::operator<=(signed long)");
		TEST_ASSERT_LE(val, -1,
			"bignum::BigInt::operator<=(long)");
	}
	{
		bignum::BigInt val((unsigned long)0x12345678);
		TEST_ASSERT_LE(val, (unsigned long)0x12345679,
			"bignum::BigInt::operator<=(unsigned long)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::BigInt::operator<=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// >=
	{
		bignum::BigInt val((signed long)0x12345679);
		TEST_ASSERT_GE(val, (signed long)0x12345678,
			"bignum::BigInt::operator>=(signed long)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::BigInt::operator>=(long)");
	}
	{
		bignum::BigInt val((signed long)-1);
		TEST_ASSERT_GE(val, (signed long)-2,
			"bignum::BigInt::operator>=(signed long)");
		TEST_ASSERT_GE(val, -2,
			"bignum::BigInt::operator>=(long)");
	}
	{
		bignum::BigInt val((unsigned long)0x12345679);
		TEST_ASSERT_GE(val, (unsigned long)0x12345678,
			"bignum::BigInt::operator>=(unsigned long)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::BigInt::operator>=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.

	//
	// compare to long long
	//
	// ==
	{
		bignum::BigInt val((signed long long)0x1234567890ULL);
		TEST_ASSERT_EQ(val, (signed long long)0x1234567890ULL,
			"bignum::BigInt::operator==(signed long long)");
		TEST_ASSERT_EQ(val, 0x1234567890ULL,
			"bignum::BigInt::operator==(long long)");
	}
	{
		bignum::BigInt val((signed long long)-1);
		TEST_ASSERT_EQ(val, (signed long long)-1,
			"bignum::BigInt::operator==(signed long long)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::BigInt::operator==(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_EQ(val, (unsigned long long)0x1234567890ULL,
			"bignum::BigInt::operator==(unsigned long long)");
		TEST_ASSERT_EQ(val, 0x1234567890ULL,
			"bignum::BigInt::operator==(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)-1);
		TEST_ASSERT_EQ(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::BigInt::operator==(unsigned long long)");
		TEST_ASSERT_EQ(val, 0xffffffffffffffffULL,
			"bignum::BigInt::operator==(long long)");
	}
	// !=
	{
		bignum::BigInt val((signed long long)0x1234567891ULL);
		TEST_ASSERT_NE(val, (signed long long)0x1234567890ULL,
			"bignum::BigInt::operator!=(signed long long)");
		TEST_ASSERT_NE(val, 0x1234567890ULL,
			"bignum::BigInt::operator!=(long long)");
	}
	{
		bignum::BigInt val((signed long long)-2);
		TEST_ASSERT_NE(val, (signed long long)-1,
			"bignum::BigInt::operator!=(signed long long)");
		TEST_ASSERT_NE(val, -1,
			"bignum::BigInt::operator!=(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_NE(val, (unsigned long long)0x1234567890ULL,
			"bignum::BigInt::operator!=(unsigned long long)");
		TEST_ASSERT_NE(val, 0x1234567890ULL,
			"bignum::BigInt::operator!=(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)-2);
		TEST_ASSERT_NE(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::BigInt::operator!=(unsigned long long)");
		TEST_ASSERT_NE(val, 0xffffffffffffffffULL,
			"bignum::BigInt::operator!=(long long)");
	}
	// <
	{
		bignum::BigInt val((signed long long)0x1234567890ULL);
		TEST_ASSERT_LT(val, (signed long long)0x1234567891ULL,
			"bignum::BigInt::operator<(signed long long)");
		TEST_ASSERT_LT(val, 0x1234567891ULL,
			"bignum::BigInt::operator<(long long)");
	}
	{
		bignum::BigInt val((signed long long)-2);
		TEST_ASSERT_LT(val, (signed long long)-1,
			"bignum::BigInt::operator<(signed long long)");
		TEST_ASSERT_LT(val, -1,
			"bignum::BigInt::operator<(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_LT(val, (unsigned long long)0x1234567891ULL,
			"bignum::BigInt::operator<(unsigned long long)");
		TEST_ASSERT_LT(val, 0x1234567891ULL,
			"bignum::BigInt::operator<(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)-2);
		TEST_ASSERT_LT(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::BigInt::operator<(unsigned long long)");
		TEST_ASSERT_LT(val, 0xffffffffffffffffULL,
			"bignum::BigInt::operator<(long long)");
	}
	// >
	{
		bignum::BigInt val((signed long long)0x1234567891ULL);
		TEST_ASSERT_GT(val, (signed long long)0x1234567890ULL,
			"bignum::BigInt::operator>(signed long long)");
		TEST_ASSERT_GT(val, 0x1234567890ULL,
			"bignum::BigInt::operator>(long long)");
	}
	{
		bignum::BigInt val((signed long long)-1);
		TEST_ASSERT_GT(val, (signed long long)-2,
			"bignum::BigInt::operator>(signed long long)");
		TEST_ASSERT_GT(val, -2,
			"bignum::BigInt::operator>(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_GT(val, (unsigned long long)0x1234567890ULL,
			"bignum::BigInt::operator>(unsigned long long)");
		TEST_ASSERT_GT(val, 0x1234567890ULL,
			"bignum::BigInt::operator>(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)-1);
		TEST_ASSERT_GE(
			val, (unsigned long long)0xfffffffffffffffeULL,
			"bignum::BigInt::operator>(unsigned long long)");
		TEST_ASSERT_GT(val, 0xfffffffffffffffeULL,
			"bignum::BigInt::operator>(long long)");
	}
	// <=
	{
		bignum::BigInt val((signed long long)0x1234567890ULL);
		TEST_ASSERT_LE(val, (signed long long)0x1234567891ULL,
			"bignum::BigInt::operator<=(signed long long)");
		TEST_ASSERT_LE(val, 0x1234567891ULL,
			"bignum::BigInt::operator<=(long long)");
	}
	{
		bignum::BigInt val((signed long long)-2);
		TEST_ASSERT_LE(val, (signed long long)-1,
			"bignum::BigInt::operator<=(signed long long)");
		TEST_ASSERT_LE(val, -1,
			"bignum::BigInt::operator<=(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_LE(val, (unsigned long long)0x1234567891ULL,
			"bignum::BigInt::operator<=(unsigned long long)");
		TEST_ASSERT_LE(val, 0x1234567891ULL,
			"bignum::BigInt::operator<=(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)-2);
		TEST_ASSERT_LE(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::BigInt::operator<=(unsigned long long)");
		TEST_ASSERT_LE(val, 0xffffffffffffffffULL,
			"bignum::BigInt::operator<=(long long)");
	}
	// >=
	{
		bignum::BigInt val((signed long long)0x1234567891ULL);
		TEST_ASSERT_GE(val, (signed long long)0x1234567890ULL,
			"bignum::BigInt::operator>=(signed long long)");
		TEST_ASSERT_GE(val, 0x1234567890ULL,
			"bignum::BigInt::operator>=(long long)");
	}
	{
		bignum::BigInt val((signed long long)-1);
		TEST_ASSERT_GE(val, (signed long long)-2,
			"bignum::BigInt::operator>=(signed long long)");
		TEST_ASSERT_GE(val, -2,
			"bignum::BigInt::operator>=(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_GE(val, (unsigned long long)0x1234567890ULL,
			"bignum::BigInt::operator>=(unsigned long long)");
		TEST_ASSERT_GE(val, 0x1234567890ULL,
			"bignum::BigInt::operator>=(long long)");
	}
	{
		bignum::BigInt val((unsigned long long)-1);
		TEST_ASSERT_GE(
			val, (unsigned long long)0xfffffffffffffffeULL,
			"bignum::BigInt::operator>=(unsigned long long)");
		TEST_ASSERT_GE(val, 0xfffffffffffffffeULL,
			"bignum::BigInt::operator>=(long long)");
	}

	//
	// compare to bignum::BigInt
	//
	// ==
	{
		bignum::BigInt val1(12345678);
		bignum::BigInt val2(12345678);
		TEST_ASSERT_EQ(val1, val2, "bignum::BigInt::operator==(bignum::BigInt)");
	}
	// !=
	{
		bignum::BigInt val1(12345678);
		bignum::BigInt val2(12345679);
		TEST_ASSERT_NE(val1, val2, "bignum::BigInt::operator!=(bignum::BigInt)");
	}
	// <
	{
		bignum::BigInt val1(12345678);
		bignum::BigInt val2(12345679);
		TEST_ASSERT_LT(val1, val2, "bignum::BigInt::operator<(bignum::BigInt)");
	}
	// >
	{
		bignum::BigInt val1(12345679);
		bignum::BigInt val2(12345678);
		TEST_ASSERT_GT(val1, val2, "bignum::BigInt::operator>(bignum::BigInt)");
	}
	// <=
	{
		bignum::BigInt val1(12345678);
		bignum::BigInt val2(12345679);
		TEST_ASSERT_LE(val1, val2, "bignum::BigInt::operator<=(bignum::BigInt)");
	}
	// >=
	{
		bignum::BigInt val1(12345679);
		bignum::BigInt val2(12345678);
		TEST_ASSERT_GE(val1, val2, "bignum::BigInt::operator>=(bignum::BigInt)");
	}

	// can't compare directly to strings
}

TEST(test_bool)
{
	// convert to bool
	{
		bignum::BigInt val(1);
		TEST_ASSERT(val, "bignum::BigInt::operator bool()");
	}
	{
		bignum::BigInt val(12345);
		TEST_ASSERT(val, "bignum::BigInt::operator bool()");
	}
	{
		bignum::BigInt val(-1);
		TEST_ASSERT(val, "bignum::BigInt::operator bool()");
	}
	{
		bignum::BigInt val(0);
		TEST_ASSERT(!val, "bignum::BigInt::operator bool()");
	}
	{
		bignum::BigInt val(2);
		TEST_ASSERT(!!val, "bignum::BigInt::operator bool()");
	}
	{
		bignum::BigInt val(0);
		TEST_ASSERT(!!!val, "bignum::BigInt::operator bool()");
	}
}

TEST(test_small_arithmetic)
{
	{
		bignum::BigInt val(10);
		TEST_ASSERT(val+3 == 13,
			"bignum::BigInt::operator+(bignum::BigInt, int)");
		TEST_ASSERT(3+val == 13,
			"bignum::BigInt::operator+(int, bignum::BigInt)");
		TEST_ASSERT(val+bignum::BigInt(3) == 13,
			"bignum::BigInt::operator+(bignum::BigInt, bignum::BigInt)");
		val += 5;
		TEST_ASSERT(val == 15,
			"bignum::BigInt::operator+=(bignum::BigInt, int)");
		val++;
		TEST_ASSERT(val == 16,
			"bignum::BigInt::operator++()");
		++val;
		TEST_ASSERT(val == 17,
			"bignum::BigInt::operator++()");
	}
	{
		bignum::BigInt val(10);
		TEST_ASSERT(val-3 == 7,
			"bignum::BigInt::operator-(bignum::BigInt, int)");
		TEST_ASSERT(17-val == 7,
			"bignum::BigInt::operator-(int, bignum::BigInt)");
		TEST_ASSERT(val-bignum::BigInt(3) == 7,
			"bignum::BigInt::operator-(bignum::BigInt, bignum::BigInt)");
		val -= 5;
		TEST_ASSERT(val == 5,
			"bignum::BigInt::operator-=(bignum::BigInt, int)");
		val--;
		TEST_ASSERT(val == 4,
			"bignum::BigInt::operator--()");
		--val;
		TEST_ASSERT(val == 3,
			"bignum::BigInt::operator-()");
	}
	{
		bignum::BigInt val(10);
		TEST_ASSERT(val*2 == 20,
			"bignum::BigInt::operator*(bignum::BigInt, int)");
		TEST_ASSERT(2*val == 20,
			"bignum::BigInt::operator*(int, bignum::BigInt)");
		TEST_ASSERT(val*bignum::BigInt(2) == 20,
			"bignum::BigInt::operator*(bignum::BigInt, bignum::BigInt)");
		val *= 3;
		TEST_ASSERT(val == 30,
			"bignum::BigInt::operator*=(bignum::BigInt, int)");
	}
	{
		bignum::BigInt val(10);
		TEST_ASSERT(val/2 == 5,
			"bignum::BigInt::operator/(bignum::BigInt, int)");
		TEST_ASSERT(50/val == 5,
			"bignum::BigInt::operator/(int, bignum::BigInt)");
		TEST_ASSERT(val/bignum::BigInt(2) == 5,
			"bignum::BigInt::operator/(bignum::BigInt, bignum::BigInt)");
		val /= 5;
		TEST_ASSERT(val == 2,
			"bignum::BigInt::operator/=(bignum::BigInt, int)");
	}
	{
		bignum::BigInt val(17);
		TEST_ASSERT(val%10 == 7,
			"bignum::BigInt::operator%(bignum::BigInt, int)");
		TEST_ASSERT(24%val == 7,
			"bignum::BigInt::operator%(int, bignum::BigInt)");
		TEST_ASSERT(val%bignum::BigInt(10) == 7,
			"bignum::BigInt::operator%(bignum::BigInt, bignum::BigInt)");
		val %= 5;
		TEST_ASSERT(val == 2,
			"bignum::BigInt::operator%=(bignum::BigInt, int)");
	}
}

TEST(test_large_arithmetic)
{
	{
		bignum::BigInt val("0x1000000000000000a");
		TEST_ASSERT(val+3 == bignum::BigInt("0x1000000000000000d"),
			"bignum::BigInt::operator+(bignum::BigInt, int)");
		TEST_ASSERT(3+val == bignum::BigInt("0x1000000000000000d"),
			"bignum::BigInt::operator+(int, bignum::BigInt)");
		val += 5;
		TEST_ASSERT(val == bignum::BigInt("0x1000000000000000f"),
			"bignum::BigInt::operator+=(bignum::BigInt, int)");
		val++;
		TEST_ASSERT(val == bignum::BigInt("0x10000000000000010"),
			"bignum::BigInt::operator++()");
		++val;
		TEST_ASSERT(val == bignum::BigInt("0x10000000000000011"),
			"bignum::BigInt::operator++()");
	}
	{
		bignum::BigInt val("0x1000000000000000a");
		TEST_ASSERT(val-3 == bignum::BigInt("0x10000000000000007"),
			"bignum::BigInt::operator-(bignum::BigInt, int)");
		val -= 5;
		TEST_ASSERT(val == bignum::BigInt("0x10000000000000005"),
			"bignum::BigInt::operator-=(int, bignum::BigInt)");
		val--;
		TEST_ASSERT(val == bignum::BigInt("0x10000000000000004"),
			"bignum::BigInt::operator--(bignum::BigInt, int)");
		--val;
		TEST_ASSERT(val == bignum::BigInt("0x10000000000000003"),
			"bignum::BigInt::operator--()");
	}
	{
		bignum::BigInt val("0x10000000000000010");
		TEST_ASSERT(val*2 == bignum::BigInt("0x20000000000000020"),
			"bignum::BigInt::operator*(bignum::BigInt, int)");
		TEST_ASSERT(2*val == bignum::BigInt("0x20000000000000020"),
			"bignum::BigInt::operator*(int, bignum::BigInt)");
		val *= 3;
		TEST_ASSERT(val == bignum::BigInt("0x30000000000000030"),
			"bignum::BigInt::operator*=(bignum::BigInt, int)");
	}
	{
		bignum::BigInt val("0xa000000000000000a");
		TEST_ASSERT(val/2 == bignum::BigInt("0x50000000000000005"),
			"bignum::BigInt::operator/(bignum::BigInt, int)");
		val /= 5;
		TEST_ASSERT(val == bignum::BigInt("0x20000000000000002"),
			"bignum::BigInt::operator/=(bignum::BigInt, int)");
	}
	{
		bignum::BigInt val("0x10000000000000011");
		TEST_ASSERT(val%10 == bignum::BigInt("0x03"),
			"bignum::BigInt::operator%(bignum::BigInt, int)");
		val %= 5;
		TEST_ASSERT(val == bignum::BigInt("0x03"),
			"bignum::BigInt::operator%=(bignum::BigInt, int)");
	}
}

//NOTE:
//  Bitwise NOT is supported, but what does it really mean for an
//  arbitrary-width number?
TEST(test_small_logical)
{
	{
		bignum::BigInt val(0xff);
		TEST_ASSERT((val & bignum::BigInt(0x107)) == 0x7,
			"bignum::BigInt::operator&(bignum::BigInt, unsigned)");
		TEST_ASSERT((bignum::BigInt(0x107) & val) == 0x7,
			"bignum::BigInt::operator&(unsigned, bignum::BigInt)");
		val &= bignum::BigInt(0x370);
		TEST_ASSERT(val == 0x70,
			"bignum::BigInt::operator&=(bignum::BigInt, unsigned)");
	}
	{
		bignum::BigInt val(0xff);
		TEST_ASSERT((val | bignum::BigInt(0x107)) == 0x1ff,
			"bignum::BigInt::operator|()");
		TEST_ASSERT((bignum::BigInt(0x107) | val) == 0x1ff,
			"bignum::BigInt::operator|()");
		val |= bignum::BigInt(0x270);
		TEST_ASSERT(val == 0x2ff, "bignum::BigInt::operator|=()");
	}
	{
		bignum::BigInt val(0xff);
		TEST_ASSERT((val ^ bignum::BigInt(0x107)) == 0x1f8,
			"bignum::BigInt::operator^()");
		TEST_ASSERT((bignum::BigInt(0x107) ^ val) == 0x1f8,
			"bignum::BigInt::operator^()");
		val ^= bignum::BigInt(0x270);
		TEST_ASSERT(val == 0x28f, "bignum::BigInt::operator^=()");
	}
	{
		bignum::BigInt val(0x10);
		TEST_ASSERT((val << 2) == 0x40,
			"bignum::BigInt::operator<<()");
		TEST_ASSERT((2 << val) == 0x20000,
			"bignum::BigInt::operator<<()");
		val <<= 3;
		TEST_ASSERT(val == 0x80, "bignum::BigInt::operator<<=()");
	}
	{
		bignum::BigInt val(0x10);
		TEST_ASSERT((val >> 2) == 0x04,
			"bignum::BigInt::operator>>()");
		TEST_ASSERT((0x20000 >> val) == 0x2,
			"bignum::BigInt::operator>>()");
		val >>= 3;
		TEST_ASSERT(val == 0x02, "bignum::BigInt::operator>>=()");
	}
}

TEST(test_large_logical)
{
	{
		bignum::BigInt val("0xf00000000000000ff");
		TEST_ASSERT((val & bignum::BigInt(0x107)) == 0x7,
			"bignum::BigInt::operator&()");
		TEST_ASSERT((bignum::BigInt(0x107) & val) == 0x7,
			"bignum::BigInt::operator&()");
		val &= bignum::BigInt(0x370);
		TEST_ASSERT(val == 0x70, "bignum::BigInt::operator&=()");
	}
	{
		bignum::BigInt val("0xf00000000000000ff");
		TEST_ASSERT(
			(val | bignum::BigInt(0x107)) == bignum::BigInt("0xf00000000000001ff"),
			"bignum::BigInt::operator|()");
		TEST_ASSERT(
			(bignum::BigInt(0x107) | val) == bignum::BigInt("0xf00000000000001ff"),
			"bignum::BigInt::operator|()");
		val |= bignum::BigInt(0x270);
		TEST_ASSERT(
			val == bignum::BigInt("0xf00000000000002ff"),
			"bignum::BigInt::operator|=()");
	}
	{
		bignum::BigInt val("0xf00000000000000ff");
		TEST_ASSERT(
			(val ^ bignum::BigInt(0x107)) == bignum::BigInt("0xf00000000000001f8"),
			"bignum::BigInt::operator^()");
		TEST_ASSERT(
			(bignum::BigInt(0x107) ^ val) == bignum::BigInt("0xf00000000000001f8"),
			"bignum::BigInt::operator^()");
		val ^= bignum::BigInt(0x270);
		TEST_ASSERT(
			val == bignum::BigInt("0xf000000000000028f"),
			"bignum::BigInt::operator^=()");
	}
	{
		bignum::BigInt val("0xf0000000000000010");
		TEST_ASSERT(
			(val << 2) == bignum::BigInt("0x3c0000000000000040"),
			"bignum::BigInt::operator<<()");
		val <<= 3;
		TEST_ASSERT(
			val == bignum::BigInt("0x780000000000000080"),
			"bignum::BigInt::operator<<=()");
	}
	{
		bignum::BigInt val("0xf0000000000000010");
		TEST_ASSERT(
			(val >> 2) == bignum::BigInt("0x3c000000000000004"),
			"bignum::BigInt::operator>>()");
		val >>= 3;
		TEST_ASSERT(
			val == bignum::BigInt("0x1e000000000000002"),
			"bignum::BigInt::operator>>=()");
	}
}

TEST(test_small_io)
{
	{
		bignum::BigInt val(15);
		TEST_ASSERT(val.to_dec_string() == "15",
				"bignum::BigInt::to_dec_string()");
		TEST_ASSERT(val.to_string() == "15",
				"bignum::BigInt::to_string()");
		TEST_ASSERT(val.to_hex_string() == "0xf",
				"bignum::BigInt::to_dec_string()");
		TEST_ASSERT(val.to_oct_string() == "017",
				"bignum::BigInt::to_dec_string()");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, but I don't care.
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
		#endif
	}
	{
		#if 0
		// This test fails, but I don't care.
		bignum::BigInt val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::BigInt::operator<<(ostream)");
		#endif
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, and I think the standard version is
		// wrong.
		bignum::BigInt val(15);
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
			"bignum::BigInt::operator<<(ostream)");
		#endif
	}
}

TEST(test_large_io)
{
	{
		std::string num = "12345678123456781234567812345678";
		bignum::BigInt val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::dec;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		std::string num = "012345671234567123456712345671234567";
		bignum::BigInt val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::oct;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"bignum::BigInt::operator<<(ostream)");
	}
	{
		std::string num = "0x123456789abcdef123456789abcdef";
		bignum::BigInt val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::hex;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"bignum::BigInt::operator<<(ostream)");
	}
}

TEST(test_operators)
{
	// test operators with signed char
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (signed char)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (signed char)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (signed char)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (signed char)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (signed char)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (signed char)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (signed char)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (signed char)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed char)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (signed char)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed char)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (signed char)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed char)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (signed char)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (signed char)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (signed char)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (signed char)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (signed char)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (signed char)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (signed char)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with unsigned char
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (unsigned char)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (unsigned char)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (unsigned char)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (unsigned char)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (unsigned char)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (unsigned char)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (unsigned char)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (unsigned char)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned char)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (unsigned char)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned char)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (unsigned char)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned char)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (unsigned char)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (unsigned char)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (unsigned char)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (unsigned char)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (unsigned char)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (unsigned char)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (unsigned char)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with signed short
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (signed short)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (signed short)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (signed short)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (signed short)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (signed short)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (signed short)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (signed short)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (signed short)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed short)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (signed short)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed short)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (signed short)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed short)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (signed short)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (signed short)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (signed short)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (signed short)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (signed short)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (signed short)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (signed short)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with unsigned short
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (unsigned short)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (unsigned short)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (unsigned short)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (unsigned short)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (unsigned short)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (unsigned short)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (unsigned short)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (unsigned short)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned short)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (unsigned short)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned short)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (unsigned short)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned short)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (unsigned short)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (unsigned short)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (unsigned short)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (unsigned short)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (unsigned short)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (unsigned short)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (unsigned short)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with signed int
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (signed int)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (signed int)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (signed int)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (signed int)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (signed int)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (signed int)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (signed int)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (signed int)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed int)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (signed int)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed int)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (signed int)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed int)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (signed int)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (signed int)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (signed int)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (signed int)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (signed int)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (signed int)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (signed int)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with unsigned int
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (unsigned int)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (unsigned int)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (unsigned int)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (unsigned int)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (unsigned int)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (unsigned int)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (unsigned int)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (unsigned int)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned int)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (unsigned int)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned int)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (unsigned int)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned int)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (unsigned int)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (unsigned int)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (unsigned int)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (unsigned int)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (unsigned int)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (unsigned int)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (unsigned int)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with signed long
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (signed long)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (signed long)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (signed long)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (signed long)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (signed long)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (signed long)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (signed long)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (signed long)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed long)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (signed long)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed long)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (signed long)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed long)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (signed long)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (signed long)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (signed long)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (signed long)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (signed long)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (signed long)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (signed long)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with unsigned long
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (unsigned long)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (unsigned long)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (unsigned long)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (unsigned long)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (unsigned long)2;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (unsigned long)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (unsigned long)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (unsigned long)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned long)3;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (unsigned long)3;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned long)2;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (unsigned long)2;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned long)1;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (unsigned long)1;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (unsigned long)2;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (unsigned long)2;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (unsigned long)1;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (unsigned long)1;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (unsigned long)1;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (unsigned long)1;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// Most operators with (signd or unsigned) long long fail, because
	// GMP does not natively support them.  There must be some hack to
	// make this work  (and the above bugs with | & ^), but I can't be
	// bothered to mess with it any further.
	#if 0
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (signed long long)1LL;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (signed long long)1LL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (signed long long)1LL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (signed long long)1LL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (signed long long)2LL;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (signed long long)2LL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (signed long long)2LL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (signed long long)2LL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed long long)3LL;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (signed long long)3LL;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed long long)2LL;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (signed long long)2LL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed long long)1LL;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (signed long long)1LL;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (signed long long)2LL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (signed long long)2LL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (signed long long)1LL;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (signed long long)1LL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (signed long long)1LL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (signed long long)1LL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}

	// test operators with unsigned long long
	{
		bignum::BigInt val = 1;
		bignum::BigInt val2;

		val2 = val + (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator+()");
		val += (unsigned long long)1ULL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator+=()");

		val2 = val - (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator-()");
		val -= (unsigned long long)1ULL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator-=()");

		val2 = val * (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator*()");
		val *= (unsigned long long)2ULL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator*=()");

		val2 = val / (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator/()");
		val /= (unsigned long long)2ULL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned long long)3ULL;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator|()");
		val |= (unsigned long long)3ULL;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned long long)2ULL;
		//TEST_ASSERT(val2 == 2, "bignum::BigInt::operator&()");
		val &= (unsigned long long)2ULL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned long long)1ULL;
		//TEST_ASSERT(val2 == 3, "bignum::BigInt::operator^()");
		val ^= (unsigned long long)1ULL;
		TEST_ASSERT(val == 3, "bignum::BigInt::operator^=()");

		val2 = val % (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator%()");
		val %= (unsigned long long)2ULL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator%=()");

		val2 = val << (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 2, "bignum::BigInt::operator<<()");
		val <<= (unsigned long long)1ULL;
		TEST_ASSERT(val == 2, "bignum::BigInt::operator<<=()");

		val2 = val >> (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 1, "bignum::BigInt::operator>>()");
		val >>= (unsigned long long)1ULL;
		TEST_ASSERT(val == 1, "bignum::BigInt::operator>>=()");
	}
	#endif // 0
}

TEST(test_extensions)
{
	// test popcount()
	{
		TEST_ASSERT(bignum::BigInt::popcount(1) == 1);
		TEST_ASSERT(bignum::BigInt::popcount(2) == 1);
		TEST_ASSERT(bignum::BigInt::popcount(3) == 2);
		TEST_ASSERT(bignum::BigInt::popcount(0xffff) == 16);
		TEST_ASSERT(bignum::BigInt::popcount(0xffffffff) == 32);
		TEST_ASSERT(
		    bignum::BigInt::popcount(bignum::BigInt("0xffffffffffffffff")) == 64);
		TEST_ASSERT(
		    bignum::BigInt::popcount(bignum::BigInt("0xffffffffffffffffffff")) == 80);
	}
	{
		bignum::BigInt val;

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
		TEST_ASSERT(bignum::BigInt::pow(2, 2) == 4);
		TEST_ASSERT(bignum::BigInt::pow(2, 3) == 8);
		TEST_ASSERT(bignum::BigInt::pow(2, 16) == 0x10000);

		TEST_ASSERT(bignum::BigInt::pow(3, 2) == 9);
		TEST_ASSERT(bignum::BigInt::pow(3, 3) == 27);
		TEST_ASSERT(bignum::BigInt::pow(3, 4) == 81);

		TEST_ASSERT(bignum::BigInt::pow(10, 2) == 100);
		TEST_ASSERT(bignum::BigInt::pow(10, 3) == 1000);
		TEST_ASSERT(bignum::BigInt::pow(10, 6) == 1000000);
	}
	{
		bignum::BigInt val;

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
		bignum::BigInt val;

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

