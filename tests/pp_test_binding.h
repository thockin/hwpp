/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TESTS_PP_TEST_BINDING_H__
#define PP_TESTS_PP_TEST_BINDING_H__

#include "pp_binding.h"
#include "pp.h"

/* define a test binding */
class pp_test_binding: public pp_binding {
    public:
	explicit pp_test_binding(): m_data(-1ULL) {}
	virtual ~pp_test_binding() {}

	pp_value read(const pp_regaddr address,
	    const pp_bitwidth width) const {
		if (address == 0x12345678)
			throw pp_binding_error("test binding read");
		return (m_data & PP_MASK(width));
	}
	void write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const {
		if (address == 0x12345678)
			throw pp_binding_error("test binding write");
		m_data &= ~PP_MASK(width);
		m_data |= value & PP_MASK(width);
	}

    private:
	mutable pp_value m_data;
};

#endif // PP_TESTS_PP_TEST_BINDING_H__
