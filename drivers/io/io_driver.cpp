#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "io_driver.h"
#include "io_binding.h"
#include "linux_io_io.h"

#define IO_SPACE_SIZE	0x10000

int force_io_driver_linkage;
static const io_driver the_io_driver;

io_driver::io_driver()
{
	register_driver(this);
}

io_driver::~io_driver()
{
}

string
io_driver::name() const
{
	return "io";
}

pp_binding_ptr
io_driver::new_binding(const std::vector<pp_regaddr> &args) const
{
	pp_regaddr base, size;

	if (args.size() != 2) {
		throw pp_driver_args_error(
		    "IO binding: <base, size>");
	}

	base = args[0];
	size = args[1];

	if (base >= IO_SPACE_SIZE) {
		throw pp_driver_args_error("IO binding: invalid base");
	}
	if (base + size > IO_SPACE_SIZE) {
		throw pp_driver_args_error("IO binding: invalid size");
	}
	return new_io_binding(io_address(base, size));
}
