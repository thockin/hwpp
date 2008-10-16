#include "pp.h"
#include "pp_datatypes.h"
#include "cpuid_driver.h"
#include "cpuid_binding.h"

// this forces linkage and avoids the static initialization order fiasco
pp_driver *
load_cpuid_driver()
{
	static cpuid_driver the_driver;
	return &the_driver;
}

cpuid_driver::cpuid_driver()
{
	system("modprobe cpuid >/dev/null 2>&1");
}

cpuid_driver::~cpuid_driver()
{
}

string
cpuid_driver::name() const
{
	return "cpuid";
}

pp_binding_ptr
cpuid_driver::new_binding(const std::vector<pp_value> &args) const
{
	if (args.size() != 1) {
		throw pp_driver::args_error("cpuid<>: <cpu>");
	}

	pp_value cpu = args[0];

	if (cpu < 0) {
		throw pp_driver::args_error("cpuid<>: invalid cpu");
	}

	return new_cpuid_binding(cpuid_address(cpu.get_uint()));
}
