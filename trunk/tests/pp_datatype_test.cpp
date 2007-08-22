#include "pp_datatypes.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

//FIXME: test lookup()

int
test_pp_enum()
{
	int ret = 0;

	/* test the basic constructor */
	pp_enum e1;

	/* test the evaluate() method */
	e1.add_value("one", 1);
	e1.add_value("two", 2);
	e1.add_value("three", 3);
	if (e1.evaluate(2) != "two") {
		PP_TEST_ERROR("pp_enum::evaluate()");
		ret++;
	}
	if (e1.evaluate(0) != "<!!unknown!!>") {
		PP_TEST_ERROR("pp_enum::evaluate()");
		ret++;
	}

	return ret;
}

int
test_pp_bitmask()
{
	int ret = 0;

	/* test the basic constructor */
	pp_bitmask b1;

	/* test the evaluate() method */
	b1.add_bit("bit-one", 1);
	b1.add_bit("bit-two", 2);
	b1.add_bit("bit-three", 3);
	if (b1.evaluate(2) != "bit-one") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}
	if (b1.evaluate(1) != "<!!bit0!!>") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}
	if (b1.evaluate(6) != "bit-one bit-two") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}
	if (b1.evaluate(0) != "") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}

	return ret;
}

int
test_pp_int()
{
	int ret = 0;

	/* test the basic constructor */
	pp_int i1;

	/* test the evaluate() method */
	if (i1.evaluate(1) != "1") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}
	if (i1.evaluate(-1ULL) != "-1") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}

	/* test the units constructor */
	pp_int i2("units");

	/* test the evaluate() method */
	if (i2.evaluate(1) != "1 units") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}
	if (i2.evaluate(-1ULL) != "-1 units") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}

	return ret;
}

int
test_pp_uint()
{
	int ret = 0;

	/* test the basic constructor */
	pp_uint u1;

	/* test the evaluate() method */
	if (u1.evaluate(1) != "1") {
		PP_TEST_ERROR("pp_uint::evaluate()");
		ret++;
	}
	if (u1.evaluate(-1ULL) != to_string(UINT64_MAX)) {
		PP_TEST_ERROR("pp_uint::evaluate()");
		ret++;
	}

	/* test the units constructor */
	pp_uint u2("units");

	/* test the evaluate() method */
	if (u2.evaluate(1) != "1 units") {
		PP_TEST_ERROR("pp_uint::evaluate()");
		ret++;
	}
	if (u2.evaluate(-1ULL) != to_string(UINT64_MAX) + " units") {
		PP_TEST_ERROR("pp_uint::evaluate()");
		ret++;
	}

	return ret;
}

int
test_pp_hex()
{
	int ret = 0;

	/* test the basic constructor */
	pp_hex h1;

	/* test the evaluate() method */
	if (h1.evaluate(1) != "0x1") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	if (h1.evaluate(-1ULL) != "0xffffffffffffffff") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}

	/* test the units constructor */
	pp_hex h2(BITS0, "units");
	if (h2.evaluate(1) != "0x1 units") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	if (h2.evaluate(-1ULL) != "0xffffffffffffffff units") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}

	/* test the width constructor */
	pp_hex h3(BITS8);

	/* test the evaluate() method */
	if (h3.evaluate(1) != "0x01") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	if (h3.evaluate((uint8_t)-1) != "0xff") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h4(BITS16);
	if (h4.evaluate(0x0201) != "0x0201") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	if (h4.evaluate((uint16_t)-1) != "0xffff") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h5(BITS32);
	if (h5.evaluate(0x04030201) != "0x04030201") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	if (h5.evaluate((uint32_t)-1) != "0xffffffff") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h6(BITS64);
	if (h6.evaluate(0x0807060504030201ULL) != "0x0807060504030201") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	if (h6.evaluate(-1ULL) != "0xffffffffffffffff") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}

	/* test the units-and-width constructor */
	pp_hex h7(BITS8, "units");

	/* test the evaluate() method */
	if (h7.evaluate(1) != "0x01 units") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h8(BITS16, "units");
	if (h8.evaluate(0x0201) != "0x0201 units") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h9(BITS32, "units");
	if (h9.evaluate(0x04030201) != "0x04030201 units") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h10(BITS64, "units");
	if (h10.evaluate(0x0807060504030201ULL) != "0x0807060504030201 units") {
		PP_TEST_ERROR("h10.evaluate()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pp_enum();
	if (r) return EXIT_FAILURE;
	r = test_pp_bitmask();
	if (r) return EXIT_FAILURE;
	r = test_pp_int();
	if (r) return EXIT_FAILURE;
	r = test_pp_uint();
	if (r) return EXIT_FAILURE;
	r = test_pp_hex();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
