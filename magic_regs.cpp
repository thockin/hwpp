#include "pp.h"
#include "pp_binding.h"
#include "pp_register.h"

#include <iostream>
/*
 * A magic binding which always reads a certain value.
 */
class magic_binding: public pp_binding
{
    public:
	explicit magic_binding(const pp_value &value): m_value(value)
	{
	}

	virtual ~magic_binding()
	{
	}

	virtual pp_value
	read(const pp_regaddr &address, const pp_bitwidth width) const
	{
		return m_value;
	}

	virtual void
	write(const pp_regaddr &address, const pp_bitwidth width,
		const pp_value &value) const
	{
		/* do nothing */
	}

	virtual string
	to_string() const
	{
		return ::to_string(boost::format("constant<0x%x>")%m_value);
	}

    private:
	pp_value m_value;
};

pp_register *magic_zeros = new pp_register(
		new magic_binding(0), 0x0, BITS64);
pp_register *magic_ones = new pp_register(
		new magic_binding(PP_MASK(PP_BITWIDTH_MAX)), 0x0, BITS64);
