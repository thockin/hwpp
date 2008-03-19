/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DRIVER_H__
#define PP_PP_DRIVER_H__

#include "pp.h"
#include "pp_binding.h"
#include "pp_scope.h"

//
// pp_driver - abstract base class for driver plugins.
//
// Constructors:
//	none
//
// Notes:
//
class pp_driver
{
    public:
	struct io_error: public std::runtime_error
	{
		explicit io_error(const string &str)
		    : runtime_error(str)
		{
		}
	};
	struct args_error: public std::runtime_error
	{
		explicit args_error(const string &str)
		    : runtime_error(str)
		{
		}
	};
	struct not_supported_error: public std::runtime_error
	{
		explicit not_supported_error(const string &str)
		    : runtime_error(str)
		{
		}
	};

	// ctor and dtor
	pp_driver() {}
	virtual ~pp_driver() {}

	//
	// pp_driver::name()
	//
	// Get the name of this driver.
	//
	virtual string
	name() const = 0;

	//
	// pp_driver::new_binding(args)
	//
	// Create a new pp_binding.
	//
	// Throws: pp_driver::args_error
	//
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_value> &args) const = 0;

	//
	// pp_driver::discover(platform)
	//
	// Enumerate devices owned by this driver, and add them to the
	// platform.
	//
	virtual void
	discover(pp_scope *platform) const
	{
		// do nothing by default
	}

	//
	// pp_driver::discovery_callback
	//
	// This is the functional signature of a driver discovery event.
	// When a driver's discover() routine finds a device that matches a
	// registered device (via pp_register_discovery()), the
	// discovery_callback is called.
	//
	typedef void (*discovery_callback)(const std::vector<pp_value> &args);

	//
	// pp_driver::register_discovery(args, function)
	//
	// Register a discovery_callback which will be called when the
	// driver's discover() routine finds a device that matches args.
	// The contents af the args vector depends on the specific driver.
	//
	virtual void
	register_discovery(const std::vector<pp_value> &args,
			discovery_callback function)
	{
		throw not_supported_error(this->name() + ": " +
				"discovery not supported for this driver");
	}
};
typedef boost::shared_ptr<pp_driver> pp_driver_ptr;

#endif // PP_PP_DRIVER_H__
