#include "pp.h"

#include <stdint.h>
#include <unistd.h>
#include <stdexcept>

#include "mem_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

#define MEM_DEVICE	"/dev/mem"

/* constructor */
mem_io::mem_io(const mem_address &address, const string &device)
    : m_address(address)
{
	open_device(device);
}

/* destructor */
mem_io::~mem_io()
{
	// m_file will close() when it's last reference goes away
}

const mem_address &
mem_io::address() const
{
	return m_address;
}

// We do this differently than most other drivers because we are using
// mmap() under the covers, and we want to use single exact-width memory
// accesses.
pp_value
mem_io::read(const pp_value &address, const pp_bitwidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	switch (width) {
	    case BITS8:
		return do_read<uint8_t>(address);
	    case BITS16:
		return do_read<uint16_t>(address);
	    case BITS32:
		return do_read<uint32_t>(address);
	    case BITS64:
		return do_read<uint64_t>(address);
	    default:
		/* we already did bounds and width checking */
		break;
	}
	do_io_error("can't get here");
	return -1;
}

// We do this differently than most other drivers because we are using
// mmap() under the covers, and we want to use single exact-width memory
// accesses.
void
mem_io::write(const pp_value &address, const pp_bitwidth width,
    const pp_value &value) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	switch (width) {
	    case BITS8:
		return do_write<uint8_t>(address, value);
	    case BITS16:
		return do_write<uint16_t>(address, value);
	    case BITS32:
		return do_write<uint32_t>(address, value);
	    case BITS64:
		return do_write<uint64_t>(address, value);
	    default:
		/* we already did bounds and width checking */
		break;
	}
}

void
mem_io::do_io_error(const string &str) const
{
	throw pp_driver_io_error(to_string(m_address) + ": " + str);
}

void
mem_io::open_device(string device)
{
	if (device == "")
		device = MEM_DEVICE;

	try {
		m_file = fs::file::open(device, O_RDONLY);
		return;
	} catch (std::exception &e) {
		/* the device seems to not exist */
		do_io_error("can't open device " + device);
	}
}

fs::file_mapping_ptr
mem_io::map(const pp_value &offset, size_t length) const
{
	if (offset.get_uint()+length > m_address.size) {
		do_io_error(to_string(
		    boost::format("can't access register 0x%x") %offset));
	}

	return m_file->mmap(m_address.base+offset.get_uint(), length);
}

void
mem_io::check_width(pp_bitwidth width) const
{
	switch (width) {
	    case BITS8:
	    case BITS16:
	    case BITS32:
	    case BITS64:
		break;
	    default:
		do_io_error(to_string(
		    boost::format("unsupported register width %d")
		    %width));
	}
}

void
mem_io::check_bounds(const pp_value &offset, size_t bytes) const
{
	/* we support 64 bit memory */
	if (offset < 0 || (offset+bytes) > PP_MASK(64)) {
		do_io_error(to_string(
		    boost::format("invalid register: %d bytes @ 0x%x")
		    %bytes %offset));
	}
}

template<typename Tdata>
pp_value
mem_io::do_read(const pp_value &offset) const
{
	fs::file_mapping_ptr mapping = map(offset, sizeof(Tdata));
	Tdata *ptr = (Tdata *)mapping->address();
	Tdata data = *ptr;
	return pp_value(data);
}

template<typename Tdata>
void
mem_io::do_write(const pp_value &offset, const pp_value &value) const
{
	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR);
	}

	fs::file_mapping_ptr mapping = map(offset, sizeof(Tdata));
	Tdata *ptr = (Tdata *)mapping->address();
	Tdata data = value.get_uint();
	*ptr = data;
}
