/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DRIVER_H__
#define PP_PP_DRIVER_H__

#include "pp.h"

/*
 * pp_driver_error - thrown when something goes awry with a driver.
 */
class pp_driver_error: public std::runtime_error
{
    public:
	explicit pp_driver_error(const string &arg)
	    : std::runtime_error(arg) {}
};

class pp_driver_args_error: public pp_driver_error
{
    public:
	explicit pp_driver_argserror(const string &arg)
	    : pp_driver_error(arg) {}
};

/*
 * pp_driver - abstract base class for driver plugins.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class pp_driver
{
    public:
	explicit pp_driver() {}
	virtual ~pp_driver() {}

	/*
	 * pp_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const = 0;

	/*
	 * pp_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const vector<pp_regaddr> &args) const = 0;
};
typedef boost::shared_ptr<pp_driver> pp_driver_ptr;

#define new_pp_driver(...) pp_driver_ptr(new pp_driver(__VA_ARGS__))

#endif // PP_PP_DRIVER_H__
