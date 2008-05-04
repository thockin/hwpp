//
// Routines for managing drivers.
//
#include "pp.h"

#include <map>
#include <vector>
#include <exception>

#include "filesystem.h"
#include "pp_driver.h"

// this is a map of driver name to pp_driver pointer
typedef std::map<string, pp_driver *> driver_map;

// this avoids the static initialization order fiasco
static driver_map &
driver_list()
{
	static driver_map the_driver_list;

	// This forces drivers to link and register before being used.
	// When we have real shared-object drivers, this will be
	// unnecessary.
	static int initialized;
	if (!initialized) {
		// Set this before actually loading any drivers, or else we
		// can recurse forever.
		initialized = 1;
		extern void pp_load_all_drivers();
		pp_load_all_drivers();
	}

	return the_driver_list;
}

void
pp_register_driver(pp_driver *driver)
{
	const string driver_name(driver->name());
	DTRACE(TRACE_DRIVER_UTILS, "register driver " + driver_name);
	if (driver_list().find(driver_name) != driver_list().end()) {
		PP_WARN("driver name collision: '"
		    + driver_name + "' - skipping it");
		return;
	}
	driver_list()[driver_name] = driver;
}

pp_driver *
pp_find_driver(const string &name)
{
	DTRACE(TRACE_DRIVER_UTILS, "find driver " + name);
	if (driver_list().find(name) == driver_list().end()) {
		throw std::out_of_range("driver not found: " + name);
	}
	return driver_list()[name];
}

// This is a hack to force device linkage.  When we have a real language,
// this will be unnecessary.
void
init_devices()
{
	extern int force_devices_linkage;
	force_devices_linkage = 1;
}

//
// Routines for discovery
//

void
pp_register_discovery(const string &driver_name,
		const std::vector<pp_value> &args,
		pp_driver::discovery_callback function)
{
	pp_driver *driver = pp_find_driver(driver_name);
	driver->register_discovery(args, function);
}

void
pp_do_discovery(const string &driver_name, pp_scope *scope)
{
	// if the caller specified a driver, use just that driver
	if (driver_name != "") {
		const pp_driver *driver = pp_find_driver(driver_name);
		driver->discover(scope);
	}

	// otherwise, let each driver do it's own discovery
	driver_map::iterator driver_iter = driver_list().begin();
	while (driver_iter != driver_list().end()) {
		driver_iter->second->discover(scope);
		driver_iter++;
	}
}
