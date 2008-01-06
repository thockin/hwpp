#include "pp.h"
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
	register_driver(this);
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
	pp_value cpu;

	if (args.size() != 1) {
		throw pp_driver_args_error("MSR binding: <cpu>");
	}

	cpu = args[0];

	return new_msr_binding(msr_address(pp_value_to<int>(cpu)));
}
