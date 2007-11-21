/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_IO_IO_DRIVER_H__
#define PP_DRIVERS_IO_IO_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "io_binding.h"

/*
 * io_driver - IO driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class io_driver: public pp_driver
{
    public:
	io_driver();
	virtual ~io_driver();

	/*
	 * io_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * io_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_regaddr> &args) const;
};

#define new_io_driver(...) pp_driver_ptr(new io_driver(__VA_ARGS__))

#endif // PP_DRIVERS_IO_IO_DRIVER_H__
