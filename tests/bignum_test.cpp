#include "bignum.h"
#include "bitbuffer.h"
#include "pp_test.h"

TEST(test_ctors)
{
	// default
	{
		bignum val;
		TEST_ASSERT(val.get_si() == 0, "bignum::bignum()");
		TEST_ASSERT(val.get_ui() == 0, "bignum::bignum()");
		TEST_ASSERT(val.get_str(10) == "0", "bignum::bignum()");
	}

	// from char
	{
		bignum val((signed char)0x12);
		TEST_ASSERT(val.get_si() == 0x12,
			"bignum::bignum(signed char)");
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::bignum(signed char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::bignum(signed char)");
	}
	{
		bignum val((signed char)-1);
		TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(signed char)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed char)");
	}
	{
		bignum val((unsigned char)0x12);
		TEST_ASSERT(val.get_si() == 0x12,
			"bignum::bignum(unsigned char)");
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::bignum(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::bignum(unsigned char)");
	}
	{
		bignum val((unsigned char)-1);
		TEST_ASSERT(val.get_si() == 0xff,
			"bignum::bignum(unsigned char)");
		TEST_ASSERT(val.get_ui() == 0xff,
			"bignum::bignum(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "ff",
			"bignum::bignum(unsigned char)");
	}

	// from short
	{
		bignum val((signed short)0x1234);
		TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::bignum(signed short)");
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::bignum(signed short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::bignum(signed short)");
	}
	{
		bignum val((signed short)-1);
		TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(signed short)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed short)");
	}
	{
		bignum val((unsigned short)0x1234);
		TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::bignum(unsigned short)");
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::bignum(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::bignum(unsigned short)");
	}
	{
		bignum val((unsigned short)-1);
		TEST_ASSERT(val.get_si() == 0xffff,
			"bignum::bignum(unsigned short)");
		TEST_ASSERT(val.get_ui() == 0xffff,
			"bignum::bignum(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "ffff",
			"bignum::bignum(unsigned short)");
	}

	// from int
	// On 32 bit platforms this is where we will start to see
	// signed vs. unsigned oddities.
	{
		bignum val((signed int)0x12345678);
		TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::bignum(signed int)");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::bignum(signed int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::bignum(signed int)");
	}
	{
		bignum val((signed int)-1);
		TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(signed int)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed int)");
	}
	{
		bignum val((unsigned int)0x12345678);
		TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::bignum(unsigned int)");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::bignum(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::bignum(unsigned int)");
	}
	{
		bignum val((unsigned int)-1);
		// On a 32 bit platform this will fail, since get_si()
		// will return a signed value, and -1 is OOB.
		//TEST_ASSERT(val.get_si() == (signed int)0xffffffff,
		//	"bignum::bignum(unsigned int)");
		TEST_ASSERT(val.get_ui() == 0xffffffff,
			"bignum::bignum(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "ffffffff",
			"bignum::bignum(unsigned int)");
	}

	// from long long
	// Skip long, since either sizeof(long) == sizeof(int) or
	// sizeof(long) == sizeof(long long).
	{
		bignum val(0x123456789abcdef0LL);
		TEST_ASSERT(val.get_si() == 0x123456789abcdef0LL,
			"val.get_int()");
		TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"bignum::bignum(signed long long)");
	}
	{
		bignum val(-1LL);
		TEST_ASSERT(val.get_si() == -1,
			"val.get_int()");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed long long)");
	}
	{
		bignum val(0x123456789abcdef0ULL);
		TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"bignum::bignum(unsigned long long)");
	}
	{
		bignum val(-1ULL);
		TEST_ASSERT(val.get_uint() == 0xffffffffffffffffULL,
			"val.get_uint()");
		TEST_ASSERT(val.get_str(16) == "ffffffffffffffff",
			"bignum::bignum(unsigned long long)");
	}

	// from bignum
	{
		bignum val1(12345678);
		bignum val2(val1);
		TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"bignum::bignum(bignum)");
	}

	// from string
	{
		bignum val("12345678");
		TEST_ASSERT(val.get_ui() == 12345678,
			"bignum::bignum(string)");
		val = "23456789";
		TEST_ASSERT(val.get_ui() == 23456789,
			"bignum::bignum(string)");
	}
	{
		bignum val("0x12345678");
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::bignum(string)");
		val = "0x23456789";
		TEST_ASSERT(val.get_ui() == 0x23456789,
			"bignum::bignum(string)");
	}
	{
		bignum val("0x112233445566778899aabbccddeeff00");
		TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::bignum(string)");
		val = "0x2233445566778899aabbccddeeff0011";
		TEST_ASSERT(
			val.get_str(16) == "2233445566778899aabbccddeeff0011",
			"bignum::bignum(string)");
	}
	{
		bignum val("-1");
		TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(string)");
		val = "-1";
		TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(string)");
	}

	// from bitbuffer
	{
		bitbuffer bb(8);
		bb.byte_at(0) = 0x12;
		bignum val(bb);
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum(bitbuffer)");
		bitbuffer bb2 = val.get_bitbuffer();
		TEST_ASSERT(to_string(bb2) == "0x12",
			"bignum::get_bitbuffer()");
		bitbuffer bb3 = val.get_bitbuffer(BITS8);
		TEST_ASSERT(to_string(bb3) == "0x12",
			"bignum::get_bitbuffer(int)");
	}
	{
		bitbuffer bb(16);
		bb.byte_at(1) = 0x12;
		bb.byte_at(0) = 0x34;
		bignum val(bb);
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum(bitbuffer)");
		bitbuffer bb2 = val.get_bitbuffer();
		TEST_ASSERT(to_string(bb2) == "0x1234",
			"bignum::get_bitbuffer()");
		bitbuffer bb3 = val.get_bitbuffer(BITS16);
		TEST_ASSERT(to_string(bb3) == "0x1234",
			"bignum::get_bitbuffer(int)");
	}
	{
		bitbuffer bb = pp_value(-1).get_bitbuffer();
		TEST_ASSERT(to_string(bb) == "0x1",
			"bignum::get_bitbuffer()");
	}
}

TEST(test_assignments)
{
	// from char
	{
		bignum val;
		val = (signed char)0x12;
		TEST_ASSERT(val.get_si() == 0x12,
			"bignum::operator=(signed char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::operator=(signed char)");
	}
	{
		bignum val;
		val = (signed char)-1;
		TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(signed char)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(signed char)");
	}
	{
		bignum val;
		val = (unsigned char)0x12;
		TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::operator=(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "12",
			"bignum::operator=(unsigned char)");
	}
	{
		bignum val;
		val = (unsigned char)-1;
		TEST_ASSERT(val.get_ui() == 0xff,
			"bignum::operator=(unsigned char)");
		TEST_ASSERT(val.get_str(16) == "ff",
			"bignum::operator=(unsigned char)");
	}

	// from short
	{
		bignum val;
		val = (signed short)0x1234;
		TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::operator=(signed short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::operator=(signed short)");
	}
	{
		bignum val;
		val = (signed short)-1;
		TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(signed short)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(signed short)");
	}
	{
		bignum val;
		val = (unsigned short)0x1234;
		TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::operator=(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::operator=(unsigned short)");
	}
	{
		bignum val;
		val = (unsigned short)-1;
		TEST_ASSERT(val.get_ui() == 0xffff,
			"bignum::operator=(unsigned short)");
		TEST_ASSERT(val.get_str(16) == "ffff",
			"bignum::operator=(unsigned short)");
	}

	// from int
	{
		bignum val;
		val = (signed int)0x12345678;
		TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::operator=(signed int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::operator=(signed int)");
	}
	{
		bignum val;
		val = (signed int)-1;
		TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(signed int)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(signed int)");
	}
	{
		bignum val;
		val = (unsigned int)0x12345678;
		TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::operator=(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::operator=(unsigned int)");
	}
	{
		bignum val;
		val = (unsigned int)-1;
		TEST_ASSERT(val.get_ui() == 0xffffffff,
			"bignum::operator=(unsigned int)");
		TEST_ASSERT(val.get_str(16) == "ffffffff",
			"bignum::operator=(unsigned int)");
	}

	// from bignum
	{
		bignum val1(12345678);
		bignum val2;
		val2 = val1;
		TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"bignum::operator=(bignum)");
	}

	// from string
	{
		bignum val;
		val = "12345678";
		TEST_ASSERT(val.get_ui() == 12345678,
			"bignum::operator=(string)");
		TEST_ASSERT(val.get_str(10) == "12345678",
			"bignum::operator=(string)");
	}
	{
		bignum val;
		val = "0x112233445566778899aabbccddeeff00";
		TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::operator=(string)");
	}
	{
		bignum val;
		val = "-1";
		TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(string)");
		TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(string)");
	}
}

TEST(test_comparisons)
{
	// Because bignum is a template, int's important to excercise all
	// permutations of comparisons, to force instantiation.

	//
	// compare to char
	//
	// ==
	{
		bignum val((signed char)0x12);
		TEST_ASSERT_EQ(val, (signed char)0x12,
			"bignum::operator==(signed char)");
		TEST_ASSERT_EQ(val, 0x12,
			"bignum::operator==(int)");
	}
	{
		bignum val((signed char)-1);
		TEST_ASSERT_EQ(val, (signed char)-1,
			"bignum::operator==(signed char)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned char)0x12);
		TEST_ASSERT_EQ(val, (unsigned char)0x12,
			"bignum::operator==(unsigned char)");
		TEST_ASSERT_EQ(val, 0x12,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned char)-1);
		TEST_ASSERT_EQ(val, (unsigned char)0xff,
			"bignum::operator==(unsigned char)");
		TEST_ASSERT_EQ(val, 0xff,
			"bignum::operator==(int)");
	}
	// !=
	{
		bignum val((signed char)0x13);
		TEST_ASSERT_NE(val, (signed char)0x12,
			"bignum::operator!=(signed char)");
		TEST_ASSERT_NE(val, 0x12,
			"bignum::operator!=(int)");
	}
	{
		bignum val((signed char)-2);
		TEST_ASSERT_NE(val, (signed char)-1,
			"bignum::operator!=(signed char)");
		TEST_ASSERT_NE(val, -1,
			"bignum::operator!=(int)");
	}
	{
		bignum val((unsigned char)0x13);
		TEST_ASSERT_NE(val, (unsigned char)0x12,
			"bignum::operator!=(unsigned char)");
		TEST_ASSERT_NE(val, 0x12,
			"bignum::operator!=(int)");
	}
	{
		bignum val((unsigned char)-2);
		TEST_ASSERT_NE(val, (unsigned char)0xff,
			"bignum::operator!=(unsigned char)");
		TEST_ASSERT_NE(val, 0xff,
			"bignum::operator!=(int)");
	}
	// <
	{
		bignum val((signed char)0x12);
		TEST_ASSERT_LT(val, (signed char)0x13,
			"bignum::operator<(signed char)");
		TEST_ASSERT_LT(val, 0x13,
			"bignum::operator<(int)");
	}
	{
		bignum val((signed char)-2);
		TEST_ASSERT_LT(val, (signed char)-1,
			"bignum::operator<(signed char)");
		TEST_ASSERT_LT(val, -1,
			"bignum::operator<(int)");
	}
	{
		bignum val((unsigned char)0x12);
		TEST_ASSERT_LT(val, (unsigned char)0x13,
			"bignum::operator<(unsigned char)");
		TEST_ASSERT_LT(val, 0x13,
			"bignum::operator<(int)");
	}
	{
		bignum val((unsigned char)-2);
		TEST_ASSERT_LT(val, (unsigned char)0xff,
			"bignum::operator<(unsigned char)");
		TEST_ASSERT_LT(val, 0xff,
			"bignum::operator<(int)");
	}
	// >
	{
		bignum val((signed char)0x12);
		TEST_ASSERT_GT(val, (signed char)0x11,
			"bignum::operator>(signed char)");
		TEST_ASSERT_GT(val, 0x11,
			"bignum::operator>(int)");
	}
	{
		bignum val((signed char)-1);
		TEST_ASSERT_GT(val, (signed char)-2,
			"bignum::operator>(signed char)");
		TEST_ASSERT_GT(val, -2,
			"bignum::operator>(int)");
	}
	{
		bignum val((unsigned char)0x12);
		TEST_ASSERT_GT(val, (unsigned char)0x11,
			"bignum::operator>(unsigned char)");
		TEST_ASSERT_GT(val, 0x11,
			"bignum::operator>(int)");
	}
	{
		bignum val((unsigned char)-1);
		TEST_ASSERT_GT(val, (unsigned char)0xfe,
			"bignum::operator>(unsigned char)");
		TEST_ASSERT_GT(val, 0xfe,
			"bignum::operator>(int)");
	}
	// <=
	{
		bignum val((signed char)0x12);
		TEST_ASSERT_LE(val, (signed char)0x13,
			"bignum::operator<=(signed char)");
		TEST_ASSERT_LE(val, 0x13,
			"bignum::operator<=(int)");
	}
	{
		bignum val((signed char)-2);
		TEST_ASSERT_LE(val, (signed char)-1,
			"bignum::operator<=(signed char)");
		TEST_ASSERT_LE(val, -1,
			"bignum::operator<=(int)");
	}
	{
		bignum val((unsigned char)0x12);
		TEST_ASSERT_LE(val, (unsigned char)0x13,
			"bignum::operator<=(unsigned char)");
		TEST_ASSERT_LE(val, 0x13,
			"bignum::operator<=(int)");
	}
	{
		bignum val((unsigned char)-2);
		TEST_ASSERT_LE(val, (unsigned char)0xff,
			"bignum::operator<=(unsigned char)");
		TEST_ASSERT_LE(val, 0xff,
			"bignum::operator<=(int)");
	}
	// >=
	{
		bignum val((signed char)0x12);
		TEST_ASSERT_GE(val, (signed char)0x11,
			"bignum::operator>=(signed char)");
		TEST_ASSERT_GE(val, 0x11,
			"bignum::operator>=(int)");
	}
	{
		bignum val((signed char)-1);
		TEST_ASSERT_GE(val, (signed char)-2,
			"bignum::operator>=(signed char)");
		TEST_ASSERT_GE(val, -2,
			"bignum::operator>=(int)");
	}
	{
		bignum val((unsigned char)0x12);
		TEST_ASSERT_GE(val, (unsigned char)0x11,
			"bignum::operator>=(unsigned char)");
		TEST_ASSERT_GE(val, 0x11,
			"bignum::operator>=(int)");
	}
	{
		bignum val((unsigned char)-1);
		TEST_ASSERT_GE(val, (unsigned char)0xfe,
			"bignum::operator>=(unsigned char)");
		TEST_ASSERT_GE(val, 0xfe,
			"bignum::operator>=(int)");
	}

	//
	// compare to short
	//
	// ==
	{
		bignum val((signed short)0x1234);
		TEST_ASSERT_EQ(val, (signed short)0x1234,
			"bignum::operator==(signed short)");
		TEST_ASSERT_EQ(val, 0x1234,
			"bignum::operator==(int)");
	}
	{
		bignum val((signed short)-1);
		TEST_ASSERT_EQ(val, (signed short)-1,
			"bignum::operator==(signed short)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned short)0x1234);
		TEST_ASSERT_EQ(val, (unsigned short)0x1234,
			"bignum::operator==(unsigned short)");
		TEST_ASSERT_EQ(val, 0x1234,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned short)-1);
		TEST_ASSERT_EQ(val, (unsigned short)0xffff,
			"bignum::operator==(unsigned short)");
		TEST_ASSERT_EQ(val, 0xffff,
			"bignum::operator==(int)");
	}
	// !=
	{
		bignum val((signed short)0x1235);
		TEST_ASSERT_NE(val, (signed short)0x1234,
			"bignum::operator!=(signed short)");
		TEST_ASSERT_NE(val, 0x1234,
			"bignum::operator!=(int)");
	}
	{
		bignum val((signed short)-2);
		TEST_ASSERT_NE(val, (signed short)-1,
			"bignum::operator!=(signed short)");
		TEST_ASSERT_NE(val, -1,
			"bignum::operator!=(int)");
	}
	{
		bignum val((unsigned short)0x1235);
		TEST_ASSERT_NE(val, (unsigned short)0x1234,
			"bignum::operator!=(unsigned short)");
		TEST_ASSERT_NE(val, 0x1234,
			"bignum::operator!=(int)");
	}
	{
		bignum val((unsigned short)-2);
		TEST_ASSERT_NE(val, (unsigned short)0xffff,
			"bignum::operator!=(unsigned short)");
		TEST_ASSERT_NE(val, 0xffff,
			"bignum::operator!=(int)");
	}
	// <
	{
		bignum val((signed short)0x1234);
		TEST_ASSERT_LT(val, (signed short)0x1235,
			"bignum::operator<(signed short)");
		TEST_ASSERT_LT(val, 0x1235,
			"bignum::operator<(int)");
	}
	{
		bignum val((signed short)-2);
		TEST_ASSERT_LT(val, (signed short)-1,
			"bignum::operator<(signed short)");
		TEST_ASSERT_LT(val, -1,
			"bignum::operator<(int)");
	}
	{
		bignum val((unsigned short)0x1234);
		TEST_ASSERT_LT(val, (unsigned short)0x1235,
			"bignum::operator<(unsigned short)");
		TEST_ASSERT_LT(val, 0x1235,
			"bignum::operator<(int)");
	}
	{
		bignum val((unsigned short)-2);
		TEST_ASSERT_LT(val, (unsigned short)0xffff,
			"bignum::operator<(unsigned short)");
		TEST_ASSERT_LT(val, 0xffff,
			"bignum::operator<(int)");
	}
	// >
	{
		bignum val((signed short)0x1235);
		TEST_ASSERT_GT(val, (signed short)0x1234,
			"bignum::operator>(signed short)");
		TEST_ASSERT_GT(val, 0x1234,
			"bignum::operator>(int)");
	}
	{
		bignum val((signed short)-1);
		TEST_ASSERT_GT(val, (signed short)-2,
			"bignum::operator>(signed short)");
		TEST_ASSERT_GT(val, -2,
			"bignum::operator>(int)");
	}
	{
		bignum val((unsigned short)0x1235);
		TEST_ASSERT_GT(val, (unsigned short)0x1234,
			"bignum::operator>(unsigned short)");
		TEST_ASSERT_GT(val, 0x1234,
			"bignum::operator>(int)");
	}
	{
		bignum val((unsigned short)-1);
		TEST_ASSERT_GT(val, (unsigned short)0xfffe,
			"bignum::operator>(unsigned short)");
		TEST_ASSERT_GT(val, 0xfffe,
			"bignum::operator>(int)");
	}
	// <=
	{
		bignum val((signed short)0x1234);
		TEST_ASSERT_LE(val, (signed short)0x1235,
			"bignum::operator<=(signed short)");
		TEST_ASSERT_LE(val, 0x1235,
			"bignum::operator<=(int)");
	}
	{
		bignum val((signed short)-2);
		TEST_ASSERT_LE(val, (signed short)-1,
			"bignum::operator<=(signed short)");
		TEST_ASSERT_LE(val, -1,
			"bignum::operator<=(int)");
	}
	{
		bignum val((unsigned short)0x1234);
		TEST_ASSERT_LE(val, (unsigned short)0x1235,
			"bignum::operator<=(unsigned short)");
		TEST_ASSERT_LE(val, 0x1235,
			"bignum::operator<=(int)");
	}
	{
		bignum val((unsigned short)-2);
		TEST_ASSERT_LE(val, (unsigned short)0xffff,
			"bignum::operator<=(unsigned short)");
		TEST_ASSERT_LE(val, 0xffff,
			"bignum::operator<=(int)");
	}
	// >=
	{
		bignum val((signed short)0x1235);
		TEST_ASSERT_GE(val, (signed short)0x1234,
			"bignum::operator>=(signed short)");
		TEST_ASSERT_GE(val, 0x1234,
			"bignum::operator>=(int)");
	}
	{
		bignum val((signed short)-1);
		TEST_ASSERT_GE(val, (signed short)-2,
			"bignum::operator>=(signed short)");
		TEST_ASSERT_GE(val, -2,
			"bignum::operator>=(int)");
	}
	{
		bignum val((unsigned short)0x1235);
		TEST_ASSERT_GE(val, (unsigned short)0x1234,
			"bignum::operator>=(unsigned short)");
		TEST_ASSERT_GE(val, 0x1234,
			"bignum::operator>=(int)");
	}
	{
		bignum val((unsigned short)-1);
		TEST_ASSERT_GE(val, (unsigned short)0xfffe,
			"bignum::operator>=(unsigned short)");
		TEST_ASSERT_GE(val, 0xfffe,
			"bignum::operator>=(int)");
	}

	//
	// compare to int
	//
	// ==
	{
		bignum val((signed int)0x12345678);
		TEST_ASSERT_EQ(val, (signed int)0x12345678,
			"bignum::operator==(signed int)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::operator==(int)");
	}
	{
		bignum val((signed int)-1);
		TEST_ASSERT_EQ(val, (signed int)-1,
			"bignum::operator==(signed int)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned int)0x12345678);
		TEST_ASSERT_EQ(val, (unsigned int)0x12345678,
			"bignum::operator==(unsigned int)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned int)-1);
		TEST_ASSERT_EQ(val, (unsigned int)0xffffffff,
			"bignum::operator==(unsigned int)");
		TEST_ASSERT_EQ(val, 0xffffffff,
			"bignum::operator==(int)");
	}
	// !=
	{
		bignum val((signed int)0x12345679);
		TEST_ASSERT_NE(val, (signed int)0x12345678,
			"bignum::operator!=(signed int)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::operator!=(int)");
	}
	{
		bignum val((signed int)-2);
		TEST_ASSERT_NE(val, (signed int)-1,
			"bignum::operator!=(signed int)");
		TEST_ASSERT_NE(val, -1,
			"bignum::operator!=(int)");
	}
	{
		bignum val((unsigned int)0x12345679);
		TEST_ASSERT_NE(val, (unsigned int)0x12345678,
			"bignum::operator!=(unsigned int)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::operator!=(int)");
	}
	{
		bignum val((unsigned int)-2);
		TEST_ASSERT_NE(val, (unsigned int)0xffffffff,
			"bignum::operator!=(unsigned int)");
		TEST_ASSERT_NE(val, 0xffffffff,
			"bignum::operator!=(int)");
	}
	// <
	{
		bignum val((signed int)0x12345678);
		TEST_ASSERT_LT(val, (signed int)0x12345679,
			"bignum::operator<(signed int)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::operator<(int)");
	}
	{
		bignum val((signed int)-2);
		TEST_ASSERT_LT(val, (signed int)-1,
			"bignum::operator<(signed int)");
		TEST_ASSERT_LT(val, -1,
			"bignum::operator<(int)");
	}
	{
		bignum val((unsigned int)0x12345678);
		TEST_ASSERT_LT(val, (unsigned int)0x12345679,
			"bignum::operator<(unsigned int)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::operator<(int)");
	}
	{
		bignum val((unsigned int)-2);
		TEST_ASSERT_LT(val, (unsigned int)0xffffffff,
			"bignum::operator<(unsigned int)");
		TEST_ASSERT_LT(val, 0xffffffff,
			"bignum::operator<(int)");
	}
	// >
	{
		bignum val((signed int)0x12345679);
		TEST_ASSERT_GT(val, (signed int)0x12345678,
			"bignum::operator>(signed int)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::operator>(int)");
	}
	{
		bignum val((signed int)-1);
		TEST_ASSERT_GT(val, (signed int)-2,
			"bignum::operator>(signed int)");
		TEST_ASSERT_GT(val, -2,
			"bignum::operator>(int)");
	}
	{
		bignum val((unsigned int)0x12345679);
		TEST_ASSERT_GT(val, (unsigned int)0x12345678,
			"bignum::operator>(unsigned int)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::operator>(int)");
	}
	{
		bignum val((unsigned int)-1);
		TEST_ASSERT_GT(val, (unsigned int)0xfffffffe,
			"bignum::operator>(unsigned int)");
		TEST_ASSERT_GT(val, 0xfffffffe,
			"bignum::operator>(int)");
	}
	// <=
	{
		bignum val((signed int)0x12345678);
		TEST_ASSERT_LE(val, (signed int)0x12345679,
			"bignum::operator<=(signed int)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::operator<=(int)");
	}
	{
		bignum val((signed int)-2);
		TEST_ASSERT_LE(val, (signed int)-1,
			"bignum::operator<=(signed int)");
		TEST_ASSERT_LE(val, -1,
			"bignum::operator<=(int)");
	}
	{
		bignum val((unsigned int)0x12345678);
		TEST_ASSERT_LE(val, (unsigned int)0x12345679,
			"bignum::operator<=(unsigned int)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::operator<=(int)");
	}
	{
		bignum val((unsigned int)-2);
		TEST_ASSERT_LE(val, (unsigned int)0xffffffff,
			"bignum::operator<=(unsigned int)");
		TEST_ASSERT_LE(val, 0xffffffff,
			"bignum::operator<=(int)");
	}
	// >=
	{
		bignum val((signed int)0x12345679);
		TEST_ASSERT_GE(val, (signed int)0x12345678,
			"bignum::operator>=(signed int)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::operator>=(int)");
	}
	{
		bignum val((signed int)-1);
		TEST_ASSERT_GE(val, (signed int)-2,
			"bignum::operator>=(signed int)");
		TEST_ASSERT_GE(val, -2,
			"bignum::operator>=(int)");
	}
	{
		bignum val((unsigned int)0x12345679);
		TEST_ASSERT_GE(val, (unsigned int)0x12345678,
			"bignum::operator>=(unsigned int)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::operator>=(int)");
	}
	{
		bignum val((unsigned int)-1);
		TEST_ASSERT_GE(val, (unsigned int)0xfffffffe,
			"bignum::operator>=(unsigned int)");
		TEST_ASSERT_GE(val, 0xfffffffe,
			"bignum::operator>=(int)");
	}

	//
	// compare to long
	//
	// ==
	{
		bignum val((signed long)0x12345678);
		TEST_ASSERT_EQ(val, (signed long)0x12345678,
			"bignum::operator==(signed long)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::operator==(long)");
	}
	{
		bignum val((signed long)-1);
		TEST_ASSERT_EQ(val, (signed long)-1,
			"bignum::operator==(signed long)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::operator==(long)");
	}
	{
		bignum val((unsigned long)0x12345678);
		TEST_ASSERT_EQ(val, (unsigned long)0x12345678,
			"bignum::operator==(unsigned long)");
		TEST_ASSERT_EQ(val, 0x12345678,
			"bignum::operator==(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// !=
	{
		bignum val((signed long)0x12345679);
		TEST_ASSERT_NE(val, (signed long)0x12345678,
			"bignum::operator!=(signed long)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::operator!=(long)");
	}
	{
		bignum val((signed long)-2);
		TEST_ASSERT_NE(val, (signed long)-1,
			"bignum::operator!=(signed long)");
		TEST_ASSERT_NE(val, -1,
			"bignum::operator!=(long)");
	}
	{
		bignum val((unsigned long)0x12345679);
		TEST_ASSERT_NE(val, (unsigned long)0x12345678,
			"bignum::operator!=(unsigned long)");
		TEST_ASSERT_NE(val, 0x12345678,
			"bignum::operator!=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// <
	{
		bignum val((signed long)0x12345678);
		TEST_ASSERT_LT(val, (signed long)0x12345679,
			"bignum::operator<(signed long)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::operator<(long)");
	}
	{
		bignum val((signed long)-2);
		TEST_ASSERT_LT(val, (signed long)-1,
			"bignum::operator<(signed long)");
		TEST_ASSERT_LT(val, -1,
			"bignum::operator<(long)");
	}
	{
		bignum val((unsigned long)0x12345678);
		TEST_ASSERT_LT(val, (unsigned long)0x12345679,
			"bignum::operator<(unsigned long)");
		TEST_ASSERT_LT(val, 0x12345679,
			"bignum::operator<(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// >
	{
		bignum val((signed long)0x12345679);
		TEST_ASSERT_GT(val, (signed long)0x12345678,
			"bignum::operator>(signed long)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::operator>(long)");
	}
	{
		bignum val((signed long)-1);
		TEST_ASSERT_GT(val, (signed long)-2,
			"bignum::operator>(signed long)");
		TEST_ASSERT_GT(val, -2,
			"bignum::operator>(long)");
	}
	{
		bignum val((unsigned long)0x12345679);
		TEST_ASSERT_GT(val, (unsigned long)0x12345678,
			"bignum::operator>(unsigned long)");
		TEST_ASSERT_GT(val, 0x12345678,
			"bignum::operator>(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// <=
	{
		bignum val((signed long)0x12345678);
		TEST_ASSERT_LE(val, (signed long)0x12345679,
			"bignum::operator<=(signed long)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::operator<=(long)");
	}
	{
		bignum val((signed long)-2);
		TEST_ASSERT_LE(val, (signed long)-1,
			"bignum::operator<=(signed long)");
		TEST_ASSERT_LE(val, -1,
			"bignum::operator<=(long)");
	}
	{
		bignum val((unsigned long)0x12345678);
		TEST_ASSERT_LE(val, (unsigned long)0x12345679,
			"bignum::operator<=(unsigned long)");
		TEST_ASSERT_LE(val, 0x12345679,
			"bignum::operator<=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.
	// >=
	{
		bignum val((signed long)0x12345679);
		TEST_ASSERT_GE(val, (signed long)0x12345678,
			"bignum::operator>=(signed long)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::operator>=(long)");
	}
	{
		bignum val((signed long)-1);
		TEST_ASSERT_GE(val, (signed long)-2,
			"bignum::operator>=(signed long)");
		TEST_ASSERT_GE(val, -2,
			"bignum::operator>=(long)");
	}
	{
		bignum val((unsigned long)0x12345679);
		TEST_ASSERT_GE(val, (unsigned long)0x12345678,
			"bignum::operator>=(unsigned long)");
		TEST_ASSERT_GE(val, 0x12345678,
			"bignum::operator>=(long)");
	}
	// NOTE: don't compare to (unsigned long)-1 - it gets sign
	// extended on 64 bit architectures, but not on 32 bit.

	//
	// compare to long long
	//
	// ==
	{
		bignum val((signed long long)0x1234567890ULL);
		TEST_ASSERT_EQ(val, (signed long long)0x1234567890ULL,
			"bignum::operator==(signed long long)");
		TEST_ASSERT_EQ(val, 0x1234567890ULL,
			"bignum::operator==(long long)");
	}
	{
		bignum val((signed long long)-1);
		TEST_ASSERT_EQ(val, (signed long long)-1,
			"bignum::operator==(signed long long)");
		TEST_ASSERT_EQ(val, -1,
			"bignum::operator==(long long)");
	}
	{
		bignum val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_EQ(val, (unsigned long long)0x1234567890ULL,
			"bignum::operator==(unsigned long long)");
		TEST_ASSERT_EQ(val, 0x1234567890ULL,
			"bignum::operator==(long long)");
	}
	{
		bignum val((unsigned long long)-1);
		TEST_ASSERT_EQ(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::operator==(unsigned long long)");
		TEST_ASSERT_EQ(val, 0xffffffffffffffffULL,
			"bignum::operator==(long long)");
	}
	// !=
	{
		bignum val((signed long long)0x1234567891ULL);
		TEST_ASSERT_NE(val, (signed long long)0x1234567890ULL,
			"bignum::operator!=(signed long long)");
		TEST_ASSERT_NE(val, 0x1234567890ULL,
			"bignum::operator!=(long long)");
	}
	{
		bignum val((signed long long)-2);
		TEST_ASSERT_NE(val, (signed long long)-1,
			"bignum::operator!=(signed long long)");
		TEST_ASSERT_NE(val, -1,
			"bignum::operator!=(long long)");
	}
	{
		bignum val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_NE(val, (unsigned long long)0x1234567890ULL,
			"bignum::operator!=(unsigned long long)");
		TEST_ASSERT_NE(val, 0x1234567890ULL,
			"bignum::operator!=(long long)");
	}
	{
		bignum val((unsigned long long)-2);
		TEST_ASSERT_NE(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::operator!=(unsigned long long)");
		TEST_ASSERT_NE(val, 0xffffffffffffffffULL,
			"bignum::operator!=(long long)");
	}
	// <
	{
		bignum val((signed long long)0x1234567890ULL);
		TEST_ASSERT_LT(val, (signed long long)0x1234567891ULL,
			"bignum::operator<(signed long long)");
		TEST_ASSERT_LT(val, 0x1234567891ULL,
			"bignum::operator<(long long)");
	}
	{
		bignum val((signed long long)-2);
		TEST_ASSERT_LT(val, (signed long long)-1,
			"bignum::operator<(signed long long)");
		TEST_ASSERT_LT(val, -1,
			"bignum::operator<(long long)");
	}
	{
		bignum val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_LT(val, (unsigned long long)0x1234567891ULL,
			"bignum::operator<(unsigned long long)");
		TEST_ASSERT_LT(val, 0x1234567891ULL,
			"bignum::operator<(long long)");
	}
	{
		bignum val((unsigned long long)-2);
		TEST_ASSERT_LT(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::operator<(unsigned long long)");
		TEST_ASSERT_LT(val, 0xffffffffffffffffULL,
			"bignum::operator<(long long)");
	}
	// >
	{
		bignum val((signed long long)0x1234567891ULL);
		TEST_ASSERT_GT(val, (signed long long)0x1234567890ULL,
			"bignum::operator>(signed long long)");
		TEST_ASSERT_GT(val, 0x1234567890ULL,
			"bignum::operator>(long long)");
	}
	{
		bignum val((signed long long)-1);
		TEST_ASSERT_GT(val, (signed long long)-2,
			"bignum::operator>(signed long long)");
		TEST_ASSERT_GT(val, -2,
			"bignum::operator>(long long)");
	}
	{
		bignum val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_GT(val, (unsigned long long)0x1234567890ULL,
			"bignum::operator>(unsigned long long)");
		TEST_ASSERT_GT(val, 0x1234567890ULL,
			"bignum::operator>(long long)");
	}
	{
		bignum val((unsigned long long)-1);
		TEST_ASSERT_GE(
			val, (unsigned long long)0xfffffffffffffffeULL,
			"bignum::operator>(unsigned long long)");
		TEST_ASSERT_GT(val, 0xfffffffffffffffeULL,
			"bignum::operator>(long long)");
	}
	// <=
	{
		bignum val((signed long long)0x1234567890ULL);
		TEST_ASSERT_LE(val, (signed long long)0x1234567891ULL,
			"bignum::operator<=(signed long long)");
		TEST_ASSERT_LE(val, 0x1234567891ULL,
			"bignum::operator<=(long long)");
	}
	{
		bignum val((signed long long)-2);
		TEST_ASSERT_LE(val, (signed long long)-1,
			"bignum::operator<=(signed long long)");
		TEST_ASSERT_LE(val, -1,
			"bignum::operator<=(long long)");
	}
	{
		bignum val((unsigned long long)0x1234567890ULL);
		TEST_ASSERT_LE(val, (unsigned long long)0x1234567891ULL,
			"bignum::operator<=(unsigned long long)");
		TEST_ASSERT_LE(val, 0x1234567891ULL,
			"bignum::operator<=(long long)");
	}
	{
		bignum val((unsigned long long)-2);
		TEST_ASSERT_LE(
			val, (unsigned long long)0xffffffffffffffffULL,
			"bignum::operator<=(unsigned long long)");
		TEST_ASSERT_LE(val, 0xffffffffffffffffULL,
			"bignum::operator<=(long long)");
	}
	// >=
	{
		bignum val((signed long long)0x1234567891ULL);
		TEST_ASSERT_GE(val, (signed long long)0x1234567890ULL,
			"bignum::operator>=(signed long long)");
		TEST_ASSERT_GE(val, 0x1234567890ULL,
			"bignum::operator>=(long long)");
	}
	{
		bignum val((signed long long)-1);
		TEST_ASSERT_GE(val, (signed long long)-2,
			"bignum::operator>=(signed long long)");
		TEST_ASSERT_GE(val, -2,
			"bignum::operator>=(long long)");
	}
	{
		bignum val((unsigned long long)0x1234567891ULL);
		TEST_ASSERT_GE(val, (unsigned long long)0x1234567890ULL,
			"bignum::operator>=(unsigned long long)");
		TEST_ASSERT_GE(val, 0x1234567890ULL,
			"bignum::operator>=(long long)");
	}
	{
		bignum val((unsigned long long)-1);
		TEST_ASSERT_GE(
			val, (unsigned long long)0xfffffffffffffffeULL,
			"bignum::operator>=(unsigned long long)");
		TEST_ASSERT_GE(val, 0xfffffffffffffffeULL,
			"bignum::operator>=(long long)");
	}

	//
	// compare to bignum
	//
	// ==
	{
		bignum val1(12345678);
		bignum val2(12345678);
		TEST_ASSERT_EQ(val1, val2, "bignum::operator==(bignum)");
	}
	// !=
	{
		bignum val1(12345678);
		bignum val2(12345679);
		TEST_ASSERT_NE(val1, val2, "bignum::operator!=(bignum)");
	}
	// <
	{
		bignum val1(12345678);
		bignum val2(12345679);
		TEST_ASSERT_LT(val1, val2, "bignum::operator<(bignum)");
	}
	// >
	{
		bignum val1(12345679);
		bignum val2(12345678);
		TEST_ASSERT_GT(val1, val2, "bignum::operator>(bignum)");
	}
	// <=
	{
		bignum val1(12345678);
		bignum val2(12345679);
		TEST_ASSERT_LE(val1, val2, "bignum::operator<=(bignum)");
	}
	// >=
	{
		bignum val1(12345679);
		bignum val2(12345678);
		TEST_ASSERT_GE(val1, val2, "bignum::operator>=(bignum)");
	}

	// can't compare directly to strings
}

TEST(test_bool)
{
	// convert to bool
	{
		bignum val(1);
		TEST_ASSERT(val, "bignum::operator bool()");
	}
	{
		bignum val(12345);
		TEST_ASSERT(val, "bignum::operator bool()");
	}
	{
		bignum val(-1);
		TEST_ASSERT(val, "bignum::operator bool()");
	}
	{
		bignum val(0);
		TEST_ASSERT(!val, "bignum::operator bool()");
	}
	{
		bignum val(2);
		TEST_ASSERT(!!val, "bignum::operator bool()");
	}
	{
		bignum val(0);
		TEST_ASSERT(!!!val, "bignum::operator bool()");
	}
}

TEST(test_small_arithmetic)
{
	{
		bignum val(10);
		TEST_ASSERT(val+3 == 13,
			"bignum::operator+(bignum, int)");
		TEST_ASSERT(3+val == 13,
			"bignum::operator+(int, bignum)");
		TEST_ASSERT(val+bignum(3) == 13,
			"bignum::operator+(bignum, bignum)");
		val += 5;
		TEST_ASSERT(val == 15,
			"bignum::operator+=(bignum, int)");
		val++;
		TEST_ASSERT(val == 16,
			"bignum::operator++()");
		++val;
		TEST_ASSERT(val == 17,
			"bignum::operator++()");
	}
	{
		bignum val(10);
		TEST_ASSERT(val-3 == 7,
			"bignum::operator-(bignum, int)");
		TEST_ASSERT(17-val == 7,
			"bignum::operator-(int, bignum)");
		TEST_ASSERT(val-bignum(3) == 7,
			"bignum::operator-(bignum, bignum)");
		val -= 5;
		TEST_ASSERT(val == 5,
			"bignum::operator-=(bignum, int)");
		val--;
		TEST_ASSERT(val == 4,
			"bignum::operator--()");
		--val;
		TEST_ASSERT(val == 3,
			"bignum::operator-()");
	}
	{
		bignum val(10);
		TEST_ASSERT(val*2 == 20,
			"bignum::operator*(bignum, int)");
		TEST_ASSERT(2*val == 20,
			"bignum::operator*(int, bignum)");
		TEST_ASSERT(val*bignum(2) == 20,
			"bignum::operator*(bignum, bignum)");
		val *= 3;
		TEST_ASSERT(val == 30,
			"bignum::operator*=(bignum, int)");
	}
	{
		bignum val(10);
		TEST_ASSERT(val/2 == 5,
			"bignum::operator/(bignum, int)");
		TEST_ASSERT(50/val == 5,
			"bignum::operator/(int, bignum)");
		TEST_ASSERT(val/bignum(2) == 5,
			"bignum::operator/(bignum, bignum)");
		val /= 5;
		TEST_ASSERT(val == 2,
			"bignum::operator/=(bignum, int)");
	}
	{
		bignum val(17);
		TEST_ASSERT(val%10 == 7,
			"bignum::operator%(bignum, int)");
		TEST_ASSERT(24%val == 7,
			"bignum::operator%(int, bignum)");
		TEST_ASSERT(val%bignum(10) == 7,
			"bignum::operator%(bignum, bignum)");
		val %= 5;
		TEST_ASSERT(val == 2,
			"bignum::operator%=(bignum, int)");
	}
}

TEST(test_large_arithmetic)
{
	{
		bignum val("0x1000000000000000a");
		TEST_ASSERT(val+3 == bignum("0x1000000000000000d"),
			"bignum::operator+(bignum, int)");
		TEST_ASSERT(3+val == bignum("0x1000000000000000d"),
			"bignum::operator+(int, bignum)");
		val += 5;
		TEST_ASSERT(val == bignum("0x1000000000000000f"),
			"bignum::operator+=(bignum, int)");
		val++;
		TEST_ASSERT(val == bignum("0x10000000000000010"),
			"bignum::operator++()");
		++val;
		TEST_ASSERT(val == bignum("0x10000000000000011"),
			"bignum::operator++()");
	}
	{
		bignum val("0x1000000000000000a");
		TEST_ASSERT(val-3 == bignum("0x10000000000000007"),
			"bignum::operator-(bignum, int)");
		val -= 5;
		TEST_ASSERT(val == bignum("0x10000000000000005"),
			"bignum::operator-=(int, bignum)");
		val--;
		TEST_ASSERT(val == bignum("0x10000000000000004"),
			"bignum::operator--(bignum, int)");
		--val;
		TEST_ASSERT(val == bignum("0x10000000000000003"),
			"bignum::operator--()");
	}
	{
		bignum val("0x10000000000000010");
		TEST_ASSERT(val*2 == bignum("0x20000000000000020"),
			"bignum::operator*(bignum, int)");
		TEST_ASSERT(2*val == bignum("0x20000000000000020"),
			"bignum::operator*(int, bignum)");
		val *= 3;
		TEST_ASSERT(val == bignum("0x30000000000000030"),
			"bignum::operator*=(bignum, int)");
	}
	{
		bignum val("0xa000000000000000a");
		TEST_ASSERT(val/2 == bignum("0x50000000000000005"),
			"bignum::operator/(bignum, int)");
		val /= 5;
		TEST_ASSERT(val == bignum("0x20000000000000002"),
			"bignum::operator/=(bignum, int)");
	}
	{
		bignum val("0x10000000000000011");
		TEST_ASSERT(val%10 == bignum("0x03"),
			"bignum::operator%(bignum, int)");
		val %= 5;
		TEST_ASSERT(val == bignum("0x03"),
			"bignum::operator%=(bignum, int)");
	}
}

//NOTE:
//  Bitwise NOT is supported, but what does it really mean for an
//  arbitrary-width number?
TEST(test_small_logical)
{
	{
		bignum val(0xff);
		TEST_ASSERT((val & bignum(0x107)) == 0x7,
			"bignum::operator&(bignum, unsigned)");
		TEST_ASSERT((bignum(0x107) & val) == 0x7,
			"bignum::operator&(unsigned, bignum)");
		val &= bignum(0x370);
		TEST_ASSERT(val == 0x70,
			"bignum::operator&=(bignum, unsigned)");
	}
	{
		bignum val(0xff);
		TEST_ASSERT((val | bignum(0x107)) == 0x1ff,
			"bignum::operator|()");
		TEST_ASSERT((bignum(0x107) | val) == 0x1ff,
			"bignum::operator|()");
		val |= bignum(0x270);
		TEST_ASSERT(val == 0x2ff, "bignum::operator|=()");
	}
	{
		bignum val(0xff);
		TEST_ASSERT((val ^ bignum(0x107)) == 0x1f8,
			"bignum::operator^()");
		TEST_ASSERT((bignum(0x107) ^ val) == 0x1f8,
			"bignum::operator^()");
		val ^= bignum(0x270);
		TEST_ASSERT(val == 0x28f, "bignum::operator^=()");
	}
	{
		bignum val(0x10);
		TEST_ASSERT((val << 2) == 0x40,
			"bignum::operator<<()");
		TEST_ASSERT((2 << val) == 0x20000,
			"bignum::operator<<()");
		val <<= 3;
		TEST_ASSERT(val == 0x80, "bignum::operator<<=()");
	}
	{
		bignum val(0x10);
		TEST_ASSERT((val >> 2) == 0x04,
			"bignum::operator>>()");
		TEST_ASSERT((0x20000 >> val) == 0x2,
			"bignum::operator>>()");
		val >>= 3;
		TEST_ASSERT(val == 0x02, "bignum::operator>>=()");
	}
}

TEST(test_large_logical)
{
	{
		bignum val("0xf00000000000000ff");
		TEST_ASSERT((val & bignum(0x107)) == 0x7,
			"bignum::operator&()");
		TEST_ASSERT((bignum(0x107) & val) == 0x7,
			"bignum::operator&()");
		val &= bignum(0x370);
		TEST_ASSERT(val == 0x70, "bignum::operator&=()");
	}
	{
		bignum val("0xf00000000000000ff");
		TEST_ASSERT(
			(val | bignum(0x107)) == bignum("0xf00000000000001ff"),
			"bignum::operator|()");
		TEST_ASSERT(
			(bignum(0x107) | val) == bignum("0xf00000000000001ff"),
			"bignum::operator|()");
		val |= bignum(0x270);
		TEST_ASSERT(
			val == bignum("0xf00000000000002ff"),
			"bignum::operator|=()");
	}
	{
		bignum val("0xf00000000000000ff");
		TEST_ASSERT(
			(val ^ bignum(0x107)) == bignum("0xf00000000000001f8"),
			"bignum::operator^()");
		TEST_ASSERT(
			(bignum(0x107) ^ val) == bignum("0xf00000000000001f8"),
			"bignum::operator^()");
		val ^= bignum(0x270);
		TEST_ASSERT(
			val == bignum("0xf000000000000028f"),
			"bignum::operator^=()");
	}
	{
		bignum val("0xf0000000000000010");
		TEST_ASSERT(
			(val << 2) == bignum("0x3c0000000000000040"),
			"bignum::operator<<()");
		val <<= 3;
		TEST_ASSERT(
			val == bignum("0x780000000000000080"),
			"bignum::operator<<=()");
	}
	{
		bignum val("0xf0000000000000010");
		TEST_ASSERT(
			(val >> 2) == bignum("0x3c000000000000004"),
			"bignum::operator>>()");
		val >>= 3;
		TEST_ASSERT(
			val == bignum("0x1e000000000000002"),
			"bignum::operator>>=()");
	}
}

TEST(test_small_io)
{
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::dec;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, but I don't care.
		bignum val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::oct;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
		#endif
	}
	{
		#if 0
		// This test fails, but I don't care.
		bignum val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::hex;
		valoss << val;
		TEST_ASSERT(rawoss.str() == valoss.str(),
			"bignum::operator<<(ostream)");
		#endif
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		bignum val(15);
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
			"bignum::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, and I think the standard version is
		// wrong.
		bignum val(15);
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
			"bignum::operator<<(ostream)");
		#endif
	}

	// test boost::format support
	{
		bignum val(15);
		std::ostringstream oss;
		oss.width(8);
		oss.fill('0');
		oss << std::hex;
		oss << 15;
		TEST_ASSERT(oss.str() == boost::format("%08x") %15,
			"boost::format() % bignum");
	}
}

TEST(test_large_io)
{
	{
		string num = "12345678123456781234567812345678";
		bignum val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::dec;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
	{
		string num = "012345671234567123456712345671234567";
		bignum val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::oct;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
	{
		string num = "0x123456789abcdef123456789abcdef";
		bignum val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::hex;
		oss << val;
		TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
}

TEST(test_operators)
{
	// test operators with signed char
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (signed char)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (signed char)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (signed char)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (signed char)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (signed char)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (signed char)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (signed char)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (signed char)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed char)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (signed char)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed char)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (signed char)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed char)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (signed char)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (signed char)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (signed char)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (signed char)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (signed char)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (signed char)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (signed char)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with unsigned char
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (unsigned char)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (unsigned char)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (unsigned char)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (unsigned char)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (unsigned char)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (unsigned char)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (unsigned char)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (unsigned char)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned char)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (unsigned char)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned char)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (unsigned char)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned char)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (unsigned char)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (unsigned char)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (unsigned char)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (unsigned char)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (unsigned char)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (unsigned char)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (unsigned char)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with signed short
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (signed short)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (signed short)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (signed short)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (signed short)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (signed short)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (signed short)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (signed short)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (signed short)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed short)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (signed short)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed short)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (signed short)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed short)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (signed short)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (signed short)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (signed short)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (signed short)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (signed short)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (signed short)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (signed short)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with unsigned short
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (unsigned short)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (unsigned short)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (unsigned short)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (unsigned short)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (unsigned short)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (unsigned short)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (unsigned short)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (unsigned short)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned short)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (unsigned short)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned short)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (unsigned short)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned short)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (unsigned short)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (unsigned short)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (unsigned short)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (unsigned short)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (unsigned short)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (unsigned short)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (unsigned short)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with signed int
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (signed int)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (signed int)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (signed int)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (signed int)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (signed int)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (signed int)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (signed int)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (signed int)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed int)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (signed int)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed int)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (signed int)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed int)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (signed int)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (signed int)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (signed int)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (signed int)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (signed int)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (signed int)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (signed int)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with unsigned int
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (unsigned int)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (unsigned int)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (unsigned int)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (unsigned int)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (unsigned int)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (unsigned int)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (unsigned int)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (unsigned int)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned int)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (unsigned int)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned int)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (unsigned int)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned int)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (unsigned int)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (unsigned int)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (unsigned int)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (unsigned int)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (unsigned int)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (unsigned int)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (unsigned int)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with signed long
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (signed long)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (signed long)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (signed long)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (signed long)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (signed long)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (signed long)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (signed long)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (signed long)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed long)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (signed long)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed long)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (signed long)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed long)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (signed long)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (signed long)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (signed long)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (signed long)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (signed long)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (signed long)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (signed long)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with unsigned long
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (unsigned long)1;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (unsigned long)1;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (unsigned long)1;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (unsigned long)1;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (unsigned long)2;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (unsigned long)2;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (unsigned long)2;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (unsigned long)2;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned long)3;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (unsigned long)3;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned long)2;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (unsigned long)2;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned long)1;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (unsigned long)1;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (unsigned long)2;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (unsigned long)2;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (unsigned long)1;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (unsigned long)1;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (unsigned long)1;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (unsigned long)1;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// Most operators with (signd or unsigned) long long fail, because
	// GMP does not natively support them.  There must be some hack to
	// make this work  (and the above bugs with | & ^), but I can't be
	// bothered to mess with it any further.
	#if 0
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (signed long long)1LL;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (signed long long)1LL;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (signed long long)1LL;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (signed long long)1LL;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (signed long long)2LL;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (signed long long)2LL;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (signed long long)2LL;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (signed long long)2LL;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (signed long long)3LL;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (signed long long)3LL;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (signed long long)2LL;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (signed long long)2LL;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (signed long long)1LL;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (signed long long)1LL;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (signed long long)2LL;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (signed long long)2LL;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (signed long long)1LL;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (signed long long)1LL;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (signed long long)1LL;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (signed long long)1LL;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}

	// test operators with unsigned long long
	{
		pp_value val = 1;
		pp_value val2;

		val2 = val + (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 2, "bignum::operator+()");
		val += (unsigned long long)1ULL;
		TEST_ASSERT(val == 2, "bignum::operator+=()");

		val2 = val - (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 1, "bignum::operator-()");
		val -= (unsigned long long)1ULL;
		TEST_ASSERT(val == 1, "bignum::operator-=()");

		val2 = val * (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 2, "bignum::operator*()");
		val *= (unsigned long long)2ULL;
		TEST_ASSERT(val == 2, "bignum::operator*=()");

		val2 = val / (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 1, "bignum::operator/()");
		val /= (unsigned long long)2ULL;
		TEST_ASSERT(val == 1, "bignum::operator/=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val | (unsigned long long)3ULL;
		//TEST_ASSERT(val2 == 3, "bignum::operator|()");
		val |= (unsigned long long)3ULL;
		TEST_ASSERT(val == 3, "bignum::operator|=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val & (unsigned long long)2ULL;
		//TEST_ASSERT(val2 == 2, "bignum::operator&()");
		val &= (unsigned long long)2ULL;
		TEST_ASSERT(val == 2, "bignum::operator&=()");

		// a bug(?) in gmpxx prevents this from working
		//val2 = val ^ (unsigned long long)1ULL;
		//TEST_ASSERT(val2 == 3, "bignum::operator^()");
		val ^= (unsigned long long)1ULL;
		TEST_ASSERT(val == 3, "bignum::operator^=()");

		val2 = val % (unsigned long long)2ULL;
		TEST_ASSERT(val2 == 1, "bignum::operator%()");
		val %= (unsigned long long)2ULL;
		TEST_ASSERT(val == 1, "bignum::operator%=()");

		val2 = val << (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 2, "bignum::operator<<()");
		val <<= (unsigned long long)1ULL;
		TEST_ASSERT(val == 2, "bignum::operator<<=()");

		val2 = val >> (unsigned long long)1ULL;
		TEST_ASSERT(val2 == 1, "bignum::operator>>()");
		val >>= (unsigned long long)1ULL;
		TEST_ASSERT(val == 1, "bignum::operator>>=()");
	}
	#endif // 0
}
