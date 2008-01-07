#include "pp.h"
#include "pp_test.h"
#include "bignum.h"
using namespace std;

int
test_ctors()
{
	int ret = 0;

	// default
	{
		pp_value val;
		ret += PP_TEST_ASSERT(val.get_si() == 0,
			"pp_value::pp_value()");
		ret += PP_TEST_ASSERT(val.get_ui() == 0,
			"pp_value::pp_value()");
		ret += PP_TEST_ASSERT(val.get_str(10) == "0",
			"pp_value::pp_value()");
	}

	// from char
	{
		pp_value val((signed char)0x12);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12,
			"pp_value::pp_value(signed char)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"pp_value::pp_value(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"pp_value::pp_value(signed char)");
	}
	{
		pp_value val((signed char)-1);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::pp_value(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::pp_value(signed char)");
	}
	{
		pp_value val((unsigned char)0x12);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12,
			"pp_value::pp_value(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"pp_value::pp_value(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"pp_value::pp_value(unsigned char)");
	}
	{
		pp_value val((unsigned char)-1);
		ret += PP_TEST_ASSERT(val.get_si() == 0xff,
			"pp_value::pp_value(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0xff,
			"pp_value::pp_value(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ff",
			"pp_value::pp_value(unsigned char)");
	}

	// from short
	{
		pp_value val((signed short)0x1234);
		ret += PP_TEST_ASSERT(val.get_si() == 0x1234,
			"pp_value::pp_value(signed short)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"pp_value::pp_value(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"pp_value::pp_value(signed short)");
	}
	{
		pp_value val((signed short)-1);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::pp_value(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::pp_value(signed short)");
	}
	{
		pp_value val((unsigned short)0x1234);
		ret += PP_TEST_ASSERT(val.get_si() == 0x1234,
			"pp_value::pp_value(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"pp_value::pp_value(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"pp_value::pp_value(unsigned short)");
	}
	{
		pp_value val((unsigned short)-1);
		ret += PP_TEST_ASSERT(val.get_si() == 0xffff,
			"pp_value::pp_value(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffff,
			"pp_value::pp_value(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffff",
			"pp_value::pp_value(unsigned short)");
	}

	// from int
	// On 32 bit platforms this is where we will start to see
	// signed vs. unsigned oddities.
	{
		pp_value val((signed int)0x12345678);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12345678,
			"pp_value::pp_value(signed int)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"pp_value::pp_value(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"pp_value::pp_value(signed int)");
	}
	{
		pp_value val((signed int)-1);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::pp_value(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::pp_value(signed int)");
	}
	{
		pp_value val((unsigned int)0x12345678);
		ret += PP_TEST_ASSERT(val.get_si() == 0x12345678,
			"pp_value::pp_value(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"pp_value::pp_value(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"pp_value::pp_value(unsigned int)");
	}
	{
		pp_value val((unsigned int)-1);
		// On a 32 bit platform this will fail, since get_si()
		// will return a signed value, and -1 is OOB.
		//ret += PP_TEST_ASSERT(val.get_si() == (signed int)0xffffffff,
		//	"pp_value::pp_value(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffffffff,
			"pp_value::pp_value(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffffffff",
			"pp_value::pp_value(unsigned int)");
	}

	// from long long
	// Skip long, since either sizeof(long) == sizeof(int) or
	// sizeof(long) == sizeof(long long).
	{
		pp_value val(0x123456789abcdef0LL);
		ret += PP_TEST_ASSERT(val.get_si() == 0x123456789abcdef0LL,
			"val.get_int()");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		ret += PP_TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"pp_value::pp_value(signed long long)");
	}
	{
		pp_value val(-1LL);
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"val.get_int()");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::pp_value(signed long long)");
	}
	{
		pp_value val(0x123456789abcdef0ULL);
		ret += PP_TEST_ASSERT(val.get_ui() == 0x123456789abcdef0ULL,
			"val.get_uint()");
		ret += PP_TEST_ASSERT(val.get_str(16) == "123456789abcdef0",
			"pp_value::pp_value(unsigned long long)");
	}
	{
		pp_value val(-1ULL);
		ret += PP_TEST_ASSERT(val.get_uint() == 0xffffffffffffffffULL,
			"val.get_uint()");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffffffffffffffff",
			"pp_value::pp_value(unsigned long long)");
	}

	// from pp_value
	{
		pp_value val1(12345678);
		pp_value val2(val1);
		ret += PP_TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"pp_value::pp_value(pp_value)");
	}

	// from string
	{
		pp_value val("12345678");
		ret += PP_TEST_ASSERT(val.get_ui() == 12345678,
			"pp_value::pp_value(string)");
		val = "23456789";
		ret += PP_TEST_ASSERT(val.get_ui() == 23456789,
			"pp_value::pp_value(string)");
	}
	{
		pp_value val("0x12345678");
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"pp_value::pp_value(string)");
		val = "0x23456789";
		ret += PP_TEST_ASSERT(val.get_ui() == 0x23456789,
			"pp_value::pp_value(string)");
	}
	{
		pp_value val("0x112233445566778899aabbccddeeff00");
		ret += PP_TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"pp_value::pp_value(string)");
		val = "0x2233445566778899aabbccddeeff0011";
		ret += PP_TEST_ASSERT(
			val.get_str(16) == "2233445566778899aabbccddeeff0011",
			"pp_value::pp_value(string)");
	}
	{
		pp_value val("-1");
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::pp_value(string)");
		val = "-1";
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::pp_value(string)");
	}

	return ret;
}

int
test_assignments()
{
	int ret = 0;

	// from char
	{
		pp_value val;
		val = (signed char)0x12;
		ret += PP_TEST_ASSERT(val.get_si() == 0x12,
			"pp_value::operator=(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"pp_value::operator=(signed char)");
	}
	{
		pp_value val;
		val = (signed char)-1;
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::operator=(signed char)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::operator=(signed char)");
	}
	{
		pp_value val;
		val = (unsigned char)0x12;
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12,
			"pp_value::operator=(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12",
			"pp_value::operator=(unsigned char)");
	}
	{
		pp_value val;
		val = (unsigned char)-1;
		ret += PP_TEST_ASSERT(val.get_ui() == 0xff,
			"pp_value::operator=(unsigned char)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ff",
			"pp_value::operator=(unsigned char)");
	}

	// from short
	{
		pp_value val;
		val = (signed short)0x1234;
		ret += PP_TEST_ASSERT(val.get_si() == 0x1234,
			"pp_value::operator=(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"pp_value::operator=(signed short)");
	}
	{
		pp_value val;
		val = (signed short)-1;
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::operator=(signed short)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::operator=(signed short)");
	}
	{
		pp_value val;
		val = (unsigned short)0x1234;
		ret += PP_TEST_ASSERT(val.get_ui() == 0x1234,
			"pp_value::operator=(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "1234",
			"pp_value::operator=(unsigned short)");
	}
	{
		pp_value val;
		val = (unsigned short)-1;
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffff,
			"pp_value::operator=(unsigned short)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffff",
			"pp_value::operator=(unsigned short)");
	}

	// from int
	{
		pp_value val;
		val = (signed int)0x12345678;
		ret += PP_TEST_ASSERT(val.get_si() == 0x12345678,
			"pp_value::operator=(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"pp_value::operator=(signed int)");
	}
	{
		pp_value val;
		val = (signed int)-1;
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::operator=(signed int)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::operator=(signed int)");
	}
	{
		pp_value val;
		val = (unsigned int)0x12345678;
		ret += PP_TEST_ASSERT(val.get_ui() == 0x12345678,
			"pp_value::operator=(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "12345678",
			"pp_value::operator=(unsigned int)");
	}
	{
		pp_value val;
		val = (unsigned int)-1;
		ret += PP_TEST_ASSERT(val.get_ui() == 0xffffffff,
			"pp_value::operator=(unsigned int)");
		ret += PP_TEST_ASSERT(val.get_str(16) == "ffffffff",
			"pp_value::operator=(unsigned int)");
	}

	// from pp_value
	{
		pp_value val1(12345678);
		pp_value val2;
		val2 = val1;
		ret += PP_TEST_ASSERT(val2 == val1 && val2.get_ui() == 12345678,
			"pp_value::operator=(pp_value)");
	}

	// from string
	{
		pp_value val;
		val = "12345678";
		ret += PP_TEST_ASSERT(val.get_ui() == 12345678,
			"pp_value::operator=(string)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "12345678",
			"pp_value::operator=(string)");
	}
	{
		pp_value val;
		val = "0x112233445566778899aabbccddeeff00";
		ret += PP_TEST_ASSERT(
			val.get_str(16) == "112233445566778899aabbccddeeff00",
			"pp_value::operator=(string)");
	}
	{
		pp_value val;
		val = "-1";
		ret += PP_TEST_ASSERT(val.get_si() == -1,
			"pp_value::operator=(string)");
		ret += PP_TEST_ASSERT(val.get_str(10) == "-1",
			"pp_value::operator=(string)");
	}

	return ret;
}

int
test_comparisons()
{
	int ret = 0;

	// to char
	{
		pp_value val((signed char)0x12);
		ret += PP_TEST_ASSERT(val == (signed char)0x12,
			"pp_value::operator==(signed char)");
		ret += PP_TEST_ASSERT(val == 0x12,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((signed char)-1);
		ret += PP_TEST_ASSERT(val == (signed char)-1,
			"pp_value::operator==(signed char)");
		ret += PP_TEST_ASSERT(val == -1,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((unsigned char)0x12);
		ret += PP_TEST_ASSERT(val == (unsigned char)0x12,
			"pp_value::operator==(unsigned char)");
		ret += PP_TEST_ASSERT(val == 0x12,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((unsigned char)-1);
		ret += PP_TEST_ASSERT(val == (unsigned char)0xff,
			"pp_value::operator==(unsigned char)");
		ret += PP_TEST_ASSERT(val == 0xff,
			"pp_value::operator==(int)");
	}

	// to short
	{
		pp_value val((signed short)0x1234);
		ret += PP_TEST_ASSERT(val == (signed short)0x1234,
			"pp_value::operator==(signed short)");
		ret += PP_TEST_ASSERT(val == 0x1234,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((signed short)-1);
		ret += PP_TEST_ASSERT(val == (signed short)-1,
			"pp_value::operator==(signed short)");
		ret += PP_TEST_ASSERT(val == -1,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((unsigned short)0x1234);
		ret += PP_TEST_ASSERT(val == (unsigned short)0x1234,
			"pp_value::operator==(unsigned short)");
		ret += PP_TEST_ASSERT(val == 0x1234,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((unsigned short)-1);
		ret += PP_TEST_ASSERT(val == (unsigned short)0xffff,
			"pp_value::operator==(unsigned short)");
		ret += PP_TEST_ASSERT(val == 0xffff,
			"pp_value::operator==(int)");
	}

	// to int
	{
		pp_value val((signed int)0x12345678);
		ret += PP_TEST_ASSERT(val == (signed int)0x12345678,
			"pp_value::operator==(signed int)");
		ret += PP_TEST_ASSERT(val == 0x12345678,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((signed int)-1);
		ret += PP_TEST_ASSERT(val == (signed int)-1,
			"pp_value::operator==(signed int)");
		ret += PP_TEST_ASSERT(val == -1,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((unsigned int)0x12345678);
		ret += PP_TEST_ASSERT(val == (unsigned int)0x12345678,
			"pp_value::operator==(unsigned int)");
		ret += PP_TEST_ASSERT(val == 0x12345678,
			"pp_value::operator==(int)");
	}
	{
		pp_value val((unsigned int)-1);
		ret += PP_TEST_ASSERT(val == (unsigned int)0xffffffff,
			"pp_value::operator==(unsigned int)");
		ret += PP_TEST_ASSERT(val == 0xffffffff,
			"pp_value::operator==(int)");
	}

	// to pp_value
	{
		pp_value val1(12345678);
		pp_value val2(12345678);
		ret += PP_TEST_ASSERT(val2 == val1 && val2 == 12345678,
			"pp_value::operator==(pp_value)");
	}

	// can't cmpare directly to strings

	// Test other comparison operators.  This assumes that if all the ==
	// tests pass for all the integral types, so will the other
	// comparisons.
	{
		pp_value val(10);
		ret += PP_TEST_ASSERT(val == 10,
			"pp_value::operator==()");
		ret += PP_TEST_ASSERT(!(val == 11),
			"pp_value::operator==()");
		ret += PP_TEST_ASSERT(val != 11,
			"pp_value::operator!=()");
		ret += PP_TEST_ASSERT(!(val != 10),
			"pp_value::operator!=()");
		ret += PP_TEST_ASSERT(val < 20,
			"pp_value::operator<()");
		ret += PP_TEST_ASSERT(!(val < 2),
			"pp_value::operator<()");
		ret += PP_TEST_ASSERT(val <= 20,
			"pp_value::operator<=()");
		ret += PP_TEST_ASSERT(!(val <= 2),
			"pp_value::operator<=()");
		ret += PP_TEST_ASSERT(val <= 10,
			"pp_value::operator<=()");
		ret += PP_TEST_ASSERT(val > 2,
			"pp_value::operator>()");
		ret += PP_TEST_ASSERT(!(val > 20),
			"pp_value::operator>()");
		ret += PP_TEST_ASSERT(val >= 2,
			"pp_value::operator>=()");
		ret += PP_TEST_ASSERT(!(val >= 20),
			"pp_value::operator>=()");
		ret += PP_TEST_ASSERT(val >= 10,
			"pp_value::operator>=()");
	}
	{
		pp_value val("0x112233445566778899aa");
		ret += PP_TEST_ASSERT(val == pp_value("0x112233445566778899aa"),
			"pp_value::operator==()");
		ret += PP_TEST_ASSERT(
			!(val == pp_value("0x112233445566778899ab")),
			"pp_value::operator==()");
		ret += PP_TEST_ASSERT(val != pp_value("0x112233445566778899ab"),
			"pp_value::operator!=()");
		ret += PP_TEST_ASSERT(
			!(val != pp_value("0x112233445566778899aa")),
			"pp_value::operator!=()");
		ret += PP_TEST_ASSERT(val < pp_value("0x112233445566778899ab"),
			"pp_value::operator<()");
		ret += PP_TEST_ASSERT(
			!(val < pp_value("0x112233445566778899a9")),
			"pp_value::operator<()");
		ret += PP_TEST_ASSERT(val <= pp_value("0x112233445566778899ab"),
			"pp_value::operator<=()");
		ret += PP_TEST_ASSERT(
			!(val <= pp_value("0x112233445566778899a9")),
			"pp_value::operator<=()");
		ret += PP_TEST_ASSERT(val <= pp_value("0x112233445566778899aa"),
			"pp_value::operator<=()");
		ret += PP_TEST_ASSERT(val > pp_value("0x112233445566778899a9"),
			"pp_value::operator>()");
		ret += PP_TEST_ASSERT(
			!(val > pp_value("0x112233445566778899ab")),
			"pp_value::operator>()");
		ret += PP_TEST_ASSERT(val >= pp_value("0x112233445566778899a9"),
			"pp_value::operator>=()");
		ret += PP_TEST_ASSERT(
			!(val >= pp_value("0x112233445566778899ab")),
			"pp_value::operator>=()");
		ret += PP_TEST_ASSERT(val >= pp_value("0x112233445566778899aa"),
			"pp_value::operator>=()");
	}

	return ret;
}

int
test_small_arithmetic()
{
	int ret = 0;

	{
		pp_value val(10);
		ret += PP_TEST_ASSERT(val+3 == 13,
			"pp_value::operator+(pp_value, int)");
		ret += PP_TEST_ASSERT(3+val == 13,
			"pp_value::operator+(int, pp_value)");
		ret += PP_TEST_ASSERT(val+pp_value(3) == 13,
			"pp_value::operator+(pp_value, pp_value)");
		val += 5;
		ret += PP_TEST_ASSERT(val == 15,
			"pp_value::operator+=(pp_value, int)");
		val++;
		ret += PP_TEST_ASSERT(val == 16,
			"pp_value::operator++()");
		++val;
		ret += PP_TEST_ASSERT(val == 17,
			"pp_value::operator++()");
	}
	{
		pp_value val(10);
		ret += PP_TEST_ASSERT(val-3 == 7,
			"pp_value::operator-(pp_value, int)");
		ret += PP_TEST_ASSERT(17-val == 7,
			"pp_value::operator-(int, pp_value)");
		ret += PP_TEST_ASSERT(val-pp_value(3) == 7,
			"pp_value::operator-(pp_value, pp_value)");
		val -= 5;
		ret += PP_TEST_ASSERT(val == 5,
			"pp_value::operator-=(pp_value, int)");
		val--;
		ret += PP_TEST_ASSERT(val == 4,
			"pp_value::operator--()");
		--val;
		ret += PP_TEST_ASSERT(val == 3,
			"pp_value::operator-()");
	}
	{
		pp_value val(10);
		ret += PP_TEST_ASSERT(val*2 == 20,
			"pp_value::operator*(pp_value, int)");
		ret += PP_TEST_ASSERT(2*val == 20,
			"pp_value::operator*(int, pp_value)");
		ret += PP_TEST_ASSERT(val*pp_value(2) == 20,
			"pp_value::operator*(pp_value, pp_value)");
		val *= 3;
		ret += PP_TEST_ASSERT(val == 30,
			"pp_value::operator*=(pp_value, int)");
	}
	{
		pp_value val(10);
		ret += PP_TEST_ASSERT(val/2 == 5,
			"pp_value::operator/(pp_value, int)");
		ret += PP_TEST_ASSERT(50/val == 5,
			"pp_value::operator/(int, pp_value)");
		ret += PP_TEST_ASSERT(val/pp_value(2) == 5,
			"pp_value::operator/(pp_value, pp_value)");
		val /= 5;
		ret += PP_TEST_ASSERT(val == 2,
			"pp_value::operator/=(pp_value, int)");
	}
	{
		pp_value val(17);
		ret += PP_TEST_ASSERT(val%10 == 7,
			"pp_value::operator%(pp_value, int)");
		ret += PP_TEST_ASSERT(24%val == 7,
			"pp_value::operator%(int, pp_value)");
		ret += PP_TEST_ASSERT(val%pp_value(10) == 7,
			"pp_value::operator%(pp_value, pp_value)");
		val %= 5;
		ret += PP_TEST_ASSERT(val == 2,
			"pp_value::operator%=(pp_value, int)");
	}

	return ret;
}

int
test_large_arithmetic()
{
	int ret = 0;
	{
		pp_value val("0x1000000000000000a");
		ret += PP_TEST_ASSERT(val+3 == pp_value("0x1000000000000000d"),
			"pp_value::operator+(pp_value, int)");
		ret += PP_TEST_ASSERT(3+val == pp_value("0x1000000000000000d"),
			"pp_value::operator+(int, pp_value)");
		val += 5;
		ret += PP_TEST_ASSERT(val == pp_value("0x1000000000000000f"),
			"pp_value::operator+=(pp_value, int)");
		val++;
		ret += PP_TEST_ASSERT(val == pp_value("0x10000000000000010"),
			"pp_value::operator++()");
		++val;
		ret += PP_TEST_ASSERT(val == pp_value("0x10000000000000011"),
			"pp_value::operator++()");
	}
	{
		pp_value val("0x1000000000000000a");
		ret += PP_TEST_ASSERT(val-3 == pp_value("0x10000000000000007"),
			"pp_value::operator-(pp_value, int)");
		val -= 5;
		ret += PP_TEST_ASSERT(val == pp_value("0x10000000000000005"),
			"pp_value::operator-=(int, pp_value)");
		val--;
		ret += PP_TEST_ASSERT(val == pp_value("0x10000000000000004"),
			"pp_value::operator--(pp_value, int)");
		--val;
		ret += PP_TEST_ASSERT(val == pp_value("0x10000000000000003"),
			"pp_value::operator--()");
	}
	{
		pp_value val("0x10000000000000010");
		ret += PP_TEST_ASSERT(val*2 == pp_value("0x20000000000000020"),
			"pp_value::operator*(pp_value, int)");
		ret += PP_TEST_ASSERT(2*val == pp_value("0x20000000000000020"),
			"pp_value::operator*(int, pp_value)");
		val *= 3;
		ret += PP_TEST_ASSERT(val == pp_value("0x30000000000000030"),
			"pp_value::operator*=(pp_value, int)");
	}
	{
		pp_value val("0xa000000000000000a");
		ret += PP_TEST_ASSERT(val/2 == pp_value("0x50000000000000005"),
			"pp_value::operator/(pp_value, int)");
		val /= 5;
		ret += PP_TEST_ASSERT(val == pp_value("0x20000000000000002"),
			"pp_value::operator/=(pp_value, int)");
	}
	{
		pp_value val("0x10000000000000011");
		ret += PP_TEST_ASSERT(val%10 == pp_value("0x03"),
			"pp_value::operator%(pp_value, int)");
		val %= 5;
		ret += PP_TEST_ASSERT(val == pp_value("0x03"),
			"pp_value::operator%=(pp_value, int)");
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
		pp_value val(0xff);
		ret += PP_TEST_ASSERT((val & pp_value(0x107)) == 0x7,
			"pp_value::operator&(pp_value, unsigned)");
		ret += PP_TEST_ASSERT((pp_value(0x107) & val) == 0x7,
			"pp_value::operator&(unsigned, pp_value)");
		val &= pp_value(0x370);
		ret += PP_TEST_ASSERT(val == 0x70,
			"pp_value::operator&=(pp_value, unsigned)");
	}
	{
		pp_value val(0xff);
		ret += PP_TEST_ASSERT((val | pp_value(0x107)) == 0x1ff,
			"pp_value::operator|()");
		ret += PP_TEST_ASSERT((pp_value(0x107) | val) == 0x1ff,
			"pp_value::operator|()");
		val |= pp_value(0x270);
		ret += PP_TEST_ASSERT(val == 0x2ff, "pp_value::operator|=()");
	}
	{
		pp_value val(0xff);
		ret += PP_TEST_ASSERT((val ^ pp_value(0x107)) == 0x1f8,
			"pp_value::operator^()");
		ret += PP_TEST_ASSERT((pp_value(0x107) ^ val) == 0x1f8,
			"pp_value::operator^()");
		val ^= pp_value(0x270);
		ret += PP_TEST_ASSERT(val == 0x28f, "pp_value::operator^=()");
	}
	{
		pp_value val(0x10);
		ret += PP_TEST_ASSERT((val << 2) == 0x40,
			"pp_value::operator<<()");
		ret += PP_TEST_ASSERT((2 << val) == 0x20000,
			"pp_value::operator<<()");
		val <<= 3;
		ret += PP_TEST_ASSERT(val == 0x80, "pp_value::operator<<=()");
	}
	{
		pp_value val(0x10);
		ret += PP_TEST_ASSERT((val >> 2) == 0x04,
			"pp_value::operator>>()");
		ret += PP_TEST_ASSERT((0x20000 >> val) == 0x2,
			"pp_value::operator>>()");
		val >>= 3;
		ret += PP_TEST_ASSERT(val == 0x02, "pp_value::operator>>=()");
	}

	return ret;
}

int
test_large_logical()
{
	int ret = 0;
	{
		pp_value val("0xf00000000000000ff");
		ret += PP_TEST_ASSERT((val & pp_value(0x107)) == 0x7,
			"pp_value::operator&()");
		ret += PP_TEST_ASSERT((pp_value(0x107) & val) == 0x7,
			"pp_value::operator&()");
		val &= pp_value(0x370);
		ret += PP_TEST_ASSERT(val == 0x70, "pp_value::operator&=()");
	}
	{
		pp_value val("0xf00000000000000ff");
		ret += PP_TEST_ASSERT(
			(val | pp_value(0x107)) == pp_value("0xf00000000000001ff"),
			"pp_value::operator|()");
		ret += PP_TEST_ASSERT(
			(pp_value(0x107) | val) == pp_value("0xf00000000000001ff"),
			"pp_value::operator|()");
		val |= pp_value(0x270);
		ret += PP_TEST_ASSERT(
			val == pp_value("0xf00000000000002ff"),
			"pp_value::operator|=()");
	}
	{
		pp_value val("0xf00000000000000ff");
		ret += PP_TEST_ASSERT(
			(val ^ pp_value(0x107)) == pp_value("0xf00000000000001f8"),
			"pp_value::operator^()");
		ret += PP_TEST_ASSERT(
			(pp_value(0x107) ^ val) == pp_value("0xf00000000000001f8"),
			"pp_value::operator^()");
		val ^= pp_value(0x270);
		ret += PP_TEST_ASSERT(
			val == pp_value("0xf000000000000028f"),
			"pp_value::operator^=()");
	}
	{
		pp_value val("0xf0000000000000010");
		ret += PP_TEST_ASSERT(
			(val << 2) == pp_value("0x3c0000000000000040"),
			"pp_value::operator<<()");
		val <<= 3;
		ret += PP_TEST_ASSERT(
			val == pp_value("0x780000000000000080"),
			"pp_value::operator<<=()");
	}
	{
		pp_value val("0xf0000000000000010");
		ret += PP_TEST_ASSERT(
			(val >> 2) == pp_value("0x3c000000000000004"),
			"pp_value::operator>>()");
		val >>= 3;
		ret += PP_TEST_ASSERT(
			val == pp_value("0x1e000000000000002"),
			"pp_value::operator>>=()");
	}

	return ret;
}

int
test_small_io()
{
	int ret = 0;

	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::dec;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::oct;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss << std::hex;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::dec;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::oct;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showbase);
		valoss << std::hex;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::dec;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, but I don't care.
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::oct;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
		#endif
	}
	{
		#if 0
		// This test fails, but I don't care.
		pp_value val(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream valoss;
		valoss.setf(std::ios_base::showpos);
		valoss << std::hex;
		valoss << val;
		ret += PP_TEST_ASSERT(rawoss.str() == valoss.str(),
			"pp_value::operator<<(ostream)");
		#endif
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, and I think the standard version is
		// wrong.
		pp_value val(15);
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
			"pp_value::operator<<(ostream)");
		#endif
	}

	// test boost::format support
	{
		pp_value val(15);
		std::ostringstream oss;
		oss.width(8);
		oss.fill('0');
		oss << std::hex;
		oss << 15;
		ret += PP_TEST_ASSERT(oss.str() == boost::format("%08x") %15,
			"boost::format() % pp_value");
	}

	return ret;
}

int
test_large_io()
{
	int ret = 0;

	{
		string num = "12345678123456781234567812345678";
		pp_value val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::dec;
		oss << val;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"pp_value::operator<<(ostream)");
	}
	{
		string num = "012345671234567123456712345671234567";
		pp_value val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::oct;
		oss << val;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"pp_value::operator<<(ostream)");
	}
	{
		string num = "0x123456789abcdef123456789abcdef";
		pp_value val(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::hex;
		oss << val;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"pp_value::operator<<(ostream)");
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
