#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "mem_driver.h"
#include "mem_binding.h"
#include "linux_mem_io.h"

#define IO_SPACE_SIZE	0x10000

int force_mem_driver_linkage;
static const mem_driver the_mem_driver;

mem_driver::mem_driver()
{
	register_driver(this);
}

mem_driver::~mem_driver()
{
}

string
mem_driver::name() const
{
	return "mem";
}

pp_binding_ptr
mem_driver::new_binding(const std::vector<pp_regaddr> &args) const
{
	pp_regaddr base, size;

	if (args.size() != 2) {
		throw pp_driver_args_error("mem<>: <base, size>");
	}

	base = args[0];
	size = args[1];

	if (base >= IO_SPACE_SIZE) {
		throw pp_driver_args_error("mem<>: invalid base");
	}
	if (base + size > IO_SPACE_SIZE) {
		throw pp_driver_args_error("mem<>: invalid size");
	}
	return new_mem_binding(mem_address(base, size));
}
