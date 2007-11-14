#include "msr_driver.h"
#include "msr_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

class fake_msr_io: public msr_io
{
    public:
	explicit fake_msr_io(): m_data(0) {}
	virtual ~fake_msr_io() {}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		if (address != 0) {
			throw pp_driver_io_error("bad register address");
		}
		return m_data & PP_MASK(width);
	}

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value)
	{
		if (address != 0) {
			throw pp_driver_io_error("bad register address");
		}
		m_data &= ~PP_MASK(width);
		m_data |= (value & PP_MASK(width));
	}

    private:
	mutable pp_value m_data;
};

int
test_msr_binding()
{
	int ret = 0;

	/* test the read() method */
	pp_binding_ptr binding = new_msr_binding(msr_address(0),
	    new fake_msr_io());
	if (binding->read(0, BITS64) != 0x00) {
		PP_TEST_ERROR("msr_binding::read()");
		ret++;
	}

	/* test the write() method */
	binding->write(0, BITS64, 0x5a);
	if (binding->read(0, BITS64) != 0x5a) {
		PP_TEST_ERROR("msr_binding::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_msr_binding();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
