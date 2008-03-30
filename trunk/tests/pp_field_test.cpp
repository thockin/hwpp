#include "pp_fields.h"
#include "pp.h"
#include "pp_test.h"
#include "pp_datatypes.h"
#include "pp_register.h"
#include "pp_regbits.h"
#include "test_binding.h"
using namespace std;

//FIXME: test lookup()
//FIXME: test compare()

int
test_int_field()
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
	pp_direct_field f1(integer.get(),
		pp_regbits(r2.get(), 7, 0) + pp_regbits(r1.get(), 7, 0));

	/* test read() */
	if (f1.read() != 8721) {
		TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1.evaluate() != "8721") {
		TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1102) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2201) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_hex_field()
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
	pp_direct_field f1(hex16.get(),
		pp_regbits(r2.get(), 7, 0) + pp_regbits(r1.get(), 7, 0));

	/* test read() */
	if (f1.read() != 0x2211) {
		TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1.evaluate() != "0x2211") {
		TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1102) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2201) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_enum_field()
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
	pp_direct_field_ptr f1 = new_pp_direct_field(e1.get(),
		pp_regbits(r2.get(), 7, 0) + pp_regbits(r1.get(), 7, 0));

	/* test read() */
	if (f1->read() != 0x2211) {
		TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1->evaluate() != "correct") {
		TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(1);
	if (f1->read() != 0x1) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1101) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2200) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

int
test_bitmask_field()
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
	pp_direct_field_ptr f1 = new_pp_direct_field(b1.get(),
		pp_regbits(r2.get(), 7, 0) + pp_regbits(r1.get(), 7, 0));

	/* test read() */
	if (f1->read() != 0x2211) {
		TEST_ERROR("pp_direct_field::read()");
		ret++;
	}
	if (f1->evaluate() != "zero four nine thirteen") {
		TEST_ERROR("pp_direct_field::evaluate()");
		ret++;
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(0x207);
	if (f1->read() != 0x207) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r1->read() != 0x1107) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}
	if (r2->read() != 0x2202) {
		TEST_ERROR("pp_direct_field::write()");
		ret++;
	}

	return ret;
}

class
test_proc_field_accessor: public proc_field_accessor
{
    public:
	virtual pp_value
	read() const
	{
		return m_data;
	}

	virtual void
	write(const pp_value &value) const
	{
		m_data = value;
	}

    private:
	mutable pp_value m_data;
};

int
test_proc_field()
{
	int ret = 0;

	pp_datatype *hex = new pp_hex();
	proc_field_accessor_ptr accessor(new test_proc_field_accessor);
	pp_proc_field f(hex, accessor);
	f.write(0x12345678);
	if (f.read() != 0x12345678) {
		TEST_ERROR("pp_proc_field::write()");
		ret++;
	}
	if (f.evaluate() != "0x12345678") {
		TEST_ERROR("pp_proc_field::write()");
		ret++;
	}

	return ret;
}

int
test_constant_field()
{
	int ret = 0;

	pp_datatype *hex = new pp_hex();
	pp_constant_field f(hex, 0x12345678);
	if (f.read() != 0x12345678) {
		TEST_ERROR("pp_constant_field::read()");
		ret++;
	}
	if (f.evaluate() != "0x12345678") {
		TEST_ERROR("pp_constant_field::read()");
		ret++;
	}
	f.write(0);
	if (f.read() != 0x12345678) {
		TEST_ERROR("pp_constant_field::write()");
		ret++;
	}

	return ret;
}

TEST_LIST(
	TEST(test_int_field),
	TEST(test_hex_field),
	TEST(test_enum_field),
	TEST(test_bitmask_field),
	TEST(test_proc_field),
	TEST(test_constant_field),
);
