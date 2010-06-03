/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DATATYPE_H__
#define PP_DATATYPE_H__

#include "pp.h"
#include <stdexcept>

namespace pp {

/*
 * Datatype - abstract base class for all datatypes.
 */
class Datatype
{
    public:
	// an invalid value error
	struct InvalidError: public std::runtime_error
	{
		explicit InvalidError(const string &str)
		    : runtime_error(str)
		{
		}
	};

	virtual ~Datatype() {}

	/*
	 * Datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const = 0;

	/*
	 * Datatype::lookup(str)
	 * Datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  Each specific subclass will override these.
	 *
	 * This can throw Datatype::InvalidError.
	 */
	virtual Value
	lookup(const string &str) const = 0;
	virtual Value
	lookup(const Value &value) const = 0;

	/*
	 * Datatype::compare(value, str)
	 * Datatype::compare(value, value)
	 * Datatype::test(value, str)
	 * Datatype::test(value, value)
	 *
	 * Compare a Value against another argument, according to the
	 * rules of this datatype.  Return an integer which indicates
	 * whether the lhs is less than, equal to, or greater than the rhs
	 * (negative, zero, positive return code, respectively).
	 *
	 * This can throw Datatype::InvalidError.
	 */
	virtual int
	compare(const Value &lhs, const string &rhs) const
	{
		/* default implementation */
		Value result = lhs - lookup(rhs);
		if (result < 0)
			return -1;
		if (result > 0)
			return 1;
		return 0;
	}
	virtual int
	compare(const Value &lhs, const Value &rhs) const
	{
		/* default implementation */
		Value result = lhs - lookup(rhs);
		if (result < 0)
			return -1;
		if (result > 0)
			return 1;
		return 0;
	}
	virtual Value
	test(const Value &lhs, const string &rhs) const
	{
		/* default implementation */
		return (lhs & lookup(rhs));
	}
	virtual Value
	test(const Value &lhs, const Value &rhs) const
	{
		/* default implementation */
		return (lhs & lookup(rhs));
	}
};
typedef boost::shared_ptr<Datatype> DatatypePtr;
typedef boost::shared_ptr<const Datatype> ConstDatatypePtr;

}  // namespace pp

#endif // PP_DATATYPE_H__
