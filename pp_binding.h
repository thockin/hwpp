/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_BINDING_H__
#define PP_PP_BINDING_H__

#include "pp.h"
#include <stdexcept>
#include <ostream>

/*
 * pp_binding - abstract base class for bound register spaces.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class pp_binding
{
    public:
	pp_binding() {}
	virtual ~pp_binding() {}

	/*
	 * pp_binding::read(address, width)
	 *
	 * Read from a register in this binding.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual pp_value
	read(const pp_value &address, const pp_bitwidth width) const = 0;

	/*
	 * pp_binding::write(address, width, value)
	 *
	 * Write to a register in this binding.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual void
	write(const pp_value &address, const pp_bitwidth width,
	    const pp_value &value) const = 0;

	/*
	 * pp_binding::to_string()
	 *
	 * Return a string representing this binding.
	 */
	virtual string
	to_string() const = 0;
};
typedef boost::shared_ptr<pp_binding> pp_binding_ptr;
typedef boost::shared_ptr<const pp_binding> pp_const_binding_ptr;

inline std::ostream &
operator<<(std::ostream& o, const pp_binding &binding)
{
	return o << binding.to_string();
}

/*
 * simple_binding - a simple binding
 *
 * Constructors:
 *	(Taddress address)
 *
 * Notes:
 */
template<class Tio, class Taddress>
class simple_binding: public pp_binding
{
    public:
	explicit simple_binding(Taddress address)
	    : m_io(address)
	{
	}

	virtual pp_value
	read(const pp_value &address, const pp_bitwidth width) const
	{
		return m_io.read(address, width);
	}

	virtual void
	write(const pp_value &address, const pp_bitwidth width,
	    const pp_value &value) const
	{
		return m_io.write(address, width, value);
	}

	virtual string
	to_string() const
	{
		return ::to_string(m_io.address());
	}

	const Taddress &
	address()
	{
		return m_io.address();
	}

    private:
	Tio m_io;
};

#endif // PP_PP_BINDING_H__
