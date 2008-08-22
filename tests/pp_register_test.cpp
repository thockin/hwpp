#include "pp_registers.h"
#include "pp_rwprocs.h"
#include "pp.h"
#include "pp_test.h"
#include "test_binding.h"
using namespace std;

void
test_pp_bound_register()
{
	/* test the binding constructor */
	pp_binding_ptr bind = new_test_binding();

	/* set up test registers */
	pp_bound_register r1(bind, 1, BITS8);
	pp_bound_register r2(bind, 2, BITS16);
	pp_bound_register r3(bind, 3, BITS32);
	pp_bound_register r4(bind, 4, BITS64);
	pp_bound_register r5(bind, 5, BITS128);

	/* test the read() method */
	if (r1.read() != PP_MASK(BITS8)) {
		TEST_ERROR("pp_bound_register::read()");
	}
	if (r2.read() != PP_MASK(BITS16)) {
		TEST_ERROR("pp_bound_register::read()");
	}
	if (r3.read() != PP_MASK(BITS32)) {
		TEST_ERROR("pp_bound_register::read()");
	}
	if (r4.read() != PP_MASK(BITS64)) {
		TEST_ERROR("pp_bound_register::read()");
	}
	if (r5.read() != PP_MASK(BITS128)) {
		TEST_ERROR("pp_bound_register::read()");
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r4.write(pp_value("0x4444444444444444"));
	if (r4.read() != pp_value("0x4444444444444444")) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r5.write(pp_value("0x55555555555555555555555555555555"));
	if (r5.read() != pp_value("0x55555555555555555555555555555555")) {
		TEST_ERROR("pp_bound_register::write()");
	}

	/* test write() by writing too many bits */
	r1.write(0xff11);
	if (r1.read() != 0x11) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r2.write(0xff2222);
	if (r2.read() != 0x2222) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r3.write(0xff33333333ULL);
	if (r3.read() != 0x33333333) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r4.write(pp_value("0xff4444444444444444"));
	if (r4.read() != pp_value("0x4444444444444444")) {
		TEST_ERROR("pp_bound_register::write()");
	}
	r5.write(pp_value("0xff55555555555555555555555555555555"));
	if (r5.read() != pp_value("0x55555555555555555555555555555555")) {
		TEST_ERROR("pp_bound_register::write()");
	}
}

class test_procs: public pp_rwprocs
{
    private:
	mutable pp_value m_data;
    public:
	test_procs(): m_data(PP_MASK(BITS128))
	{
	}
	pp_value
	read() const
	{
		return m_data;
	}
	void
	write(const pp_value &value) const
	{
		m_data = value;
	}
};

void
test_pp_proc_register()
{
	pp_rwprocs_ptr procs(new test_procs);

	/* set up test registers */
	pp_proc_register r1(procs, BITS8);
	pp_proc_register r2(procs, BITS16);
	pp_proc_register r3(procs, BITS32);
	pp_proc_register r4(procs, BITS64);
	pp_proc_register r5(procs, BITS128);

	/* test the read() method */
	if (r1.read() != PP_MASK(BITS8)) {
		TEST_ERROR("pp_proc_register::read()");
	}
	if (r2.read() != PP_MASK(BITS16)) {
		TEST_ERROR("pp_proc_register::read()");
	}
	if (r3.read() != PP_MASK(BITS32)) {
		TEST_ERROR("pp_proc_register::read()");
	}
	if (r4.read() != PP_MASK(BITS64)) {
		TEST_ERROR("pp_proc_register::read()");
	}
	if (r5.read() != PP_MASK(BITS128)) {
		TEST_ERROR("pp_proc_register::read()");
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r4.write(pp_value("0x4444444444444444"));
	if (r4.read() != pp_value("0x4444444444444444")) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r5.write(pp_value("0x555555555555555555555555"));
	if (r5.read() != pp_value("0x555555555555555555555555")) {
		TEST_ERROR("pp_proc_register::write()");
	}

	/* test write() by writing too many bits */
	r1.write(0xff11);
	if (r1.read() != 0x11) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r2.write(0xff2222);
	if (r2.read() != 0x2222) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r3.write(0xff33333333ULL);
	if (r3.read() != 0x33333333) {
		TEST_ERROR("pp_proc_register::write()");
	}
	r5.write(pp_value("0xff55555555555555555555555555555555"));
	if (r5.read() != pp_value("0x55555555555555555555555555555555")) {
		TEST_ERROR("pp_proc_register::write()");
	}
}

TEST_LIST(
	TEST(test_pp_bound_register),
	TEST(test_pp_proc_register),
);
