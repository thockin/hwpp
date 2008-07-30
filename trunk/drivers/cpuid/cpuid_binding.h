/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_CPUID_CPUID_BINDING_H__
#define PP_DRIVERS_CPUID_CPUID_BINDING_H__

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"
#include <iostream>

/*
 * cpuid_address
 */
struct cpuid_address
{
	/* constructors */
	cpuid_address(int c)
	    : cpu(c)
	{
	}
	unsigned cpu;
};

inline bool
operator<(const cpuid_address &left, const cpuid_address &right)
{
	return (left.cpu < right.cpu);
}

inline std::ostream &
operator<<(std::ostream& out, const cpuid_address &addr)
{
	out << "cpuid<" << addr.cpu << ">";
	return out;
}

/*
 * cpuid_io - Linux-specific CPUID IO
 */
class cpuid_io
{
    public:
	cpuid_io(const cpuid_address &address);
	~cpuid_io();

	pp_value
	read(const pp_value &address, const pp_bitwidth width) const;

	void
	write(const pp_value &address, const pp_bitwidth width,
	    const pp_value &value) const;

	const cpuid_address &
	address() const;

	static void
	enumerate(std::vector<cpuid_address> *addresses);

    private:
	cpuid_address m_address;

	void
	do_io_error(const string &str) const;

	void
	check_width(pp_bitwidth width) const;

	void
	check_bounds(const pp_value &offset, unsigned bytes) const;
};

/*
 * cpuid_binding - CPUID binding for register spaces
 */
typedef simple_binding<cpuid_io, cpuid_address> cpuid_binding;

#define new_cpuid_binding(...) pp_binding_ptr(new cpuid_binding(__VA_ARGS__))

#endif // PP_DRIVERS_CPUID_CPUID_BINDING_H__
