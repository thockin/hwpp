/*
 * Routines for managing drivers.
 */
#include <map>
#include <iostream>

#include "pp.h"
#include "pp_driver.h"

typedef std::map<string, const pp_driver *> driver_map;

/* this avoid the static initialization order fiasco */
static driver_map &
driver_list()
{
	static driver_map the_driver_list;
	return the_driver_list;
}

void
register_driver(const pp_driver *driver)
{
	driver_list()[driver->name()] = driver;
}

//FIXME: unregister_driver

const pp_driver *
find_driver(const string &name)
{
	return driver_list()[name];
}

void
init_drivers()
{
	extern int force_pci_driver_linkage;
	force_pci_driver_linkage = 1;
}
