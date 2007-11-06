/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_FIELDS_H__
#define PP_PP_FIELDS_H__

#include "pp_field.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include <vector>

/*
 * regbits - a bit range from a register.
 *
 * Constructors:
 * 	(const pp_register *reg, const int shift,
 * 	 const pp_value mask, const int position)
 *
 * Notes:
 *	This class is a helper for pp_regbits_field.
 */
class regbits
{
    public:
	explicit regbits(const pp_register *reg, const int shift,
	    const pp_value mask, const int position)
	    : m_reg(reg), m_regshift(shift), m_mask(mask),
	      m_position(position) {}
	virtual ~regbits() {}

	/*
	 * regbits::read()
	 *
	 * Read the value of this regbits.  The resulting value is aligned
	 * to the requested position.
	 *
	 * Throws: pp_driver_error
	 */
	pp_value
	read() const
	{
		pp_value result = m_reg->read();
		result >>= m_regshift;
		result &= m_mask;
		result <<= m_position;
		return result;
	}

	/*
	 * regbits::write(value)
	 *
	 * Write a value to this regbits.  The value is assumed to be
	 * positioned in the output position.
	 *
	 * Throws: pp_driver_error
	 */
	void
	write(const pp_value value) const
	{
		pp_value myval = value;
		myval >>= m_position;
		myval &= m_mask;
		myval <<= m_regshift;
		pp_value tmp = m_reg->read();
		tmp &= ~(m_mask << m_regshift);
		tmp |= myval;
		m_reg->write(tmp);
	}

    private:
	const pp_register *m_reg;
	int m_regshift;
	pp_value m_mask;
	int m_position;
};

/*
 * magic registers - used for filling hardcoded regbits
 */
extern pp_register *magic_zeros;
extern pp_register *magic_ones;

/*
 * pp_regbits_field - a field that maps directly to register bits.
 *
 * Constructors:
 * 	(const pp_const_datatype &datatype)
 *
 * Notes:
 */
class pp_regbits_field: public pp_field
{
    public:
	explicit pp_regbits_field(const pp_datatype *datatype)
	    : pp_field(datatype) {}
	virtual ~pp_regbits_field() {}

	/*
	 * pp_regbits_field::read()
	 *
	 * Read the current value of this field.
	 *
	 * Throws: pp_driver_error
	 */
	virtual pp_value
	read() const
	{
		pp_value result = 0;
		for (size_t i=0; i < m_regbits.size(); i++) {
			result |= m_regbits[i].read();
		}
		return result;
	}

	/*
	 * pp_regbits_field::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: pp_driver_error
	 */
	virtual void
	write(const pp_value value) const
	{
		for (size_t i=0; i < m_regbits.size(); i++) {
			m_regbits[i].write(value);
		}
	}

	/*
	 * pp_regbits_field::add_regbits(const pp_register *reg,
	 * 	const int shift, const pp_value mask, const int position)
	 *
	 * Add register bits to this field.
	 */
	 //FIXME: just pass a vector to ctor?
	void
	add_regbits(const pp_register *reg,
	    const int shift, const pp_value mask, const int position)
	{
		m_regbits.push_back(regbits(reg, shift, mask, position));
	}

    private:
	std::vector<regbits> m_regbits;
};
typedef boost::shared_ptr<pp_regbits_field> pp_regbits_field_ptr;

#define new_pp_regbits_field(...) pp_regbits_field_ptr(new pp_regbits_field(__VA_ARGS__))

class proc_field_accessor
{
    public:
	virtual ~proc_field_accessor() {}
	virtual pp_value read() = 0;
	virtual void write(pp_value value) = 0;
};
typedef boost::shared_ptr<proc_field_accessor> proc_field_accessor_ptr;

/*
 * pp_proc_field - a field that is a procedure
 *
 * Constructors:
 * 	(const pp_const_datatype &datatype, proc_field_accessor_ptr access)
 *
 * Notes:
 */
class pp_proc_field: public pp_field
{
    public:
	explicit pp_proc_field(const pp_datatype *datatype,
	    proc_field_accessor_ptr access)
	    : pp_field(datatype), m_access(access)
	{
	}

	virtual ~pp_proc_field()
	{
	}

	/*
	 * pp_proc_field::read()
	 *
	 * Read the current value of this field.
	 *
	 * Throws: pp_driver_error
	 */
	virtual pp_value
	read() const
	{
		return m_access->read();
	}

	/*
	 * pp_proc_field::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: pp_driver_error
	 */
	virtual void
	write(const pp_value value) const
	{
		m_access->write(value);
	}

    private:
	proc_field_accessor_ptr m_access;
};
typedef boost::shared_ptr<pp_proc_field> pp_proc_field_ptr;

#define new_pp_proc_field(...) pp_proc_field_ptr(new pp_proc_field(__VA_ARGS__))

#endif // PP_PP_FIELDS_H__
