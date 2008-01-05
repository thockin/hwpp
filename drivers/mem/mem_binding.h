/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MEM_MEM_BINDING_H__
#define PP_DRIVERS_MEM_MEM_BINDING_H__

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"
#include <iostream>

/*
 * mem_address
 */
struct mem_address
{
	/* constructors */
	mem_address()
	    : base((typeof(base))-1ULL), size((typeof(size))-1ULL)
	{
	}
	mem_address(uint64_t b, uint64_t s)
	    : base(b), size(s)
	{
	}

	uint64_t base;
	uint64_t size;
};

inline bool
operator<(const mem_address &left, const mem_address &right)
{
	return (left.base < right.base);
}

inline std::ostream &
operator<<(std::ostream& out, const mem_address &addr)
{
	out << boost::format("mem<0x%016llx,0x%016llx>") %addr.base %addr.size;
	return out;
}

/*
 * mem_io - Linux-specific mem IO
 */
class mem_io
{
    public:
	mem_io(const mem_address &address, const string &device = "");
	~mem_io();

	const mem_address &
	address() const;

	pp_value
	read(const pp_value &address, const pp_bitwidth width) const;

	void
	write(const pp_value &address, const pp_bitwidth width,
	    const pp_value &value) const;

    private:
	mem_address m_address;
	fs::file_ptr m_file;

	pp_driver_io_error
	do_io_error(const string &str) const;

	void
	open_device(string device);

	fs::file_mapping_ptr
	map(const pp_value &offset, std::size_t length) const;

	template<typename Tdata>
	pp_value
	do_read(const pp_value &offset) const;

	template<typename Tdata>
	void
	do_write(const pp_value &offset, const pp_value &value) const;
};

/*
 * mem_binding - IO binding for register spaces
 *
 * Constructors:
 *	(mem_address address)
 */
typedef simple_binding<mem_io, mem_address> mem_binding;

#define new_mem_binding(...) pp_binding_ptr(new mem_binding(__VA_ARGS__))

#endif // PP_DRIVERS_MEM_MEM_BINDING_H__
