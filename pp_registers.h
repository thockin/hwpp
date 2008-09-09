/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_REGISTERS_H__
#define PP_PP_REGISTERS_H__

#include "pp.h"
#include "pp_register.h"
#include "pp_binding.h"
#include "pp_rwprocs.h"
#include "pp_context.h"
#include "runtime.h"


/*
 * pp_bound_register - a register definition.
 *
 * Notes:
 */
class pp_bound_register: public pp_register
{
    private:
	pp_binding_const_ptr m_binding;
	pp_value m_address;

    public:
	pp_bound_register(const pp_binding_const_ptr &binding,
	    const pp_value &address, const pp_bitwidth width)
	    : pp_register(width), m_binding(binding), m_address(address)
	{
	}

	/*
	 * Describe this register.
	 */
	virtual string
	describe() const
	{
		return to_string(m_address);
	}

	/*
	 * Read the value of this register.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual pp_value
	read() const
	{
		return m_binding->read(m_address, m_width);
	}

	/*
	 * Write a value to this register.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual void
	write(const pp_value &value) const
	{
		m_binding->write(m_address, m_width, value);
	}
};

#define new_pp_bound_register(...) \
		pp_register_ptr(new pp_bound_register(__VA_ARGS__))

/*
 * pp_proc_register - a procedure-register definition.
 *
 * Notes:
 */
class pp_proc_register: public pp_register
{
    private:
	pp_rwprocs_ptr m_access;
	pp_context m_context;

    public:
	pp_proc_register(const pp_rwprocs_ptr &access, const pp_bitwidth width)
	    : pp_register(width), m_access(access),
	      m_context(pp_context_snapshot())
	{
	}

	/*
	 * Describe this register.
	 */
	virtual string
	describe() const
	{
		return "procs";
	}

	/*
	 * Read the value of this register.
	 *
	 * FIXME: need better throws!
	 * Throws: pp_driver::io_error
	 */
	virtual pp_value
	read() const
	{
		pp_context_push(m_context);
		pp_value ret = m_access->read() & PP_MASK(width());
		pp_context_pop();
		return ret;
	}

	/*
	 * Write a value to this register.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual void
	write(const pp_value &value) const
	{
		pp_context_push(m_context);
		m_access->write(value & PP_MASK(width()));
		pp_context_pop();
	}
};

#define new_pp_proc_register(...) \
		pp_register_ptr(new pp_proc_register(__VA_ARGS__))

#endif // PP_PP_REGISTERS_H__
