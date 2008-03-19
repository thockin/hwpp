#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "mem_driver.h"
#include "mem_binding.h"

// this forces linkage and avoids the static initialization order fiasco
void
load_mem_driver()
{
	static mem_driver the_mem_driver;
}

mem_driver::mem_driver()
{
	pp_register_driver(this);
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
mem_driver::new_binding(const std::vector<pp_value> &args) const
{
	pp_value base, size;

	if (args.size() != 2) {
		throw pp_driver::args_error("mem<>: <base, size>");
	}

	base = args[0];
	size = args[1];

	if (base < 0) {
		throw pp_driver::args_error("mem<>: invalid base");
	}
	if (size < 0) {
		throw pp_driver::args_error("mem<>: invalid size");
	}

	return new_mem_binding(mem_address(base.get_uint(), size.get_uint()));
}
