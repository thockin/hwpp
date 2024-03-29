/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_MEM_MEM_DRIVER_H__
#define HWPP_DRIVERS_MEM_MEM_DRIVER_H__

#include "hwpp.h"
#include "driver.h"
#include "mem_binding.h"

namespace hwpp { 

/*
 * MemDriver - IO driver plugin.
 */
class MemDriver: public Driver
{
    public:
	MemDriver();
	virtual ~MemDriver();

	/*
	 * MemDriver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * MemDriver::new_binding(args)
	 *
	 * Create a new Binding.
	 *
	 * Throws: Driver::ArgsError
	 */
	virtual BindingPtr
	new_binding(const std::vector<Value> &args) const;
};

#define new_mem_driver(...) DriverPtr(new MemDriver(__VA_ARGS__))

}  // namespace hwpp

#endif // HWPP_DRIVERS_MEM_MEM_DRIVER_H__
