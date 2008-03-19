/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_MSR_DRIVER_H__
#define PP_DRIVERS_MSR_MSR_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "msr_binding.h"

/*
 * msr_driver - MSR driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class msr_driver: public pp_driver
{
    public:
	msr_driver();
	virtual ~msr_driver();

	/*
	 * msr_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * msr_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver::args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_value> &args) const;
};

#define new_msr_driver(...) pp_driver_ptr(new msr_driver(__VA_ARGS__))

#endif // PP_DRIVERS_MSR_MSR_DRIVER_H__
