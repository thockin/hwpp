/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_REGISTER_H__
#define PP_PP_REGISTER_H__

#include "pp.h"
#include "pp_dirent.h"
#include "pp_binding.h"

#include <stdexcept>

/*
 * pp_register - a register definition.
 *
 * Constructors:
 *	(const pp_binding_ptr &binding, const pp_regaddr address,
 *	    const pp_bitwidth width)
 *
 * Notes:
 */
class pp_register: public pp_dirent
{
    public:
	explicit pp_register(const pp_binding_ptr &binding,
	    const pp_regaddr address, const pp_bitwidth width)
	    : pp_dirent(PP_DIRENT_REGISTER),
	      m_binding(binding), m_address(address), m_width(width) {}
	virtual ~pp_register() {}

	/*
	 * pp_register::address()
	 *
	 * Get the address of this register.
	 */
	pp_regaddr
	address() const
	{
		return m_address;
	}

	/*
	 * pp_register::width()
	 *
	 * Get the access width of this register.
	 */
	pp_bitwidth
	width() const
	{
		return m_width;
	}

	/*
	 * pp_register::read()
	 *
	 * Read the value of this register.
	 *
	 * Throws: pp_binding_error
	 */
	pp_value
	read() const
	{
		return m_binding->read(m_address, m_width);
	}

	/*
	 * pp_register::write(value)
	 *
	 * Write a value to this register.
	 *
	 * Throws: pp_binding_error
	 */
	void
	write(const pp_value value) const
	{
		m_binding->write(m_address, m_width, value);
	}

    private:
	pp_binding_ptr m_binding;
	pp_regaddr m_address;
	pp_bitwidth m_width;
};
typedef boost::shared_ptr<pp_register> pp_register_ptr;

inline pp_register_ptr
pp_register_from_dirent(pp_dirent_ptr dirent)
{
	if (dirent->dirent_type() != PP_DIRENT_REGISTER) {
		throw std::runtime_error(
		    "non-register dirent used as register");
	}
	return boost::static_pointer_cast<pp_register>(dirent);
}

#define new_pp_register(...) pp_register_ptr(new pp_register(__VA_ARGS__))

#endif // PP_PP_REGISTER_H__
