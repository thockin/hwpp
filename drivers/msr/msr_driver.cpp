#include "pp.h"
#include "stdlib.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "msr_driver.h"
#include "msr_binding.h"

// this forces linkage and avoids the static initialization order fiasco
void
load_msr_driver()
{
	static msr_driver the_msr_driver;
}

msr_driver::msr_driver()
{
	system("modprobe msr >/dev/null 2>&1");
	pp_register_driver(this);
}

msr_driver::~msr_driver()
{
}

string
msr_driver::name() const
{
	return "msr";
}

pp_binding_ptr
msr_driver::new_binding(const std::vector<pp_value> &args) const
{
	if (args.size() != 1) {
		throw pp_driver::args_error("msr<>: <cpu>");
	}

	pp_value cpu = args[0];

	if (cpu < 0) {
		throw pp_driver::args_error("msr<>: invalid cpu");
	}

	return new_msr_binding(msr_address(cpu.get_uint()));
}
