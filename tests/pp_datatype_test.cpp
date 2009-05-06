#include "pp.h"
#include "pp_datatypes.h"
#include "pp_lambda.h"
#include "pp_test.h"

TEST(test_pp_enum_datatype)
{
	// test the basic constructor
	pp_enum_datatype e;

	// test the evaluate() method
	e.add_value("one", 1);
	e.add_value("two", 2);
	e.add_value("three", 3);
	if (e.evaluate(2) != "two") {
		TEST_FAIL("pp_enum_datatype::evaluate()");
	}
	if (e.evaluate(0) != "<!0!>") {
		TEST_FAIL("pp_enum_datatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(e.lookup("one") == 1,
			"pp_enum_datatype::lookup(string)");
	TEST_ASSERT(e.lookup("two") == 2,
			"pp_enum_datatype::lookup(string)");
	TEST_ASSERT(e.lookup(1) == 1, "pp_enum_datatype::lookup(int)");
	TEST_ASSERT(e.lookup(2) == 2, "pp_enum_datatype::lookup(int)");
	try {
		e.lookup("foo");
		TEST_FAIL("pp_enum_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		e.lookup(4);
		TEST_FAIL("pp_enum_datatype::lookup(int)");
	} catch (pp_datatype::invalid_error &e) {
	}
}

TEST(test_pp_multi_datatype)
{
	// test basic constructor
	{
		pp_multi_datatype m;
	}

	// test the add_range() method
	try {
		// adding to beginning
		pp_multi_datatype m;
		m.add_range(new_pp_hex_datatype(), 20, 30);
		m.add_range(new_pp_int_datatype(), 15, 16);
		m.add_range(new_pp_int_datatype(), 0, 10);
	} catch (std::exception &e) {
		TEST_FAIL("pp_multi_datatype::add_range()");
	}
	try {
		// adding in between
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 20, 30);
		m.add_range(new_pp_int_datatype(), 15, 16);
	} catch (std::exception &e) {
		TEST_FAIL("pp_multi_datatype::add_range()");
	}
	try {
		// adding to end
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 12);
		m.add_range(new_pp_string_datatype(), 13, 20);
	} catch (std::exception &e) {
		TEST_FAIL("pp_multi_datatype::add_range()");
	}
	try {
		// overlap at the beginning
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 12);
		m.add_range(new_pp_int_datatype(), 0, 3);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 8, 9);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle across two ranges
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 9, 12);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle on a min
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 11, 11);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle on a max
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 10, 10);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap at the end
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 12, 15);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap touching the end
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 13, 15);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// min > max
		pp_multi_datatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 25, 20);
		TEST_FAIL("pp_multi_datatype::add_range()");
	} catch (std::exception &e) {
	}

	{
		// test the evaluate() method
		pp_multi_datatype m;
		m.add_range(new_pp_bool_datatype("true", "false"), 0, 1);
		m.add_range(new_pp_int_datatype("units"), 5, 10);
		m.add_range(new_pp_int_datatype(), 11, 12);
		m.add_range(new_pp_hex_datatype(BITS32), 15, 20);
		util::KeyedVector<string, pp_value> enumlist;
		enumlist.insert("3", 25);
		enumlist.insert("11", 26);
		enumlist.insert("test_key", 27);
		m.add_range(new_pp_enum_datatype(enumlist), 25, 27);
		// ...bool
		TEST_ASSERT(m.evaluate(pp_value(0)) == "false",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(1)) == "true",
				"pp_multi_datatype::evaluate()");
		// ...int
		TEST_ASSERT(m.evaluate(pp_value(5)) == "5 units",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(6)) == "6 units",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(10)) == "10 units",
				"pp_multi_datatype::evaluate()");
		// ...hex
		TEST_ASSERT(m.evaluate(pp_value(15)) == "0x0000000f",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(16)) == "0x00000010",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(20)) == "0x00000014",
				"pp_multi_datatype::evaluate()");
		// ...enum
		TEST_ASSERT(m.evaluate(pp_value(25)) == "3",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(26)) == "11",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(27)) == "test_key",
				"pp_multi_datatype::evaluate()");
		// ...out of range
		TEST_ASSERT(m.evaluate(pp_value(3)) == "<!3!>",
				"pp_multi_datatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp_value(30)) == "<!30!>",
				"pp_multi_datatype::evaluate()");

		// test the lookup() method
		// ...lookup(pp_value)
		try {
			TEST_ASSERT(m.lookup(pp_value(0)) == pp_value(0),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup(pp_value(7)) == pp_value(7),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup(pp_value(20)) == pp_value(20),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		try {
			// out of range
			m.lookup(pp_value(13));
			TEST_FAIL("pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
		}
		// ...lookup(string)
		try {
			TEST_ASSERT(m.lookup("true") == pp_value(1),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("true") == pp_value(1),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("0x0000000f") == pp_value(15),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("test_key") == pp_value(27),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
		// FIXME: This test fails because of the ambiguity in the
		// multi datatype; both pp_value(11) and pp_value(26)
		// produce the string "11". For now, lookup(str) is defined to
		// return the lowest pp_value the given string could have
		// produced.
#if 0
		try {
			TEST_ASSERT(m.lookup("11") == pp_value(26),
				"pp_multi_datatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp_multi_datatype::lookup()");
		}
#endif
	}
}

TEST(test_pp_string_datatype)
{
	// test the basic constructor
	pp_string_datatype s;

	// test the evaluate() method
	if (s.evaluate(0x41) != "A") {
		TEST_FAIL("pp_string_datatype::evaluate() returns " +
			   s.evaluate(pp_value(0x41)));
	}
	// ...test null character
	if (s.evaluate(pp_value(0x00)) != "") {
		TEST_FAIL("pp_string_datatype::evaluate() returns " +
			   s.evaluate(pp_value(0x00)));
	}
	// ...test multicharacter strings
	if (s.evaluate(pp_value("0x6f6c6c6548")) != "Hello") {
		TEST_FAIL("pp_string_datatype::evaluate() returns " +
			   s.evaluate(pp_value("0x6f6c6c6548")));
	}
	// ...test multicharacter strings with nulls
	if (s.evaluate(pp_value("0x00006f6c6c6548")) != "Hello") {
		TEST_FAIL("pp_string_datatype::evaluate() returns " +
			   s.evaluate(pp_value("0x00006f6c6c6548")));
	}
	if (s.evaluate(pp_value("0x6f006c006c0065004800"))
	    == "Hello") {
		// These should not be equal because the string returned
		// by evaluate() contains embedded null characters.
		TEST_FAIL("pp_string_datatype::evaluate() returns " +
			   s.evaluate(pp_value("0x6f006c006c0065004800")));
	}

	// test lookup()
	TEST_ASSERT(s.lookup(pp_value(0x00)) == pp_value(0x00),
			"pp_string_datatype::lookup(pp_value)");
	TEST_ASSERT(s.lookup(pp_value("0x1234567890"))
			   == pp_value("0x1234567890"),
			"pp_string_datatype::lookup(pp_value)");
	TEST_ASSERT(s.lookup("A") == pp_value(0x41),
			"pp_string_datatype::lookup(string)");
	TEST_ASSERT(s.lookup("") == pp_value(0x00),
			"pp_string_datatype::lookup(string)");
	TEST_ASSERT(s.lookup("Hello")
		           == pp_value("0x6f6c6c6548"),
			"pp_string_datatype::lookup(pp_value)");

	// test that evaluate() and lookup() are inverse
	TEST_ASSERT(s.lookup(s.evaluate(0x333231)) == 0x333231,
			"pp_string_datatype::lookup(string) and "
			"pp_string_datatype::evaluate(pp_value)");
	TEST_ASSERT(s.lookup(s.evaluate(pp_value("0x6d086c0065004800")))
		== pp_value("0x6d086c0065004800"),
			"pp_string_datatype::lookup(string) and "
			"pp_string_datatype::evaluate(pp_value)");
	TEST_ASSERT(s.evaluate(s.lookup("xyz")) == "xyz",
			"pp_string_datatype::lookup(string) and "
			"pp_string_datatype::evaluate(pp_value)");
}

TEST(test_pp_bool_datatype)
{
	// test the basic constructor
	pp_bool_datatype b("TRUE", "FALSE");

	// test the evaluate() method
	if (b.evaluate(0) != "FALSE") {
		TEST_FAIL("pp_bool_datatype::evaluate()");
	}
	if (b.evaluate(1) != "TRUE") {
		TEST_FAIL("pp_bool_datatype::evaluate()");
	}
	if (b.evaluate(2) != "TRUE") {
		TEST_FAIL("pp_bool_datatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(b.lookup("FALSE") == 0,
	    "pp_bool_datatype::lookup(string)");
	TEST_ASSERT(b.lookup("TRUE") == 1,
	    "pp_bool_datatype::lookup(string)");
	TEST_ASSERT(b.lookup(pp_value(0)) == 0,
	    "pp_bool_datatype::lookup(int)");
	TEST_ASSERT(b.lookup(1) == 1,
	    "pp_bool_datatype::lookup(int)");
	TEST_ASSERT(b.lookup(4) == 1,
	    "pp_bool_datatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_FAIL("pp_bool_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}
}

TEST(test_pp_bitmask_datatype)
{
	// test the basic constructor
	pp_bitmask_datatype b;

	// test the evaluate() method
	b.add_bit("bit_one", 1);
	b.add_bit("bit_two", 2);
	b.add_bit("bit_three", 3);
	if (b.evaluate(2) != "bit_one") {
		TEST_FAIL("pp_bitmask_datatype::evaluate()");
	}
	if (b.evaluate(1) != "<!0!>") {
		TEST_FAIL("pp_bitmask_datatype::evaluate()");
	}
	if (b.evaluate(6) != "bit_one bit_two") {
		TEST_FAIL("pp_bitmask_datatype::evaluate()");
	}
	if (b.evaluate(0) != "") {
		TEST_FAIL("pp_bitmask_datatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(b.lookup("bit_one") == 1,
	    "pp_bitmask_datatype::lookup(string)");
	TEST_ASSERT(b.lookup("bit_two") == 2,
	    "pp_bitmask_datatype::lookup(string)");
	TEST_ASSERT(b.lookup(1) == 1,
			"pp_bitmask_datatype::lookup(int)");
	TEST_ASSERT(b.lookup(2) == 2,
			"pp_bitmask_datatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_FAIL("pp_bitmask_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}
	try {
		b.lookup(4);
		TEST_FAIL("pp_bitmask_datatype::lookup(int)");
	} catch (pp_datatype::invalid_error &e) {
	}
}

TEST(test_pp_int_datatype)
{
	// test the basic constructor
	pp_int_datatype i1;

	// test the evaluate() method
	if (i1.evaluate(1) != "1") {
		TEST_FAIL("pp_int_datatype::evaluate()");
	}
	if (i1.evaluate(-1) != "-1") {
		TEST_FAIL("pp_int_datatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(i1.lookup("1") == 1,
			"pp_int_datatype::lookup(string)");
	TEST_ASSERT(i1.lookup("23") == 23,
			"pp_int_datatype::lookup(string)");
	TEST_ASSERT(i1.lookup(1) == 1, "pp_int_datatype::lookup(int)");
	TEST_ASSERT(i1.lookup(23) == 23, "pp_int_datatype::lookup(int)");
	try {
		i1.lookup("foo");
		TEST_FAIL("pp_int_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the units constructor
	pp_int_datatype i2("units");

	// test the evaluate() method
	if (i2.evaluate(1) != "1 units") {
		TEST_FAIL("pp_int_datatype::evaluate()");
	}
	if (i2.evaluate(-1) != "-1 units") {
		TEST_FAIL("pp_int_datatype::evaluate()");
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	TEST_ASSERT(i2.lookup("1 units") == 1,
	    "pp_int_datatype::lookup(string)");
	TEST_ASSERT(i2.lookup("23 units") == 23,
	    "pp_int_datatype::lookup(string)");
	#endif
	TEST_ASSERT(i2.lookup(1) == 1,
	    "pp_int_datatype::lookup(int)");
	TEST_ASSERT(i2.lookup(23) == 23,
	    "pp_int_datatype::lookup(int)");
	try {
		i2.lookup("foo");
		TEST_FAIL("pp_int_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}
}

TEST(test_pp_hex_datatype)
{
	// test the basic constructor
	pp_hex_datatype h1;

	// test the evaluate() method
	if (h1.evaluate(1) != "0x1") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(h1.lookup("0x1") == 1,
	    "pp_hex_datatype::lookup(string)");
	TEST_ASSERT(h1.lookup("0x23") ==35,
	    "pp_hex_datatype::lookup(string)");
	TEST_ASSERT(h1.lookup(1) == 1,
	    "pp_hex_datatype::lookup(int)");
	TEST_ASSERT(h1.lookup(23) == 23,
	    "pp_hex_datatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_FAIL("pp_hex_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the units constructor
	pp_hex_datatype h2(BITS0, "units");
	if (h2.evaluate(1) != "0x1 units") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	TEST_ASSERT(h1.lookup("0x1 units") == 1,
	    "pp_hex_datatype::lookup(string)");
	TEST_ASSERT(h1.lookup("0x23 units") ==35,
	    "pp_hex_datatype::lookup(string)");
	#endif
	TEST_ASSERT(h1.lookup(1) == 1,
	    "pp_hex_datatype::lookup(int)");
	TEST_ASSERT(h1.lookup(23) == 23,
	    "pp_hex_datatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_FAIL("pp_hex_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}

	// test the width constructor
	pp_hex_datatype h3(BITS8);

	// test the evaluate() method
	if (h3.evaluate(1) != "0x01") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}
	pp_hex_datatype h4(BITS16);
	if (h4.evaluate(0x0201) != "0x0201") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}
	pp_hex_datatype h5(BITS32);
	if (h5.evaluate(0x04030201) != "0x04030201") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}
	pp_hex_datatype h6(BITS64);
	if (h6.evaluate(pp_value("0x0807060504030201")) !=
			"0x0807060504030201") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}

	// test the units-and-width constructor
	pp_hex_datatype h7(BITS8, "units");

	// test the evaluate() method
	if (h7.evaluate(1) != "0x01 units") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}
	pp_hex_datatype h8(BITS16, "units");
	if (h8.evaluate(0x0201) != "0x0201 units") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}
	pp_hex_datatype h9(BITS32, "units");
	if (h9.evaluate(0x04030201) != "0x04030201 units") {
		TEST_FAIL("pp_hex_datatype::evaluate()");
	}
	pp_hex_datatype h10(BITS64, "units");
	if (h10.evaluate(pp_value("0x0807060504030201")) !=
			 "0x0807060504030201 units") {
		TEST_FAIL("h10.evaluate()");
	}
}

TEST(test_pp_transform_datatype)
{
	// test the basic constructor
	pp_transform_datatype_ptr t1 = new_pp_transform_datatype(
		new_pp_int_datatype(), _1+1, _1-1);

	// test the evaluate() method
	if (t1->evaluate(0) != "1") {
		TEST_FAIL("pp_transform_datatype::evaluate()");
	}
	if (t1->evaluate(1) != "2") {
		TEST_FAIL("pp_transform_datatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(t1->lookup("1") == 0,
			"pp_transform_datatype::lookup(string)");
	TEST_ASSERT(t1->lookup("23") == 22,
			"pp_transform_datatype::lookup(string)");
	TEST_ASSERT(t1->lookup(1) == 0,
			"pp_transform_datatype::lookup(int)");
	TEST_ASSERT(t1->lookup(23) == 22,
			"pp_transform_datatype::lookup(int)");
	try {
		t1->lookup("foo");
		TEST_FAIL("pp_transform_datatype::lookup(string)");
	} catch (pp_datatype::invalid_error &e) {
	}
}

TEST(test_pp_fixed_datatype)
{
	// test the basic constructor
	pp_fixed_datatype_ptr f1 = new_pp_fixed_datatype(2);

	// test the evaluate() method
	std::string str;
	str = f1->evaluate(0);
	TEST_ASSERT(str == "0.0")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(1);
	TEST_ASSERT(str == "0.25")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(2);
	TEST_ASSERT(str == "0.5")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(3);
	TEST_ASSERT(str == "0.75")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(4);
	TEST_ASSERT(str == "1.0")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(5);
	TEST_ASSERT(str == "1.25")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(6);
	TEST_ASSERT(str == "1.5")
		<< "pp_fixed_datatype::evaluate(): " << str;
	str = f1->evaluate(7);
	TEST_ASSERT(str == "1.75")
		<< "pp_fixed_datatype::evaluate(): " << str;

	// test lookup
	// FIXME:
}
