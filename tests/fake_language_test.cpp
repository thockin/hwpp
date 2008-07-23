/*
 * Copyright 2008 Tim Hockin
 */

#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
#include "fake_language.h"
#include "device_init.h"
using namespace std;

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

int
test_parse_errors()
{
	int ret = 0;

	// basic setup
	platform_global_init();

	//
	// GET_DIRENT
	//
	// should throw, but not a parse_error
	try {
		GET_DIRENT("valid");
		ret += TEST_ERROR("GET_DIRENT()");
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("GET_DIRENT()");
	} catch (exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		GET_DIRENT("123_invalid");
		ret += TEST_ERROR("GET_DIRENT()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("GET_DIRENT()");
	}

	//
	// GET_FIELD
	//
	// should throw, but not a parse_error
	try {
		GET_FIELD("valid");
		ret += TEST_ERROR("GET_FIELD()");
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("GET_FIELD()");
	} catch (exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		GET_FIELD("123_invalid");
		ret += TEST_ERROR("GET_FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("GET_FIELD()");
	}

	//
	// GET_REGISTER
	//
	// should throw, but not a parse_error
	try {
		GET_REGISTER("valid");
		ret += TEST_ERROR("GET_REGISTER()");
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("GET_REGISTER()");
	} catch (exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		GET_REGISTER("123_invalid");
		ret += TEST_ERROR("GET_REGISTER()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("GET_REGISTER()");
	}

	//
	// DEFINED
	//
	// should not throw
	try {
		DEFINED("valid");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("DEFINED()");
	} catch (exception &e) {
		ret += TEST_ERROR("DEFINED()");
	}
	// should throw a parse_error
	try {
		DEFINED("123_invalid");
		ret += TEST_ERROR("DEFINED()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("DEFINED()");
	}

	//
	// READ
	//
	// should throw, but not a parse_error
	try {
		READ("valid");
		ret += TEST_ERROR("READ()");
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("READ()");
	} catch (exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		READ("123_invalid");
		ret += TEST_ERROR("READ()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("READ()");
	}

	//
	// WRITE
	//
	// should throw, but not a parse_error
	try {
		WRITE("valid", 123);
		ret += TEST_ERROR("WRITE()");
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("WRITE()");
	} catch (exception &e) {
		// expected
	}
	// should throw a parse_error
	try {
		WRITE("123_invalid", 123);
		ret += TEST_ERROR("WRITE()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("WRITE()");
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
		ret += TEST_ERROR("OPEN_SCOPE()");
	} catch (exception &e) {
		ret += TEST_ERROR("OPEN_SCOPE()");
	}
	// should throw a parse_error
	try {
		OPEN_SCOPE("123_invalid");
		ret += TEST_ERROR("OPEN_SCOPE()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("OPEN_SCOPE()");
	}
	// should not throw
	try {
		OPEN_SCOPE("scope2");
		CLOSE_SCOPE("also_valid");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("OPEN_SCOPE()");
	} catch (exception &e) {
		ret += TEST_ERROR("OPEN_SCOPE()");
	}
	// should throw a parse_error
	try {
		OPEN_SCOPE("scope3");
		CLOSE_SCOPE("123_invalid");
		ret += TEST_ERROR("OPEN_SCOPE()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("OPEN_SCOPE()");
	}

	//
	// set up a bound scope for testing
	//
	OPEN_SCOPE("bound", new_test_binding());

	//
	// REGN
	//
	// should not throw
	try {
		REG8("%reg8", 0);
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG8()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG8()");
	}
	// should throw a parse_error
	try {
		REG8("%123_invalid", 123);
		ret += TEST_ERROR("REG8()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG8()");
	}
	// should not throw
	try {
		REG16("%reg16", 0);
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG16()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG16()");
	}
	// should throw a parse_error
	try {
		REG16("%123_invalid", 123);
		ret += TEST_ERROR("REG16()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG16()");
	}
	// should not throw
	try {
		REG32("%reg32", 0);
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG32()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG32()");
	}
	// should throw a parse_error
	try {
		REG32("%123_invalid", 123);
		ret += TEST_ERROR("REG32()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG32()");
	}
	// should not throw
	try {
		REG64("%reg64", 0);
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG64()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG64()");
	}
	// should throw a parse_error
	try {
		REG64("%123_invalid", 123);
		ret += TEST_ERROR("REG64()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG64()");
	}
	// should not throw
	try {
		REG128("%reg128", 0);
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG128()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG128()");
	}
	// should throw a parse_error
	try {
		REG128("%123_invalid", 123);
		ret += TEST_ERROR("REG128()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG128()");
	}
	// should not throw
	try {
		REG8("%proc_reg8", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG8()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG8()");
	}
	// should throw a parse_error
	try {
		REG8("%123_invalid", PROCS(test_procs));
		ret += TEST_ERROR("REG8()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG8()");
	}
	// should not throw
	try {
		REG16("%proc_reg16", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG16()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG16()");
	}
	// should throw a parse_error
	try {
		REG16("%123_invalid", PROCS(test_procs));
		ret += TEST_ERROR("REG16()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG16()");
	}
	// should not throw
	try {
		REG32("%proc_reg32", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG32()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG32()");
	}
	// should throw a parse_error
	try {
		REG32("%123_invalid", PROCS(test_procs));
		ret += TEST_ERROR("REG32()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG32()");
	}
	// should not throw
	try {
		REG64("%proc_reg64", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG64()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG64()");
	}
	// should throw a parse_error
	try {
		REG64("%123_invalid", PROCS(test_procs));
		ret += TEST_ERROR("REG64()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG64()");
	}
	// should not throw
	try {
		REG128("%proc_reg128", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REG128()");
	} catch (exception &e) {
		ret += TEST_ERROR("REG128()");
	}
	// should throw a parse_error
	try {
		REG128("%123_invalid", PROCS(test_procs));
		ret += TEST_ERROR("REG128()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REG128()");
	}

	//
	// BITS
	//
	// should not throw
	try {
		BITS("%reg8");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BITS()");
	} catch (exception &e) {
		ret += TEST_ERROR("BITS()");
	}
	// should throw a parse_error
	try {
		BITS("%123_invalid");
		ret += TEST_ERROR("BITS()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("BITS()");
	}

	//
	// REGFIELDN
	//
	// should not throw
	try {
		REGFIELD8("regfield8", 0, "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD8()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD8()");
	}
	// should throw a parse_error
	try {
		REGFIELD8("123_invalid", 0, "int_t");
		ret += TEST_ERROR("REGFIELD8()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD8()");
	}
	// should not throw
	try {
		REGFIELD16("regfield16", 0, "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD16()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD16()");
	}
	// should throw a parse_error
	try {
		REGFIELD16("123_invalid", 0, "int_t");
		ret += TEST_ERROR("REGFIELD16()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD16()");
	}
	// should not throw
	try {
		REGFIELD32("regfield32", 0, "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD32()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD32()");
	}
	// should throw a parse_error
	try {
		REGFIELD32("123_invalid", 0, "int_t");
		ret += TEST_ERROR("REGFIELD32()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD32()");
	}
	// should not throw
	try {
		REGFIELD64("regfield64", 0, "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD64()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD64()");
	}
	// should throw a parse_error
	try {
		REGFIELD64("123_invalid", 0, "int_t");
		ret += TEST_ERROR("REGFIELD64()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD64()");
	}
	// should not throw
	try {
		REGFIELD128("regfield128", 0, "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD128()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD128()");
	}
	// should throw a parse_error
	try {
		REGFIELD128("123_invalid", 0, "int_t");
		ret += TEST_ERROR("REGFIELD128()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD128()");
	}
	// should not throw
	try {
		REGFIELD8("proc_regfield8", 0, "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD8()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD8()");
	}
	// should throw a parse_error
	try {
		REGFIELD8("123_invalid", PROCS(test_procs), "int_t");
		ret += TEST_ERROR("REGFIELD8()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD8()");
	}
	// should not throw
	try {
		REGFIELD16("proc_regfield16", PROCS(test_procs), "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD16()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD16()");
	}
	// should throw a parse_error
	try {
		REGFIELD16("123_invalid", PROCS(test_procs), "int_t");
		ret += TEST_ERROR("REGFIELD16()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD16()");
	}
	// should not throw
	try {
		REGFIELD32("proc_regfield32", PROCS(test_procs), "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD32()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD32()");
	}
	// should throw a parse_error
	try {
		REGFIELD32("123_invalid", PROCS(test_procs), "int_t");
		ret += TEST_ERROR("REGFIELD32()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD32()");
	}
	// should not throw
	try {
		REGFIELD64("proc_regfield64", PROCS(test_procs), "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD64()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD64()");
	}
	// should throw a parse_error
	try {
		REGFIELD64("123_invalid", PROCS(test_procs), "int_t");
		ret += TEST_ERROR("REGFIELD64()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD64()");
	}
	// should not throw
	try {
		REGFIELD128("proc_regfield128", PROCS(test_procs), "int_t");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("REGFIELD128()");
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD128()");
	}
	// should throw a parse_error
	try {
		REGFIELD128("123_invalid", PROCS(test_procs), "int_t");
		ret += TEST_ERROR("REGFIELD128()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("REGFIELD128()");
	}

	//
	// FIELD
	//
	// should not throw
	try {
		FIELD("field1", "int_t", BITS("%reg8"));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("FIELD()");
	} catch (exception &e) {
		ret += TEST_ERROR("FIELD()");
	}
	// should throw a parse_error
	try {
		FIELD("123_invalid", "int_t", BITS("%reg8"));
		ret += TEST_ERROR("FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("FIELD()");
	}
	// should not throw
	try {
		FIELD("field2", "int_t", 123);
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("FIELD()");
	} catch (exception &e) {
		ret += TEST_ERROR("FIELD()");
	}
	// should throw a parse_error
	try {
		FIELD("123_invalid", "int_t", 123);
		ret += TEST_ERROR("FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("FIELD()");
	}
	// should not throw
	try {
		FIELD("field3", "int_t", PROCS(test_procs));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("FIELD()");
	} catch (exception &e) {
		ret += TEST_ERROR("FIELD()");
	}
	// should throw a parse_error
	try {
		FIELD("123_invalid", "int_t", PROCS(test_procs));
		ret += TEST_ERROR("FIELD()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("FIELD()");
	}

	//
	// INT
	//
	// should not throw
	try {
		INT("int1");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("INT()");
	} catch (exception &e) {
		ret += TEST_ERROR("INT()");
	}
	// should throw a parse_error
	try {
		INT("123_invalid");
		ret += TEST_ERROR("INT()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("INT()");
	}
	// should not throw
	try {
		ANON_INT();
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ANON_INT()");
	} catch (exception &e) {
		ret += TEST_ERROR("ANON_INT()");
	}

	//
	// HEX
	//
	// should not throw
	try {
		HEX("hex1");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("HEX()");
	} catch (exception &e) {
		ret += TEST_ERROR("HEX()");
	}
	// should throw a parse_error
	try {
		HEX("123_invalid");
		ret += TEST_ERROR("HEX()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("HEX()");
	}
	// should not throw
	try {
		ANON_HEX();
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ANON_HEX()");
	} catch (exception &e) {
		ret += TEST_ERROR("ANON_HEX()");
	}

	//
	// BITMASK
	//
	// should not throw
	try {
		BITMASK("bitmask1", KV("valid1", 1));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BITMASK()");
	} catch (exception &e) {
		ret += TEST_ERROR("BITMASK()");
	}
	// should not throw
	try {
		BITMASK("bitmask2", KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BITMASK()");
	} catch (exception &e) {
		ret += TEST_ERROR("BITMASK()");
	}
	// should throw a parse_error
	try {
		BITMASK("123_invalid", KV("valid1", 1), KV("valid2", 2));
		ret += TEST_ERROR("BITMASK()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("BITMASK()");
	}
	// should not throw
	try {
		BITMASK("bitmask3", KV("123_valid", 1));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BITMASK()");
	} catch (exception &e) {
		ret += TEST_ERROR("BITMASK()");
	}
	// should not throw
	try {
		ANON_BITMASK(KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ANON_BITMASK()");
	} catch (exception &e) {
		ret += TEST_ERROR("ANON_BITMASK()");
	}

	//
	// ENUM
	//
	// should not throw
	try {
		ENUM("enum1", KV("valid1", 1));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ENUM()");
	} catch (exception &e) {
		ret += TEST_ERROR("ENUM()");
	}
	// should not throw
	try {
		ENUM("enum2", KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ENUM()");
	} catch (exception &e) {
		ret += TEST_ERROR("ENUM()");
	}
	// should throw a parse_error
	try {
		ENUM("123_invalid", KV("valid1", 1), KV("valid2", 2));
		ret += TEST_ERROR("ENUM()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("ENUM()");
	}
	// should not throw
	try {
		ENUM("enum3", KV("123_valid", 1));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ENUM()");
	} catch (exception &e) {
		ret += TEST_ERROR("ENUM()");
	}
	// should not throw
	try {
		ANON_ENUM(KV("valid1", 1), KV("valid2", 2));
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ANON_ENUM()");
	} catch (exception &e) {
		ret += TEST_ERROR("ANON_ENUM()");
	}

	//
	// BOOL
	//
	// should not throw
	try {
		BOOL("bool1", "valid1", "valid2");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BOOL()");
	} catch (exception &e) {
		ret += TEST_ERROR("BOOL()");
	}
	// should throw a parse_error
	try {
		BOOL("123_invalid", "valid1", "valid2");
		ret += TEST_ERROR("BOOL()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("BOOL()");
	}
	// should not throw
	try {
		BOOL("bool3", "123_valid", "456_valid");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BOOL()");
	} catch (exception &e) {
		ret += TEST_ERROR("BOOL()");
	}
	// should not throw
	try {
		ANON_BOOL("valid1", "valid2");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("ANON_BOOL()");
	} catch (exception &e) {
		ret += TEST_ERROR("ANON_BOOL()");
	}

	//
	// BOOKMARK
	//
	// should not throw
	try {
		BOOKMARK("valid");
		// expected
	} catch (pp_parse_error &e) {
		ret += TEST_ERROR("BOOKMARK()");
	} catch (exception &e) {
		ret += TEST_ERROR("BOOKMARK()");
	}
	// should throw a parse_error
	try {
		BOOKMARK("123_invalid");
		ret += TEST_ERROR("BOOKMARK()");
	} catch (pp_parse_error &e) {
		// expected
	} catch (exception &e) {
		ret += TEST_ERROR("BOOKMARK()");
	}

	return ret;
}

TEST_LIST(
	TEST(test_parse_errors),
);
