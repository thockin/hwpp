#include "pp.h"

#include <stdint.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "cpuid_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

#define CPUID_DEVICE_DIR	"/dev/cpu"
#define CPUID_DEV_MAJOR		203
#define CPU_SYSFS_DIR		"/sys/devices/system/cpu"

/* constructor */
cpuid_io::cpuid_io(const cpuid_address &address, const string &devdir,
		int major, int minor)
    : m_address(address)
{
	open_device(devdir, major, minor);
}

/* destructor */
cpuid_io::~cpuid_io()
{
	// m_file will close() when it's last reference goes away
}

const cpuid_address &
cpuid_io::address() const
{
	return m_address;
}

pp_value
cpuid_io::read(const pp_value &address, const pp_bitwidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	seek(m_address.function);
	// /dev/cpuid requires us to read 16 bytes
	uint8_t ar[16];
	if (m_file->read(ar, 16) != 16) {
		// We already did bounds checking, so this must be bad.
		do_io_error(to_string(
		    boost::format("error reading register 0x%x") %address));
	}
	bitbuffer bitbuf(width);
	memcpy(bitbuf.get(), ar + (address.get_int()*4), width/CHAR_BIT);

	return pp_value(bitbuf);
}

void
cpuid_io::write(const pp_value &address, const pp_bitwidth width,
    const pp_value &value) const
{
	// do nothing - CPUID is read-only
}

void
cpuid_io::enumerate(std::vector<cpuid_address> *addresses)
{
	// This does not currently support non sysfs systems, but it could.
	fs::directory_ptr dir = fs::directory::open(CPU_SYSFS_DIR);

	fs::direntry_ptr de;
	while ((de = dir->read())) {
		/* each CPU gets a dir */
		if (!de->is_dir())
			continue;
		/* each dir is named cpuN */
		if (de->name().substr(0,3) != "cpu")
			continue;

		/* parse the file name */
		std::istringstream iss(de->name());
		char c;
		unsigned cpu;
		// name comes in as "cpuN", but all we care about is N
		iss >> c >> c >> c >> cpu;

		addresses->push_back(cpuid_address(cpu, 0));
	}
	std::sort(addresses->begin(), addresses->end());
}

void
cpuid_io::do_io_error(const string &str) const
{
	throw pp_driver::io_error(to_string(m_address) + ": " + str);
}

void
cpuid_io::open_device(string devdir, int major, int minor)
{
	string filename;

	if (devdir == "")
		devdir = CPUID_DEVICE_DIR;
	if (major == -1)
		major = CPUID_DEV_MAJOR;
	if (minor == -1)
		minor = m_address.cpu;

	/* try to open it through /dev */
	filename = to_string(
	    boost::format("%s/%d/cpuid") %devdir %m_address.cpu);
	try {
		m_file = fs::file::open(filename, O_RDONLY);
		return;
	} catch (sys_error::not_found &e) {
		/* do nothing yet */
	}

	/* fall back on a self-made device node */
	filename = fs::file::tempname();
	fs::device::mkdev(filename, 0600, S_IFCHR, major, minor);
	try {
		m_file = fs::device::open(filename, O_RDONLY);
		return;
	} catch (std::exception &e) {
		/* the device seems to not exist */
		do_io_error("can't open device " + filename);
	}
}

void
cpuid_io::check_width(pp_bitwidth width) const
{
	switch (width) {
	    case BITS32:
		break;
	    default:
		do_io_error(to_string(
		    boost::format("unsupported register width %d") %width));
	}
}

void
cpuid_io::check_bounds(const pp_value &offset, unsigned bytes) const
{
	/* CPUID has a very small register space per function */
	if (offset < 0 || offset >= 4) {
		do_io_error(to_string(
		    boost::format("invalid register: %d bytes @ 0x%x")
		    %bytes %offset));
	}
}

void
cpuid_io::seek(const pp_value &offset) const
{
	m_file->seek(offset.get_uint(), SEEK_SET);
}
