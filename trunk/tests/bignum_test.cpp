#include "pp.h"
#include "pp_test.h"
#include "bitbuffer.h"
#include "bignum.h"
using namespace std;

int
test_ctors()
{
	int ret = 0;

	// default
	{
		bignum val;
		ret += PP_TEST_ASSERT(val.get_si() == 0,
			"bignum::bignum()");
		ret += PP_TEST_ASSERT(val.get_ui() == 0,
			"bignum::bignum()");
		ret += PP_TEST_ASSERT(val.get_str(10) == "0",
			"bignum::bignum()");
	}

	// from char
	{
		bignum val((signed char)0x12);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12,
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"bignum::bignum(signed char)");
	}
	{
		bignum val((signed char)-1);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed char)");
	}
	{
		bignum val((unsigned char)0x12);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"bignum::bignum(unsigned char)");
	}
	{
		bignum val((unsigned char)-1);
		ret += PP_TEST_ASSERT(val.get_si() == 0xff,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0xff,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ff",
			"bignum::bignum(unsigned char)");
	}

	// from short
	{
		bignum val((signed short)0x1234);
		ret += PP_TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::bignum(signed short)");
	}
	{
		bignum val((signed short)-1);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed short)");
	}
	{
		bignum val((unsigned short)0x1234);
		ret += PP_TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::bignum(unsigned short)");
	}
	{
		bignum val((unsigned short)-1);
		ret += PP_TEST_ASSERT(val.get_si() == 0xffff,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffff,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffff",
			"bignum::bignum(unsigned short)");
	}

	// from int
	// On 32 bit platforms this is where we will start to see
	// signed vs. unsigned oddities.
	{
		bignum val((signed int)0x12345678);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::bignum(signed int)");
	}
	{
		bignum val((signed int)-1);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed int)");
	}
	{
		bignum val((unsigned int)0x12345678);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::bignum(unsigned int)");
	}
	{
		bignum val((unsigned int)-1);
		// On a 32 bit platform this will fail, since get_si()
		// will return a signed value, and -1 is OOB.
		//ret += PP_TEST_ASSERT(val.get_si() == (signed int)0xffffffff,
		//	"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffffffff,
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffffffff",
			"bignum::bignum(unsigned int)");
	}

	// from long long
	// Skip long, since either sizeof(long) == sizeof(int) or
	// sizeof(long) == sizeof(long long).
	{
		bignum val(0x123456789abcdef0LL);
		ret += PP_TEST_ASSERT(val.get_si() == 0x123456789abcdef0LL,
			"val.get_int()");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		ret += PP_TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"bignum::bignum(signed long long)");
	}
	{
		bignum val(-1LL);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"val.get_int()");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::bignum(signed long long)");
	}
	{
		bignum val(0x123456789abcdef0ULL);
		ret += PP_TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		ret += PP_TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"bignum::bignum(unsigned long long)");
	}
	{
		bignum val(-1ULL);
		ret += PP_TEST_ASSERT(val.get_uint() == 0xffffffffffffffffULL,
			"val.get_uint()");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffffffffffffffff",
			"bignum::bignum(unsigned long long)");
	}

	// from bignum
	{
		bignum val1(12345678);
		bignum val2(val1);
		ret += PP_TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"bignum::bignum(bignum)");
	}

	// from string
	{
		bignum val("12345678");
		ret += PP_TEST_ASSERT(val.get_ui() == 12345678,
			"bignum::bignum(string)");
		val = "23456789";
		ret += PP_TEST_ASSERT(val.get_ui() == 23456789,
			"bignum::bignum(string)");
	}
	{
		bignum val("0x12345678");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::bignum(string)");
		val = "0x23456789";
		ret += PP_TEST_ASSERT(val.get_ui() == 0x23456789,
			"bignum::bignum(string)");
	}
	{
		bignum val("0x112233445566778899aabbccddeeff00");
		ret += PP_TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::bignum(string)");
		val = "0x2233445566778899aabbccddeeff0011";
		ret += PP_TEST_ASSERT(
			val.get_str(16) == "2233445566778899aabbccddeeff0011",
			"bignum::bignum(string)");
	}
	{
		bignum val("-1");
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(string)");
		val = "-1";
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::bignum(string)");
	}

	// from bitbuffer
	{
		bitbuffer bb(8);
		bb.byte_at(0) = 0x12;
		bignum val(bb);
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"bignum(bitbuffer)");
		bitbuffer bb2 = val.get_bitbuffer();
		ret += PP_TEST_ASSERT(to_string(bb2) == "0x12",
			"bignum::get_bitbuffer()");
		bitbuffer bb3 = val.get_bitbuffer(BITS8);
		ret += PP_TEST_ASSERT(to_string(bb3) == "0x12",
			"bignum::get_bitbuffer(int)");
	}
	{
		bitbuffer bb(16);
		bb.byte_at(1) = 0x12;
		bb.byte_at(0) = 0x34;
		bignum val(bb);
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum(bitbuffer)");
		bitbuffer bb2 = val.get_bitbuffer();
		ret += PP_TEST_ASSERT(to_string(bb2) == "0x1234",
			"bignum::get_bitbuffer()");
		bitbuffer bb3 = val.get_bitbuffer(BITS16);
		ret += PP_TEST_ASSERT(to_string(bb3) == "0x1234",
			"bignum::get_bitbuffer(int)");
	}
	{
		bitbuffer bb = pp_value(-1).get_bitbuffer();
		ret += PP_TEST_ASSERT(to_string(bb) == "0x1",
			"bignum::get_bitbuffer()");
	}

	return ret;
}

int
test_assignments()
{
	int ret = 0;

	// from char
	{
		bignum val;
		val = (signed char)0x12;
		ret += PP_TEST_ASSERT(val.get_si() == 0x12,
			"bignum::operator=(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"bignum::operator=(signed char)");
	}
	{
		bignum val;
		val = (signed char)-1;
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(signed char)");
	}
	{
		bignum val;
		val = (unsigned char)0x12;
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"bignum::operator=(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"bignum::operator=(unsigned char)");
	}
	{
		bignum val;
		val = (unsigned char)-1;
		ret += PP_TEST_ASSERT(val.get_ui() == 0xff,
			"bignum::operator=(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ff",
			"bignum::operator=(unsigned char)");
	}

	// from short
	{
		bignum val;
		val = (signed short)0x1234;
		ret += PP_TEST_ASSERT(val.get_si() == 0x1234,
			"bignum::operator=(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::operator=(signed short)");
	}
	{
		bignum val;
		val = (signed short)-1;
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(signed short)");
	}
	{
		bignum val;
		val = (unsigned short)0x1234;
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"bignum::operator=(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"bignum::operator=(unsigned short)");
	}
	{
		bignum val;
		val = (unsigned short)-1;
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffff,
			"bignum::operator=(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffff",
			"bignum::operator=(unsigned short)");
	}

	// from int
	{
		bignum val;
		val = (signed int)0x12345678;
		ret += PP_TEST_ASSERT(val.get_si() == 0x12345678,
			"bignum::operator=(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::operator=(signed int)");
	}
	{
		bignum val;
		val = (signed int)-1;
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(signed int)");
	}
	{
		bignum val;
		val = (unsigned int)0x12345678;
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"bignum::operator=(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"bignum::operator=(unsigned int)");
	}
	{
		bignum val;
		val = (unsigned int)-1;
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffffffff,
			"bignum::operator=(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffffffff",
			"bignum::operator=(unsigned int)");
	}

	// from bignum
	{
		bignum val1(12345678);
		bignum val2;
		val2 = val1;
		ret += PP_TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"bignum::operator=(bignum)");
	}

	// from string
	{
		bignum val;
		val = "12345678";
		ret += PP_TEST_ASSERT(val.get_ui() == 12345678,
			"bignum::operator=(string)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "12345678",
			"bignum::operator=(string)");
	}
	{
		bignum val;
		val = "0x112233445566778899aabbccddeeff00";
		ret += PP_TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::operator=(string)");
	}
	{
		bignum val;
		val = "-1";
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"bignum::operator=(string)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"bignum::operator=(string)");
	}

	return ret;
}

int
test_comparisons()
{
	int ret = 0;

	// to char
	{
		bignum val((signed char)0x12);
		ret += PP_TEST_ASSERT(val == (signed char)0x12,
			"bignum::operator==(signed char)");
		ret += PP_TEST_ASSERT(val == 0x12,
			"bignum::operator==(int)");
	}
	{
		bignum val((signed char)-1);
		ret += PP_TEST_ASSERT(val == (signed char)-1,
			"bignum::operator==(signed char)");
		ret += PP_TEST_ASSERT(val == -1,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned char)0x12);
		ret += PP_TEST_ASSERT(val == (unsigned char)0x12,
			"bignum::operator==(unsigned char)");
		ret += PP_TEST_ASSERT(val == 0x12,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned char)-1);
		ret += PP_TEST_ASSERT(val == (unsigned char)0xff,
			"bignum::operator==(unsigned char)");
		ret += PP_TEST_ASSERT(val == 0xff,
			"bignum::operator==(int)");
	}

	// to short
	{
		bignum val((signed short)0x1234);
		ret += PP_TEST_ASSERT(val == (signed short)0x1234,
			"bignum::operator==(signed short)");
		ret += PP_TEST_ASSERT(val == 0x1234,
			"bignum::operator==(int)");
	}
	{
		bignum val((signed short)-1);
		ret += PP_TEST_ASSERT(val == (signed short)-1,
			"bignum::operator==(signed short)");
		ret += PP_TEST_ASSERT(val == -1,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned short)0x1234);
		ret += PP_TEST_ASSERT(val == (unsigned short)0x1234,
			"bignum::operator==(unsigned short)");
		ret += PP_TEST_ASSERT(val == 0x1234,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned short)-1);
		ret += PP_TEST_ASSERT(val == (unsigned short)0xffff,
			"bignum::operator==(unsigned short)");
		ret += PP_TEST_ASSERT(val == 0xffff,
			"bignum::operator==(int)");
	}

	// to int
	{
		bignum val((signed int)0x12345678);
		ret += PP_TEST_ASSERT(val == (signed int)0x12345678,
			"bignum::operator==(signed int)");
		ret += PP_TEST_ASSERT(val == 0x12345678,
			"bignum::operator==(int)");
	}
	{
		bignum val((signed int)-1);
		ret += PP_TEST_ASSERT(val == (signed int)-1,
			"bignum::operator==(signed int)");
		ret += PP_TEST_ASSERT(val == -1,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned int)0x12345678);
		ret += PP_TEST_ASSERT(val == (unsigned int)0x12345678,
			"bignum::operator==(unsigned int)");
		ret += PP_TEST_ASSERT(val == 0x12345678,
			"bignum::operator==(int)");
	}
	{
		bignum val((unsigned int)-1);
		ret += PP_TEST_ASSERT(val == (unsigned int)0xffffffff,
			"bignum::operator==(unsigned int)");
		ret += PP_TEST_ASSERT(val == 0xffffffff,
			"bignum::operator==(int)");
	}

	// to bignum
	{
		bignum val1(12345678);
		bignum val2(12345678);
		ret += PP_TEST_ASSERT(val2 == val1 && val2 == 12345678,
			"bignum::operator==(bignum)");
	}

	// can't cmpare directly to strings

	// Test other comparison operators.  This assumes that if all the ==
	// tests pass for all the integral types, so will the other
	// comparisons.
	{
		bignum val(10);
		ret += PP_TEST_ASSERT(val == 10,
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(!(val == 11),
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(val != 11,
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(!(val != 10),
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(val < 20,
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(!(val < 2),
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(val <= 20,
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(!(val <= 2),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(val <= 10,
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(val > 2,
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(!(val > 20),
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(val >= 2,
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(!(val >= 20),
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(val >= 10,
			"bignum::operator>=()");
	}
	{
		bignum val("0x112233445566778899aa");
		ret += PP_TEST_ASSERT(val == bignum("0x112233445566778899aa"),
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(
			!(val == bignum("0x112233445566778899ab")),
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(val != bignum("0x112233445566778899ab"),
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(
			!(val != bignum("0x112233445566778899aa")),
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(val < bignum("0x112233445566778899ab"),
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(
			!(val < bignum("0x112233445566778899a9")),
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(val <= bignum("0x112233445566778899ab"),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(
			!(val <= bignum("0x112233445566778899a9")),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(val <= bignum("0x112233445566778899aa"),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(val > bignum("0x112233445566778899a9"),
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(
			!(val > bignum("0x112233445566778899ab")),
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(val >= bignum("0x112233445566778899a9"),
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(
			!(val >= bignum("0x112233445566778899ab")),
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(val >= bignum("0x112233445566778899aa"),
			"bignum::operator>=()");
	}

	return ret;
}

int
test_small_arithmetic()
{
	int ret = 0;

	{
		bignum val(10);
		ret += PP_TEST_ASSERT(val+3 == 13,
			"bignum::operator+(bignum, int)");
		ret += PP_TEST_ASSERT(3+val == 13,
			"bignum::operator+(int, bignum)");
		ret += PP_TEST_ASSERT(val+bignum(3) == 13,
			"bignum::operator+(bignum, bignum)");
		val += 5;
		ret += PP_TEST_ASSERT(val == 15,
			"bignum::operator+=(bignum, int)");
		val++;
		ret += PP_TEST_ASSERT(val == 16,
			"bignum::operator++()");
		++val;
		ret += PP_TEST_ASSERT(val == 17,
			"bignum::operator++()");
	}
	{
		bignum val(10);
		ret += PP_TEST_ASSERT(val-3 == 7,
			"bignum::operator-(bignum, int)");
		ret += PP_TEST_ASSERT(17-val == 7,
			"bignum::operator-(int, bignum)");
		ret += PP_TEST_ASSERT(val-bignum(3) == 7,
			"bignum::operator-(bignum, bignum)");
		val -= 5;
		ret += PP_TEST_ASSERT(val == 5,
			"bignum::operator-=(bignum, int)");
		val--;
		ret += PP_TEST_ASSERT(val == 4,
			"bignum::operator--()");
		--val;
		ret += PP_TEST_ASSERT(val == 3,
			"bignum::operator-()");
	}
	{
		bignum val(10);
		ret += PP_TEST_ASSERT(val*2 == 20,
			"bignum::operator*(bignum, int)");
		ret += PP_TEST_ASSERT(2*val == 20,
			"bignum::operator*(int, bignum)");
		ret += PP_TEST_ASSERT(val*bignum(2) == 20,
			"bignum::operator*(bignum, bignum)");
		val *= 3;
		ret += PP_TEST_ASSERT(val == 30,
			"bignum::operator*=(bignum, int)");
	}
	{
		bignum val(10);
		ret += PP_TEST_ASSERT(val/2 == 5,
			"bignum::operator/(bignum, int)");
		ret += PP_TEST_ASSERT(50/val == 5,
			"bignum::operator/(int, bignum)");
		ret += PP_TEST_ASSERT(val/bignum(2) == 5,
			"bignum::operator/(bignum, bignum)");
		val /= 5;
		ret += PP_TEST_ASSERT(val == 2,
			"bignum::operator/=(bignum, int)");
	}
	{
		bignum val(17);
		ret += PP_TEST_ASSERT(val%10 == 7,
			"bignum::operator%(bignum, int)");
		ret += PP_TEST_ASSERT(24%val == 7,
			"bignum::operator%(int, bignum)");
		ret += PP_TEST_ASSERT(val%bignum(10) == 7,
			"bignum::operator%(bignum, bignum)");
		val %= 5;
		ret += PP_TEST_ASSERT(val == 2,
			"bignum::operator%=(bignum, int)");
	}

	return ret;
}

int
test_large_arithmetic()
{
	int ret = 0;
	{
		bignum val("0x1000000000000000a");
		ret += PP_TEST_ASSERT(val+3 == bignum("0x1000000000000000d"),
			"bignum::operator+(bignum, int)");
		ret += PP_TEST_ASSERT(3+val == bignum("0x1000000000000000d"),
			"bignum::operator+(int, bignum)");
		val += 5;
		ret += PP_TEST_ASSERT(val == bignum("0x1000000000000000f"),
			"bignum::operator+=(bignum, int)");
		val++;
		ret += PP_TEST_ASSERT(val == bignum("0x10000000000000010"),
			"bignum::operator++()");
		++val;
		ret += PP_TEST_ASSERT(val == bignum("0x10000000000000011"),
			"bignum::operator++()");
	}
	{
		bignum val("0x1000000000000000a");
		ret += PP_TEST_ASSERT(val-3 == bignum("0x10000000000000007"),
			"bignum::operator-(bignum, int)");
		val -= 5;
		ret += PP_TEST_ASSERT(val == bignum("0x10000000000000005"),
			"bignum::operator-=(int, bignum)");
		val--;
		ret += PP_TEST_ASSERT(val == bignum("0x10000000000000004"),
			"bignum::operator--(bignum, int)");
		--val;
		ret += PP_TEST_ASSERT(val == bignum("0x10000000000000003"),
			"bignum::operator--()");
	}
	{
		bignum val("0x10000000000000010");
		ret += PP_TEST_ASSERT(val*2 == bignum("0x20000000000000020"),
			"bignum::operator*(bignum, int)");
		ret += PP_TEST_ASSERT(2*val == bignum("0x20000000000000020"),
			"bignum::operator*(int, bignum)");
		val *= 3;
		ret += PP_TEST_ASSERT(val == bignum("0x30000000000000030"),
			"bignum::operator*=(bignum, int)");
	}
	{
		bignum val("0xa000000000000000a");
		ret += PP_TEST_ASSERT(val/2 == bignum("0x50000000000000005"),
			"bignum::operator/(bignum, int)");
		val /= 5;
		ret += PP_TEST_ASSERT(val == bignum("0x20000000000000002"),
			"bignum::operator/=(bignum, int)");
	}
	{
		bignum val("0x10000000000000011");
		ret += PP_TEST_ASSERT(val%10 == bignum("0x03"),
			"bignum::operator%(bignum, int)");
		val %= 5;
		ret += PP_TEST_ASSERT(val == bignum("0x03"),
			"bignum::operator%=(bignum, int)");
	}

	return ret;
}

//NOTE:
//  Bitwise NOT is supported, but what does it really mean for an
//  arbitrary-width number?
int
test_small_logical()
{
	int ret = 0;

	{
		bignum val(0xff);
		ret += PP_TEST_ASSERT((val & bignum(0x107)) == 0x7,
			"bignum::operator&(bignum, unsigned)");
		ret += PP_TEST_ASSERT((bignum(0x107) & val) == 0x7,
			"bignum::operator&(unsigned, bignum)");
		val &= bignum(0x370);
		ret += PP_TEST_ASSERT(val == 0x70,
			"bignum::operator&=(bignum, unsigned)");
	}
	{
		bignum val(0xff);
		ret += PP_TEST_ASSERT((val | bignum(0x107)) == 0x1ff,
			"bignum::operator|()");
		ret += PP_TEST_ASSERT((bignum(0x107) | val) == 0x1ff,
			"bignum::operator|()");
		val |= bignum(0x270);
		ret += PP_TEST_ASSERT(val == 0x2ff, "bignum::operator|=()");
	}
	{
		bignum val(0xff);
		ret += PP_TEST_ASSERT((val ^ bignum(0x107)) == 0x1f8,
			"bignum::operator^()");
		ret += PP_TEST_ASSERT((bignum(0x107) ^ val) == 0x1f8,
			"bignum::operator^()");
		val ^= bignum(0x270);
		ret += PP_TEST_ASSERT(val == 0x28f, "bignum::operator^=()");
	}
	{
		bignum val(0x10);
		ret += PP_TEST_ASSERT((val << 2) == 0x40,
			"bignum::operator<<()");
		ret += PP_TEST_ASSERT((2 << val) == 0x20000,
			"bignum::operator<<()");
		val <<= 3;
		ret += PP_TEST_ASSERT(val == 0x80, "bignum::operator<<=()");
	}
	{
		bignum val(0x10);
		ret += PP_TEST_ASSERT((val >> 2) == 0x04,
			"bignum::operator>>()");
		ret += PP_TEST_ASSERT((0x20000 >> val) == 0x2,
			"bignum::operator>>()");
		val >>= 3;
		ret += PP_TEST_ASSERT(val == 0x02, "bignum::operator>>=()");
	}

	return ret;
}

int
test_large_logical()
{
	int ret = 0;
	{
		bignum val("0xf00000000000000ff");
		ret += PP_TEST_ASSERT((val & bignum(0x107)) == 0x7,
			"bignum::operator&()");
		ret += PP_TEST_ASSERT((bignum(0x107) & val) == 0x7,
			"bignum::operator&()");
		val &= bignum(0x370);
		ret += PP_TEST_ASSERT(val == 0x70, "bignum::operator&=()");
	}
	{
		bignum val("0xf00000000000000ff");
		ret += PP_TEST_ASSERT(
			(val | bignum(0x107)) == bignum("0xf00000000000001ff"),
			"bignum::operator|()");
		ret += PP_TEST_ASSERT(
			(bignum(0x107) | val) == bignum("0xf00000000000001ff"),
			"bignum::operator|()");
		val |= bignum(0x270);
		ret += PP_TEST_ASSERT(
			val == bignum("0xf00000000000002ff"),
			"bignum::operator|=()");
	}
	{
		bignum val("0xf00000000000000ff");
		ret += PP_TEST_ASSERT(
			(val ^ bignum(0x107)) == bignum("0xf00000000000001f8"),
			"bignum::operator^()");
		ret += PP_TEST_ASSERT(
			(bignum(0x107) ^ val) == bignum("0xf00000000000001f8"),
			"bignum::operator^()");
		val ^= bignum(0x270);
		ret += PP_TEST_ASSERT(
			val == bignum("0xf000000000000028f"),
			"bignum::operator^=()");
	}
	{
		bignum val("0xf0000000000000010");
		ret += PP_TEST_ASSERT(
			(val << 2) == bignum("0x3c0000000000000040"),
			"bignum::operator<<()");
		val <<= 3;
		ret += PP_TEST_ASSERT(
			val == bignum("0x780000000000000080"),
			"bignum::operator<<=()");
	}
	{
		bignum val("0xf0000000000000010");
		ret += PP_TEST_ASSERT(
			(val >> 2) == bignum("0x3c000000000000004"),
			"bignum::operator>>()");
		val >>= 3;
		ret += PP_TEST_ASSERT(
			val == bignum("0x1e000000000000002"),
			"bignum::operator>>=()");
	}

	return ret;
}

int
test_small_io()
{
	int ret = 0;

	{
		bignum val(15);
		std::ostringstream rawoss;
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::dec;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
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
		ret += PP_TEST_ASSERT(oss.str() == boost::format("%08x") %15,
			"boost::format() % bignum");
	}

	return ret;
}

int
test_large_io()
{
	int ret = 0;

	{
		string num = "12345678123456781234567812345678";
		bignum val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::dec;
		oss << val;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
	{
		string num = "012345671234567123456712345671234567";
		bignum val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::oct;
		oss << val;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
	{
		string num = "0x123456789abcdef123456789abcdef";
		bignum val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::hex;
		oss << val;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}

	return ret;
}

int
main(void)
{
	int ret = 0;

	ret += test_ctors();
	ret += test_assignments();
	ret += test_comparisons();
	ret += test_small_arithmetic();
	ret += test_large_arithmetic();
	ret += test_small_logical();
	ret += test_large_logical();
	ret += test_small_io();
	ret += test_large_io();

	return ret;
}
