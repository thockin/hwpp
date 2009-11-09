/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_DRIVERS_CPU_CPU_DRIVER_H__
#define PP_DRIVERS_CPU_CPU_DRIVER_H__

#include "pp/pp.h"
#include "pp/driver.h"
#include "cpu_address.h"

#include <vector>

namespace pp { 

/*
 * CpuDriver - CPU driver plugin.
 */
class CpuDriver: public Driver
{
    public:
	CpuDriver();
	virtual ~CpuDriver();

	/*
	 * CpuDriver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * CpuDriver::discover()
	 *
	 * Discover devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual void
	discover() const;

	/*
	 * CpuDriver::register_discovery(args, function)
	 *
	 * Register a DiscoveryCallback which will be called when the
	 * driver's discover() routine finds a device that matches args.
	 * The contents af the args vector depends on the specific driver.
	 */
	virtual void
	register_discovery(const std::vector<Value> &args,
			DiscoveryCallback function);

	static void
	enumerate(const string &path, std::vector<CpuAddress> *addresses);

	static Value
	cpuid(const CpuAddress &address, unsigned function);

    private:
	struct DiscoveryRequest {
		Value vendor;
		uint16_t family_min;
		uint16_t family_max;
		uint16_t model_min;
		uint16_t model_max;
		uint16_t stepping_min;
		uint16_t stepping_max;
		DiscoveryCallback function;
	};

	const DiscoveryRequest *
	find_discovery_request(const CpuAddress &addr) const;

	std::vector<DiscoveryRequest> m_callbacks;
	DiscoveryCallback m_catchall;

	static void
	do_io_error(const CpuAddress &address, const string &str);
};

#define new_cpu_driver(...) DriverPtr(new CpuDriver(__VA_ARGS__))

}  // namespace pp

#endif // PP_DRIVERS_CPU_CPU_DRIVER_H__
