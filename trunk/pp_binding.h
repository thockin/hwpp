/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_BINDING_H__
#define PP_PP_BINDING_H__

#include "pp.h"
#include <stdexcept>

/*
 * pp_binding - abstract base class for bound register spaces.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class pp_binding
{
    public:
	explicit pp_binding() {}
	virtual ~pp_binding() {}

	/*
	 * pp_binding::read(address, width)
	 *
	 * Read from a register in this space.
	 *
	 * Throws: pp_driver_error
	 */
	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const = 0;

	/*
	 * pp_binding::write(address, width, value)
	 *
	 * Write to a register in this space.
	 *
	 * Throws: pp_driver_error
	 */
	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const = 0;
};
typedef boost::shared_ptr<pp_binding> pp_binding_ptr;
typedef boost::shared_ptr<const pp_binding> pp_const_binding_ptr;

#endif // PP_PP_BINDING_H__
