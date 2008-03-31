#include "pp.h"
#include <stdlib.h>

typedef void (*driver_init_func)();

DRIVER_INIT_PROTOS

// this gets patched up at compile time
static driver_init_func driver_init_functions[] = {
	DRIVER_INIT_FUNCS
	NULL
};

// it's exposed because it has to be, but it's not an end-user API
void
pp_load_all_drivers()
{
	DTRACE(TRACE_DRIVER_UTILS, "loading all drivers");
	for (int i=0; driver_init_functions[i] != NULL; i++) {
		driver_init_functions[i]();
	}
}
