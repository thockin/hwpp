/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_IO_IO_DRIVER_H__
#define PP_DRIVERS_IO_IO_DRIVER_H__

#include "pp.h"
#include "driver.h"
#include "io_binding.h"

namespace pp { 

/*
 * IoDriver - IO driver plugin.
 */
class IoDriver: public Driver
{
    public:
	IoDriver();
	virtual ~IoDriver();

	/*
	 * IoDriver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * IoDriver::new_binding(args)
	 *
	 * Create a new Binding.
	 *
	 * Throws: Driver::ArgsError
	 */
	virtual BindingPtr
	new_binding(const std::vector<Value> &args) const;
};

#define new_io_driver(...) DriverPtr(new IoDriver(__VA_ARGS__))

}  // namespace pp

#endif // PP_DRIVERS_IO_IO_DRIVER_H__
