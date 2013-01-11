//
// Routines for managing drivers.
//
#include "drivers.h"

#include <map>
#include <vector>
#include <exception>

#include "hwpp.h"
#include "driver.h"
#include "util/filesystem.h"

namespace hwpp {

// this is a map of driver name to Driver pointer
typedef std::map<string, Driver *> DriverMap;

// this avoids the static initialization order fiasco
static DriverMap &
driver_list()
{
	static DriverMap the_driver_list;

	// This forces drivers to link and register before being used.
	// When we have real shared-object drivers, this will be
	// unnecessary.
	static int initialized;
	if (!initialized) {
		// Set this before actually loading any drivers, or else we
		// can recurse forever.
		initialized = 1;
		extern void load_all_drivers();
		load_all_drivers();
	}

	return the_driver_list;
}

void
register_driver(Driver *driver)
{
	const string driver_name(driver->name());
	DTRACE(TRACE_DRIVER_UTILS, "register driver " + driver_name);
	if (driver_list().find(driver_name) != driver_list().end()) {
		WARN("driver name collision: '"
		    + driver_name + "' - skipping it");
		return;
	}
	driver_list()[driver_name] = driver;
}

Driver *
find_driver(const string &name)
{
	DTRACE(TRACE_DRIVER_UTILS, "find driver " + name);
	if (driver_list().find(name) == driver_list().end()) {
		throw std::out_of_range("driver not found: " + name);
	}
	return driver_list()[name];
}

namespace device {
extern int force_devices_linkage;
}  // namespace device

// This is a hack to force device linkage.  When we have a real language,
// this will be unnecessary.
void
init_devices()
{
	hwpp::device::force_devices_linkage = 1;
}

//
// Routines for discovery
//

void
register_discovery(const string &driver_name,
                   const std::vector<Value> &args,
                   Driver::DiscoveryCallback function)
{
	Driver *driver = find_driver(driver_name);
	driver->register_discovery(args, function);
}

void
do_discovery(const string &driver_name)
{
	// if the caller specified a driver, use just that driver
	if (driver_name != "") {
		const Driver *driver = find_driver(driver_name);
		driver->discover();
		return;
	}

	// otherwise, let each driver do it's own discovery
	DriverMap::iterator driver_iter = driver_list().begin();
	while (driver_iter != driver_list().end()) {
		driver_iter->second->discover();
		driver_iter++;
	}
}

}  // namespace hwpp
