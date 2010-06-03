/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_REGISTER_TYPES_H__
#define PP_REGISTER_TYPES_H__

#include "pp.h"
#include "register.h"
#include "binding.h"
#include "rwprocs.h"
#include "context.h"
#include "runtime.h"

namespace pp {

/*
 * BoundRegister - a register definition.
 *
 * Notes:
 */
class BoundRegister: public Register
{
    private:
	ConstBindingPtr m_binding;
	Value m_address;

    public:
	BoundRegister(const ConstBindingPtr &binding,
	    const Value &address, const BitWidth width)
	    : Register(width), m_binding(binding), m_address(address)
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
	 * Throws: Driver::IoError
	 */
	virtual Value
	read() const
	{
		return m_binding->read(m_address, m_width);
	}

	/*
	 * Write a value to this register.
	 *
	 * Throws: Driver::IoError
	 */
	virtual void
	write(const Value &value) const
	{
		m_binding->write(m_address, m_width, value);
	}
};

#define new_pp_bound_register(...) \
		::pp::RegisterPtr(new ::pp::BoundRegister(__VA_ARGS__))

/*
 * ProcRegister - a procedure-register definition.
 *
 * Notes:
 */
class ProcRegister: public Register
{
    private:
	RwProcsPtr m_access;
	Runtime *m_runtime;
	ContextPtr m_context;

    public:
	ProcRegister(const RwProcsPtr &access, const BitWidth width,
	             Runtime *runtime)
	    : Register(width), m_access(access), m_runtime(runtime),
	      m_context(runtime->context_snapshot())
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
	 * Throws: Driver::IoError
	 */
	virtual Value
	read() const
	{
		m_runtime->context_push(m_context);
		Value ret = m_access->read() & MASK(width());
		m_runtime->context_pop();
		return ret;
	}

	/*
	 * Write a value to this register.
	 *
	 * Throws: Driver::IoError
	 */
	virtual void
	write(const Value &value) const
	{
		m_runtime->context_push(m_context);
		m_access->write(value & MASK(width()));
		m_runtime->context_pop();
	}
};

#define new_pp_proc_register(...) \
		::pp::RegisterPtr(new ::pp::ProcRegister(__VA_ARGS__))

}  // namespace pp

#endif // PP_REGISTER_TYPES_H__
