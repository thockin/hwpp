#include "hwpp.h"
#include "util/printfxx.h"
#include "binding.h"
#include "register_types.h"

namespace hwpp {

/*
 * A magic binding which always reads a certain value.
 */
class ConstantValueBinding: public Binding
{
    public:
	explicit ConstantValueBinding(const Value &value): m_value(value)
	{
	}

	virtual ~ConstantValueBinding()
	{
	}

	virtual Value
	read(const Value &address, const BitWidth width) const
	{
		(void)address;
		(void)width;
		return m_value;
	}

	virtual void
	write(const Value &address, const BitWidth width,
		const Value &value) const
	{
		(void)address;
		(void)width;
		(void)value;
		/* do nothing */
	}

	virtual string
	to_string() const
	{
		return sprintfxx("constant<0x%x>", m_value);
	}

    private:
	Value m_value;
};

ConstRegisterPtr magic_zeros(new BoundRegister(
		BindingPtr(new ConstantValueBinding(0)),
		0x0, BIT_WIDTH_MAX));
ConstRegisterPtr magic_ones(new BoundRegister(
		BindingPtr(new ConstantValueBinding(MASK(BIT_WIDTH_MAX))),
		0x0, BIT_WIDTH_MAX));

}  // namespace hwpp
