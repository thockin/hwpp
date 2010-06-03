#ifndef PP_DRIVERS_H__
#define PP_DRIVERS_H__

#include "pp.h"
#include "driver.h"
#include "runtime.h"

namespace pp {

extern void
register_driver(Driver *driver);

extern Driver *
find_driver(const string &name);

extern void
register_discovery(const string &driver_name,
                   const std::vector<Value> &args,
                   Driver::DiscoveryCallback function = NULL);

extern void
do_discovery(const string &driver_name);

inline void
do_discovery()
{
	do_discovery("");
}

}  // namespace pp

#endif // PP_DRIVERS_H__
