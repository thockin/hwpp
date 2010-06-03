#include "pp.h"
#include "register_types.h"
#include "runtime.h"
#include "rwprocs.h"
#include "test_binding.h"
#include "util/test.h"

TEST(test_pp_bound_register)
{
	/* test the binding constructor */
	pp::BindingPtr bind = new_test_binding();

	/* set up test registers */
	pp::BoundRegister r1(bind, 1, pp::BITS8);
	pp::BoundRegister r2(bind, 2, pp::BITS16);
	pp::BoundRegister r3(bind, 3, pp::BITS32);
	pp::BoundRegister r4(bind, 4, pp::BITS64);
	pp::BoundRegister r5(bind, 5, pp::BITS128);

	/* test the read() method */
	if (r1.read() != pp::MASK(pp::BITS8)) {
		TEST_FAIL("pp::BoundRegister::read()");
	}
	if (r2.read() != pp::MASK(pp::BITS16)) {
		TEST_FAIL("pp::BoundRegister::read()");
	}
	if (r3.read() != pp::MASK(pp::BITS32)) {
		TEST_FAIL("pp::BoundRegister::read()");
	}
	if (r4.read() != pp::MASK(pp::BITS64)) {
		TEST_FAIL("pp::BoundRegister::read()");
	}
	if (r5.read() != pp::MASK(pp::BITS128)) {
		TEST_FAIL("pp::BoundRegister::read()");
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r4.write(pp::Value("0x4444444444444444"));
	if (r4.read() != pp::Value("0x4444444444444444")) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r5.write(pp::Value("0x55555555555555555555555555555555"));
	if (r5.read() != pp::Value("0x55555555555555555555555555555555")) {
		TEST_FAIL("pp::BoundRegister::write()");
	}

	/* test write() by writing too many bits */
	r1.write(0xff11);
	if (r1.read() != 0x11) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r2.write(0xff2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r3.write(0xff33333333ULL);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r4.write(pp::Value("0xff4444444444444444"));
	if (r4.read() != pp::Value("0x4444444444444444")) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
	r5.write(pp::Value("0xff55555555555555555555555555555555"));
	if (r5.read() != pp::Value("0x55555555555555555555555555555555")) {
		TEST_FAIL("pp::BoundRegister::write()");
	}
}

class TestProcs: public pp::RwProcs
{
    private:
	mutable pp::Value m_data;
    public:
	TestProcs(): m_data(pp::MASK(pp::BITS128))
	{
	}
	pp::Value
	read() const
	{
		return m_data;
	}
	void
	write(const pp::Value &value) const
	{
		m_data = value;
	}
};

TEST(test_pp_proc_register)
{
	pp::RwProcsPtr procs(new TestProcs);

	/* set up test registers */
	pp::ProcRegister r1(procs, pp::BITS8, pp::global_runtime());
	pp::ProcRegister r2(procs, pp::BITS16, pp::global_runtime());
	pp::ProcRegister r3(procs, pp::BITS32, pp::global_runtime());
	pp::ProcRegister r4(procs, pp::BITS64, pp::global_runtime());
	pp::ProcRegister r5(procs, pp::BITS128, pp::global_runtime());

	/* test the read() method */
	if (r1.read() != pp::MASK(pp::BITS8)) {
		TEST_FAIL("pp::ProcRegister::read()");
	}
	if (r2.read() != pp::MASK(pp::BITS16)) {
		TEST_FAIL("pp::ProcRegister::read()");
	}
	if (r3.read() != pp::MASK(pp::BITS32)) {
		TEST_FAIL("pp::ProcRegister::read()");
	}
	if (r4.read() != pp::MASK(pp::BITS64)) {
		TEST_FAIL("pp::ProcRegister::read()");
	}
	if (r5.read() != pp::MASK(pp::BITS128)) {
		TEST_FAIL("pp::ProcRegister::read()");
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r4.write(pp::Value("0x4444444444444444"));
	if (r4.read() != pp::Value("0x4444444444444444")) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r5.write(pp::Value("0x555555555555555555555555"));
	if (r5.read() != pp::Value("0x555555555555555555555555")) {
		TEST_FAIL("pp::ProcRegister::write()");
	}

	/* test write() by writing too many bits */
	r1.write(0xff11);
	if (r1.read() != 0x11) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r2.write(0xff2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r3.write(0xff33333333ULL);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
	r5.write(pp::Value("0xff55555555555555555555555555555555"));
	if (r5.read() != pp::Value("0x55555555555555555555555555555555")) {
		TEST_FAIL("pp::ProcRegister::write()");
	}
}
