/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_IO_IO_BINDING_H__
#define HWPP_DRIVERS_IO_IO_BINDING_H__

#include "hwpp.h"
#include "util/printfxx.h"
#include "binding.h"
#include "driver.h"
#include "util/filesystem.h"
#include <iostream>

namespace hwpp { 

/*
 * IoAddress
 */
struct IoAddress
{
	/* constructors */
	IoAddress(uint16_t b, uint16_t s)
	    : base(b), size(s)
	{
	}
	uint16_t base;
	uint16_t size;
};

inline bool
operator<(const IoAddress &left, const IoAddress &right)
{
	return (left.base < right.base);
}

inline std::ostream &
operator<<(std::ostream& out, const IoAddress &addr)
{
	out << sprintfxx("io<0x%04x,0x%04x>", addr.base, addr.size);
	return out;
}

/*
 * IoIo - Linux-specific port IO
 */
class IoIo
{
    public:
	IoIo(const IoAddress &address, const string &device = "");
	~IoIo();

	const IoAddress &
	address() const;

	Value
	read(const Value &address, const BitWidth width) const;

	void
	write(const Value &address, const BitWidth width,
	    const Value &value) const;

    private:
	IoAddress m_address;
	filesystem::FilePtr m_file;

	void
	do_io_error(const string &str) const;

	void
	open_device(string device);

	void
	check_width(BitWidth width) const;

	void
	check_bounds(const Value &offset, unsigned bytes) const;

	void
	seek(const Value &offset) const;
};

/*
 * IoBinding - IO binding for register spaces
 */
typedef SimpleBinding<IoIo, IoAddress> IoBinding;

#define new_io_binding(...) BindingPtr(new IoBinding(__VA_ARGS__))

}  // namespace hwpp

#endif // HWPP_DRIVERS_IO_IO_BINDING_H__
