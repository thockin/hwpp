/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVER_H__
#define HWPP_DRIVER_H__

#include "hwpp.h"
#include "binding.h"
#include "scope.h"

namespace hwpp {

//
// Driver - abstract base class for driver plugins.
//
class Driver
{
    public:
	struct IoError: public std::runtime_error
	{
		explicit IoError(const string &str)
		    : runtime_error(str)
		{
		}
	};
	struct ArgsError: public std::runtime_error
	{
		explicit ArgsError(const string &str)
		    : runtime_error(str)
		{
		}
	};
	struct NotSupportedError: public std::runtime_error
	{
		explicit NotSupportedError(const string &str)
		    : runtime_error(str)
		{
		}
	};

	// ctor and dtor
	Driver() {}
	virtual ~Driver() {}

	//
	// Driver::name()
	//
	// Get the name of this driver.
	//
	virtual string
	name() const = 0;

	//
	// Driver::new_binding(args)
	//
	// Create a new Binding.
	//
	// Throws: Driver::ArgsError
	//
	virtual BindingPtr
	new_binding(const std::vector<Value> &args) const
	{
		(void)args;
		throw NotSupportedError(this->name() + ": " +
				"binding not supported for this driver");
	}

	//
	// Driver::discover()
	//
	// Enumerate devices owned by this driver, and add them to the
	// platform.
	//
	virtual void
	discover() const
	{
		// do nothing by default
	}

	//
	// Driver::DiscoveryCallback
	//
	// This is the functional signature of a driver discovery event.
	// When a driver's discover() routine finds a device that matches a
	// registered device (via register_discovery()), the
	// DiscoveryCallback is called.
	//
	typedef void (*DiscoveryCallback)(const std::vector<Value> &args);

	//
	// Driver::register_discovery(args, function)
	//
	// Register a DiscoveryCallback which will be called when the
	// driver's discover() routine finds a device that matches args.
	// The contents af the args vector depends on the specific driver.
	// If the callback_function is NULL, the driver should ignore any
	// devices that match args.
	//
	virtual void
	register_discovery(const std::vector<Value> &args,
	                   DiscoveryCallback function)
	{
		(void)args;
		(void)function;
		throw NotSupportedError(this->name() + ": " +
		    "discovery not supported for this driver");
	}
};
typedef boost::shared_ptr<Driver> DriverPtr;

}  // namespace hwpp

#endif // HWPP_DRIVER_H__
