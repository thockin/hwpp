#ifndef PP_DRIVERS_H__
#define PP_DRIVERS_H__

#include "pp.h"
#include "pp_driver.h"

extern void
register_driver(pp_driver *driver);

extern pp_driver *
find_driver(const string &name);

extern void
register_discovery(const string &driver_name,
		const std::vector<pp_regaddr> &args,
		pp_driver::discovery_callback function);

extern void
do_discovery(pp_scope *platform);

#endif // PP_DRIVERS_H__
