#include "pp/pp.h"
#include "pp/datatype_types.h"
#include "pp/util/bignum_lambda.h"
#include "pp/util/test.h"

TEST(test_pp_enum_datatype)
{
	// test the basic constructor
	pp::EnumDatatype e;

	// test the evaluate() method
	e.add_value("one", 1);
	e.add_value("two", 2);
	e.add_value("three", 3);
	if (e.evaluate(2) != "two") {
		TEST_FAIL("pp::EnumDatatype::evaluate()");
	}
	if (e.evaluate(0) != "<!0!>") {
		TEST_FAIL("pp::EnumDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(e.lookup("one") == 1,
			"pp::EnumDatatype::lookup(string)");
	TEST_ASSERT(e.lookup("two") == 2,
			"pp::EnumDatatype::lookup(string)");
	TEST_ASSERT(e.lookup(1) == 1, "pp::EnumDatatype::lookup(int)");
	TEST_ASSERT(e.lookup(2) == 2, "pp::EnumDatatype::lookup(int)");
	try {
		e.lookup("foo");
		TEST_FAIL("pp::EnumDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}
	try {
		e.lookup(4);
		TEST_FAIL("pp::EnumDatatype::lookup(int)");
	} catch (pp::Datatype::InvalidError &e) {
	}
}

TEST(test_pp_multi_datatype)
{
	// test basic constructor
	{
		pp::MultiDatatype m;
	}

	// test the add_range() method
	try {
		// adding to beginning
		pp::MultiDatatype m;
		m.add_range(new_pp_hex_datatype(), 20, 30);
		m.add_range(new_pp_int_datatype(), 15, 16);
		m.add_range(new_pp_int_datatype(), 0, 10);
	} catch (std::exception &e) {
		TEST_FAIL("pp::MultiDatatype::add_range()");
	}
	try {
		// adding in between
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 20, 30);
		m.add_range(new_pp_int_datatype(), 15, 16);
	} catch (std::exception &e) {
		TEST_FAIL("pp::MultiDatatype::add_range()");
	}
	try {
		// adding to end
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 12);
		m.add_range(new_pp_string_datatype(), 13, 20);
	} catch (std::exception &e) {
		TEST_FAIL("pp::MultiDatatype::add_range()");
	}
	try {
		// overlap at the beginning
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 12);
		m.add_range(new_pp_int_datatype(), 0, 3);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 8, 9);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle across two ranges
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 9, 12);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle on a min
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 11, 11);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap in the middle on a max
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 10, 10);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap at the end
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 12, 15);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// overlap touching the end
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 13, 15);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}
	try {
		// min > max
		pp::MultiDatatype m;
		m.add_range(new_pp_int_datatype(), 0, 10);
		m.add_range(new_pp_hex_datatype(), 11, 13);
		m.add_range(new_pp_int_datatype(), 25, 20);
		TEST_FAIL("pp::MultiDatatype::add_range()");
	} catch (std::exception &e) {
	}

	{
		// test the evaluate() method
		pp::MultiDatatype m;
		m.add_range(new_pp_bool_datatype("true", "false"), 0, 1);
		m.add_range(new_pp_int_datatype("units"), 5, 10);
		m.add_range(new_pp_int_datatype(), 11, 12);
		m.add_range(new_pp_hex_datatype(pp::BITS32), 15, 20);
		util::KeyedVector<string, pp::Value> enumlist;
		enumlist.insert("3", 25);
		enumlist.insert("11", 26);
		enumlist.insert("test_key", 27);
		m.add_range(new_pp_enum_datatype(enumlist), 25, 27);
		// ...bool
		TEST_ASSERT(m.evaluate(pp::Value(0)) == "false",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(1)) == "true",
				"pp::MultiDatatype::evaluate()");
		// ...int
		TEST_ASSERT(m.evaluate(pp::Value(5)) == "5 units",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(6)) == "6 units",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(10)) == "10 units",
				"pp::MultiDatatype::evaluate()");
		// ...hex
		TEST_ASSERT(m.evaluate(pp::Value(15)) == "0x0000000f",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(16)) == "0x00000010",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(20)) == "0x00000014",
				"pp::MultiDatatype::evaluate()");
		// ...enum
		TEST_ASSERT(m.evaluate(pp::Value(25)) == "3",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(26)) == "11",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(27)) == "test_key",
				"pp::MultiDatatype::evaluate()");
		// ...out of range
		TEST_ASSERT(m.evaluate(pp::Value(3)) == "<!3!>",
				"pp::MultiDatatype::evaluate()");
		TEST_ASSERT(m.evaluate(pp::Value(30)) == "<!30!>",
				"pp::MultiDatatype::evaluate()");

		// test the lookup() method
		// ...lookup(pp::Value)
		try {
			TEST_ASSERT(m.lookup(pp::Value(0)) == pp::Value(0),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup(pp::Value(7)) == pp::Value(7),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup(pp::Value(20)) == pp::Value(20),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		try {
			// out of range
			m.lookup(pp::Value(13));
			TEST_FAIL("pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
		}
		// ...lookup(string)
		try {
			TEST_ASSERT(m.lookup("true") == pp::Value(1),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("true") == pp::Value(1),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("0x0000000f") == pp::Value(15),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		try {
			TEST_ASSERT(m.lookup("test_key") == pp::Value(27),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
		// FIXME: This test fails because of the ambiguity in the
		// multi datatype; both pp::Value(11) and pp::Value(26)
		// produce the string "11". For now, lookup(str) is defined to
		// return the lowest pp::Value the given string could have
		// produced.
#if 0
		try {
			TEST_ASSERT(m.lookup("11") == pp::Value(26),
				"pp::MultiDatatype::lookup()");
		} catch (std::exception &e) {
			TEST_FAIL("pp::MultiDatatype::lookup()");
		}
#endif
	}
}

TEST(test_pp_string_datatype)
{
	// test the basic constructor
	pp::StringDatatype s;

	// test the evaluate() method
	if (s.evaluate(0x41) != "A") {
		TEST_FAIL("pp::StringDatatype::evaluate() returns " +
			   s.evaluate(pp::Value(0x41)));
	}
	// ...test null character
	if (s.evaluate(pp::Value(0x00)) != "") {
		TEST_FAIL("pp::StringDatatype::evaluate() returns " +
			   s.evaluate(pp::Value(0x00)));
	}
	// ...test multicharacter strings
	if (s.evaluate(pp::Value("0x6f6c6c6548")) != "Hello") {
		TEST_FAIL("pp::StringDatatype::evaluate() returns " +
			   s.evaluate(pp::Value("0x6f6c6c6548")));
	}
	// ...test multicharacter strings with nulls
	if (s.evaluate(pp::Value("0x00006f6c6c6548")) != "Hello") {
		TEST_FAIL("pp::StringDatatype::evaluate() returns " +
			   s.evaluate(pp::Value("0x00006f6c6c6548")));
	}
	if (s.evaluate(pp::Value("0x6f006c006c0065004800"))
	    == "Hello") {
		// These should not be equal because the string returned
		// by evaluate() contains embedded null characters.
		TEST_FAIL("pp::StringDatatype::evaluate() returns " +
			   s.evaluate(pp::Value("0x6f006c006c0065004800")));
	}

	// test lookup()
	TEST_ASSERT(s.lookup(pp::Value(0x00)) == pp::Value(0x00),
			"pp::StringDatatype::lookup(pp::Value)");
	TEST_ASSERT(s.lookup(pp::Value("0x1234567890"))
			   == pp::Value("0x1234567890"),
			"pp::StringDatatype::lookup(pp::Value)");
	TEST_ASSERT(s.lookup("A") == pp::Value(0x41),
			"pp::StringDatatype::lookup(string)");
	TEST_ASSERT(s.lookup("") == pp::Value(0x00),
			"pp::StringDatatype::lookup(string)");
	TEST_ASSERT(s.lookup("Hello")
		           == pp::Value("0x6f6c6c6548"),
			"pp::StringDatatype::lookup(pp::Value)");

	// test that evaluate() and lookup() are inverse
	TEST_ASSERT(s.lookup(s.evaluate(0x333231)) == 0x333231,
			"pp::StringDatatype::lookup(string) and "
			"pp::StringDatatype::evaluate(pp::Value)");
	TEST_ASSERT(s.lookup(s.evaluate(pp::Value("0x6d086c0065004800")))
		== pp::Value("0x6d086c0065004800"),
			"pp::StringDatatype::lookup(string) and "
			"pp::StringDatatype::evaluate(pp::Value)");
	TEST_ASSERT(s.evaluate(s.lookup("xyz")) == "xyz",
			"pp::StringDatatype::lookup(string) and "
			"pp::StringDatatype::evaluate(pp::Value)");
}

TEST(test_pp_bool_datatype)
{
	// test the basic constructor
	pp::BoolDatatype b("TRUE", "FALSE");

	// test the evaluate() method
	if (b.evaluate(0) != "FALSE") {
		TEST_FAIL("pp::BoolDatatype::evaluate()");
	}
	if (b.evaluate(1) != "TRUE") {
		TEST_FAIL("pp::BoolDatatype::evaluate()");
	}
	if (b.evaluate(2) != "TRUE") {
		TEST_FAIL("pp::BoolDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(b.lookup("FALSE") == 0,
	    "pp::BoolDatatype::lookup(string)");
	TEST_ASSERT(b.lookup("TRUE") == 1,
	    "pp::BoolDatatype::lookup(string)");
	TEST_ASSERT(b.lookup(pp::Value(0)) == 0,
	    "pp::BoolDatatype::lookup(int)");
	TEST_ASSERT(b.lookup(1) == 1,
	    "pp::BoolDatatype::lookup(int)");
	TEST_ASSERT(b.lookup(4) == 1,
	    "pp::BoolDatatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_FAIL("pp::BoolDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}
}

TEST(test_pp_bitmask_datatype)
{
	// test the basic constructor
	pp::BitmaskDatatype b;

	// test the evaluate() method
	b.add_bit("bit_one", 1);
	b.add_bit("bit_two", 2);
	b.add_bit("bit_three", 3);
	if (b.evaluate(2) != "bit_one") {
		TEST_FAIL("pp::BitmaskDatatype::evaluate()");
	}
	if (b.evaluate(1) != "<!0!>") {
		TEST_FAIL("pp::BitmaskDatatype::evaluate()");
	}
	if (b.evaluate(6) != "bit_one bit_two") {
		TEST_FAIL("pp::BitmaskDatatype::evaluate()");
	}
	if (b.evaluate(0) != "") {
		TEST_FAIL("pp::BitmaskDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(b.lookup("bit_one") == 1,
	    "pp::BitmaskDatatype::lookup(string)");
	TEST_ASSERT(b.lookup("bit_two") == 2,
	    "pp::BitmaskDatatype::lookup(string)");
	TEST_ASSERT(b.lookup(1) == 1,
			"pp::BitmaskDatatype::lookup(int)");
	TEST_ASSERT(b.lookup(2) == 2,
			"pp::BitmaskDatatype::lookup(int)");
	try {
		b.lookup("foo");
		TEST_FAIL("pp::BitmaskDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}
	try {
		b.lookup(4);
		TEST_FAIL("pp::BitmaskDatatype::lookup(int)");
	} catch (pp::Datatype::InvalidError &e) {
	}
}

TEST(test_pp_int_datatype)
{
	// test the basic constructor
	pp::IntDatatype i1;

	// test the evaluate() method
	if (i1.evaluate(1) != "1") {
		TEST_FAIL("pp::IntDatatype::evaluate()");
	}
	if (i1.evaluate(-1) != "-1") {
		TEST_FAIL("pp::IntDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(i1.lookup("1") == 1,
			"pp::IntDatatype::lookup(string)");
	TEST_ASSERT(i1.lookup("23") == 23,
			"pp::IntDatatype::lookup(string)");
	TEST_ASSERT(i1.lookup(1) == 1, "pp::IntDatatype::lookup(int)");
	TEST_ASSERT(i1.lookup(23) == 23, "pp::IntDatatype::lookup(int)");
	try {
		i1.lookup("foo");
		TEST_FAIL("pp::IntDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}

	// test the units constructor
	pp::IntDatatype i2("units");

	// test the evaluate() method
	if (i2.evaluate(1) != "1 units") {
		TEST_FAIL("pp::IntDatatype::evaluate()");
	}
	if (i2.evaluate(-1) != "-1 units") {
		TEST_FAIL("pp::IntDatatype::evaluate()");
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	TEST_ASSERT(i2.lookup("1 units") == 1,
	    "pp::IntDatatype::lookup(string)");
	TEST_ASSERT(i2.lookup("23 units") == 23,
	    "pp::IntDatatype::lookup(string)");
	#endif
	TEST_ASSERT(i2.lookup(1) == 1,
	    "pp::IntDatatype::lookup(int)");
	TEST_ASSERT(i2.lookup(23) == 23,
	    "pp::IntDatatype::lookup(int)");
	try {
		i2.lookup("foo");
		TEST_FAIL("pp::IntDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}
}

TEST(test_pp_hex_datatype)
{
	// test the basic constructor
	pp::HexDatatype h1;

	// test the evaluate() method
	if (h1.evaluate(1) != "0x1") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(h1.lookup("0x1") == 1,
	    "pp::HexDatatype::lookup(string)");
	TEST_ASSERT(h1.lookup("0x23") ==35,
	    "pp::HexDatatype::lookup(string)");
	TEST_ASSERT(h1.lookup(1) == 1,
	    "pp::HexDatatype::lookup(int)");
	TEST_ASSERT(h1.lookup(23) == 23,
	    "pp::HexDatatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_FAIL("pp::HexDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}

	// test the units constructor
	pp::HexDatatype h2(pp::BITS0, "units");
	if (h2.evaluate(1) != "0x1 units") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}

	// test lookup()
	//FIXME: this test fails because bignum fails to parse "1 units"
	// and returns 0, yet "1 " parses just fine.
	#if 0
	TEST_ASSERT(h1.lookup("0x1 units") == 1,
	    "pp::HexDatatype::lookup(string)");
	TEST_ASSERT(h1.lookup("0x23 units") ==35,
	    "pp::HexDatatype::lookup(string)");
	#endif
	TEST_ASSERT(h1.lookup(1) == 1,
	    "pp::HexDatatype::lookup(int)");
	TEST_ASSERT(h1.lookup(23) == 23,
	    "pp::HexDatatype::lookup(int)");
	try {
		h1.lookup("foo");
		TEST_FAIL("pp::HexDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}

	// test the width constructor
	pp::HexDatatype h3(pp::BITS8);

	// test the evaluate() method
	if (h3.evaluate(1) != "0x01") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}
	pp::HexDatatype h4(pp::BITS16);
	if (h4.evaluate(0x0201) != "0x0201") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}
	pp::HexDatatype h5(pp::BITS32);
	if (h5.evaluate(0x04030201) != "0x04030201") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}
	pp::HexDatatype h6(pp::BITS64);
	if (h6.evaluate(pp::Value("0x0807060504030201")) !=
			"0x0807060504030201") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}

	// test the units-and-width constructor
	pp::HexDatatype h7(pp::BITS8, "units");

	// test the evaluate() method
	if (h7.evaluate(1) != "0x01 units") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}
	pp::HexDatatype h8(pp::BITS16, "units");
	if (h8.evaluate(0x0201) != "0x0201 units") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}
	pp::HexDatatype h9(pp::BITS32, "units");
	if (h9.evaluate(0x04030201) != "0x04030201 units") {
		TEST_FAIL("pp::HexDatatype::evaluate()");
	}
	pp::HexDatatype h10(pp::BITS64, "units");
	if (h10.evaluate(pp::Value("0x0807060504030201")) !=
			 "0x0807060504030201 units") {
		TEST_FAIL("h10.evaluate()");
	}
}

TEST(test_pp_transform_datatype)
{
	// test the basic constructor
	pp::TransformDatatypePtr t1 = new_pp_transform_datatype(
		new_pp_int_datatype(), _1+1, _1-1);

	// test the evaluate() method
	if (t1->evaluate(0) != "1") {
		TEST_FAIL("pp::TransformDatatype::evaluate()");
	}
	if (t1->evaluate(1) != "2") {
		TEST_FAIL("pp::TransformDatatype::evaluate()");
	}

	// test lookup()
	TEST_ASSERT(t1->lookup("1") == 0,
			"pp::TransformDatatype::lookup(string)");
	TEST_ASSERT(t1->lookup("23") == 22,
			"pp::TransformDatatype::lookup(string)");
	TEST_ASSERT(t1->lookup(1) == 0,
			"pp::TransformDatatype::lookup(int)");
	TEST_ASSERT(t1->lookup(23) == 22,
			"pp::TransformDatatype::lookup(int)");
	try {
		t1->lookup("foo");
		TEST_FAIL("pp::TransformDatatype::lookup(string)");
	} catch (pp::Datatype::InvalidError &e) {
	}
}

TEST(test_pp_fixed_datatype)
{
	// test the basic constructor
	pp::FixedDatatypePtr f1 = new_pp_fixed_datatype(2);

	// test the evaluate() method
	std::string str;
	str = f1->evaluate(0);
	TEST_ASSERT(str == "0.0")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(1);
	TEST_ASSERT(str == "0.25")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(2);
	TEST_ASSERT(str == "0.5")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(3);
	TEST_ASSERT(str == "0.75")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(4);
	TEST_ASSERT(str == "1.0")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(5);
	TEST_ASSERT(str == "1.25")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(6);
	TEST_ASSERT(str == "1.5")
		<< "pp::FixedDatatype::evaluate(): " << str;
	str = f1->evaluate(7);
	TEST_ASSERT(str == "1.75")
		<< "pp::FixedDatatype::evaluate(): " << str;

	// test lookup
	// FIXME:
}
