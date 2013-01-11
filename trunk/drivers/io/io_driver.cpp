#include "hwpp.h"
#include "datatype_types.h"
#include "io_driver.h"
#include "io_binding.h"

namespace hwpp { 

#define IO_SPACE_SIZE	0x10000

// this forces linkage and avoids the static initialization order fiasco
Driver *
load_io_driver()
{
	static IoDriver the_driver;
	return &the_driver;
}

IoDriver::IoDriver()
{
}

IoDriver::~IoDriver()
{
}

string
IoDriver::name() const
{
	return "io";
}

BindingPtr
IoDriver::new_binding(const std::vector<Value> &args) const
{
	Value base, size;

	if (args.size() != 2) {
		throw Driver::ArgsError("io<>: <base, size>");
	}

	base = args[0];
	size = args[1];

	if (base < 0 || base >= IO_SPACE_SIZE) {
		throw Driver::ArgsError("io<>: invalid base");
	}
	if (size < 0 || base + size > IO_SPACE_SIZE) {
		throw Driver::ArgsError("io<>: invalid size");
	}

	return new_io_binding(IoAddress(base.as_uint(), size.as_uint()));
}

}  // namespace hwpp
