/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_CPU_CPU_BINDING_H__
#define HWPP_DRIVERS_CPU_CPU_BINDING_H__

#include "hwpp.h"
#include <iostream>

namespace hwpp { 

/*
 * CpuAddress
 */
struct CpuAddress
{
	/* constructors */
	CpuAddress(int c)
	    : cpu(c)
	{
	}
	unsigned cpu;
};

inline bool
operator<(const CpuAddress &left, const CpuAddress &right)
{
	return (left.cpu < right.cpu);
}

inline std::ostream &
operator<<(std::ostream& out, const CpuAddress &addr)
{
	out << "cpu<" << addr.cpu << ">";
	return out;
}

}  // namespace hwpp

#endif // HWPP_DRIVERS_CPU_CPU_BINDING_H__
