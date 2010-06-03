/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_CPU_CPU_BINDING_H__
#define PP_DRIVERS_CPU_CPU_BINDING_H__

#include "pp.h"
#include <iostream>

namespace pp { 

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

}  // namespace pp

#endif // PP_DRIVERS_CPU_CPU_BINDING_H__
