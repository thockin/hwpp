#include "hwpp.h"
#include "datatype_types.h"
#include "msr_driver.h"
#include "msr_binding.h"

namespace hwpp { 

// this forces linkage and avoids the static initialization order fiasco
Driver *
load_msr_driver()
{
	static MsrDriver the_driver;
	return &the_driver;
}

MsrDriver::MsrDriver()
{
	system("modprobe msr >/dev/null 2>&1");
}

MsrDriver::~MsrDriver()
{
}

string
MsrDriver::name() const
{
	return "msr";
}

BindingPtr
MsrDriver::new_binding(const std::vector<Value> &args) const
{
	if (args.size() != 1) {
		throw Driver::ArgsError("msr<>: <cpu>");
	}

	Value cpu = args[0];

	if (cpu < 0) {
		throw Driver::ArgsError("msr<>: invalid cpu");
	}

	return new_msr_binding(MsrAddress(cpu.as_uint()));
}

}  // namespace hwpp
