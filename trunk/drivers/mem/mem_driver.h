/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MEM_MEM_DRIVER_H__
#define PP_DRIVERS_MEM_MEM_DRIVER_H__

#include "pp/pp.h"
#include "pp/driver.h"
#include "mem_binding.h"

namespace pp { 

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

}  // namespace pp

#endif // PP_DRIVERS_MEM_MEM_DRIVER_H__
