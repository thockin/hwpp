#include "hwpp.h"
#include "register_types.h"
#include "runtime.h"
#include "rwprocs.h"
#include "test_binding.h"
#include "util/test.h"

TEST(test_hwpp_bound_register)
{
	/* test the binding constructor */
	hwpp::BindingPtr bind = new_test_binding();

	/* set up test registers */
	hwpp::BoundRegister r1(bind, 1, hwpp::BITS8);
	hwpp::BoundRegister r2(bind, 2, hwpp::BITS16);
	hwpp::BoundRegister r3(bind, 3, hwpp::BITS32);
	hwpp::BoundRegister r4(bind, 4, hwpp::BITS64);
	hwpp::BoundRegister r5(bind, 5, hwpp::BITS128);

	/* test the read() method */
	if (r1.read() != hwpp::MASK(hwpp::BITS8)) {
		TEST_FAIL("hwpp::BoundRegister::read()");
	}
	if (r2.read() != hwpp::MASK(hwpp::BITS16)) {
		TEST_FAIL("hwpp::BoundRegister::read()");
	}
	if (r3.read() != hwpp::MASK(hwpp::BITS32)) {
		TEST_FAIL("hwpp::BoundRegister::read()");
	}
	if (r4.read() != hwpp::MASK(hwpp::BITS64)) {
		TEST_FAIL("hwpp::BoundRegister::read()");
	}
	if (r5.read() != hwpp::MASK(hwpp::BITS128)) {
		TEST_FAIL("hwpp::BoundRegister::read()");
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r4.write(hwpp::Value("0x4444444444444444"));
	if (r4.read() != hwpp::Value("0x4444444444444444")) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r5.write(hwpp::Value("0x55555555555555555555555555555555"));
	if (r5.read() != hwpp::Value("0x55555555555555555555555555555555")) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}

	/* test write() by writing too many bits */
	r1.write(0xff11);
	if (r1.read() != 0x11) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r2.write(0xff2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r3.write(0xff33333333ULL);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r4.write(hwpp::Value("0xff4444444444444444"));
	if (r4.read() != hwpp::Value("0x4444444444444444")) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
	r5.write(hwpp::Value("0xff55555555555555555555555555555555"));
	if (r5.read() != hwpp::Value("0x55555555555555555555555555555555")) {
		TEST_FAIL("hwpp::BoundRegister::write()");
	}
}

class TestProcs: public hwpp::RwProcs
{
    private:
	mutable hwpp::Value m_data;
    public:
	TestProcs(): m_data(hwpp::MASK(hwpp::BITS128))
	{
	}
	hwpp::Value
	read() const
	{
		return m_data;
	}
	void
	write(const hwpp::Value &value) const
	{
		m_data = value;
	}
};

TEST(test_hwpp_proc_register)
{
	hwpp::RwProcsPtr procs(new TestProcs);

	/* set up test registers */
	hwpp::ProcRegister r1(procs, hwpp::BITS8, hwpp::global_runtime());
	hwpp::ProcRegister r2(procs, hwpp::BITS16, hwpp::global_runtime());
	hwpp::ProcRegister r3(procs, hwpp::BITS32, hwpp::global_runtime());
	hwpp::ProcRegister r4(procs, hwpp::BITS64, hwpp::global_runtime());
	hwpp::ProcRegister r5(procs, hwpp::BITS128, hwpp::global_runtime());

	/* test the read() method */
	if (r1.read() != hwpp::MASK(hwpp::BITS8)) {
		TEST_FAIL("hwpp::ProcRegister::read()");
	}
	if (r2.read() != hwpp::MASK(hwpp::BITS16)) {
		TEST_FAIL("hwpp::ProcRegister::read()");
	}
	if (r3.read() != hwpp::MASK(hwpp::BITS32)) {
		TEST_FAIL("hwpp::ProcRegister::read()");
	}
	if (r4.read() != hwpp::MASK(hwpp::BITS64)) {
		TEST_FAIL("hwpp::ProcRegister::read()");
	}
	if (r5.read() != hwpp::MASK(hwpp::BITS128)) {
		TEST_FAIL("hwpp::ProcRegister::read()");
	}

	/* test the write() method */
	r1.write(0x11);
	if (r1.read() != 0x11) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r2.write(0x2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r3.write(0x33333333);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r4.write(hwpp::Value("0x4444444444444444"));
	if (r4.read() != hwpp::Value("0x4444444444444444")) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r5.write(hwpp::Value("0x555555555555555555555555"));
	if (r5.read() != hwpp::Value("0x555555555555555555555555")) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}

	/* test write() by writing too many bits */
	r1.write(0xff11);
	if (r1.read() != 0x11) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r2.write(0xff2222);
	if (r2.read() != 0x2222) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r3.write(0xff33333333ULL);
	if (r3.read() != 0x33333333) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
	r5.write(hwpp::Value("0xff55555555555555555555555555555555"));
	if (r5.read() != hwpp::Value("0x55555555555555555555555555555555")) {
		TEST_FAIL("hwpp::ProcRegister::write()");
	}
}
