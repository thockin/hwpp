/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_CPUID_CPUID_DRIVER_H__
#define PP_DRIVERS_CPUID_CPUID_DRIVER_H__

#include "pp/pp.h"
#include "pp/driver.h"
#include "cpuid_binding.h"

namespace pp { 

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

}  // namespace pp

#endif // PP_DRIVERS_CPUID_CPUID_DRIVER_H__
