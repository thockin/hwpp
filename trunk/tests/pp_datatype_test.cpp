#include "pp_datatypes.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

int
test_pp_enum()
{
	int ret = 0;

	// test the basic constructor
	pp_enum e;

	// test the evaluate() method
	e.add_value("one", 1);
	e.add_value("two", 2);
	e.add_value("three", 3);
	if (e.evaluate(2) != "two") {
		PP_TEST_ERROR("pp_enum::evaluate()");
		ret++;
	}
	if (e.evaluate(0) != "<!!unknown!!>") {
		PP_TEST_ERROR("pp_enum::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(e.lookup("one") == 1, "pp_enum::lookup(string)");
	ret += PP_TEST_ASSERT(e.lookup("two") == 2, "pp_enum::lookup(string)");
	ret += PP_TEST_ASSERT(e.lookup(1) == 1, "pp_enum::lookup(int)");
	ret += PP_TEST_ASSERT(e.lookup(2) == 2, "pp_enum::lookup(int)");
	try {
		e.lookup("foo");
		PP_TEST_ERROR("pp_enum::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		e.lookup(4);
		PP_TEST_ERROR("pp_enum::lookup(int)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_bool()
{
	int ret = 0;

	// test the basic constructor
	pp_bool b("TRUE", "FALSE");

	// test the evaluate() method
	if (b.evaluate(0) != "FALSE") {
		PP_TEST_ERROR("pp_bool::evaluate()");
		ret++;
	}
	if (b.evaluate(1) != "TRUE") {
		PP_TEST_ERROR("pp_bool::evaluate()");
		ret++;
	}
	if (b.evaluate(2) != "TRUE") {
		PP_TEST_ERROR("pp_bool::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(b.lookup("FALSE") == 0,
	    "pp_bool::lookup(string)");
	ret += PP_TEST_ASSERT(b.lookup("TRUE") == 1,
	    "pp_bool::lookup(string)");
	ret += PP_TEST_ASSERT(b.lookup(pp_value(0)) == 0,
	    "pp_bool::lookup(int)");
	ret += PP_TEST_ASSERT(b.lookup(1) == 1,
	    "pp_bool::lookup(int)");
	try {
		b.lookup("foo");
		PP_TEST_ERROR("pp_bool::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		b.lookup(4);
		PP_TEST_ERROR("pp_bool::lookup(int)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_bitmask()
{
	int ret = 0;

	// test the basic constructor
	pp_bitmask b;

	// test the evaluate() method
	b.add_bit("bit_one", 1);
	b.add_bit("bit_two", 2);
	b.add_bit("bit_three", 3);
	if (b.evaluate(2) != "bit_one") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}
	if (b.evaluate(1) != "<!!bit0!!>") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}
	if (b.evaluate(6) != "bit_one bit_two") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}
	if (b.evaluate(0) != "") {
		PP_TEST_ERROR("pp_bitmask::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(b.lookup("bit_one") ==1,
	    "pp_bitmask::lookup(string)");
	ret += PP_TEST_ASSERT(b.lookup("bit_two") == 2,
	    "pp_bitmask::lookup(string)");
	ret += PP_TEST_ASSERT(b.lookup(1) == 1, "pp_bitmask::lookup(int)");
	ret += PP_TEST_ASSERT(b.lookup(2) == 2, "pp_bitmask::lookup(int)");
	try {
		b.lookup("foo");
		PP_TEST_ERROR("pp_bitmask::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		b.lookup(4);
		PP_TEST_ERROR("pp_bitmask::lookup(int)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_int()
{
	int ret = 0;

	// test the basic constructor
	pp_int i1;

	// test the evaluate() method
	if (i1.evaluate(1) != "1") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}
	if (i1.evaluate(-1) != "-1") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(i1.lookup("1") == 1, "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(i1.lookup("23") == 23, "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(i1.lookup(1) == 1, "pp_int::lookup(int)");
	ret += PP_TEST_ASSERT(i1.lookup(23) == 23, "pp_int::lookup(int)");
	try {
		i1.lookup("foo");
		PP_TEST_ERROR("pp_int::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the units constructor
	pp_int i2("units");

	// test the evaluate() method
	if (i2.evaluate(1) != "1 units") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}
	if (i2.evaluate(-1) != "-1 units") {
		PP_TEST_ERROR("pp_int::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(i2.lookup("1 units") == 1,
	    "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(i2.lookup("23 units") == 23,
	    "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(i2.lookup(1) == 1,
	    "pp_int::lookup(int)");
	ret += PP_TEST_ASSERT(i2.lookup(23) == 23,
	    "pp_int::lookup(int)");
	try {
		i2.lookup("foo");
		PP_TEST_ERROR("pp_int::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	return ret;
}

int
test_pp_hex()
{
	int ret = 0;

	// test the basic constructor
	pp_hex h1;

	// test the evaluate() method
	if (h1.evaluate(1) != "0x1") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(h1.lookup("0x1") == 1,
	    "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(h1.lookup("0x23") ==35,
	    "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(h1.lookup(1) == 1,
	    "pp_int::lookup(int)");
	ret += PP_TEST_ASSERT(h1.lookup(23) == 23,
	    "pp_int::lookup(int)");
	try {
		h1.lookup("foo");
		PP_TEST_ERROR("pp_int::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the units constructor
	pp_hex h2(BITS0, "units");
	if (h2.evaluate(1) != "0x1 units") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}

	// test lookup()
	ret += PP_TEST_ASSERT(h1.lookup("0x1 units") == 1,
	    "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(h1.lookup("0x23 units") ==35,
	    "pp_int::lookup(string)");
	ret += PP_TEST_ASSERT(h1.lookup(1) == 1,
	    "pp_int::lookup(int)");
	ret += PP_TEST_ASSERT(h1.lookup(23) == 23,
	    "pp_int::lookup(int)");
	try {
		h1.lookup("foo");
		PP_TEST_ERROR("pp_int::lookup(string)");
		ret++;
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the width constructor
	pp_hex h3(BITS8);

	// test the evaluate() method
	if (h3.evaluate(1) != "0x01") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h4(BITS16);
	if (h4.evaluate(0x0201) != "0x0201") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h5(BITS32);
	if (h5.evaluate(0x04030201) != "0x04030201") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}
	pp_hex h6(BITS64);
	if (h6.evaluate(pp_value("0x0807060504030201")) != "0x0807060504030201") {
		PP_TEST_ERROR("pp_hex::evaluate()");
		ret++;
	}

	// test the units-and-width constructor
	pp_hex h7(BITS8, "units");

	// test the evaluate() method
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
	if (h10.evaluate(pp_value("0x0807060504030201")) != "0x0807060504030201 units") {
		PP_TEST_ERROR("h10.evaluate()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r += test_pp_enum();
	r += test_pp_bool();
	r += test_pp_bitmask();
	r += test_pp_int();
	r += test_pp_hex();

	return EXIT_SUCCESS;
}
