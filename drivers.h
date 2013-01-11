#ifndef HWPP_DRIVERS_H__
#define HWPP_DRIVERS_H__

#include "hwpp.h"
#include "driver.h"
#include "runtime.h"

namespace hwpp {

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

}  // namespace hwpp

#endif // HWPP_DRIVERS_H__
