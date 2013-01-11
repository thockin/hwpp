/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_TESTS_TEST_BINDING_H__
#define HWPP_TESTS_TEST_BINDING_H__

#include "binding.h"
#include "driver.h"
#include "hwpp.h"
#
/* define a test binding */
class TestBinding: public hwpp::Binding
{
    public:
	explicit TestBinding(): m_data(hwpp::MASK(hwpp::BIT_WIDTH_MAX)) {}
	virtual ~TestBinding() {}

	virtual hwpp::Value
	read(const hwpp::Value &address, const hwpp::BitWidth width) const
	{
		if (address == 0x12345678)
			throw hwpp::Driver::IoError("test binding read");
		return (m_data & hwpp::MASK(width));
	}

	virtual void
	write(const hwpp::Value &address, const hwpp::BitWidth width,
	    const hwpp::Value &value) const
	{
		if (address == 0x12345678)
			throw hwpp::Driver::IoError("test binding write");
		m_data ^= (m_data & hwpp::MASK(width));
		m_data |= value & hwpp::MASK(width);
	}

	virtual string
	to_string() const
	{
		return "test";
	}

    private:
	mutable hwpp::Value m_data;
};

#define new_test_binding(...) hwpp::BindingPtr(new TestBinding(__VA_ARGS__))

#endif // HWPP_TESTS_TEST_BINDING_H__
