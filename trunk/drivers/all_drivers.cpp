#include "pp/pp.h"
#include "pp/drivers.h"
#include "pp/driver.h"

namespace pp {

typedef Driver *(*DriverInitFunction)();

DRIVER_INIT_PROTOS

// this gets patched up at compile time
static DriverInitFunction driver_init_functions[] = {
	DRIVER_INIT_FUNCS
	NULL
};

// it's exposed because it has to be, but it's not an end-user API
void
load_all_drivers()
{
	DTRACE(TRACE_DRIVER_UTILS, "loading all drivers");
	for (int i=0; driver_init_functions[i] != NULL; i++) {
		Driver *driver = driver_init_functions[i]();
		register_driver(driver);
	}
}

}  // namespace pp
