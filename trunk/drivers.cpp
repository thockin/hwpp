/*
 * Routines for managing drivers.
 */
#include <map>
#include <vector>
#include <exception>

#include "pp.h"
#include "pp_driver.h"
#include "pp_platform.h"

typedef std::map<string, pp_driver *> driver_map;

/* this avoid the static initialization order fiasco */
static driver_map &
driver_list()
{
	static driver_map the_driver_list;
	return the_driver_list;
}

void
register_driver(pp_driver *driver)
{
	driver_list()[driver->name()] = driver;
}

//FIXME: unregister_driver

pp_driver *
find_driver(const string &name)
{
	return driver_list()[name];
}

void
init_drivers()
{
	extern int force_pci_driver_linkage;
	force_pci_driver_linkage = 1;
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
	} catch (std::exception e) {
		delayed_discovery d = {
			driver_name,
			args,
			function
		};
		discovery_list().push_back(d);
	}
}

void
do_discovery(pp_platform *platform)
{
	/* first finish any discovery registrations */
	discovery_vector::iterator discovery_iter = discovery_list().begin();
	while (discovery_iter != discovery_list().end()) {
		try {
			pp_driver *driver;
			driver = find_driver(discovery_iter->driver_name);
			driver->register_discovery(discovery_iter->args,
					discovery_iter->function);
		} catch (std::exception e) {
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
