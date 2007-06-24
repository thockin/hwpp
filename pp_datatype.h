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
	evaluate(const pp_value value) const = 0;

	/*
	 * pp_datatype::lookup(str)
	 * pp_datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  Each specific subclass will override one or more of
	 * these methods.
	 *
	 * This can throw pp_datatype_invalid_error.
	 */
	virtual pp_value
	lookup(const string &str) const
	{
		/* default implementation */
		throw pp_datatype_invalid_error(
		    "pp_datatype::lookup(string) is not implemented");
	}
	virtual pp_value
	lookup(const pp_value value) const
	{
		/* default implementation */
		throw pp_datatype_invalid_error(
		    "pp_datatype::lookup(pp_value) is not implemented");
	}
};
typedef boost::shared_ptr<pp_datatype> pp_datatype_ptr;

#endif // PP_PP_DATATYPE_H__
