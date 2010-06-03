#include "pp.h"
#include "datatype_types.h"
#include "cpuid_driver.h"
#include "cpuid_binding.h"

namespace pp { 

// this forces linkage and avoids the static initialization order fiasco
Driver *
load_cpuid_driver()
{
	static CpuidDriver the_driver;
	return &the_driver;
}

CpuidDriver::CpuidDriver()
{
	system("modprobe cpuid >/dev/null 2>&1");
}

CpuidDriver::~CpuidDriver()
{
}

string
CpuidDriver::name() const
{
	return "cpuid";
}

BindingPtr
CpuidDriver::new_binding(const std::vector<Value> &args) const
{
	if (args.size() != 1) {
		throw Driver::ArgsError("cpuid<>: <cpu>");
	}

	Value cpu = args[0];

	if (cpu < 0) {
		throw Driver::ArgsError("cpuid<>: invalid cpu");
	}

	return new_cpuid_binding(CpuidAddress(cpu.get_uint()));
}

}  // namespace pp
