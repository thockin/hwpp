#include "pp_fields.h"
#include "pp.h"
#include "pp_test.h"
#include "pp_datatypes.h"
#include "pp_register.h"
#include "test_binding.h"
using namespace std;

//FIXME: test lookup()
//FIXME: test compare()

int
test_regbits()
{
	int ret = 0;

	pp_binding_ptr ts = new_test_binding();
	pp_register_ptr r1 = new_pp_register(ts.get(), 1, BITS16);

	/* test a read() with valid shift */
	regbits rb1(r1.get(), 0, 0xf, 16);
	if (rb1.read() != 0xf0000) {
		PP_TEST_ERROR("regbits::read()");
		ret++;
	}
	/* test a read() with invalid shift */
	regbits rb2(r1.get(), 32, 0x1, 0);
	if (rb2.read() != 0) {
		PP_TEST_ERROR("regbits::read()");
		ret++;
	}

	/* test the write() method */
	rb1.write(0x90000);
	if (rb1.read() != 0x90000) {
		PP_TEST_ERROR("regbits::write()");
		ret++;
	}
	if (r1->read() != 0xfff9) {
		PP_TEST_ERROR("regbits::write()");
		ret++;
	}

	return ret;
}

int
test_pp_int_field()
{
	int ret = 0;

	/* two bindings with one reg each */
	pp_binding_ptr bind1 = new_test_binding();
	bind1->write(0, BITS16, 0x1111);
	pp_register_ptr r1 = new_pp_register(bind1.get(), 1, BITS16);
	pp_binding_ptr bind2 = new_test_binding();
	bind2->write(0, BITS16, 0x2222);
	pp_register_ptr r2 = new_pp_register(bind2.get(), 1, BITS16);

	pp_datatype_ptr integer = new_pp_int();
	pp_direct_field f1(integer);
	f1.add_regbits(r1.get(), 0, 0xff, 0);
	f1.add_regbits(r2.get(), 0, 0xff, 8);

	/* test read() */
	if (f1.read() != 8721) {
		PP_TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1.evaluate() != "8721") {
		PP_TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1102) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2201) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_pp_uint_field()
{
	int ret = 0;

	/* two bindings with one reg each */
	pp_binding_ptr bind1 = new_test_binding();
	bind1->write(0, BITS16, 0x1111);
	pp_register_ptr r1 = new_pp_register(bind1.get(), 1, BITS16);
	pp_binding_ptr bind2 = new_test_binding();
	bind2->write(0, BITS16, 0x2222);
	pp_register_ptr r2 = new_pp_register(bind2.get(), 1, BITS16);

	pp_datatype_ptr uinteger = new_pp_uint();
	pp_direct_field f1(uinteger);
	f1.add_regbits(r1.get(), 0, 0xff, 0);
	f1.add_regbits(r2.get(), 0, 0xff, 8);

	/* test read() */
	if (f1.read() != 8721) {
		PP_TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1.evaluate() != "8721") {
		PP_TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1102) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2201) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_pp_hex_field()
{
	int ret = 0;

	/* two bindings with one reg each */
	pp_binding_ptr bind1 = new_test_binding();
	bind1->write(0, BITS16, 0x1111);
	pp_register_ptr r1 = new_pp_register(bind1.get(), 1, BITS16);
	pp_binding_ptr bind2 = new_test_binding();
	bind2->write(0, BITS16, 0x2222);
	pp_register_ptr r2 = new_pp_register(bind2.get(), 1, BITS16);

	/* test a hex16 field */
	pp_datatype_ptr hex16 = new_pp_hex(BITS16);
	pp_direct_field f1(hex16);
	f1.add_regbits(r1.get(), 0, 0xff, 0);
	f1.add_regbits(r2.get(), 0, 0xff, 8);

	/* test read() */
	if (f1.read() != 0x2211) {
		PP_TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1.evaluate() != "0x2211") {
		PP_TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1102) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2201) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_pp_enum_field()
{
	int ret = 0;

	/* two bindings with one reg each */
	pp_binding_ptr bind1 = new_test_binding();
	bind1->write(0, BITS16, 0x1111);
	pp_register_ptr r1 = new_pp_register(bind1.get(), 1, BITS16);
	pp_binding_ptr bind2 = new_test_binding();
	bind2->write(0, BITS16, 0x2222);
	pp_register_ptr r2 = new_pp_register(bind2.get(), 1, BITS16);

	/* test an enum field */
	pp_enum_ptr e1 = new_pp_enum();
	e1->add_value("one", 1);
	e1->add_value("two", 2);
	e1->add_value("three", 3);
	e1->add_value("correct", 0x2211);
	pp_direct_field_ptr f1 = new_pp_direct_field(e1);
	f1->add_regbits(r1.get(), 0, 0xff, 0);
	f1->add_regbits(r2.get(), 0, 0xff, 8);

	/* test read() */
	if (f1->read() != 0x2211) {
		PP_TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1->evaluate() != "correct") {
		PP_TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(1);
	if (f1->read() != 0x1) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1101) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2200) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_pp_bitmask_field()
{
	int ret = 0;

	/* two bindings with one reg each */
	pp_binding_ptr bind1 = new_test_binding();
	bind1->write(0, BITS16, 0x1111);
	pp_register_ptr r1 = new_pp_register(bind1.get(), 1, BITS16);
	pp_binding_ptr bind2 = new_test_binding();
	bind2->write(0, BITS16, 0x2222);
	pp_register_ptr r2 = new_pp_register(bind2.get(), 1, BITS16);

	/* test a bitmask field */
	pp_bitmask_ptr b1 = new_pp_bitmask();
	b1->add_bit("zero", 0);
	b1->add_bit("one", 1);
	b1->add_bit("two", 2);
	b1->add_bit("three", 3);
	b1->add_bit("four", 4);
	b1->add_bit("nine", 9);
	b1->add_bit("thirteen", 13);
	pp_direct_field_ptr f1 = new_pp_direct_field(b1);
	f1->add_regbits(r1.get(), 0, 0xff, 0);
	f1->add_regbits(r2.get(), 0, 0xff, 8);

	/* test read() */
	if (f1->read() != 0x2211) {
		PP_TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1->evaluate() != "zero four nine thirteen") {
		PP_TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(0x207);
	if (f1->read() != 0x207) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1107) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2202) {
		PP_TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_regbits();
	if (r) return EXIT_FAILURE;
	r = test_pp_int_field();
	if (r) return EXIT_FAILURE;
	r = test_pp_uint_field();
	if (r) return EXIT_FAILURE;
	r = test_pp_hex_field();
	if (r) return EXIT_FAILURE;
	r = test_pp_enum_field();
	if (r) return EXIT_FAILURE;
	r = test_pp_bitmask_field();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
