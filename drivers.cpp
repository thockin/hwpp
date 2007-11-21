/*
 * Routines for managing drivers.
 */
#include <map>
#include <vector>
#include <exception>

#include "pp.h"
#include "debug.h"
#include "pp_driver.h"

typedef std::map<string, pp_driver *> driver_map;

/* this avoids the static initialization order fiasco */
static driver_map &
driver_list()
{
	static driver_map the_driver_list;

	// This forces drivers to link and register before being used.
	// When we have real shared-object drivers, this will be
	// unnecessary.
	static int initialized;
	if (!initialized) {
		initialized = 1;
		extern void load_pci_driver(); load_pci_driver();
		extern void load_io_driver(); load_io_driver();
		extern void load_mem_driver(); load_mem_driver();
		extern void load_msr_driver(); load_msr_driver();
	}

	return the_driver_list;
}

void
register_driver(pp_driver *driver)
{
	DTRACE(TRACE_DRIVER_UTILS, "register driver " + driver->name());
	driver_list()[driver->name()] = driver;
}

//FIXME: unregister_driver

pp_driver *
find_driver(const string &name)
{
	DTRACE(TRACE_DRIVER_UTILS, "find driver " + name);
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

/*
 * Routines for discovery
 */

struct delayed_discovery {
	string driver_name;
	std::vector<pp_regaddr> args;
	pp_driver::discovery_callback function;
};
typedef std::vector<delayed_discovery> discovery_vector;

/* this avoid the static initialization order fiasco */
static discovery_vector &
discovery_list()
{
	static discovery_vector the_discovery_list;
	return the_discovery_list;
}

void
register_discovery(const string &driver_name,
		const std::vector<pp_regaddr> &args,
		pp_driver::discovery_callback function)
{
	try {
		pp_driver *driver = find_driver(driver_name);
		driver->register_discovery(args, function);
	} catch (std::exception &e) {
		delayed_discovery d = {
			driver_name,
			args,
			function
		};
		discovery_list().push_back(d);
	}
}

void
do_discovery(pp_scope *platform)
{
	/* first finish any discovery registrations */
	discovery_vector::iterator discovery_iter = discovery_list().begin();
	while (discovery_iter != discovery_list().end()) {
		try {
			pp_driver *driver;
			driver = find_driver(discovery_iter->driver_name);
			driver->register_discovery(discovery_iter->args,
					discovery_iter->function);
		} catch (std::exception &e) {
			//FIXME: error
		}
		discovery_iter++;
	}

	/* let each driver do it's own discovery */
	driver_map::iterator driver_iter = driver_list().begin();
	while (driver_iter != driver_list().end()) {
		driver_iter->second->discover(platform);
		driver_iter++;
	}
}
