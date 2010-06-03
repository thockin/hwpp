/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_MSR_BINDING_H__
#define PP_DRIVERS_MSR_MSR_BINDING_H__

#include "pp.h"
#include "binding.h"
#include "driver.h"
#include "util/filesystem.h"
#include <iostream>

namespace pp { 

/*
 * MsrAddress
 */
struct MsrAddress
{
	explicit
	MsrAddress(int c)
	    : cpu(c)
	{
	}
	unsigned cpu;
};

inline bool
operator<(const MsrAddress &left, const MsrAddress &right)
{
	return (left.cpu < right.cpu);
}

inline std::ostream &
operator<<(std::ostream& out, const MsrAddress &addr)
{
	out << "msr<" << addr.cpu << ">";
	return out;
}

/*
 * MsrIo - Linux-specific MSR access
 */

class MsrIo
{
    public:
	MsrIo(const MsrAddress &address,
	    const string &devdir = "", int major = -1, int minor = -1);
	~MsrIo();

	const MsrAddress &
	address() const;

	Value
	read(const Value &address, const BitWidth width) const;

	void
	write(const Value &address, const BitWidth width,
	    const Value &value) const;

    private:
	MsrAddress m_address;
	filesystem::FilePtr m_file;

	void
	do_io_error(const string &str) const;

	void
	open_device(string devdir, int major, int minor);

	void
	check_width(BitWidth width) const;

	void
	check_bounds(const Value &offset, unsigned bytes) const;

	void
	seek(const Value &offset) const;
};

/*
 * MsrBinding - MSR binding for register spaces
 */
typedef SimpleBinding<MsrIo, MsrAddress> MsrBinding;

#define new_msr_binding(...) BindingPtr(new MsrBinding(__VA_ARGS__))

}  // namespace pp

#endif // PP_DRIVERS_MSR_MSR_BINDING_H__
