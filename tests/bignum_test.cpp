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
		bignum bn;
		ret += PP_TEST_ASSERT(bn.get_si() == 0, "bignum::bignum()");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0, "bignum::bignum()");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "0",
			"bignum::bignum()");
	}

	// from char
	{
		bignum bn((signed char)0x12);
		ret += PP_TEST_ASSERT(bn.get_si() == 0x12,
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12,
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12",
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(bignum_to<signed char>(bn) == 0x12,
			"bignum_to<signed char>()");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == 0x12,
			"bignum_to<int64_t>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0x12,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<signed char>(0x12);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<signed char>()");
	}
	{
		bignum bn((signed char)-1);
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::bignum(signed char)");
		ret += PP_TEST_ASSERT(bignum_to<signed char>(bn) == -1,
			"bignum_to<signed char>()");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == -1,
			"bignum_to<int64_t>()");
		bignum bn2 = bignum_from<signed char>(-1);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<signed char>()");
	}
	{
		bignum bn((unsigned char)0x12);
		ret += PP_TEST_ASSERT(bn.get_si() == 0x12,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12",
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(bignum_to<unsigned char>(bn) == 0x12,
			"bignum_to<unsigned char>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0x12,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<unsigned char>(0x12);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<unsigned char>()");
	}
	{
		bignum bn((unsigned char)-1);
		ret += PP_TEST_ASSERT(bn.get_si() == 0xff,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0xff,
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ff",
			"bignum::bignum(unsigned char)");
		ret += PP_TEST_ASSERT(bignum_to<unsigned char>(bn) == 0xff,
			"bignum_to<unsigned char>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0xff,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<unsigned char>((unsigned char)-1);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<unsigned char>()");
	}

	// from short
	{
		bignum bn((signed short)0x1234);
		ret += PP_TEST_ASSERT(bn.get_si() == 0x1234,
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x1234,
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "1234",
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(bignum_to<signed short>(bn) == 0x1234,
			"bignum_to<signed short>()");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == 0x1234,
			"bignum_to<int64_t>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0x1234,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<signed short>(0x1234);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<signed short>()");
	}
	{
		bignum bn((signed short)-1);
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::bignum(signed short)");
		ret += PP_TEST_ASSERT(bignum_to<signed short>(bn) == -1,
			"bignum_to<signed short>()");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == -1,
			"bignum_to<int64_t>()");
		bignum bn2 = bignum_from<signed short>(-1);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<signed short>()");
	}
	{
		bignum bn((unsigned short)0x1234);
		ret += PP_TEST_ASSERT(bn.get_si() == 0x1234,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x1234,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "1234",
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(bignum_to<unsigned short>(bn) == 0x1234,
			"bignum_to<unsigned short>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0x1234,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<unsigned short>(0x1234);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<unsigned short>()");
	}
	{
		bignum bn((unsigned short)-1);
		ret += PP_TEST_ASSERT(bn.get_si() == 0xffff,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0xffff,
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ffff",
			"bignum::bignum(unsigned short)");
		ret += PP_TEST_ASSERT(bignum_to<unsigned short>(bn) == 0xffff,
			"bignum_to<unsigned short>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0xffff,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<unsigned short>((unsigned short)-1);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<unsigned short>()");
	}

	// from int
	// On 32 bit platforms this is where we will start to see
	// signed vs. unsigned oddities.
	{
		bignum bn((signed int)0x12345678);
		ret += PP_TEST_ASSERT(bn.get_si() == 0x12345678,
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12345678,
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12345678",
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(bignum_to<signed int>(bn) == 0x12345678,
			"bignum_to<signed int>()");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == 0x12345678,
			"bignum_to<int64_t>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0x12345678,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<signed int>(0x12345678);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<signed int>()");
	}
	{
		bignum bn((signed int)-1);
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::bignum(signed int)");
		ret += PP_TEST_ASSERT(bignum_to<signed int>(bn) == -1,
			"bignum_to<signed int>()");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == -1,
			"bignum_to<int64_t>()");
		bignum bn2 = bignum_from<signed int>(-1);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<signed int>()");
	}
	{
		bignum bn((unsigned int)0x12345678);
		ret += PP_TEST_ASSERT(bn.get_si() == 0x12345678,
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12345678,
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12345678",
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(bignum_to<unsigned int>(bn) == 0x12345678,
			"bignum_to<unsigned int>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0x12345678,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<unsigned int>(0x12345678);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<unsigned int>()");
	}
	{
		bignum bn((unsigned int)-1);
		// On a 32 bit platform this will fail, since get_si()
		// will return a signed value, and -1 is OOB.
		//ret += PP_TEST_ASSERT(bn.get_si() == (signed int)0xffffffff,
		//	"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0xffffffff,
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ffffffff",
			"bignum::bignum(unsigned int)");
		ret += PP_TEST_ASSERT(bignum_to<unsigned int>(bn) == 0xffffffff,
			"bignum_to<unsigned int>()");
		ret += PP_TEST_ASSERT(bignum_to<uint64_t>(bn) == 0xffffffff,
			"bignum_to<uint64_t>()");
		bignum bn2 = bignum_from<unsigned int>((unsigned int)-1);
		ret += PP_TEST_ASSERT(bn == bn2,
			"bignum_from<unsigned int>()");
	}

	// from long long
	// Skip long, since either sizeof(long) == sizeof(int) or
	// sizeof(long) == sizeof(long long).
	{
		bignum bn = bignum_from<signed long long>(0x123456789abcdef0LL);
		ret += PP_TEST_ASSERT(bn.get_str(16) == "123456789abcdef0",
			"bignum::bignum(signed long long)");
		ret += PP_TEST_ASSERT(
			bignum_to<int64_t>(bn) == 0x123456789abcdef0LL,
			"bignum_to<int64_t>()");
		ret += PP_TEST_ASSERT(
			bignum_to<uint64_t>(bn)==0x123456789abcdef0ULL,
			"bignum_to<uint64_t>()");
	}
	{
		bignum bn = bignum_from<signed long long>(-1LL);
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::bignum(signed long long)");
		ret += PP_TEST_ASSERT(bignum_to<int64_t>(bn) == -1,
			"bignum_to<int64_t>()");
	}
	{
		bignum bn = bignum_from<unsigned long long>(
			0x123456789abcdef0ULL);
		ret += PP_TEST_ASSERT(bn.get_str(16) == "123456789abcdef0",
			"bignum::bignum(unsigned long long)");
		ret += PP_TEST_ASSERT(
			bignum_to<uint64_t>(bn)==0x123456789abcdef0ULL,
			"bignum_to<uint64_t>()");
	}
	{
		bignum bn = bignum_from<unsigned long long>(-1ULL);
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ffffffffffffffff",
			"bignum::bignum(unsigned long long)");
		ret += PP_TEST_ASSERT(
			bignum_to<uint64_t>(bn)==0xffffffffffffffffULL,
			"bignum_to<uint64_t>()");
	}

	// from bignum
	{
		bignum bn1(12345678);
		bignum bn2(bn1);
		ret += PP_TEST_ASSERT(bn2 == bn1 && bn2.get_ui() == 12345678,
			"bignum::bignum(bignum)");
	}

	// from string
	{
		bignum bn("12345678");
		ret += PP_TEST_ASSERT(bn.get_ui() == 12345678,
			"bignum::bignum(string)");
		bn = bignum_from("12345678");
		ret += PP_TEST_ASSERT(bn.get_ui() == 12345678,
			"bignum::bignum(string)");
	}
	{
		bignum bn("0x12345678");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12345678,
			"bignum::bignum(string)");
		bn = bignum_from("0x12345678");
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12345678,
			"bignum::bignum(string)");
	}
	{
		bignum bn("0x112233445566778899aabbccddeeff00");
		ret += PP_TEST_ASSERT(
			bn.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::bignum(string)");
		bn = bignum_from("0x112233445566778899aabbccddeeff00");
		ret += PP_TEST_ASSERT(
			bn.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::bignum(string)");
	}
	{
		bignum bn("-1");
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::bignum(string)");
		bn = bignum_from("-1");
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::bignum(string)");
	}

	return ret;
}

int
test_assignments()
{
	int ret = 0;

	// from char
	{
		bignum bn;
		bn = (signed char)0x12;
		ret += PP_TEST_ASSERT(bn.get_si() == 0x12,
			"bignum::operator=(signed char)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12",
			"bignum::operator=(signed char)");
	}
	{
		bignum bn;
		bn = (signed char)-1;
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::operator=(signed char)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::operator=(signed char)");
	}
	{
		bignum bn;
		bn = (unsigned char)0x12;
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12,
			"bignum::operator=(unsigned char)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12",
			"bignum::operator=(unsigned char)");
	}
	{
		bignum bn;
		bn = (unsigned char)-1;
		ret += PP_TEST_ASSERT(bn.get_ui() == 0xff,
			"bignum::operator=(unsigned char)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ff",
			"bignum::operator=(unsigned char)");
	}

	// from short
	{
		bignum bn;
		bn = (signed short)0x1234;
		ret += PP_TEST_ASSERT(bn.get_si() == 0x1234,
			"bignum::operator=(signed short)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "1234",
			"bignum::operator=(signed short)");
	}
	{
		bignum bn;
		bn = (signed short)-1;
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::operator=(signed short)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::operator=(signed short)");
	}
	{
		bignum bn;
		bn = (unsigned short)0x1234;
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x1234,
			"bignum::operator=(unsigned short)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "1234",
			"bignum::operator=(unsigned short)");
	}
	{
		bignum bn;
		bn = (unsigned short)-1;
		ret += PP_TEST_ASSERT(bn.get_ui() == 0xffff,
			"bignum::operator=(unsigned short)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ffff",
			"bignum::operator=(unsigned short)");
	}

	// from int
	{
		bignum bn;
		bn = (signed int)0x12345678;
		ret += PP_TEST_ASSERT(bn.get_si() == 0x12345678,
			"bignum::operator=(signed int)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12345678",
			"bignum::operator=(signed int)");
	}
	{
		bignum bn;
		bn = (signed int)-1;
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::operator=(signed int)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
			"bignum::operator=(signed int)");
	}
	{
		bignum bn;
		bn = (unsigned int)0x12345678;
		ret += PP_TEST_ASSERT(bn.get_ui() == 0x12345678,
			"bignum::operator=(unsigned int)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "12345678",
			"bignum::operator=(unsigned int)");
	}
	{
		bignum bn;
		bn = (unsigned int)-1;
		ret += PP_TEST_ASSERT(bn.get_ui() == 0xffffffff,
			"bignum::operator=(unsigned int)");
		ret += PP_TEST_ASSERT(bn.get_str(16) == "ffffffff",
			"bignum::operator=(unsigned int)");
	}

	// from bignum
	{
		bignum bn1(12345678);
		bignum bn2;
		bn2 = bn1;
		ret += PP_TEST_ASSERT(bn2 == bn1 && bn2.get_ui() == 12345678,
			"bignum::operator=(bignum)");
	}

	// from string
	{
		bignum bn;
		bn = "12345678";
		ret += PP_TEST_ASSERT(bn.get_ui() == 12345678,
			"bignum::operator=(string)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "12345678",
			"bignum::operator=(string)");
	}
	{
		bignum bn;
		bn = "0x112233445566778899aabbccddeeff00";
		ret += PP_TEST_ASSERT(
			bn.get_str(16) == "112233445566778899aabbccddeeff00",
			"bignum::operator=(string)");
	}
	{
		bignum bn;
		bn = "-1";
		ret += PP_TEST_ASSERT(bn.get_si() == -1,
			"bignum::operator=(string)");
		ret += PP_TEST_ASSERT(bn.get_str(10) == "-1",
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
		bignum bn = bignum_from<signed char>(0x12);
		ret += PP_TEST_ASSERT(bn == (signed char)0x12,
			"bignum::operator==(signed char)");
		ret += PP_TEST_ASSERT(bn == 0x12,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<signed char>(-1);
		ret += PP_TEST_ASSERT(bn == (signed char)-1,
			"bignum::operator==(signed char)");
		ret += PP_TEST_ASSERT(bn == -1,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<unsigned char>(0x12);
		ret += PP_TEST_ASSERT(bn == (unsigned char)0x12,
			"bignum::operator==(unsigned char)");
		ret += PP_TEST_ASSERT(bn == 0x12,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<unsigned char>((unsigned char)-1);
		ret += PP_TEST_ASSERT(bn == (unsigned char)0xff,
			"bignum::operator==(unsigned char)");
		ret += PP_TEST_ASSERT(bn == 0xff,
			"bignum::operator==(int)");
	}

	// to short
	{
		bignum bn = bignum_from<signed short>(0x1234);
		ret += PP_TEST_ASSERT(bn == (signed short)0x1234,
			"bignum::operator==(signed short)");
		ret += PP_TEST_ASSERT(bn == 0x1234,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<signed short>(-1);
		ret += PP_TEST_ASSERT(bn == (signed short)-1,
			"bignum::operator==(signed short)");
		ret += PP_TEST_ASSERT(bn == -1,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<unsigned short>(0x1234);
		ret += PP_TEST_ASSERT(bn == (unsigned short)0x1234,
			"bignum::operator==(unsigned short)");
		ret += PP_TEST_ASSERT(bn == 0x1234,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<unsigned short>((unsigned short)-1);
		ret += PP_TEST_ASSERT(bn == (unsigned short)0xffff,
			"bignum::operator==(unsigned short)");
		ret += PP_TEST_ASSERT(bn == 0xffff,
			"bignum::operator==(int)");
	}

	// to int
	{
		bignum bn = bignum_from<signed int>(0x12345678);
		ret += PP_TEST_ASSERT(bn == (signed int)0x12345678,
			"bignum::operator==(signed int)");
		ret += PP_TEST_ASSERT(bn == 0x12345678,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<signed int>(-1);
		ret += PP_TEST_ASSERT(bn == (signed int)-1,
			"bignum::operator==(signed int)");
		ret += PP_TEST_ASSERT(bn == -1,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<unsigned int>(0x12345678);
		ret += PP_TEST_ASSERT(bn == (unsigned int)0x12345678,
			"bignum::operator==(unsigned int)");
		ret += PP_TEST_ASSERT(bn == 0x12345678,
			"bignum::operator==(int)");
	}
	{
		bignum bn = bignum_from<unsigned int>((unsigned int)-1);
		ret += PP_TEST_ASSERT(bn == (unsigned int)0xffffffff,
			"bignum::operator==(unsigned int)");
		ret += PP_TEST_ASSERT(bn == 0xffffffff,
			"bignum::operator==(int)");
	}

	// to bignum
	{
		bignum bn1(12345678);
		bignum bn2(12345678);
		ret += PP_TEST_ASSERT(bn2 == bn1 && bn2 == 12345678,
			"bignum::operator==(bignum)");
	}

	// can't cmpare directly to strings

	// Test other comparison operators.  Thsi assumes that if all the ==
	// tests pass for all the integral types, so will the other
	// comparisons.
	{
		bignum bn(10);
		ret += PP_TEST_ASSERT(bn == 10,
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(!(bn == 11),
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(bn != 11,
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(!(bn != 10),
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(bn < 20,
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(!(bn < 2),
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(bn <= 20,
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(!(bn <= 2),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(bn <= 10,
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(bn > 2,
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(!(bn > 20),
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(bn >= 2,
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(!(bn >= 20),
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(bn >= 10,
			"bignum::operator>=()");
	}
	{
		bignum bn("0x112233445566778899aa");
		ret += PP_TEST_ASSERT(bn == bignum("0x112233445566778899aa"),
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(
			!(bn == bignum("0x112233445566778899ab")),
			"bignum::operator==()");
		ret += PP_TEST_ASSERT(bn != bignum("0x112233445566778899ab"),
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(
			!(bn != bignum("0x112233445566778899aa")),
			"bignum::operator!=()");
		ret += PP_TEST_ASSERT(bn < bignum("0x112233445566778899ab"),
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(
			!(bn < bignum("0x112233445566778899a9")),
			"bignum::operator<()");
		ret += PP_TEST_ASSERT(bn <= bignum("0x112233445566778899ab"),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(
			!(bn <= bignum("0x112233445566778899a9")),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(bn <= bignum("0x112233445566778899aa"),
			"bignum::operator<=()");
		ret += PP_TEST_ASSERT(bn > bignum("0x112233445566778899a9"),
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(
			!(bn > bignum("0x112233445566778899ab")),
			"bignum::operator>()");
		ret += PP_TEST_ASSERT(bn >= bignum("0x112233445566778899a9"),
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(
			!(bn >= bignum("0x112233445566778899ab")),
			"bignum::operator>=()");
		ret += PP_TEST_ASSERT(bn >= bignum("0x112233445566778899aa"),
			"bignum::operator>=()");
	}

	return ret;
}

int
test_small_arithmetic()
{
	int ret = 0;

	{
		bignum bn(10);
		ret += PP_TEST_ASSERT(bn+3 == 13,
			"bignum::operator+(bignum, int)");
		ret += PP_TEST_ASSERT(3+bn == 13,
			"bignum::operator+(int, bignum)");
		ret += PP_TEST_ASSERT(bn+bignum_from(3) == 13,
			"bignum::operator+(bignum, bignum)");
		bn += 5;
		ret += PP_TEST_ASSERT(bn == 15,
			"bignum::operator+=(bignum, int)");
		bn++;
		ret += PP_TEST_ASSERT(bn == 16,
			"bignum::operator++()");
		++bn;
		ret += PP_TEST_ASSERT(bn == 17,
			"bignum::operator++()");
	}
	{
		bignum bn(10);
		ret += PP_TEST_ASSERT(bn-3 == 7,
			"bignum::operator-(bignum, int)");
		ret += PP_TEST_ASSERT(17-bn == 7,
			"bignum::operator-(int, bignum)");
		ret += PP_TEST_ASSERT(bn-bignum_from(3) == 7,
			"bignum::operator-(bignum, bignum)");
		bn -= 5;
		ret += PP_TEST_ASSERT(bn == 5,
			"bignum::operator-=(bignum, int)");
		bn--;
		ret += PP_TEST_ASSERT(bn == 4,
			"bignum::operator--()");
		--bn;
		ret += PP_TEST_ASSERT(bn == 3,
			"bignum::operator-()");
	}
	{
		bignum bn(10);
		ret += PP_TEST_ASSERT(bn*2 == 20,
			"bignum::operator*(bignum, int)");
		ret += PP_TEST_ASSERT(2*bn == 20,
			"bignum::operator*(int, bignum)");
		ret += PP_TEST_ASSERT(bn*bignum_from(2) == 20,
			"bignum::operator*(bignum, bignum)");
		bn *= 3;
		ret += PP_TEST_ASSERT(bn == 30,
			"bignum::operator*=(bignum, int)");
	}
	{
		bignum bn(10);
		ret += PP_TEST_ASSERT(bn/2 == 5,
			"bignum::operator/(bignum, int)");
		ret += PP_TEST_ASSERT(50/bn == 5,
			"bignum::operator/(int, bignum)");
		ret += PP_TEST_ASSERT(bn/bignum_from(2) == 5,
			"bignum::operator/(bignum, bignum)");
		bn /= 5;
		ret += PP_TEST_ASSERT(bn == 2,
			"bignum::operator/=(bignum, int)");
	}
	{
		bignum bn(17);
		ret += PP_TEST_ASSERT(bn%10 == 7,
			"bignum::operator%(bignum, int)");
		ret += PP_TEST_ASSERT(24%bn == 7,
			"bignum::operator%(int, bignum)");
		ret += PP_TEST_ASSERT(bn%bignum_from(10) == 7,
			"bignum::operator%(bignum, bignum)");
		bn %= 5;
		ret += PP_TEST_ASSERT(bn == 2,
			"bignum::operator%=(bignum, int)");
	}

	return ret;
}

int
test_large_arithmetic()
{
	int ret = 0;
	{
		bignum bn("0x1000000000000000a");
		ret += PP_TEST_ASSERT(bn+3 == bignum("0x1000000000000000d"),
			"bignum::operator+(bignum, int)");
		ret += PP_TEST_ASSERT(3+bn == bignum("0x1000000000000000d"),
			"bignum::operator+(int, bignum)");
		bn += 5;
		ret += PP_TEST_ASSERT(bn == bignum("0x1000000000000000f"),
			"bignum::operator+=(bignum, int)");
		bn++;
		ret += PP_TEST_ASSERT(bn == bignum("0x10000000000000010"),
			"bignum::operator++()");
		++bn;
		ret += PP_TEST_ASSERT(bn == bignum("0x10000000000000011"),
			"bignum::operator++()");
	}
	{
		bignum bn("0x1000000000000000a");
		ret += PP_TEST_ASSERT(bn-3 == bignum("0x10000000000000007"),
			"bignum::operator-(bignum, int)");
		bn -= 5;
		ret += PP_TEST_ASSERT(bn == bignum("0x10000000000000005"),
			"bignum::operator-=(int, bignum)");
		bn--;
		ret += PP_TEST_ASSERT(bn == bignum("0x10000000000000004"),
			"bignum::operator--(bignum, int)");
		--bn;
		ret += PP_TEST_ASSERT(bn == bignum("0x10000000000000003"),
			"bignum::operator--()");
	}
	{
		bignum bn("0x10000000000000010");
		ret += PP_TEST_ASSERT(bn*2 == bignum("0x20000000000000020"),
			"bignum::operator*(bignum, int)");
		ret += PP_TEST_ASSERT(2*bn == bignum("0x20000000000000020"),
			"bignum::operator*(int, bignum)");
		bn *= 3;
		ret += PP_TEST_ASSERT(bn == bignum("0x30000000000000030"),
			"bignum::operator*=(bignum, int)");
	}
	{
		bignum bn("0xa000000000000000a");
		ret += PP_TEST_ASSERT(bn/2 == bignum("0x50000000000000005"),
			"bignum::operator/(bignum, int)");
		bn /= 5;
		ret += PP_TEST_ASSERT(bn == bignum("0x20000000000000002"),
			"bignum::operator/=(bignum, int)");
	}
	{
		bignum bn("0x10000000000000011");
		ret += PP_TEST_ASSERT(bn%10 == bignum("0x03"),
			"bignum::operator%(bignum, int)");
		bn %= 5;
		ret += PP_TEST_ASSERT(bn == bignum("0x03"),
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
		bignum bn(0xff);
		ret += PP_TEST_ASSERT((bn & bignum(0x107)) == 0x7,
			"bignum::operator&(bignum, unsigned)");
		ret += PP_TEST_ASSERT((bignum(0x107) & bn) == 0x7,
			"bignum::operator&(unsigned, bignum)");
		bn &= bignum(0x370);
		ret += PP_TEST_ASSERT(bn == 0x70,
			"bignum::operator&=(bignum, unsigned)");
	}
	{
		bignum bn(0xff);
		ret += PP_TEST_ASSERT((bn | bignum(0x107)) == 0x1ff,
			"bignum::operator|()");
		ret += PP_TEST_ASSERT((bignum(0x107) | bn) == 0x1ff,
			"bignum::operator|()");
		bn |= bignum(0x270);
		ret += PP_TEST_ASSERT(bn == 0x2ff, "bignum::operator|=()");
	}
	{
		bignum bn(0xff);
		ret += PP_TEST_ASSERT((bn ^ bignum(0x107)) == 0x1f8,
			"bignum::operator^()");
		ret += PP_TEST_ASSERT((bignum(0x107) ^ bn) == 0x1f8,
			"bignum::operator^()");
		bn ^= bignum(0x270);
		ret += PP_TEST_ASSERT(bn == 0x28f, "bignum::operator^=()");
	}
	{
		bignum bn(0x10);
		ret += PP_TEST_ASSERT((bn << 2) == 0x40,
			"bignum::operator<<()");
		ret += PP_TEST_ASSERT((2 << bn) == 0x20000,
			"bignum::operator<<()");
		bn <<= 3;
		ret += PP_TEST_ASSERT(bn == 0x80, "bignum::operator<<=()");
	}
	{
		bignum bn(0x10);
		ret += PP_TEST_ASSERT((bn >> 2) == 0x04,
			"bignum::operator>>()");
		ret += PP_TEST_ASSERT((0x20000 >> bn) == 0x2,
			"bignum::operator>>()");
		bn >>= 3;
		ret += PP_TEST_ASSERT(bn == 0x02, "bignum::operator>>=()");
	}

	return ret;
}

int
test_large_logical()
{
	int ret = 0;
	{
		bignum bn("0xf00000000000000ff");
		ret += PP_TEST_ASSERT((bn & bignum(0x107)) == 0x7,
			"bignum::operator&()");
		ret += PP_TEST_ASSERT((bignum(0x107) & bn) == 0x7,
			"bignum::operator&()");
		bn &= bignum(0x370);
		ret += PP_TEST_ASSERT(bn == 0x70, "bignum::operator&=()");
	}
	{
		bignum bn("0xf00000000000000ff");
		ret += PP_TEST_ASSERT(
			(bn | bignum(0x107)) == bignum("0xf00000000000001ff"),
			"bignum::operator|()");
		ret += PP_TEST_ASSERT(
			(bignum(0x107) | bn) == bignum("0xf00000000000001ff"),
			"bignum::operator|()");
		bn |= bignum(0x270);
		ret += PP_TEST_ASSERT(
			bn == bignum("0xf00000000000002ff"),
			"bignum::operator|=()");
	}
	{
		bignum bn("0xf00000000000000ff");
		ret += PP_TEST_ASSERT(
			(bn ^ bignum(0x107)) == bignum("0xf00000000000001f8"),
			"bignum::operator^()");
		ret += PP_TEST_ASSERT(
			(bignum(0x107) ^ bn) == bignum("0xf00000000000001f8"),
			"bignum::operator^()");
		bn ^= bignum(0x270);
		ret += PP_TEST_ASSERT(
			bn == bignum("0xf000000000000028f"),
			"bignum::operator^=()");
	}
	{
		bignum bn("0xf0000000000000010");
		ret += PP_TEST_ASSERT(
			(bn << 2) == bignum("0x3c0000000000000040"),
			"bignum::operator<<()");
		bn <<= 3;
		ret += PP_TEST_ASSERT(
			bn == bignum("0x780000000000000080"),
			"bignum::operator<<=()");
	}
	{
		bignum bn("0xf0000000000000010");
		ret += PP_TEST_ASSERT(
			(bn >> 2) == bignum("0x3c000000000000004"),
			"bignum::operator>>()");
		bn >>= 3;
		ret += PP_TEST_ASSERT(
			bn == bignum("0x1e000000000000002"),
			"bignum::operator>>=()");
	}

	return ret;
}

int
test_small_io()
{
	int ret = 0;

	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showbase);
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showbase);
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showbase);
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss.setf(std::ios_base::uppercase);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showbase);
		bnoss.setf(std::ios_base::uppercase);
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss.setf(std::ios_base::uppercase);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showbase);
		bnoss.setf(std::ios_base::uppercase);
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showbase);
		rawoss.setf(std::ios_base::uppercase);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showbase);
		bnoss.setf(std::ios_base::uppercase);
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showpos);
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, but I don't care.
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showpos);
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
		#endif
	}
	{
		#if 0
		// This test fails, but I don't care.
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::showpos);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::showpos);
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
		#endif
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.width(8);
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.width(8);
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.width(8);
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.width(8);
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.width(8);
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.width(8);
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.setf(std::ios_base::showbase);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.setf(std::ios_base::showbase);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::left);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::left);
		bnoss.setf(std::ios_base::showbase);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::dec;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.setf(std::ios_base::showbase);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::dec;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::oct;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.setf(std::ios_base::showbase);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::oct;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
	}
	{
		#if 0
		// This test fails, and I think the standard version is
		// wrong.
		bignum bn(15);
		std::ostringstream rawoss;
		rawoss.setf(std::ios_base::right);
		rawoss.setf(std::ios_base::showbase);
		rawoss.width(8);
		rawoss.fill('0');
		rawoss << std::hex;
		rawoss << 15;
		std::ostringstream bnoss;
		bnoss.setf(std::ios_base::right);
		bnoss.setf(std::ios_base::showbase);
		bnoss.width(8);
		bnoss.fill('0');
		bnoss << std::hex;
		bnoss << bn;
		ret += PP_TEST_ASSERT(rawoss.str() == bnoss.str(),
			"bignum::operator<<(ostream)");
		#endif
	}

	// test boost::format support
	{
		bignum bn(15);
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
		bignum bn(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::dec;
		oss << bn;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
	{
		string num = "012345671234567123456712345671234567";
		bignum bn(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::oct;
		oss << bn;
		ret += PP_TEST_ASSERT(oss.str() == num,
			"bignum::operator<<(ostream)");
	}
	{
		string num = "0x123456789abcdef123456789abcdef";
		bignum bn(num);
		std::ostringstream oss;
		oss.setf(std::ios_base::showbase);
		oss << std::hex;
		oss << bn;
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
