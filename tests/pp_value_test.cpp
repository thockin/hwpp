#include "pp.h"
#include "pp_test.h"

using namespace std;

int
test_small_ctors()
{
	int ret = 0;

	{
		pp_value val;
		val = 12345678;
		if (val != 12345678) {
			PP_TEST_ERROR("pp_value::pp_value(int)");
			ret++;
		}
	}

	{
		pp_value val(12345678);
		if (val != 12345678) {
			PP_TEST_ERROR("pp_value::pp_value(int)");
			ret++;
		}
	}

	{
		pp_value val1(12345678);
		pp_value val2(val1);
		if (val2 != val1 || val2 != 12345678) {
			PP_TEST_ERROR("pp_value::pp_value(pp_value)");
			ret++;
		}
	}

	{
		pp_value val = 12345678;
		if (val != 12345678) {
			PP_TEST_ERROR("pp_value::operator=()");
			ret++;
		}
	}

	{
		pp_value val = pp_value_from_string("12345678");
		if (val != 12345678) {
			PP_TEST_ERROR("pp_value::pp_value(string)");
			ret++;
		}
	}
	{
		pp_value val = pp_value_from_string("0x1234");
		if (val != 0x1234) {
			PP_TEST_ERROR("pp_value::pp_value(string)");
			ret++;
		}
	}
	{
		pp_value val = pp_value_from_string("-1");
		if (val != pp_value(-1)) {
			PP_TEST_ERROR("pp_value::pp_value(string)");
			ret++;
		}
	}

	return ret;
}

int
test_large_ctors()
{
	int ret = 0;
	return ret;
}

int
test_small_arithmetic()
{
	int ret = 0;

	// test ADD
	{
		pp_value val = 1234;
		if (val + 4 != 1238) {
			PP_TEST_ERROR("pp_value::operator+()");
			ret++;
		}
		val += 3;
		if (val != 1237) {
			PP_TEST_ERROR("pp_value::operator+=()");
			ret++;
		}
	}

	// test SUBTRACT
	{
		pp_value val = 1234;
		if (val - 4 != 1230) {
			PP_TEST_ERROR("pp_value::operator-()");
			ret++;
		}
		val -= 3;
		if (val != 1231) {
			PP_TEST_ERROR("pp_value::operator-=()");
			ret++;
		}
	}

	// test MULTIPLY
	{
		pp_value val = 1234;
		if (val * 2 != 2468) {
			PP_TEST_ERROR("pp_value::operator*()");
			ret++;
		}
		val *= 3;
		if (val != 3702) {
			PP_TEST_ERROR("pp_value::operator*=()");
			ret++;
		}
	}

	// test DIVIDE
	{
		pp_value val = 1234;
		if (val / 2 != 617) {
			PP_TEST_ERROR("pp_value::operator/()");
			ret++;
		}
		val /= 3;
		if (val != 411) {
			PP_TEST_ERROR("pp_value::operator/=()");
			ret++;
		}
	}

	// test MODULO
	{
		pp_value val = 1234;
		if (val % 5 != 4) {
			PP_TEST_ERROR("pp_value::operator%()");
			ret++;
		}
		val %= 20;
		if (val != 14) {
			PP_TEST_ERROR("pp_value::operator%=()");
			ret++;
		}
	}

	// test UNARY PLUS and MINUS
	{
		pp_value val = 1234;
		val = +val;
		if (val != 1234) {
			PP_TEST_ERROR("pp_value::operator+()");
			ret++;
		}
		val = -val;
		if (val != pp_value(-1234)) {
			PP_TEST_ERROR("pp_value::operator%=()");
			ret++;
		}
	}

	return ret;
}

int
test_large_arithmetic()
{
	int ret = 0;
	return ret;
}

//NOTE:
//  Bitwise NOT is not asupported.  For an arbitrary-width number, it does
//  not have a clear meaning.
int
test_small_logical()
{
	int ret = 0;

	// test left-shift
	{
		pp_value val = 1024;
		if (val << 2 != 4096) {
			PP_TEST_ERROR("pp_value::operator<<()");
			ret++;
		}
		val <<= 3;
		if (val != 8192) {
			PP_TEST_ERROR("pp_value::operator<<=()");
			ret++;
		}
	}
	{
		pp_value val = 1025;
		if (val << 2 != 4100) {
			PP_TEST_ERROR("pp_value::operator<<()");
			ret++;
		}
		val <<= 3;
		if (val != 8200) {
			PP_TEST_ERROR("pp_value::operator<<=()");
			ret++;
		}
	}

	// test right-shift
	{
		pp_value val = 1024;
		if (val >> 2 != 256) {
			PP_TEST_ERROR("pp_value::operator>>()");
			ret++;
		}
		val >>= 3;
		if (val != 128) {
			PP_TEST_ERROR("pp_value::operator>>=()");
			ret++;
		}
	}
	{
		pp_value val = 1025;
		if (val >> 2 != 256) {
			PP_TEST_ERROR("pp_value::operator>>()");
			ret++;
		}
		val >>= 3;
		if (val != 128) {
			PP_TEST_ERROR("pp_value::operator>>=()");
			ret++;
		}
	}

	// test bitwise AND
	{
		pp_value val = 0xff;
		if ((val & 0x107) != 0x7) {
			PP_TEST_ERROR("pp_value::operator&()");
			ret++;
		}
		val &= 0x270;
		if (val != 0x70) {
			PP_TEST_ERROR("pp_value::operator&=()");
			ret++;
		}
	}

	// test bitwise OR
	{
		pp_value val = 0xff;
		if ((val | 0x107) != 0x1ff) {
			PP_TEST_ERROR("pp_value::operator|()");
			ret++;
		}
		val |= 0x270;
		if (val != 0x2ff) {
			PP_TEST_ERROR("pp_value::operator|=()");
			ret++;
		}
	}

	// test bitwise XOR
	{
		pp_value val = 0xff;
		if ((val ^ 0x107) != 0x1f8) {
			PP_TEST_ERROR("pp_value::operator^()");
			ret++;
		}
		val ^= 0x270;
		if (val != 0x28f) {
			PP_TEST_ERROR("pp_value::operator^=()");
			ret++;
		}
	}

	return ret;
}

int
test_large_logical()
{
	int ret = 0;
	return ret;
}

int
test_small_boolean()
{
	int ret = 0;

	// test boolean conversion
	{
		pp_value val = 0;
		if (val) {
			PP_TEST_ERROR("pp_value::operator bool()");
			ret++;
		}
	}
	{
		pp_value val = 1;
		if (!val) {
			PP_TEST_ERROR("pp_value::operator!()");
			ret++;
		}
	}

	// test comparators
	{
		pp_value val = 1234;
		if (val == 1235) {
			PP_TEST_ERROR("pp_value::operator==()");
			ret++;
		}
		if (val != 1234) {
			PP_TEST_ERROR("pp_value::operator!=()");
			ret++;
		}
		if (val < 1) {
			PP_TEST_ERROR("pp_value::operator<()");
			ret++;
		}
		if (val <= 1) {
			PP_TEST_ERROR("pp_value::operator<=()");
			ret++;
		}
		if (val > 2000) {
			PP_TEST_ERROR("pp_value::operator>()");
			ret++;
		}
		if (val >= 2000) {
			PP_TEST_ERROR("pp_value::operator>=()");
			ret++;
		}
	}

	return ret;
}

int
test_large_boolean()
{
	int ret = 0;
	return ret;
}

int
main(void)
{
	int ret = 0;

	ret += test_small_ctors();
	ret += test_large_ctors();
	ret += test_small_arithmetic();
	ret += test_large_arithmetic();
	ret += test_small_logical();
	ret += test_large_logical();
	ret += test_small_boolean();
	ret += test_large_logical();

	return ret;
}
