/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_TESTS_TEST_BINDING_H__
#define PP_TESTS_TEST_BINDING_H__

#include "binding.h"
#include "driver.h"
#include "pp.h"
#
/* define a test binding */
class TestBinding: public pp::Binding
{
    public:
	explicit TestBinding(): m_data(pp::MASK(pp::BIT_WIDTH_MAX)) {}
	virtual ~TestBinding() {}

	virtual pp::Value
	read(const pp::Value &address, const pp::BitWidth width) const
	{
		if (address == 0x12345678)
			throw pp::Driver::IoError("test binding read");
		return (m_data & pp::MASK(width));
	}

	virtual void
	write(const pp::Value &address, const pp::BitWidth width,
	    const pp::Value &value) const
	{
		if (address == 0x12345678)
			throw pp::Driver::IoError("test binding write");
		m_data ^= (m_data & pp::MASK(width));
		m_data |= value & pp::MASK(width);
	}

	virtual string
	to_string() const
	{
		return "test";
	}

    private:
	mutable pp::Value m_data;
};

#define new_test_binding(...) pp::BindingPtr(new TestBinding(__VA_ARGS__))

#endif // PP_TESTS_TEST_BINDING_H__
