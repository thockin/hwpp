//
// Routines for managing drivers.
//
#include "pp.h"

#include <map>
#include <vector>
#include <exception>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include "filesystem.h"
#include "shared_object.h"
#include "pp_driver.h"

// this is a map of driver name to pp_driver pointer
typedef std::map<string, pp_driver *> driver_map;

// forward declaration
static void
load_all_drivers();

// this avoids the static initialization order fiasco
static driver_map &
driver_list()
{
	static driver_map the_driver_list;

	// this forces drivers to be loaded before being used
	static int initialized;
	if (!initialized) {
		// Set this before actually loading any drivers, or else we
		// can recurse forever.
		initialized = 1;
		load_all_drivers();
	}

	return the_driver_list;
}

static void
register_driver(pp_driver *driver, const shared_object &shobj)
{
	const string driver_name(driver->name());
	DTRACE(TRACE_DRIVER_UTILS, "register driver " + driver_name);
	if (driver_list().find(driver_name) != driver_list().end()) {
		WARN("driver name collision: "
		    + driver_name + " (" + shobj.path() + ") - skipping it");
		return;
	}
	driver_list()[driver_name] = driver;

	// We need to keep a copy of the shared_object so that it doesn't
	// get closed.  One day we might want to unload/reload driver
	// shared_objects, in which case we can handle this more elegantly.
	static std::vector<shared_object> shobj_vector;
	shobj_vector.push_back(shobj);
}

// this is the driver_path of last resort
static const char *default_driver_path = "drivers";

static string
get_driver_path()
{
	const char *from_env = getenv("PP_DRIVER_PATH");
	if (from_env != NULL) {
		return from_env;
	}
	return default_driver_path;
}

static void
load_driver_from_file(const string &path)
{
	static const boost::regex dot_so_regex("\\.so$");
	if (regex_search(path, dot_so_regex)) {
		DTRACE(TRACE_DRIVER_UTILS, "found a .so file: " + path);
		shared_object so;
		try {
			// open the .so, flag it not to auto-close
			so.open(path);
		} catch (shared_object::load_error &e) {
			DTRACE(TRACE_DRIVER_UTILS,
			    string("could not load ") + e.what());
			return;
		}
		typedef pp_driver *(*drv_init_func)();
		drv_init_func fn;
		try {
			fn = (drv_init_func)so.lookup_symbol("create_driver");
		} catch (shared_object::symbol_not_found_error &e) {
			DTRACE(TRACE_DRIVER_UTILS,
			    string("could not init ") + e.what());
			so.close();
			return;
		}
		// register the driver
		try {
			pp_driver *driver = fn();
			register_driver(driver, so);
		} catch (std::exception &e) {
			DTRACE(TRACE_DRIVER_UTILS,
			    string("could not load a driver from ") + path
			    + ": " + e.what());
		}
	}
}

static void
load_drivers_in_dir(const string &path)
{
	// just in case
	if (path.empty()) {
		return;
	}

	fs::directory_ptr dir = fs::directory::open(path);
	fs::direntry_ptr de;
	while ((de = dir->read())) {
		// skip dotfiles
		if (de->name()[0] == '.') {
			continue;
		}
		if (de->is_dir()) {
			// recurse into dirs
			load_drivers_in_dir(path + "/" + de->name());
		} else {
			// examine files
			load_driver_from_file(path + "/" + de->name());
		}
	}
}

static void
load_all_drivers()
{
	DTRACE(TRACE_DRIVER_UTILS, "loading all drivers");
	string driver_path = get_driver_path();
	DTRACE(TRACE_DRIVER_UTILS, "driver path = " + driver_path);

	using boost::is_any_of;
	using boost::token_compress_on;
	std::vector<string> parts;
	boost::split(parts, driver_path, is_any_of(":"), token_compress_on);

	std::for_each(parts.begin(), parts.end(), load_drivers_in_dir);
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
pp_do_discovery(pp_scope *platform)
{
	// let each driver do it's own discovery
	driver_map::iterator driver_iter = driver_list().begin();
	while (driver_iter != driver_list().end()) {
		driver_iter->second->discover(platform);
		driver_iter++;
	}
}
