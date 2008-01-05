#include "pp.h"

#include <stdint.h>
#include <unistd.h>
#include <stdexcept>

#include "io_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

#define IO_DEVICE	"/dev/port"

/* constructor */
io_io::io_io(const io_address &address, const string &device)
    : m_address(address)
{
	open_device(device);
}

/* destructor */
io_io::~io_io()
{
	m_file->close();
}

const io_address &
io_io::address() const
{
	return m_address;
}

pp_value
io_io::read(const pp_value &address, const pp_bitwidth width) const
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
	    boost::format("unsupported register width %d") %width));
}

void
io_io::write(const pp_value &address, const pp_bitwidth width,
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
	    boost::format("unsupported register width %d") %width));
}

pp_driver_io_error
io_io::do_io_error(const string &str) const
{
	return pp_driver_io_error(to_string(m_address) + ": " + str);
}

void
io_io::open_device(string device)
{
	if (device == "")
		device = IO_DEVICE;

	try {
		m_file = fs::file::open(device, O_RDONLY);
		return;
	} catch (std::exception &e) {
		/* the device seems to not exist */
		throw do_io_error("can't open device " + device);
	}
}

void
io_io::seek(const pp_value &offset) const
{
	if (offset >= m_address.size) {
		throw do_io_error(to_string(
		    boost::format("can't access register 0x%x") %offset));
	}

	m_file->seek(m_address.base+offset, SEEK_SET);
}

template<typename Tdata>
pp_value
io_io::do_read(const pp_value &offset) const
{
	seek(offset);
	Tdata data;
	if (m_file->read(&data, sizeof(data)) != sizeof(data)) {
		throw do_io_error(to_string(
		    boost::format("can't read register 0x%x") %offset));
	}
	return data;
}

template<typename Tdata>
void
io_io::do_write(const pp_value &offset, const pp_value &value) const
{
	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR);
	}

	seek(offset);
	Tdata data = value;
	if (m_file->write(&data, sizeof(data)) != sizeof(data)) {
		throw do_io_error(to_string(
		    boost::format("can't write register 0x%x") %offset));
	}
}
