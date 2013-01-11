/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DRIVERS_MEM_MEM_BINDING_H__
#define HWPP_DRIVERS_MEM_MEM_BINDING_H__

#include "hwpp.h"
#include "util/printfxx.h"
#include "binding.h"
#include "driver.h"
#include "util/filesystem.h"
#include <iostream>

namespace hwpp { 

/*
 * MemAddress
 */
struct MemAddress
{
	/* constructors */
	MemAddress(uint64_t b, uint64_t s)
	    : base(b), size(s)
	{
	}
	uint64_t base;
	uint64_t size;
};

inline bool
operator<(const MemAddress &left, const MemAddress &right)
{
	return (left.base < right.base);
}

inline std::ostream &
operator<<(std::ostream& out, const MemAddress &addr)
{
	out << sprintfxx("mem<0x%016llx,0x%016llx>", addr.base, addr.size);
	return out;
}

/*
 * MemIo - Linux-specific mem IO
 */
class MemIo
{
    public:
	MemIo(const MemAddress &address, const string &device = "");
	~MemIo();

	const MemAddress &
	address() const;

	Value
	read(const Value &address, const BitWidth width) const;

	void
	write(const Value &address, const BitWidth width,
	    const Value &value) const;

    private:
	MemAddress m_address;
	filesystem::FilePtr m_file;

	void
	do_io_error(const string &str) const;

	void
	open_device(string device);

	filesystem::FileMappingPtr
	map(const Value &offset, std::size_t length) const;

	void
	check_width(BitWidth width) const;

	void
	check_bounds(const Value &offset, size_t bytes) const;

	template<typename Tdata>
	Value
	do_read(const Value &offset) const;

	template<typename Tdata>
	void
	do_write(const Value &offset, const Value &value) const;
};

/*
 * MemBinding - Memory binding for register spaces
 */
typedef SimpleBinding<MemIo, MemAddress> MemBinding;

#define new_mem_binding(...) BindingPtr(new MemBinding(__VA_ARGS__))

}  // namespace hwpp

#endif // HWPP_DRIVERS_MEM_MEM_BINDING_H__
