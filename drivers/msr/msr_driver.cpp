#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "msr_driver.h"
#include "msr_binding.h"
#include "linux_msr_io.h"

int force_msr_driver_linkage;
static const msr_driver the_msr_driver;

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
msr_driver::new_binding(const std::vector<pp_regaddr> &args) const
{
	pp_regaddr cpu;

	if (args.size() != 1) {
		throw pp_driver_args_error(
		    "MSR binding: <cpu>");
	}

	cpu = args[0];

	return new_msr_binding(msr_address(cpu));
}
