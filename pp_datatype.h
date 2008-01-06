/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DATATYPE_H__
#define PP_PP_DATATYPE_H__

#include "pp.h"
#include <stdexcept>

class pp_datatype_invalid_error: public std::runtime_error
{
    public:
	pp_datatype_invalid_error(const string &str): runtime_error(str) {}
};

/*
 * pp_datatype - abstract base class for all datatypes.
 *
 * Constructors:
 * 	none
 *
 * Notes:
 */
class pp_datatype
{
    public:
	virtual ~pp_datatype() {}

	/*
	 * pp_datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const = 0;

	/*
	 * pp_datatype::lookup(str)
	 * pp_datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  Each specific subclass will override these.
	 *
	 * This can throw pp_datatype_invalid_error.
	 */
	virtual pp_value
	lookup(const string &str) const = 0;
	virtual pp_value
	lookup(const pp_value &value) const = 0;

	/*
	 * pp_datatype::compare(value, str)
	 * pp_datatype::compare(value, value)
	 * pp_datatype::test(value, str)
	 * pp_datatype::test(value, value)
	 *
	 * Compare a pp_value against another argument, according to the
	 * rules of this datatype.  Return an integer which indicates
	 * whether the lhs is less than, equal to, or greater than the rhs
	 * (negative, zero, positive return code, respectively).
	 *
	 * This can throw pp_datatype_invalid_error.
	 */
	virtual int
	compare(const pp_value &lhs, const string &rhs) const
	{
		/* default implementation */
		pp_value result = lhs - lookup(rhs);
		if (result < 0)
			return -1;
		if (result > 0)
			return 1;
		return 0;
	}
	virtual int
	compare(const pp_value &lhs, const pp_value &rhs) const
	{
		/* default implementation */
		pp_value result = lhs - lookup(rhs);
		if (result < 0)
			return -1;
		if (result > 0)
			return 1;
		return 0;
	}
	virtual pp_value
	test(const pp_value &lhs, const string &rhs) const
	{
		/* default implementation */
		return (lhs & lookup(rhs));
	}
	virtual pp_value
	test(const pp_value &lhs, const pp_value &rhs) const
	{
		/* default implementation */
		return (lhs & lookup(rhs));
	}
};
typedef boost::shared_ptr<pp_datatype> pp_datatype_ptr;
typedef boost::shared_ptr<const pp_datatype> pp_const_datatype_ptr;

#endif // PP_PP_DATATYPE_H__
