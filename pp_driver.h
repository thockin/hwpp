/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DRIVER_H__
#define PP_PP_DRIVER_H__

#include "pp.h"
#include "pp_platform.h"

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
	explicit pp_driver_args_error(const string &arg)
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
	new_binding(const std::vector<pp_regaddr> &args) const = 0;

	/*
	 * pp_driver::discover(platform)
	 *
	 * Enumerate devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual int
	discover(pp_platform *platform) const = 0;

	/*
	 * pp_driver::discover_callback
	 *
	 * This is the functional signature of a driver discovery event.
	 * When a driver's discover() routine finds a device that matches a
	 * registered device (via register_discovery()), the
	 * discover_callback is called.
	 */
	typedef void (*discover_callback)(pp_platform *platform,
			const pp_driver *driver,
			const std::vector<pp_regaddr> &args);

	/*
	 * pp_driver::register_discovery(args, function)
	 *
	 * Register a discover_callback which will be called when the
	 * driver's discover() routine finds a device that matches args.
	 * The contents af the args vector depends on the specific driver.
	 */
	virtual void
	register_discovery(const std::vector<pp_regaddr> &args,
			discover_callback function) = 0;
};
typedef boost::shared_ptr<pp_driver> pp_driver_ptr;

#endif // PP_PP_DRIVER_H__