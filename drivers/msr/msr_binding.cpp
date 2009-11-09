#include "pp/pp.h"
#include "pp/util/printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include "msr_binding.h"
#include "pp/driver.h"
#include "pp/util/filesystem.h"
#include "pp/util/syserror.h"
#include "pp/util/bit_buffer.h"

namespace pp { 

#define MSR_DEVICE_DIR	"/dev/cpu"
#define MSR_DEV_MAJOR	202

/* constructor */
MsrIo::MsrIo(const MsrAddress &address,
    const string &devdir, int major, int minor)
    : m_address(address)
{
	open_device(devdir, major, minor);
}

/* destructor */
MsrIo::~MsrIo()
{
	// m_file will close() when it's last reference goes away
}

const MsrAddress &
MsrIo::address() const
{
	return m_address;
}

Value
MsrIo::read(const Value &address, const BitWidth width) const
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
MsrIo::write(const Value &address, const BitWidth width,
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
MsrIo::do_io_error(const string &str) const
{
	throw Driver::IoError(to_string(m_address) + ": " + str);
}

void
MsrIo::open_device(string devdir, int major, int minor)
{
	string filename;

	if (devdir == "")
		devdir = MSR_DEVICE_DIR;
	if (major == -1)
		major = MSR_DEV_MAJOR;
	if (minor == -1)
		minor = m_address.cpu;

	/* try to open it through /dev */
	filename = sprintfxx("%s/%d/msr", devdir, m_address.cpu);
	try {
		m_file = filesystem::File::open(filename, O_RDONLY);
		return;
	} catch (syserr::NotFound &e) {
		/* do nothing yet */
	}

	/* fall back on a self-made device node */
	filename = filesystem::File::tempname();
	filesystem::Device::mkdev(filename, 0600, S_IFCHR, major, minor);
	m_file = filesystem::Device::open(filename, O_RDONLY);
	return;
}

void
MsrIo::check_width(BitWidth width) const
{
	/* we only support 64 bit accesses */
	switch (width) {
	    case BITS64:
		break;
	    default:
		do_io_error(sprintfxx("unsupported register width %d", width));
	}
}

void
MsrIo::check_bounds(const Value &offset, unsigned bytes) const
{
	/* MSRs have 32 bit addresses */
	if (offset < 0 || (offset+bytes) > MASK(32)) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}


void
MsrIo::seek(const Value &offset) const
{
	m_file->seek(offset.get_uint(), SEEK_SET);
}

}  // namespace pp
