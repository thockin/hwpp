/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_FIELD_H__
#define PP_PP_FIELD_H__

#include "pp.h"
#include "pp_datatype.h"

/*
 * pp_field - abstract base class for a data field.
 *
 * Constructors:
 * 	(const pp_const_datatype &datatype)
 *
 * Notes:
 */
class pp_field
{
    public:
	explicit pp_field(const pp_const_datatype_ptr &datatype)
	    : m_datatype(datatype) {}
	virtual ~pp_field() {}

	/*
	 * pp_field::read()
	 *
	 * Read the current value of this field.
	 */
	virtual pp_value
	read() const = 0;

	/*
	 * pp_field::write(value)
	 *
	 * Write a value to this field.
	 */
	virtual void
	write(const pp_value value) const = 0;

	/*
	 * pp_field::evaluate()
	 *
	 * Evaluate the value of this field against it's datatype. This
	 * method returns a string containing the evaluated representation
	 * of the field.
	 */
	virtual string
	evaluate() const
	{
		pp_value value = read();
		return m_datatype->evaluate(value);
	}

	/*
	 * pp_field::lookup(str)
	 * pp_field::lookup(value)
	 *
	 * Lookup the numeric value of a (potentially valid) evaluation for
	 * this field.
	 *
	 * This can throw pp_datatype_invalid_error.
	 */
	virtual pp_value
	lookup(const string &str) const
	{
		return m_datatype->lookup(str);
	}
	virtual pp_value
	lookup(const pp_value value) const
	{
		return m_datatype->lookup(value);
	}

    private:
	pp_const_datatype_ptr m_datatype;
};
typedef boost::shared_ptr<pp_field> pp_field_ptr;

#endif // PP_PP_FIELD_H__
