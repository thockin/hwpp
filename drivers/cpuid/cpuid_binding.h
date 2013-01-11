/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_CPUID_CPUID_BINDING_H__
#define HWPP_DRIVERS_CPUID_CPUID_BINDING_H__

#include "hwpp.h"
#include "binding.h"
#include "driver.h"
#include <iostream>

namespace hwpp { 

/*
 * CpuidAddress
 */
struct CpuidAddress
{
	/* constructors */
	CpuidAddress(int c)
	    : cpu(c)
	{
	}
	unsigned cpu;
};

inline bool
operator<(const CpuidAddress &left, const CpuidAddress &right)
{
	return (left.cpu < right.cpu);
}

inline std::ostream &
operator<<(std::ostream& out, const CpuidAddress &addr)
{
	out << "cpuid<" << addr.cpu << ">";
	return out;
}

/*
 * CpuidIo - Linux-specific CPUID IO
 */
class CpuidIo
{
    public:
	CpuidIo(const CpuidAddress &address);
	~CpuidIo();

	Value
	read(const Value &address, const BitWidth width) const;

	void
	write(const Value &address, const BitWidth width,
	    const Value &value) const;

	const CpuidAddress &
	address() const;

    private:
	CpuidAddress m_address;

	void
	do_io_error(const string &str) const;

	void
	check_width(BitWidth width) const;

	void
	check_bounds(const Value &offset, unsigned bytes) const;
};

/*
 * CpuidBinding - CPUID binding for register spaces
 */
typedef SimpleBinding<CpuidIo, CpuidAddress> CpuidBinding;

#define new_cpuid_binding(...) BindingPtr(new CpuidBinding(__VA_ARGS__))

}  // namespace hwpp

#endif // HWPP_DRIVERS_CPUID_CPUID_BINDING_H__
