/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_FIELDS_H__
#define PP_PP_FIELDS_H__

#include "pp_field.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_regbits.h"
#include "pp_context.h"
#include "runtime.h"
#include <vector>

/*
 * pp_direct_field - a field that maps directly to register bits.
 *
 * Constructors:
 * 	(const pp_datatype *datatype, const pp_regbits &regbits)
 *
 * Notes:
 */
class pp_direct_field: public pp_field
{
    public:
	pp_direct_field(const pp_datatype *datatype, const pp_regbits &regbits)
	    : pp_field(datatype), m_regbits(regbits)
	{}
	virtual ~pp_direct_field()
	{}

	/*
	 * pp_direct_field::describe()
	 */
	virtual string
	describe() const
	{
		return m_regbits.describe();
	}

	/*
	 * pp_direct_field::read()
	 *
	 * Read the current value of this field.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual pp_value
	read() const
	{
		return m_regbits.read();
	}

	/*
	 * pp_direct_field::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual void
	write(const pp_value &value) const
	{
		m_regbits.write(value);
	}

    private:
	pp_regbits m_regbits;
};
typedef boost::shared_ptr<pp_direct_field> pp_direct_field_ptr;

#define new_pp_direct_field(...) \
	pp_direct_field_ptr(new pp_direct_field(__VA_ARGS__))

// a helper for proc_fields
class proc_field_accessor
{
    public:
	virtual ~proc_field_accessor() {}
	virtual pp_value read() const = 0;
	virtual void write(const pp_value &value) const = 0;
};
typedef boost::shared_ptr<const proc_field_accessor> proc_field_accessor_ptr;

/*
 * pp_proc_field - a field that is a procedure.
 *
 * Constructors:
 * 	(const pp_datatype *datatype, const proc_field_accessor_ptr &access)
 *
 * Notes:
 */
class pp_proc_field: public pp_field
{
    public:
	pp_proc_field(const pp_datatype *datatype,
	    const proc_field_accessor_ptr &access)
	    : pp_field(datatype), m_access(access),
	      m_context(pp_get_current_context())
	{}
	virtual ~pp_proc_field()
	{}

	/*
	 * pp_direct_field::describe()
	 */
	virtual string
	describe() const
	{
		return "procedures";
	}

	/*
	 * pp_proc_field::read()
	 *
	 * Read the current value of this field.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual pp_value
	read() const
	{
		pp_saved_context old_ctxt = pp_set_current_context(m_context);
		return m_access->read();
	}

	/*
	 * pp_proc_field::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual void
	write(const pp_value &value) const
	{
		pp_saved_context old_ctxt = pp_set_current_context(m_context);
		m_access->write(value);
	}

    private:
	proc_field_accessor_ptr m_access;
	pp_context m_context;
};
typedef boost::shared_ptr<pp_proc_field> pp_proc_field_ptr;

#define new_pp_proc_field(...) \
	pp_proc_field_ptr(new pp_proc_field(__VA_ARGS__))

/*
 * pp_constant_field - a field that returns a constant value.
 *
 * Constructors:
 * 	(const pp_datatype *datatype, const pp_value &value)
 *
 * Notes:
 */
class pp_constant_field: public pp_field
{
    public:
	pp_constant_field(const pp_datatype *datatype, const pp_value &value)
	    : pp_field(datatype), m_value(value)
	{}
	virtual ~pp_constant_field()
	{}

	/*
	 * pp_direct_field::describe()
	 */
	virtual string
	describe() const
	{
		return "constant";
	}

	/*
	 * pp_constant_field::read()
	 *
	 * Read the current value of this field.
	 */
	virtual pp_value
	read() const
	{
		return m_value;
	}

	/*
	 * pp_constant_field::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: pp_driver::io_error
	 */
	virtual void
	write(const pp_value &value) const
	{
		// discard writes
	}

    private:
	pp_value m_value;
};
typedef boost::shared_ptr<pp_constant_field> pp_constant_field_ptr;

#define new_pp_constant_field(...) \
	pp_constant_field_ptr(new pp_constant_field(__VA_ARGS__))

#endif // PP_PP_FIELDS_H__
