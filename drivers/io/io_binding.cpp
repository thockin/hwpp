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
	    case BITS16:
	    case BITS32:
	    case BITS64:
		break;
	    default:
		throw do_io_error(to_string(
		    boost::format("unsupported register width %d") %width));
	}

	seek(address);
	bitbuffer bb(width);
	if (m_file->read(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		throw do_io_error(to_string(
		    boost::format("can't read register 0x%x") %address));
	}

	return pp_value(bb);
}

void
io_io::write(const pp_value &address, const pp_bitwidth width,
    const pp_value &value) const
{
	switch (width) {
	    case BITS8:
	    case BITS16:
	    case BITS32:
	    case BITS64:
		break;
	    default:
		throw do_io_error(to_string(
		    boost::format("unsupported register width %d") %width));
	}

	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR);
	}

	seek(address);
	bitbuffer bb = value.get_bitbuffer(width);
	if (m_file->write(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		throw do_io_error(to_string(
		    boost::format("can't write register 0x%x") %address));
	}
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

	m_file->seek(m_address.base+offset.get_uint(), SEEK_SET);
}
