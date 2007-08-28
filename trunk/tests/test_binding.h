/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TESTS_TEST_BINDING_H__
#define PP_TESTS_TEST_BINDING_H__

#include "pp_binding.h"
#include "pp_driver.h"
#include "pp.h"

/* define a test binding */
class test_binding: public pp_binding
{
    public:
	explicit test_binding(): m_data(-1ULL) {}
	virtual ~test_binding() {}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		if (address == 0x12345678)
			throw pp_driver_io_error("test binding read");
		return (m_data & PP_MASK(width));
	}

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const
	{
		if (address == 0x12345678)
			throw pp_driver_io_error("test binding write");
		m_data &= ~PP_MASK(width);
		m_data |= value & PP_MASK(width);
	}

	virtual string
	to_string() const
	{
		return "test";
	}

    private:
	mutable pp_value m_data;
};

#define new_test_binding(...) pp_binding_ptr(new test_binding(__VA_ARGS__))

#endif // PP_TESTS_TEST_BINDING_H__
