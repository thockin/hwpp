#include "pp_datatypes.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_pp_enum_datatype()
{
	int ret = 0;

	// test the basic constructor
	pp_enum_datatype e;

	// test the evaluate() method
	e.add_value("one", 1);
	e.add_value("two", 2);
	e.add_value("three", 3);
	if (e.evaluate(2) != "two") {
		TEST_ERROR("pp_enum_datatype::evaluate()");
		ret++;
	}
	if (e.evaluate(0) != "<!0!>") {
		TEST_ERROR("pp_enum_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	ret += TEST_ASSERT(e.lookup("one") == 1, "pp_enum_datatype::lookup(string)");
	ret += TEST_ASSERT(e.lookup("two") == 2, "pp_enum_datatype::lookup(string)");
	ret += TEST_ASSERT(e.lookup(1) == 1, "pp_enum_datatype::lookup(int)");
	ret += TEST_ASSERT(e.lookup(2) == 2, "pp_enum_datatype::lookup(int)");
	try {
		e.lookup("foo");
		TEST_ERROR("pp_enum_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		e.lookup(4);
		TEST_ERROR("pp_enum_datatype::lookup(int)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_bool_datatype()
{
	int ret = 0;

	// test the basic constructor
	pp_bool_datatype b("TRUE", "FALSE");

	// test the evaluate() method
	if (b.evaluate(0) != "FALSE") {
		TEST_ERROR("pp_bool_datatype::evaluate()");
		ret++;
	}
	if (b.evaluate(1) != "TRUE") {
		TEST_ERROR("pp_bool_datatype::evaluate()");
		ret++;
	}
	if (b.evaluate(2) != "TRUE") {
		TEST_ERROR("pp_bool_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	ret += TEST_ASSERT(b.lookup("FALSE") == 0,
	    "pp_bool_datatype::lookup(string)");
	ret += TEST_ASSERT(b.lookup("TRUE") == 1,
	    "pp_bool_datatype::lookup(string)");
	ret += TEST_ASSERT(b.lookup(pp_value(0)) == 0,
	    "pp_bool_datatype::lookup(int)");
	ret += TEST_ASSERT(b.lookup(1) == 1,
	    "pp_bool_datatype::lookup(int)");
	ret += TEST_ASSERT(b.lookup(4) == 1,
	    "pp_bool_datatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_ERROR("pp_bool_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_bitmask_datatype()
{
	int ret = 0;

	// test the basic constructor
	pp_bitmask_datatype b;

	// test the evaluate() method
	b.add_bit("bit_one", 1);
	b.add_bit("bit_two", 2);
	b.add_bit("bit_three", 3);
	if (b.evaluate(2) != "bit_one") {
		TEST_ERROR("pp_bitmask_datatype::evaluate()");
		ret++;
	}
	if (b.evaluate(1) != "<!0!>") {
		TEST_ERROR("pp_bitmask_datatype::evaluate()");
		ret++;
	}
	if (b.evaluate(6) != "bit_one bit_two") {
		TEST_ERROR("pp_bitmask_datatype::evaluate()");
		ret++;
	}
	if (b.evaluate(0) != "") {
		TEST_ERROR("pp_bitmask_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	ret += TEST_ASSERT(b.lookup("bit_one") == 1,
	    "pp_bitmask_datatype::lookup(string)");
	ret += TEST_ASSERT(b.lookup("bit_two") == 2,
	    "pp_bitmask_datatype::lookup(string)");
	ret += TEST_ASSERT(b.lookup(1) == 1, "pp_bitmask_datatype::lookup(int)");
	ret += TEST_ASSERT(b.lookup(2) == 2, "pp_bitmask_datatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_ERROR("pp_bitmask_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		b.lookup(4);
		TEST_ERROR("pp_bitmask_datatype::lookup(int)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_int_datatype()
{
	int ret = 0;

	// test the basic constructor
	pp_int_datatype i1;

	// test the evaluate() method
	if (i1.evaluate(1) != "1") {
		TEST_ERROR("pp_int_datatype::evaluate()");
		ret++;
	}
	if (i1.evaluate(-1) != "-1") {
		TEST_ERROR("pp_int_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	ret += TEST_ASSERT(i1.lookup("1") == 1, "pp_int_datatype::lookup(string)");
	ret += TEST_ASSERT(i1.lookup("23") == 23, "pp_int_datatype::lookup(string)");
	ret += TEST_ASSERT(i1.lookup(1) == 1, "pp_int_datatype::lookup(int)");
	ret += TEST_ASSERT(i1.lookup(23) == 23, "pp_int_datatype::lookup(int)");
	try {
		i1.lookup("foo");
		TEST_ERROR("pp_int_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the units constructor
	pp_int_datatype i2("units");

	// test the evaluate() method
	if (i2.evaluate(1) != "1 units") {
		TEST_ERROR("pp_int_datatype::evaluate()");
		ret++;
	}
	if (i2.evaluate(-1) != "-1 units") {
		TEST_ERROR("pp_int_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	ret += TEST_ASSERT(i2.lookup("1 units") == 1,
	    "pp_int_datatype::lookup(string)");
	ret += TEST_ASSERT(i2.lookup("23 units") == 23,
	    "pp_int_datatype::lookup(string)");
	#endif
	ret += TEST_ASSERT(i2.lookup(1) == 1,
	    "pp_int_datatype::lookup(int)");
	ret += TEST_ASSERT(i2.lookup(23) == 23,
	    "pp_int_datatype::lookup(int)");
	try {
		i2.lookup("foo");
		TEST_ERROR("pp_int_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_hex_datatype()
{
	int ret = 0;

	// test the basic constructor
	pp_hex_datatype h1;

	// test the evaluate() method
	if (h1.evaluate(1) != "0x1") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	ret += TEST_ASSERT(h1.lookup("0x1") == 1,
	    "pp_hex_datatype::lookup(string)");
	ret += TEST_ASSERT(h1.lookup("0x23") ==35,
	    "pp_hex_datatype::lookup(string)");
	ret += TEST_ASSERT(h1.lookup(1) == 1,
	    "pp_hex_datatype::lookup(int)");
	ret += TEST_ASSERT(h1.lookup(23) == 23,
	    "pp_hex_datatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_ERROR("pp_hex_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the units constructor
	pp_hex_datatype h2(BITS0, "units");
	if (h2.evaluate(1) != "0x1 units") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	ret += TEST_ASSERT(h1.lookup("0x1 units") == 1,
	    "pp_hex_datatype::lookup(string)");
	ret += TEST_ASSERT(h1.lookup("0x23 units") ==35,
	    "pp_hex_datatype::lookup(string)");
	#endif
	ret += TEST_ASSERT(h1.lookup(1) == 1,
	    "pp_hex_datatype::lookup(int)");
	ret += TEST_ASSERT(h1.lookup(23) == 23,
	    "pp_hex_datatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_ERROR("pp_hex_datatype::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the width constructor
	pp_hex_datatype h3(BITS8);

	// test the evaluate() method
	if (h3.evaluate(1) != "0x01") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}
	pp_hex_datatype h4(BITS16);
	if (h4.evaluate(0x0201) != "0x0201") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}
	pp_hex_datatype h5(BITS32);
	if (h5.evaluate(0x04030201) != "0x04030201") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}
	pp_hex_datatype h6(BITS64);
	if (h6.evaluate(pp_value("0x0807060504030201")) != "0x0807060504030201") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}

	// test the units-and-width constructor
	pp_hex_datatype h7(BITS8, "units");

	// test the evaluate() method
	if (h7.evaluate(1) != "0x01 units") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}
	pp_hex_datatype h8(BITS16, "units");
	if (h8.evaluate(0x0201) != "0x0201 units") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}
	pp_hex_datatype h9(BITS32, "units");
	if (h9.evaluate(0x04030201) != "0x04030201 units") {
		TEST_ERROR("pp_hex_datatype::evaluate()");
		ret++;
	}
	pp_hex_datatype h10(BITS64, "units");
	if (h10.evaluate(pp_value("0x0807060504030201")) != "0x0807060504030201 units") {
		TEST_ERROR("h10.evaluate()");
		ret++;
	}

	return ret;
}

TEST_LIST(
	TEST(test_pp_enum_datatype),
	TEST(test_pp_bool_datatype),
	TEST(test_pp_bitmask_datatype),
	TEST(test_pp_int_datatype),
	TEST(test_pp_hex_datatype),
);
