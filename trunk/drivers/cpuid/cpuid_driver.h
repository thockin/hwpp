/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_CPUID_CPUID_DRIVER_H__
#define HWPP_DRIVERS_CPUID_CPUID_DRIVER_H__

#include "hwpp.h"
#include "driver.h"
#include "cpuid_binding.h"

namespace hwpp { 

/*
 * CpuidDriver - CPUID driver plugin.
 */
class CpuidDriver: public Driver
{
    public:
	CpuidDriver();
	virtual ~CpuidDriver();

	/*
	 * CpuidDriver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * CpuidDriver::new_binding(args)
	 *
	 * Create a new Binding.
	 *
	 * Throws: Driver::ArgsError
	 */
	virtual BindingPtr
	new_binding(const std::vector<Value> &args) const;
};

#define new_cpuid_driver(...) DriverPtr(new CpuidDriver(__VA_ARGS__))

}  // namespace hwpp

#endif // HWPP_DRIVERS_CPUID_CPUID_DRIVER_H__
