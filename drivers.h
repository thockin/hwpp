#ifndef PP_DRIVERS_H__
#define PP_DRIVERS_H__

#include "pp.h"
#include "pp_driver.h"

extern pp_driver *
pp_find_driver(const string &name);

extern void
pp_register_discovery(const string &driver_name,
		const std::vector<pp_value> &args,
		pp_driver::discovery_callback function);

extern void
pp_do_discovery(pp_scope *platform);

#endif // PP_DRIVERS_H__
