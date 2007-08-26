#ifndef PP_DRIVERS_H__
#define PP_DRIVERS_H__

#include "pp.h"
#include "pp_driver.h"

void
register_driver(const pp_driver *driver);

const pp_driver *
find_driver(const string &name);

#endif // PP_DRIVERS_H__
