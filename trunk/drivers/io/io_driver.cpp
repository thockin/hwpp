#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "io_driver.h"
#include "io_binding.h"

#define IO_SPACE_SIZE	0x10000

// this forces linkage and avoids the static initialization order fiasco
void
load_io_driver()
{
	static io_driver the_io_driver;
}

io_driver::io_driver()
{
	pp_register_driver(this);
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
io_driver::new_binding(const std::vector<pp_value> &args) const
{
	pp_value base, size;

	if (args.size() != 2) {
		throw pp_driver_args_error("io<>: <base, size>");
	}

	base = args[0];
	size = args[1];

	if (base < 0 || base >= IO_SPACE_SIZE) {
		throw pp_driver_args_error("io<>: invalid base");
	}
	if (size < 0 || base + size > IO_SPACE_SIZE) {
		throw pp_driver_args_error("io<>: invalid size");
	}

	return new_io_binding(io_address(base.get_uint(), size.get_uint()));
}
