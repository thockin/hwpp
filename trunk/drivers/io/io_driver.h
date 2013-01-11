/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_IO_IO_DRIVER_H__
#define HWPP_DRIVERS_IO_IO_DRIVER_H__

#include "hwpp.h"
#include "driver.h"
#include "io_binding.h"

namespace hwpp { 

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

}  // namespace hwpp

#endif // HWPP_DRIVERS_IO_IO_DRIVER_H__
