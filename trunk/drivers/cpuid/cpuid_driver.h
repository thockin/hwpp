/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_CPUID_CPUID_DRIVER_H__
#define PP_DRIVERS_CPUID_CPUID_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "cpuid_binding.h"

/*
 * cpuid_driver - CPUID driver plugin.
 */
class cpuid_driver: public pp_driver
{
    public:
	cpuid_driver();
	virtual ~cpuid_driver();

	/*
	 * cpuid_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * cpuid_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver::args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_value> &args) const;
};

#define new_cpuid_driver(...) pp_driver_ptr(new cpuid_driver(__VA_ARGS__))

#endif // PP_DRIVERS_CPUID_CPUID_DRIVER_H__
