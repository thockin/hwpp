#include "pp/pp.h"
#include "pp/field_types.h"
#include "pp/datatype_types.h"
#include "pp/register_types.h"
#include "pp/regbits.h"
#include "test_binding.h"
#include "pp_test.h"

//FIXME: test lookup()
//FIXME: test compare()

TEST(test_int_field)
{
	/* two bindings with one reg each */
	pp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, pp::BITS16, 0x1111);
	pp::RegisterPtr r1 = new_pp_bound_register(bind1, 1, pp::BITS16);
	pp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, pp::BITS16, 0x2222);
	pp::RegisterPtr r2 = new_pp_bound_register(bind2, 1, pp::BITS16);

	pp::DatatypePtr integer = new_pp_int_datatype();
	pp::DirectField f1(integer,
		pp::RegBits(r2, 7, 0) + pp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1.read() != 8721) {
		TEST_FAIL("pp::DirectField::read()");
	}
	if (f1.evaluate() != "8721") {
		TEST_FAIL("pp::DirectField::evaluate()");
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r1->read() != 0x1102) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r2->read() != 0x2201) {
		TEST_FAIL("pp::DirectField::write()");
	}
}

TEST(test_hex_field)
{
	/* two bindings with one reg each */
	pp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, pp::BITS16, 0x1111);
	pp::RegisterPtr r1 = new_pp_bound_register(bind1, 1, pp::BITS16);
	pp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, pp::BITS16, 0x2222);
	pp::RegisterPtr r2 = new_pp_bound_register(bind2, 1, pp::BITS16);

	/* test a hex16 field */
	pp::DatatypePtr hex16 = new_pp_hex_datatype(pp::BITS16);
	pp::DirectField f1(hex16,
		pp::RegBits(r2, 7, 0) + pp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1.read() != 0x2211) {
		TEST_FAIL("pp::DirectField::read()");
	}
	if (f1.evaluate() != "0x2211") {
		TEST_FAIL("pp::DirectField::evaluate()");
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r1->read() != 0x1102) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r2->read() != 0x2201) {
		TEST_FAIL("pp::DirectField::write()");
	}
}

TEST(test_enum_field)
{
	/* two bindings with one reg each */
	pp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, pp::BITS16, 0x1111);
	pp::RegisterPtr r1 = new_pp_bound_register(bind1, 1, pp::BITS16);
	pp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, pp::BITS16, 0x2222);
	pp::RegisterPtr r2 = new_pp_bound_register(bind2, 1, pp::BITS16);

	/* test an enum field */
	pp::EnumDatatypePtr e1 = new_pp_enum_datatype();
	e1->add_value("one", 1);
	e1->add_value("two", 2);
	e1->add_value("three", 3);
	e1->add_value("correct", 0x2211);
	pp::DirectFieldPtr f1 = new_pp_direct_field(e1,
		pp::RegBits(r2, 7, 0) + pp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1->read() != 0x2211) {
		TEST_FAIL("pp::DirectField::read()");
	}
	if (f1->evaluate() != "correct") {
		TEST_FAIL("pp::DirectField::evaluate()");
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(1);
	if (f1->read() != 0x1) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r1->read() != 0x1101) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r2->read() != 0x2200) {
		TEST_FAIL("pp::DirectField::write()");
	}
}

TEST(test_bitmask_field)
{
	/* two bindings with one reg each */
	pp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, pp::BITS16, 0x1111);
	pp::RegisterPtr r1 = new_pp_bound_register(bind1, 1, pp::BITS16);
	pp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, pp::BITS16, 0x2222);
	pp::RegisterPtr r2 = new_pp_bound_register(bind2, 1, pp::BITS16);

	/* test a bitmask field */
	pp::BitmaskDatatypePtr b1 = new_pp_bitmask_datatype();
	b1->add_bit("zero", 0);
	b1->add_bit("one", 1);
	b1->add_bit("two", 2);
	b1->add_bit("three", 3);
	b1->add_bit("four", 4);
	b1->add_bit("nine", 9);
	b1->add_bit("thirteen", 13);
	pp::DirectFieldPtr f1 = new_pp_direct_field(b1,
		pp::RegBits(r2, 7, 0) + pp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1->read() != 0x2211) {
		TEST_FAIL("pp::DirectField::read()");
	}
	if (f1->evaluate() != "zero four nine thirteen") {
		TEST_FAIL("pp::DirectField::evaluate()");
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(0x207);
	if (f1->read() != 0x207) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r1->read() != 0x1107) {
		TEST_FAIL("pp::DirectField::write()");
	}
	if (r2->read() != 0x2202) {
		TEST_FAIL("pp::DirectField::write()");
	}
}

class TestProcs: public pp::RwProcs
{
    private:
	mutable pp::Value m_data;
    public:
	virtual pp::Value
	read() const
	{
		return m_data;
	}
	virtual void
	write(const pp::Value &value) const
	{
		m_data = value;
	}
};

TEST(test_proc_field)
{
	// without this, there is no "current context"
	pp::ScopePtr root = pp::initialize_device_tree();

	pp::DatatypePtr hex = new_pp_hex_datatype();
	pp::RwProcsPtr procs(new TestProcs);
	pp::ProcField f(hex, procs);
	f.write(0x12345678);
	if (f.read() != 0x12345678) {
		TEST_FAIL("pp::ProcField::write()");
	}
	if (f.evaluate() != "0x12345678") {
		TEST_FAIL("pp::ProcField::write()");
	}
}

TEST(test_constant_field)
{
	pp::DatatypePtr hex = new_pp_hex_datatype();
	pp::ConstantField f(hex, 0x12345678);
	if (f.read() != 0x12345678) {
		TEST_FAIL("pp::ConstantField::read()");
	}
	if (f.evaluate() != "0x12345678") {
		TEST_FAIL("pp::ConstantField::read()");
	}
	f.write(0);
	if (f.read() != 0x12345678) {
		TEST_FAIL("pp::ConstantField::write()");
	}
}
