#include "pp/pp.h"
#include "pp/datatype_types.h"
#include "mem_driver.h"
#include "mem_binding.h"

namespace pp { 

// this forces linkage and avoids the static initialization order fiasco
Driver *
load_mem_driver()
{
	static MemDriver the_driver;
	return &the_driver;
}

MemDriver::MemDriver()
{
}

MemDriver::~MemDriver()
{
}

string
MemDriver::name() const
{
	return "mem";
}

BindingPtr
MemDriver::new_binding(const std::vector<Value> &args) const
{
	Value base, size;

	if (args.size() != 2) {
		throw Driver::ArgsError("mem<>: <base, size>");
	}

	base = args[0];
	size = args[1];

	if (base < 0) {
		throw Driver::ArgsError("mem<>: invalid base");
	}
	if (size < 0) {
		throw Driver::ArgsError("mem<>: invalid size");
	}

	return new_mem_binding(MemAddress(base.get_uint(), size.get_uint()));
}

}  // namespace pp
