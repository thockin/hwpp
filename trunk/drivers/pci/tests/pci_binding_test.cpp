#include "pci_binding.h"
#include "pp.h"
#include "pp_test.h"
using namespace std;

class fake_pci_io: public pci_io
{
    public:
    	//FIXME: pci_io should be abstract
	explicit fake_pci_io(): pci_io(0,0,0,0), m_data(0) {}
	virtual ~fake_pci_io() {}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		if (address != 0) {
			throw pp_binding_error("bad register address");
		}
		return m_data & PP_MASK(width);
	}

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const
	{
		if (address != 0) {
			throw pp_binding_error("bad register address");
		}
		m_data &= ~PP_MASK(width);
		m_data |= (value & PP_MASK(width));
	}

    private:
	mutable pp_value m_data;
};

int
test_pci_binding()
{
	int ret = 0;

	/* test the read() method */
	pp_binding_ptr binding = new_pci_binding(pci_address(0, 0x18, 0),
	    new fake_pci_io());
	if (binding->read(0, BITS8) != 0x00) {
		PP_TEST_ERROR("pp_binding::read()");
		ret++;
	}

	/* test the write() method */
	binding->write(0, BITS8, 0x5a);
	if (binding->read(0, BITS8) != 0x5a) {
		PP_TEST_ERROR("pp_binding::write()");
		ret++;
	}

	return ret;
}

int
main()
{
	int r;

	r = test_pci_binding();
	if (r) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
