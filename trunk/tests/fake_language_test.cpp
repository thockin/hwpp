/*
 * Copyright 2008 Tim Hockin
 */

#include "pp.h"
#include "fake_language.h"
#include "device_init.h"
#include "test_binding.h"
#include "pp_test.h"

// used later, must be out-of-line
class test_procs: public pp_rwprocs
{
	pp_value
	read() const
	{
		return 0;
	}
	void
	write(const pp_value &value) const
	{
		(void)value;
	}
};

TEST_SETUP()
{
	// basic setup
	global_datatypes_init();
}

TEST(test_parse_errors)
{
	//
	// set up a bound scope for testing
	//
	OPEN_SCOPE("bound", new_test_binding());

	//
	// GET_DIRENT
	//
	// should throw, but not a parse_error
	try {
		GET_DIRENT("valid");
		TEST_FAIL("GET_DIRENT()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("GET_DIRENT()");
	} catch (std::exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		GET_DIRENT("123_invalid");
		TEST_FAIL("GET_DIRENT()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("GET_DIRENT()");
	}

	//
	// GET_FIELD
	//
	// should throw, but not a parse_error
	try {
		GET_FIELD("valid");
		TEST_FAIL("GET_FIELD()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("GET_FIELD()");
	} catch (std::exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		GET_FIELD("123_invalid");
		TEST_FAIL("GET_FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("GET_FIELD()");
	}

	//
	// GET_REGISTER
	//
	// should throw, but not a parse_error
	try {
		GET_REGISTER("valid");
		TEST_FAIL("GET_REGISTER()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("GET_REGISTER()");
	} catch (std::exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		GET_REGISTER("123_invalid");
		TEST_FAIL("GET_REGISTER()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("GET_REGISTER()");
	}

	//
	// DEFINED
	//
	// should not throw
	try {
		DEFINED("valid");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("DEFINED()");
	} catch (std::exception &e) {
		TEST_FAIL("DEFINED()");
	}
	// should throw a parse_error
	try {
		DEFINED("123_invalid");
		TEST_FAIL("DEFINED()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("DEFINED()");
	}

	//
	// READ
	//
	// should not throw
	try {
		READ(REG32(0));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("READ()");
	} catch (std::exception &e) {
		TEST_FAIL("READ()");
	}
	// should throw, but not a parse_error
	try {
		READ("valid");
		TEST_FAIL("READ()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("READ()");
	} catch (std::exception &e) {
		// expected
	}
	try {
		READ(REG32(0));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("READ()");
	} catch (std::exception &e) {
		TEST_FAIL("READ()");
	}
	// should throw a parse_error
	try {
		READ("123_invalid");
		TEST_FAIL("READ()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("READ()");
	}

	//
	// WRITE
	//
	// should not throw
	try {
		WRITE(REG32(0), 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("WRITE()");
	} catch (std::exception &e) {
		TEST_FAIL("WRITE()");
	}
	// should throw, but not a parse_error
	try {
		WRITE("valid", 123);
		TEST_FAIL("WRITE()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("WRITE()");
	} catch (std::exception &e) {
		// expected
	}
	try {
		WRITE(REG32(0), 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("READ()");
	} catch (std::exception &e) {
		TEST_FAIL("READ()");
	}
	// should throw a parse_error
	try {
		WRITE("123_invalid", 123);
		TEST_FAIL("WRITE()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("WRITE()");
	}

	//
	// OPEN_SCOPE, CLOSE_SCOPE
	//
	// should not throw
	try {
		OPEN_SCOPE("scope1");
		CLOSE_SCOPE();
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("OPEN_SCOPE()");
	} catch (std::exception &e) {
		TEST_FAIL("OPEN_SCOPE()");
	}
	// should throw a parse_error
	try {
		OPEN_SCOPE("123_invalid");
		TEST_FAIL("OPEN_SCOPE()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("OPEN_SCOPE()");
	}
	// should not throw
	try {
		OPEN_SCOPE("scope2");
		CLOSE_SCOPE("also_valid");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("OPEN_SCOPE()");
	} catch (std::exception &e) {
		TEST_FAIL("OPEN_SCOPE()");
	}
	// should throw a parse_error
	try {
		OPEN_SCOPE("scope3");
		CLOSE_SCOPE("123_invalid");
		TEST_FAIL("OPEN_SCOPE()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("OPEN_SCOPE()");
	}

	//
	// REGN
	//
	// should not throw
	try {
		REG8("%reg8", 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG8()");
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should not throw
	try {
		REG8(0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG8()");
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should not throw
	try {
		REG8("%bound_reg8", new_test_binding(), 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG8()");
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should not throw
	try {
		REG8(new_test_binding(), 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG8()");
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should throw a parse_error
	try {
		REG8("%123_invalid", 123);
		TEST_FAIL("REG8()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should not throw
	try {
		REG16("%reg16", 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG16()");
	} catch (std::exception &e) {
		TEST_FAIL("REG16()");
	}
	// should throw a parse_error
	try {
		REG16("%123_invalid", 123);
		TEST_FAIL("REG16()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG16()");
	}
	// should not throw
	try {
		REG32("%reg32", 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG32()");
	} catch (std::exception &e) {
		TEST_FAIL("REG32()");
	}
	// should throw a parse_error
	try {
		REG32("%123_invalid", 123);
		TEST_FAIL("REG32()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG32()");
	}
	// should not throw
	try {
		REG64("%reg64", 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG64()");
	} catch (std::exception &e) {
		TEST_FAIL("REG64()");
	}
	// should throw a parse_error
	try {
		REG64("%123_invalid", 123);
		TEST_FAIL("REG64()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG64()");
	}
	// should not throw
	try {
		REG128("%reg128", 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG128()");
	} catch (std::exception &e) {
		TEST_FAIL("REG128()");
	}
	// should throw a parse_error
	try {
		REG128("%123_invalid", 123);
		TEST_FAIL("REG128()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG128()");
	}
	// should not throw
	try {
		REG8("%proc_reg8", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG8()");
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should not throw
	try {
		REG8(PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG8()");
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should throw a parse_error
	try {
		REG8("%123_invalid", PROCS(test_procs));
		TEST_FAIL("REG8()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG8()");
	}
	// should not throw
	try {
		REG16("%proc_reg16", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG16()");
	} catch (std::exception &e) {
		TEST_FAIL("REG16()");
	}
	// should throw a parse_error
	try {
		REG16("%123_invalid", PROCS(test_procs));
		TEST_FAIL("REG16()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG16()");
	}
	// should not throw
	try {
		REG32("%proc_reg32", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG32()");
	} catch (std::exception &e) {
		TEST_FAIL("REG32()");
	}
	// should throw a parse_error
	try {
		REG32("%123_invalid", PROCS(test_procs));
		TEST_FAIL("REG32()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG32()");
	}
	// should not throw
	try {
		REG64("%proc_reg64", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG64()");
	} catch (std::exception &e) {
		TEST_FAIL("REG64()");
	}
	// should throw a parse_error
	try {
		REG64("%123_invalid", PROCS(test_procs));
		TEST_FAIL("REG64()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG64()");
	}
	// should not throw
	try {
		REG128("%proc_reg128", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("REG128()");
	} catch (std::exception &e) {
		TEST_FAIL("REG128()");
	}
	// should throw a parse_error
	try {
		REG128("%123_invalid", PROCS(test_procs));
		TEST_FAIL("REG128()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("REG128()");
	}

	//
	// BITS
	//
	// should not throw
	try {
		BITS("%reg8");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should not throw
	try {
		BITS("%reg8", 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should not throw
	try {
		BITS("%reg8", 7, 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should not throw
	try {
		BITS(REG8(0));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should not throw
	try {
		BITS(REG8(0), 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should not throw
	try {
		BITS(REG8(0), 7, 0);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should throw a parse_error
	try {
		BITS("%123_invalid");
		TEST_FAIL("BITS()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("BITS()");
	}
	// should throw, but not a parse error
	try {
		BITS("%reg8", 8);
		TEST_FAIL("BITS()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		// expected
	}
	// should throw, but not a parse error
	try {
		BITS("%reg8", 8, 0);
		TEST_FAIL("BITS()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		// expected
	}
	// should throw, but not a parse error
	try {
		BITS(REG8(0), 8);
		TEST_FAIL("BITS()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		// expected
	}
	// should throw, but not a parse error
	try {
		BITS(REG8(0), 8, 0);
		TEST_FAIL("BITS()");
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITS()");
	} catch (std::exception &e) {
		// expected
	}

	//
	// FIELD
	//
	// should not throw
	try {
		FIELD("field1", "int_t", BITS("%reg8"));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("FIELD()");
	} catch (std::exception &e) {
		TEST_FAIL("FIELD()");
	}
	// should throw a parse_error
	try {
		FIELD("123_invalid", "int_t", BITS("%reg8"));
		TEST_FAIL("FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("FIELD()");
	}
	// should not throw
	try {
		FIELD("field2", "int_t", 123);
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("FIELD()");
	} catch (std::exception &e) {
		TEST_FAIL("FIELD()");
	}
	// should throw a parse_error
	try {
		FIELD("123_invalid", "int_t", 123);
		TEST_FAIL("FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("FIELD()");
	}
	// should not throw
	try {
		FIELD("field3", "int_t", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("FIELD()");
	} catch (std::exception &e) {
		TEST_FAIL("FIELD()");
	}
	// should throw a parse_error
	try {
		FIELD("123_invalid", "int_t", PROCS(test_procs));
		TEST_FAIL("FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("FIELD()");
	}

	//
	// INT
	//
	// should not throw
	try {
		INT("int1");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("INT()");
	} catch (std::exception &e) {
		TEST_FAIL("INT()");
	}
	// should throw a parse_error
	try {
		INT("123_invalid");
		TEST_FAIL("INT()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("INT()");
	}
	// should not throw
	try {
		ANON_INT();
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ANON_INT()");
	} catch (std::exception &e) {
		TEST_FAIL("ANON_INT()");
	}

	//
	// HEX
	//
	// should not throw
	try {
		HEX("hex1");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("HEX()");
	} catch (std::exception &e) {
		TEST_FAIL("HEX()");
	}
	// should throw a parse_error
	try {
		HEX("123_invalid");
		TEST_FAIL("HEX()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("HEX()");
	}
	// should not throw
	try {
		ANON_HEX();
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ANON_HEX()");
	} catch (std::exception &e) {
		TEST_FAIL("ANON_HEX()");
	}

	//
	// BITMASK
	//
	// should not throw
	try {
		BITMASK("bitmask1", KV("valid1", 1));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITMASK()");
	} catch (std::exception &e) {
		TEST_FAIL("BITMASK()");
	}
	// should not throw
	try {
		BITMASK("bitmask2", KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITMASK()");
	} catch (std::exception &e) {
		TEST_FAIL("BITMASK()");
	}
	// should throw a parse_error
	try {
		BITMASK("123_invalid", KV("valid1", 1), KV("valid2", 2));
		TEST_FAIL("BITMASK()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("BITMASK()");
	}
	// should not throw
	try {
		BITMASK("bitmask3", KV("123_valid", 1));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BITMASK()");
	} catch (std::exception &e) {
		TEST_FAIL("BITMASK()");
	}
	// should not throw
	try {
		ANON_BITMASK(KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ANON_BITMASK()");
	} catch (std::exception &e) {
		TEST_FAIL("ANON_BITMASK()");
	}

	//
	// MULTI
	//
	// should not throw
	try {
		MULTI("mp1", RANGE(ANON_INT(), 0, 10));
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("MULTI()");
	} 
	// should not throw
	try {
		MULTI("mp2", RANGE(ANON_INT(), 0, 10),
				 RANGE("hex32_t", 11, 15));
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("MULTI()");
	}
	// should not throw
	try {
		ANON_MULTI(RANGE(ANON_INT(), 0, 10),
			       RANGE("hex32_t", 11, 15));
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("MULTI()");
	}
	// should throw a parse_error due to invalid typename
	try {
		MULTI("123_invalid", RANGE(ANON_INT(), 0, 10),
					 RANGE("hex32_t", 11, 15));
		TEST_FAIL("MULTI()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("MULTI()");
	} 
	// should throw an invalid_error due to overlapping ranges
	try {
		MULTI("mp2", RANGE(ANON_INT(), 0, 10),
				 RANGE("hex32_t", 11, 15),
				 RANGE(ANON_INT("units"), 10, 11));
		TEST_FAIL("MULTI()");
	} catch (pp_datatype::invalid_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("MULTI()");
	}

	//
	// ENUM
	//
	// should not throw
	try {
		ENUM("enum1", KV("valid1", 1));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ENUM()");
	} catch (std::exception &e) {
		TEST_FAIL("ENUM()");
	}
	// should not throw
	try {
		ENUM("enum2", KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ENUM()");
	} catch (std::exception &e) {
		TEST_FAIL("ENUM()");
	}
	// should throw a parse_error
	try {
		ENUM("123_invalid", KV("valid1", 1), KV("valid2", 2));
		TEST_FAIL("ENUM()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("ENUM()");
	}
	// should not throw
	try {
		ENUM("enum3", KV("123_valid", 1));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ENUM()");
	} catch (std::exception &e) {
		TEST_FAIL("ENUM()");
	}
	// should not throw
	try {
		ANON_ENUM(KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ANON_ENUM()");
	} catch (std::exception &e) {
		TEST_FAIL("ANON_ENUM()");
	}

	//
	// BOOL
	//
	// should not throw
	try {
		BOOL("bool1", "valid1", "valid2");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BOOL()");
	} catch (std::exception &e) {
		TEST_FAIL("BOOL()");
	}
	// should throw a parse_error
	try {
		BOOL("123_invalid", "valid1", "valid2");
		TEST_FAIL("BOOL()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("BOOL()");
	}
	// should not throw
	try {
		BOOL("bool3", "123_valid", "456_valid");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BOOL()");
	} catch (std::exception &e) {
		TEST_FAIL("BOOL()");
	}
	// should not throw
	try {
		ANON_BOOL("valid1", "valid2");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("ANON_BOOL()");
	} catch (std::exception &e) {
		TEST_FAIL("ANON_BOOL()");
	}

	//
	// BOOKMARK
	//
	// should not throw
	try {
		BOOKMARK("valid");
		// expected
	} catch (pp_parse_error &e) {
		TEST_FAIL("BOOKMARK()");
	} catch (std::exception &e) {
		TEST_FAIL("BOOKMARK()");
	}
	// should throw a parse_error
	try {
		BOOKMARK("123_invalid");
		TEST_FAIL("BOOKMARK()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (std::exception &e) {
		TEST_FAIL("BOOKMARK()");
	}
}
