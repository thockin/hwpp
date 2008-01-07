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
	m_file->close();
}

const mem_address &
mem_io::address() const
{
	return m_address;
}

pp_value
mem_io::read(const pp_value &address, const pp_bitwidth width) const
{
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
		break;
	}
	throw do_io_error(to_string(
	    boost::format("unsupported register width %d")
	    %width));
}

void
mem_io::write(const pp_value &address, const pp_bitwidth width,
    const pp_value &value) const
{
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
		break;
	}
	throw do_io_error(to_string(
	    boost::format("unsupported register width %d")
	    %width));
}

pp_driver_io_error
mem_io::do_io_error(const string &str) const
{
	return pp_driver_io_error(to_string(m_address) + ": " + str);
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
		throw do_io_error("can't open device " + device);
	}
}

fs::file_mapping_ptr
mem_io::map(const pp_value &offset, std::size_t length) const
{
	if (offset.get_uint()+length > m_address.size) {
		throw do_io_error(to_string(
		    boost::format("can't access register 0x%x")
		    %offset));
	}

	return m_file->mmap(m_address.base+offset.get_uint(), length);
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
