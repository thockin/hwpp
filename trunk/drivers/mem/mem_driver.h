/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MEM_MEM_DRIVER_H__
#define PP_DRIVERS_MEM_MEM_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "mem_binding.h"

/*
 * mem_driver - IO driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class mem_driver: public pp_driver
{
    public:
	mem_driver();
	virtual ~mem_driver();

	/*
	 * mem_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * mem_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_regaddr> &args) const;
};

#define new_mem_driver(...) pp_driver_ptr(new mem_driver(__VA_ARGS__))

#endif // PP_DRIVERS_MEM_MEM_DRIVER_H__
