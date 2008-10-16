#include "pp.h"
#include "printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
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
	// m_file will close() when it's last reference goes away
}

const io_address &
io_io::address() const
{
	return m_address;
}

pp_value
io_io::read(const pp_value &address, const pp_bitwidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	seek(address);
	bitbuffer bb(width);
	if (m_file->read(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		// We already did bounds checking, so this must be bad.
		do_io_error(sprintfxx("error reading register 0x%x: %s",
		                      address, strerror(errno)));
	}

	return pp_value(bb);
}

void
io_io::write(const pp_value &address, const pp_bitwidth width,
    const pp_value &value) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR);
	}

	seek(address);
	bitbuffer bb = value.get_bitbuffer(width);
	if (m_file->write(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		// We already did bounds checking, so this must be bad.
		do_io_error(sprintfxx("error writing register 0x%x: %s",
		                      address, strerror(errno)));
	}
}

void
io_io::do_io_error(const string &str) const
{
	throw pp_driver::io_error(to_string(m_address) + ": " + str);
}

void
io_io::open_device(string device)
{
	if (device == "")
		device = IO_DEVICE;

	m_file = fs::file::open(device, O_RDONLY);
	return;
}

void
io_io::check_width(pp_bitwidth width) const
{
	switch (width) {
	    case BITS8:
	    case BITS16:
	    case BITS32:
	    case BITS64:
		break;
	    default:
		do_io_error(sprintfxx("unsupported register width %d", width));
	}
}

void
io_io::check_bounds(const pp_value &offset, unsigned bytes) const
{
	if (offset < 0 || (offset+bytes) > m_address.size) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}

void
io_io::seek(const pp_value &offset) const
{
	m_file->seek(m_address.base+offset.get_uint(), SEEK_SET);
}
