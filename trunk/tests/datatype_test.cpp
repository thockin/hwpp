#include "hwpp.h"
#include "datatype_types.h"
#include "util/bignum_lambda.h"
#include "util/test.h"

TEST(test_hwpp_enum_datatype)
{
	// test the basic constructor
	hwpp::EnumDatatype e;

	// test the evaluate() method
	e.add_value("one", 1);
	e.add_value("two", 2);
	e.add_value("three", 3);
	if (e.evaluate(2) != "two") {
		TEST_FAIL("hwpp::EnumDatatype::evaluate()");
	}
	if (e.evaluate(0) != "<!0!>") {
		TEST_FAIL("hwpp::EnumDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(e.lookup("one") == 1,
			"hwpp::EnumDatatype::lookup(string)");
	TEST_ASSERT(e.lookup("two") == 2,
			"hwpp::EnumDatatype::lookup(string)");
	TEST_ASSERT(e.lookup(1) == 1, "hwpp::EnumDatatype::lookup(int)");
	TEST_ASSERT(e.lookup(2) == 2, "hwpp::EnumDatatype::lookup(int)");
	try {
		e.lookup("foo");
		TEST_FAIL("hwpp::EnumDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
	try {
		e.lookup(4);
		TEST_FAIL("hwpp::EnumDatatype::lookup(int)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
}

TEST(test_hwpp_multi_datatype)
{
	// test basic constructor
	{
		hwpp::MultiDatatype m;
	}

	// test the add_range() method
	try {
		// adding to beginning
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_hex_datatype(), 20, 30);
		m.add_range(new_hwpp_int_datatype(), 15, 16);
		m.add_range(new_hwpp_int_datatype(), 0, 10);
	} catch (std::exception &e) {
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	}
	try {
		// adding in between
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 20, 30);
		m.add_range(new_hwpp_int_datatype(), 15, 16);
	} catch (std::exception &e) {
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	}
	try {
		// adding to end
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 12);
		m.add_range(new_hwpp_string_datatype(), 13, 20);
	} catch (std::exception &e) {
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	}
	try {
		// overlap at the beginning
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 12);
		m.add_range(new_hwpp_int_datatype(), 0, 3);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 8, 9);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle across two ranges
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 9, 12);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle on a min
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 11, 11);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle on a max
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 10, 10);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap at the end
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 12, 15);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap touching the end
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 13, 15);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// min > max
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_int_datatype(), 0, 10);
		m.add_range(new_hwpp_hex_datatype(), 11, 13);
		m.add_range(new_hwpp_int_datatype(), 25, 20);
		TEST_FAIL("hwpp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}

	{
		// test the evaluate() method
		hwpp::MultiDatatype m;
		m.add_range(new_hwpp_bool_datatype("true", "false"), 0, 1);
		m.add_range(new_hwpp_int_datatype("units"), 5, 10);
		m.add_range(new_hwpp_int_datatype(), 11, 12);
		m.add_range(new_hwpp_hex_datatype(hwpp::BITS32), 15, 20);
		util::KeyedVector<string, hwpp::Value> enumlist;
		enumlist.insert("3", 25);
		enumlist.insert("11", 26);
		enumlist.insert("test_key", 27);
		m.add_range(new_hwpp_enum_datatype(enumlist), 25, 27);
		// ...bool
		TEST_ASSERT(m.evaluate(hwpp::Value(0)) == "false",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(1)) == "true",
				"hwpp::MultiDatatype::evaluate()");
		// ...int
		TEST_ASSERT(m.evaluate(hwpp::Value(5)) == "5 units",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(6)) == "6 units",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(10)) == "10 units",
				"hwpp::MultiDatatype::evaluate()");
		// ...hex
		TEST_ASSERT(m.evaluate(hwpp::Value(15)) == "0x0000000f",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(16)) == "0x00000010",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(20)) == "0x00000014",
				"hwpp::MultiDatatype::evaluate()");
		// ...enum
		TEST_ASSERT(m.evaluate(hwpp::Value(25)) == "3",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(26)) == "11",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(27)) == "test_key",
				"hwpp::MultiDatatype::evaluate()");
		// ...out of range
		TEST_ASSERT(m.evaluate(hwpp::Value(3)) == "<!3!>",
				"hwpp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(hwpp::Value(30)) == "<!30!>",
				"hwpp::MultiDatatype::evaluate()");

		// test the lookup() method
		// ...lookup(hwpp::Value)
		try {
			TEST_ASSERT(m.lookup(hwpp::Value(0)) == hwpp::Value(0),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup(hwpp::Value(7)) == hwpp::Value(7),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup(hwpp::Value(20)) == hwpp::Value(20),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		try {
			// out of range
			m.lookup(hwpp::Value(13));
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
		}
		// ...lookup(string)
		try {
			TEST_ASSERT(m.lookup("true") == hwpp::Value(1),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("true") == hwpp::Value(1),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("0x0000000f") == hwpp::Value(15),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("test_key") == hwpp::Value(27),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
		// FIXME: This test fails because of the ambiguity in the
		// multi datatype; both hwpp::Value(11) and hwpp::Value(26)
		// produce the string "11". For now, lookup(str) is defined to
		// return the lowest hwpp::Value the given string could have
		// produced.
#if 0
		try {
			TEST_ASSERT(m.lookup("11") == hwpp::Value(26),
				"hwpp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("hwpp::MultiDatatype::lookup()");
		}
#endif
	}
}

TEST(test_hwpp_string_datatype)
{
	// test the basic constructor
	hwpp::StringDatatype s;

	// test the evaluate() method
	if (s.evaluate(0x41) != "A") {
		TEST_FAIL("hwpp::StringDatatype::evaluate() returns " +
			   s.evaluate(hwpp::Value(0x41)));
	}
	// ...test null character
	if (s.evaluate(hwpp::Value(0x00)) != "") {
		TEST_FAIL("hwpp::StringDatatype::evaluate() returns " +
			   s.evaluate(hwpp::Value(0x00)));
	}
	// ...test multicharacter strings
	if (s.evaluate(hwpp::Value("0x6f6c6c6548")) != "Hello") {
		TEST_FAIL("hwpp::StringDatatype::evaluate() returns " +
			   s.evaluate(hwpp::Value("0x6f6c6c6548")));
	}
	// ...test multicharacter strings with nulls
	if (s.evaluate(hwpp::Value("0x00006f6c6c6548")) != "Hello") {
		TEST_FAIL("hwpp::StringDatatype::evaluate() returns " +
			   s.evaluate(hwpp::Value("0x00006f6c6c6548")));
	}
	if (s.evaluate(hwpp::Value("0x6f006c006c0065004800"))
	    == "Hello") {
		// These should not be equal because the string returned
		// by evaluate() contains embedded null characters.
		TEST_FAIL("hwpp::StringDatatype::evaluate() returns " +
			   s.evaluate(hwpp::Value("0x6f006c006c0065004800")));
	}

	// test lookup()
	TEST_ASSERT(s.lookup(hwpp::Value(0x00)) == hwpp::Value(0x00),
			"hwpp::StringDatatype::lookup(hwpp::Value)");
	TEST_ASSERT(s.lookup(hwpp::Value("0x1234567890"))
			   == hwpp::Value("0x1234567890"),
			"hwpp::StringDatatype::lookup(hwpp::Value)");
	TEST_ASSERT(s.lookup("A") == hwpp::Value(0x41),
			"hwpp::StringDatatype::lookup(string)");
	TEST_ASSERT(s.lookup("") == hwpp::Value(0x00),
			"hwpp::StringDatatype::lookup(string)");
	TEST_ASSERT(s.lookup("Hello")
		           == hwpp::Value("0x6f6c6c6548"),
			"hwpp::StringDatatype::lookup(hwpp::Value)");

	// test that evaluate() and lookup() are inverse
	TEST_ASSERT(s.lookup(s.evaluate(0x333231)) == 0x333231,
			"hwpp::StringDatatype::lookup(string) and "
			"hwpp::StringDatatype::evaluate(hwpp::Value)");
	TEST_ASSERT(s.lookup(s.evaluate(hwpp::Value("0x6d086c0065004800")))
		== hwpp::Value("0x6d086c0065004800"),
			"hwpp::StringDatatype::lookup(string) and "
			"hwpp::StringDatatype::evaluate(hwpp::Value)");
	TEST_ASSERT(s.evaluate(s.lookup("xyz")) == "xyz",
			"hwpp::StringDatatype::lookup(string) and "
			"hwpp::StringDatatype::evaluate(hwpp::Value)");
}

TEST(test_hwpp_bool_datatype)
{
	// test the basic constructor
	hwpp::BoolDatatype b("TRUE", "FALSE");

	// test the evaluate() method
	if (b.evaluate(0) != "FALSE") {
		TEST_FAIL("hwpp::BoolDatatype::evaluate()");
	}
	if (b.evaluate(1) != "TRUE") {
		TEST_FAIL("hwpp::BoolDatatype::evaluate()");
	}
	if (b.evaluate(2) != "TRUE") {
		TEST_FAIL("hwpp::BoolDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(b.lookup("FALSE") == 0,
	    "hwpp::BoolDatatype::lookup(string)");
	TEST_ASSERT(b.lookup("TRUE") == 1,
	    "hwpp::BoolDatatype::lookup(string)");
	TEST_ASSERT(b.lookup(hwpp::Value(0)) == 0,
	    "hwpp::BoolDatatype::lookup(int)");
	TEST_ASSERT(b.lookup(1) == 1,
	    "hwpp::BoolDatatype::lookup(int)");
	TEST_ASSERT(b.lookup(4) == 1,
	    "hwpp::BoolDatatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_FAIL("hwpp::BoolDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
}

TEST(test_hwpp_bitmask_datatype)
{
	// test the basic constructor
	hwpp::BitmaskDatatype b;

	// test the evaluate() method
	b.add_bit("bit_one", 1);
	b.add_bit("bit_two", 2);
	b.add_bit("bit_three", 3);
	if (b.evaluate(2) != "bit_one") {
		TEST_FAIL("hwpp::BitmaskDatatype::evaluate()");
	}
	if (b.evaluate(1) != "<!0!>") {
		TEST_FAIL("hwpp::BitmaskDatatype::evaluate()");
	}
	if (b.evaluate(6) != "bit_one bit_two") {
		TEST_FAIL("hwpp::BitmaskDatatype::evaluate()");
	}
	if (b.evaluate(0) != "") {
		TEST_FAIL("hwpp::BitmaskDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(b.lookup("bit_one") == 1,
	    "hwpp::BitmaskDatatype::lookup(string)");
	TEST_ASSERT(b.lookup("bit_two") == 2,
	    "hwpp::BitmaskDatatype::lookup(string)");
	TEST_ASSERT(b.lookup(1) == 1,
			"hwpp::BitmaskDatatype::lookup(int)");
	TEST_ASSERT(b.lookup(2) == 2,
			"hwpp::BitmaskDatatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_FAIL("hwpp::BitmaskDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
	try {
		b.lookup(4);
		TEST_FAIL("hwpp::BitmaskDatatype::lookup(int)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
}

TEST(test_hwpp_int_datatype)
{
	// test the basic constructor
	hwpp::IntDatatype i1;

	// test the evaluate() method
	if (i1.evaluate(1) != "1") {
		TEST_FAIL("hwpp::IntDatatype::evaluate()");
	}
	if (i1.evaluate(-1) != "-1") {
		TEST_FAIL("hwpp::IntDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(i1.lookup("1") == 1,
			"hwpp::IntDatatype::lookup(string)");
	TEST_ASSERT(i1.lookup("23") == 23,
			"hwpp::IntDatatype::lookup(string)");
	TEST_ASSERT(i1.lookup(1) == 1, "hwpp::IntDatatype::lookup(int)");
	TEST_ASSERT(i1.lookup(23) == 23, "hwpp::IntDatatype::lookup(int)");
	try {
		i1.lookup("foo");
		TEST_FAIL("hwpp::IntDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}

	// test the units constructor
	hwpp::IntDatatype i2("units");

	// test the evaluate() method
	if (i2.evaluate(1) != "1 units") {
		TEST_FAIL("hwpp::IntDatatype::evaluate()");
	}
	if (i2.evaluate(-1) != "-1 units") {
		TEST_FAIL("hwpp::IntDatatype::evaluate()");
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	TEST_ASSERT(i2.lookup("1 units") == 1,
	    "hwpp::IntDatatype::lookup(string)");
	TEST_ASSERT(i2.lookup("23 units") == 23,
	    "hwpp::IntDatatype::lookup(string)");
	#endif
	TEST_ASSERT(i2.lookup(1) == 1,
	    "hwpp::IntDatatype::lookup(int)");
	TEST_ASSERT(i2.lookup(23) == 23,
	    "hwpp::IntDatatype::lookup(int)");
	try {
		i2.lookup("foo");
		TEST_FAIL("hwpp::IntDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
}

TEST(test_hwpp_hex_datatype)
{
	// test the basic constructor
	hwpp::HexDatatype h1;

	// test the evaluate() method
	if (h1.evaluate(1) != "0x1") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(h1.lookup("0x1") == 1,
	    "hwpp::HexDatatype::lookup(string)");
	TEST_ASSERT(h1.lookup("0x23") ==35,
	    "hwpp::HexDatatype::lookup(string)");
	TEST_ASSERT(h1.lookup(1) == 1,
	    "hwpp::HexDatatype::lookup(int)");
	TEST_ASSERT(h1.lookup(23) == 23,
	    "hwpp::HexDatatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_FAIL("hwpp::HexDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}

	// test the units constructor
	hwpp::HexDatatype h2(hwpp::BITS0, "units");
	if (h2.evaluate(1) != "0x1 units") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	TEST_ASSERT(h1.lookup("0x1 units") == 1,
	    "hwpp::HexDatatype::lookup(string)");
	TEST_ASSERT(h1.lookup("0x23 units") ==35,
	    "hwpp::HexDatatype::lookup(string)");
	#endif
	TEST_ASSERT(h1.lookup(1) == 1,
	    "hwpp::HexDatatype::lookup(int)");
	TEST_ASSERT(h1.lookup(23) == 23,
	    "hwpp::HexDatatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_FAIL("hwpp::HexDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}

	// test the width constructor
	hwpp::HexDatatype h3(hwpp::BITS8);

	// test the evaluate() method
	if (h3.evaluate(1) != "0x01") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}
	hwpp::HexDatatype h4(hwpp::BITS16);
	if (h4.evaluate(0x0201) != "0x0201") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}
	hwpp::HexDatatype h5(hwpp::BITS32);
	if (h5.evaluate(0x04030201) != "0x04030201") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}
	hwpp::HexDatatype h6(hwpp::BITS64);
	if (h6.evaluate(hwpp::Value("0x0807060504030201")) !=
			"0x0807060504030201") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}

	// test the units-and-width constructor
	hwpp::HexDatatype h7(hwpp::BITS8, "units");

	// test the evaluate() method
	if (h7.evaluate(1) != "0x01 units") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}
	hwpp::HexDatatype h8(hwpp::BITS16, "units");
	if (h8.evaluate(0x0201) != "0x0201 units") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}
	hwpp::HexDatatype h9(hwpp::BITS32, "units");
	if (h9.evaluate(0x04030201) != "0x04030201 units") {
		TEST_FAIL("hwpp::HexDatatype::evaluate()");
	}
	hwpp::HexDatatype h10(hwpp::BITS64, "units");
	if (h10.evaluate(hwpp::Value("0x0807060504030201")) !=
			 "0x0807060504030201 units") {
		TEST_FAIL("h10.evaluate()");
	}
}

TEST(test_hwpp_transform_datatype)
{
	// test the basic constructor
	hwpp::TransformDatatypePtr t1 = new_hwpp_transform_datatype(
		new_hwpp_int_datatype(), _1+1, _1-1);

	// test the evaluate() method
	if (t1->evaluate(0) != "1") {
		TEST_FAIL("hwpp::TransformDatatype::evaluate()");
	}
	if (t1->evaluate(1) != "2") {
		TEST_FAIL("hwpp::TransformDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(t1->lookup("1") == 0,
			"hwpp::TransformDatatype::lookup(string)");
	TEST_ASSERT(t1->lookup("23") == 22,
			"hwpp::TransformDatatype::lookup(string)");
	TEST_ASSERT(t1->lookup(1) == 0,
			"hwpp::TransformDatatype::lookup(int)");
	TEST_ASSERT(t1->lookup(23) == 22,
			"hwpp::TransformDatatype::lookup(int)");
	try {
		t1->lookup("foo");
		TEST_FAIL("hwpp::TransformDatatype::lookup(string)");
	} catch (hwpp::Datatype::InvalidError &e) {
	}
}

TEST(test_hwpp_fixed_datatype)
{
	// test the basic constructor
	hwpp::FixedDatatypePtr f1 = new_hwpp_fixed_datatype(2);

	// test the evaluate() method
	std::string str;
	str = f1->evaluate(0);
	TEST_ASSERT(str == "0.0")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(1);
	TEST_ASSERT(str == "0.25")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(2);
	TEST_ASSERT(str == "0.5")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(3);
	TEST_ASSERT(str == "0.75")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(4);
	TEST_ASSERT(str == "1.0")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(5);
	TEST_ASSERT(str == "1.25")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(6);
	TEST_ASSERT(str == "1.5")
		<< "hwpp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(7);
	TEST_ASSERT(str == "1.75")
		<< "hwpp::FixedDatatype::evaluate(): " << str;

	// test lookup
	// FIXME:
}
