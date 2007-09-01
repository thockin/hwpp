#include "io_driver.h"
#include "io_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

class fake_io_io: public io_io
{
    public:
	explicit fake_io_io(): m_data(0) {}
	virtual ~fake_io_io() {}

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
test_io_binding()
{
	int ret = 0;

	/* test the read() method */
	pp_binding_ptr binding = new_io_binding(io_address(0x1234, 0x5678),
	    new fake_io_io());
	if (binding->read(0, BITS8) != 0x00) {
		PP_TEST_ERROR("io_binding::read()");
		ret++;
	}

	/* test the write() method */
	binding->write(0, BITS8, 0x5a);
	if (binding->read(0, BITS8) != 0x5a) {
		PP_TEST_ERROR("io_binding::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_io_binding();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
