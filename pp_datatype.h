/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DATATYPE_H__
#define PP_PP_DATATYPE_H__

#include "pp.h"

/*
 * pp_datatype - abstract base class for all datatypes.
 *
 * Constructors:
 * 	none
 *
 * Notes:
 */
class pp_datatype {
    public:
	virtual ~pp_datatype() {}

	/*
	 * pp_datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string evaluate(const pp_value value) const = 0;
};
typedef boost::shared_ptr<pp_datatype> pp_datatype_ptr;

#endif // PP_PP_DATATYPE_H__
