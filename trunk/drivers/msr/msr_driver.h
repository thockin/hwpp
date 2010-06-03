/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_MSR_DRIVER_H__
#define PP_DRIVERS_MSR_MSR_DRIVER_H__

#include "pp.h"
#include "driver.h"
#include "msr_binding.h"

namespace pp { 

/*
 * MsrDriver - MSR driver plugin.
 */
class MsrDriver: public Driver
{
    public:
	MsrDriver();
	virtual ~MsrDriver();

	/*
	 * MsrDriver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * MsrDriver::new_binding(args)
	 *
	 * Create a new Binding.
	 *
	 * Throws: Driver::ArgsError
	 */
	virtual BindingPtr
	new_binding(const std::vector<Value> &args) const;
};

#define new_msr_driver(...) DriverPtr(new MsrDriver(__VA_ARGS__))

}  // namespace pp

#endif // PP_DRIVERS_MSR_MSR_DRIVER_H__
