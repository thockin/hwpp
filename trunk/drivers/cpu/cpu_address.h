/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_CPU_CPU_BINDING_H__
#define PP_DRIVERS_CPU_CPU_BINDING_H__

#include "pp.h"
#include <iostream>

/*
 * cpuid_address
 */
struct cpu_address
{
	/* constructors */
	cpu_address(int c)
	    : cpu(c)
	{
	}
	unsigned cpu;
};

inline bool
operator<(const cpu_address &left, const cpu_address &right)
{
	return (left.cpu < right.cpu);
}

inline std::ostream &
operator<<(std::ostream& out, const cpu_address &addr)
{
	out << "cpu<" << addr.cpu << ">";
	return out;
}

#endif // PP_DRIVERS_CPU_CPU_BINDING_H__
