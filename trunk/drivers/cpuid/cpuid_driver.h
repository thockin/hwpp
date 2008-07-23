/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_CPUID_CPUID_DRIVER_H__
#define PP_DRIVERS_CPUID_CPUID_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "cpuid_binding.h"

/*
 * cpuid_driver - CPUID driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
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

	/*
	 * cpuid_driver::discover()
	 *
	 * Discover devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual void
	discover() const;

	/*
	 * cpuid_driver::register_discovery(args, function)
	 *
	 * Register a discovery_callback which will be called when the
	 * driver's discover() routine finds a device that matches args.
	 * The contents af the args vector depends on the specific driver.
	 */
	virtual void
	register_discovery(const std::vector<pp_value> &args,
			discovery_callback function);

    private:
	struct discovery_request {
		pp_value vendor;
		uint16_t family_min;
		uint16_t family_max;
		uint16_t model_min;
		uint16_t model_max;
		uint16_t stepping_min;
		uint16_t stepping_max;
		discovery_callback function;
	};

	const discovery_request *
	find_discovery_request(const cpuid_address &addr) const;

	std::vector<discovery_request> m_callbacks;
	discovery_callback m_catchall;
};

#define new_cpuid_driver(...) pp_driver_ptr(new cpuid_driver(__VA_ARGS__))

#endif // PP_DRIVERS_CPUID_CPUID_DRIVER_H__
