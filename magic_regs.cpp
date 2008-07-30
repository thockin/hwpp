#include "pp.h"
#include "pp_binding.h"
#include "pp_registers.h"

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
	read(const pp_value &address, const pp_bitwidth width) const
	{
		(void)address;
		(void)width;
		return m_value;
	}

	virtual void
	write(const pp_value &address, const pp_bitwidth width,
		const pp_value &value) const
	{
		(void)address;
		(void)width;
		(void)value;
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

pp_register_const_ptr magic_zeros(new pp_bound_register(
		new magic_binding(0), 0x0, PP_BITWIDTH_MAX));
pp_register_const_ptr magic_ones(new pp_bound_register(
		new magic_binding(PP_MASK(PP_BITWIDTH_MAX)),
		0x0, PP_BITWIDTH_MAX));
