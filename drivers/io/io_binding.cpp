#include "pp.h"
#include "util/printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include "io_binding.h"
#include "driver.h"
#include "util/filesystem.h"
#include "util/bit_buffer.h"

namespace pp { 

#define IO_DEVICE	"/dev/port"

/* constructor */
IoIo::IoIo(const IoAddress &address, const string &device)
    : m_address(address)
{
	open_device(device);
}

/* destructor */
IoIo::~IoIo()
{
	// m_file will close() when it's last reference goes away
}

const IoAddress &
IoIo::address() const
{
	return m_address;
}

Value
IoIo::read(const Value &address, const BitWidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	seek(address);
	util::BitBuffer bb(width);
	if (m_file->read(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		// We already did bounds checking, so this must be bad.
		do_io_error(sprintfxx("error reading register 0x%x: %s",
		                      address, strerror(errno)));
	}

	return Value(bb);
}

void
IoIo::write(const Value &address, const BitWidth width,
    const Value &value) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR);
	}

	seek(address);
	util::BitBuffer bb = value.get_bitbuffer(width);
	if (m_file->write(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		// We already did bounds checking, so this must be bad.
		do_io_error(sprintfxx("error writing register 0x%x: %s",
		                      address, strerror(errno)));
	}
}

void
IoIo::do_io_error(const string &str) const
{
	throw Driver::IoError(to_string(m_address) + ": " + str);
}

void
IoIo::open_device(string device)
{
	if (device == "")
		device = IO_DEVICE;

	m_file = filesystem::File::open(device, O_RDONLY);
	return;
}

void
IoIo::check_width(BitWidth width) const
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
IoIo::check_bounds(const Value &offset, unsigned bytes) const
{
	if (offset < 0 || (offset+bytes) > m_address.size) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}

void
IoIo::seek(const Value &offset) const
{
	m_file->seek(m_address.base+offset.get_uint(), SEEK_SET);
}

}  // namespace pp
