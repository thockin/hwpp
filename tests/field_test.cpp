#include "hwpp.h"
#include "device_init.h"
#include "field_types.h"
#include "datatype_types.h"
#include "register_types.h"
#include "regbits.h"
#include "test_binding.h"
#include "util/test.h"

//FIXME: test lookup()
//FIXME: test compare()

TEST(test_int_field)
{
	/* two bindings with one reg each */
	hwpp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, hwpp::BITS16, 0x1111);
	hwpp::RegisterPtr r1 = new_hwpp_bound_register(bind1, 1, hwpp::BITS16);
	hwpp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, hwpp::BITS16, 0x2222);
	hwpp::RegisterPtr r2 = new_hwpp_bound_register(bind2, 1, hwpp::BITS16);

	hwpp::DatatypePtr integer = new_hwpp_int_datatype();
	hwpp::DirectField f1(integer,
		hwpp::RegBits(r2, 7, 0) + hwpp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1.read() != 8721) {
		TEST_FAIL("hwpp::DirectField::read()");
	}
	if (f1.evaluate() != "8721") {
		TEST_FAIL("hwpp::DirectField::evaluate()");
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r1->read() != 0x1102) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r2->read() != 0x2201) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
}

TEST(test_hex_field)
{
	/* two bindings with one reg each */
	hwpp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, hwpp::BITS16, 0x1111);
	hwpp::RegisterPtr r1 = new_hwpp_bound_register(bind1, 1, hwpp::BITS16);
	hwpp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, hwpp::BITS16, 0x2222);
	hwpp::RegisterPtr r2 = new_hwpp_bound_register(bind2, 1, hwpp::BITS16);

	/* test a hex16 field */
	hwpp::DatatypePtr hex16 = new_hwpp_hex_datatype(hwpp::BITS16);
	hwpp::DirectField f1(hex16,
		hwpp::RegBits(r2, 7, 0) + hwpp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1.read() != 0x2211) {
		TEST_FAIL("hwpp::DirectField::read()");
	}
	if (f1.evaluate() != "0x2211") {
		TEST_FAIL("hwpp::DirectField::evaluate()");
	}

	/* test write */
	f1.write(0x0102);
	if (f1.read() != 0x0102) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r1->read() != 0x1102) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r2->read() != 0x2201) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
}

TEST(test_enum_field)
{
	/* two bindings with one reg each */
	hwpp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, hwpp::BITS16, 0x1111);
	hwpp::RegisterPtr r1 = new_hwpp_bound_register(bind1, 1, hwpp::BITS16);
	hwpp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, hwpp::BITS16, 0x2222);
	hwpp::RegisterPtr r2 = new_hwpp_bound_register(bind2, 1, hwpp::BITS16);

	/* test an enum field */
	hwpp::EnumDatatypePtr e1 = new_hwpp_enum_datatype();
	e1->add_value("one", 1);
	e1->add_value("two", 2);
	e1->add_value("three", 3);
	e1->add_value("correct", 0x2211);
	hwpp::DirectFieldPtr f1 = new_hwpp_direct_field(e1,
		hwpp::RegBits(r2, 7, 0) + hwpp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1->read() != 0x2211) {
		TEST_FAIL("hwpp::DirectField::read()");
	}
	if (f1->evaluate() != "correct") {
		TEST_FAIL("hwpp::DirectField::evaluate()");
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(1);
	if (f1->read() != 0x1) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r1->read() != 0x1101) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r2->read() != 0x2200) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
}

TEST(test_bitmask_field)
{
	/* two bindings with one reg each */
	hwpp::BindingPtr bind1 = new_test_binding();
	bind1->write(0, hwpp::BITS16, 0x1111);
	hwpp::RegisterPtr r1 = new_hwpp_bound_register(bind1, 1, hwpp::BITS16);
	hwpp::BindingPtr bind2 = new_test_binding();
	bind2->write(0, hwpp::BITS16, 0x2222);
	hwpp::RegisterPtr r2 = new_hwpp_bound_register(bind2, 1, hwpp::BITS16);

	/* test a bitmask field */
	hwpp::BitmaskDatatypePtr b1 = new_hwpp_bitmask_datatype();
	b1->add_bit("zero", 0);
	b1->add_bit("one", 1);
	b1->add_bit("two", 2);
	b1->add_bit("three", 3);
	b1->add_bit("four", 4);
	b1->add_bit("nine", 9);
	b1->add_bit("thirteen", 13);
	hwpp::DirectFieldPtr f1 = new_hwpp_direct_field(b1,
		hwpp::RegBits(r2, 7, 0) + hwpp::RegBits(r1, 7, 0));

	/* test read() */
	if (f1->read() != 0x2211) {
		TEST_FAIL("hwpp::DirectField::read()");
	}
	if (f1->evaluate() != "zero four nine thirteen") {
		TEST_FAIL("hwpp::DirectField::evaluate()");
	}

	/* test write */
	//FIXME: write by string? or lookup by string?
	f1->write(0x207);
	if (f1->read() != 0x207) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r1->read() != 0x1107) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
	if (r2->read() != 0x2202) {
		TEST_FAIL("hwpp::DirectField::write()");
	}
}

class TestProcs: public hwpp::RwProcs
{
    private:
	mutable hwpp::Value m_data;
    public:
	virtual hwpp::Value
	read() const
	{
		return m_data;
	}
	virtual void
	write(const hwpp::Value &value) const
	{
		m_data = value;
	}
};

TEST(test_proc_field)
{
	hwpp::DatatypePtr hex = new_hwpp_hex_datatype();
	hwpp::RwProcsPtr procs(new TestProcs);
	hwpp::ProcField f(hex, procs);
	f.write(0x12345678);
	if (f.read() != 0x12345678) {
		TEST_FAIL("hwpp::ProcField::write()");
	}
	if (f.evaluate() != "0x12345678") {
		TEST_FAIL("hwpp::ProcField::write()");
	}
}

TEST(test_constant_field)
{
	hwpp::DatatypePtr hex = new_hwpp_hex_datatype();
	hwpp::ConstantField f(hex, 0x12345678);
	if (f.read() != 0x12345678) {
		TEST_FAIL("hwpp::ConstantField::read()");
	}
	if (f.evaluate() != "0x12345678") {
		TEST_FAIL("hwpp::ConstantField::read()");
	}
	f.write(0);
	if (f.read() != 0x12345678) {
		TEST_FAIL("hwpp::ConstantField::write()");
	}
}
