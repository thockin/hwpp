#include <stdint.h>
#include <stdexcept>

#include "pp.h"
#include "msr_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

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
	m_file->close();
}

const msr_address &
msr_io::address() const
{
	return m_address;
}

pp_value
msr_io::read(const pp_regaddr &address, const pp_bitwidth width) const
{
	switch (width) {
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
msr_io::write(const pp_regaddr &address, const pp_bitwidth width,
    const pp_value &value) const
{
	switch (width) {
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
msr_io::do_io_error(const string &str) const
{
	return pp_driver_io_error(to_string(m_address) + ": " + str);
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
	filename = to_string(
	    boost::format("%s/%d/msr") %devdir %m_address.cpu);
	try {
		m_file = fs::file::open(filename, O_RDONLY);
		return;
	} catch (fs::not_found_error &e) {
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
		throw do_io_error("can't open device " + filename);
	}
}

void
msr_io::seek(const pp_regaddr &offset) const
{
	/* MSRs have 32 bit addresses */
	if (offset >= 0x100000000ULL) {
		throw do_io_error(to_string(
		    boost::format("can't access register 0x%x")
		    %offset));
	}

	m_file->seek(offset, SEEK_SET);
}

template<typename Tdata>
pp_value
msr_io::do_read(const pp_regaddr &offset) const
{
	seek(offset);
	Tdata data;
	if (m_file->read(&data, sizeof(data)) != sizeof(data)) {
		throw do_io_error(to_string(
		    boost::format("error reading register 0x%x")
		    %offset));
	}
	return data;
}

template<typename Tdata>
void
msr_io::do_write(const pp_regaddr &offset, const pp_value &value) const
{
	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR);
	}

	seek(offset);
	Tdata data = value;
	if (m_file->write(&data, sizeof(data)) != sizeof(data)) {
		throw do_io_error(to_string(
		    boost::format("error writing register 0x%x")
		    %offset));
	}
}
