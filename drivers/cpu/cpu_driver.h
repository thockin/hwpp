/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_DRIVERS_CPU_CPU_DRIVER_H__
#define PP_DRIVERS_CPU_CPU_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "cpu_address.h"

#include <vector>

/*
 * cpu_driver - CPU driver plugin.
 */
class cpu_driver: public pp_driver
{
    public:
	cpu_driver();
	virtual ~cpu_driver();

	/*
	 * cpu_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * cpu_driver::discover()
	 *
	 * Discover devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual void
	discover() const;

	/*
	 * cpu_driver::register_discovery(args, function)
	 *
	 * Register a discovery_callback which will be called when the
	 * driver's discover() routine finds a device that matches args.
	 * The contents af the args vector depends on the specific driver.
	 */
	virtual void
	register_discovery(const std::vector<pp_value> &args,
			discovery_callback function);

	static void
	enumerate(const string &path, std::vector<cpu_address> *addresses);

	static pp_value
	cpuid(const cpu_address &address, unsigned function);

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
	find_discovery_request(const cpu_address &addr) const;

	std::vector<discovery_request> m_callbacks;
	discovery_callback m_catchall;

	static void
	do_io_error(const cpu_address &address, const string &str);
};

#define new_cpu_driver(...) pp_driver_ptr(new cpu_driver(__VA_ARGS__))

#endif // PP_DRIVERS_CPU_CPU_DRIVER_H__
