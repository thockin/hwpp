/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_IO_IO_BINDING_H__
#define PP_DRIVERS_IO_IO_BINDING_H__

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"
#include <iostream>

/*
 * io_address
 */
struct io_address
{
	/* constructors */
	io_address(uint16_t b, uint16_t s)
	    : base(b), size(s)
	{
	}
	uint16_t base;
	uint16_t size;
};

inline bool
operator<(const io_address &left, const io_address &right)
{
	return (left.base < right.base);
}

inline std::ostream &
operator<<(std::ostream& out, const io_address &addr)
{
	out << boost::format("io<0x%04x,0x%04x>") %addr.base %addr.size;
	return out;
}

/*
 * io_io - Linux-specific port IO
 */
class io_io
{
    public:
	io_io(const io_address &address, const string &device = "");
	~io_io();

	const io_address &
	address() const;

	pp_value
	read(const pp_value &address, const pp_bitwidth width) const;

	void
	write(const pp_value &address, const pp_bitwidth width,
	    const pp_value &value) const;

    private:
	io_address m_address;
	fs::file_ptr m_file;

	void
	do_io_error(const string &str) const;

	void
	open_device(string device);

	void
	check_width(pp_bitwidth width) const;

	void
	check_bounds(const pp_value &offset, unsigned bytes) const;

	void
	seek(const pp_value &offset) const;
};

/*
 * io_binding - IO binding for register spaces
 */
typedef simple_binding<io_io, io_address> io_binding;

#define new_io_binding(...) pp_binding_ptr(new io_binding(__VA_ARGS__))

#endif // PP_DRIVERS_IO_IO_BINDING_H__
