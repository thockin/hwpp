#include "pp.h"
#include "printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include "msr_binding.h"
#include "pp_driver.h"
#include "filesystem.h"
#include "syserror.h"

#define MSR_DEVICE_DIR	"/dev/cpu"
#define MSR_DEV_MAJOR	202

/* constructor */
msr_io::msr_io(const msr_address &address,
    const string &devdir, int major, int minor)
    : m_address(address)
{
	open_device(devdir, major, minor);
}

/* destructor */
msr_io::~msr_io()
{
	// m_file will close() when it's last reference goes away
}

const msr_address &
msr_io::address() const
{
	return m_address;
}

pp_value
msr_io::read(const pp_value &address, const pp_bitwidth width) const
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
msr_io::write(const pp_value &address, const pp_bitwidth width,
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
msr_io::do_io_error(const string &str) const
{
	throw pp_driver::io_error(to_string(m_address) + ": " + str);
}

void
msr_io::open_device(string devdir, int major, int minor)
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
		m_file = fs::file::open(filename, O_RDONLY);
		return;
	} catch (syserr::not_found &e) {
		/* do nothing yet */
	}

	/* fall back on a self-made device node */
	filename = fs::file::tempname();
	fs::device::mkdev(filename, 0600, S_IFCHR, major, minor);
	m_file = fs::device::open(filename, O_RDONLY);
	return;
}

void
msr_io::check_width(pp_bitwidth width) const
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
msr_io::check_bounds(const pp_value &offset, unsigned bytes) const
{
	/* MSRs have 32 bit addresses */
	if (offset < 0 || (offset+bytes) > PP_MASK(32)) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}


void
msr_io::seek(const pp_value &offset) const
{
	m_file->seek(offset.get_uint(), SEEK_SET);
}
