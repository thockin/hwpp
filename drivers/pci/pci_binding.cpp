#include "pp.h"

#include <stdint.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <sstream>

#include "pci_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

// helpers, local to this file
static void
enumerate_sysfs(std::vector<pci_address> *addresses);
static void
enumerate_procfs(std::vector<pci_address> *addresses);

#define PCI_SYSFS_DIR	"/sys/bus/pci/devices"
#define PCI_PROCFS_DIR	"/proc/bus/pci"

/* constructor */
pci_io::pci_io(const pci_address &address, const string &devdir)
    : m_address(address)
{
	open_device(devdir);
}

/* destructor */
pci_io::~pci_io()
{
	// m_file will close() when it's last reference goes away
}

const pci_address &
pci_io::address() const
{
	return m_address;
}

pp_value
pci_io::read(const pp_value &address, const pp_bitwidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	seek(address);
	bitbuffer bb(width, 0xff);
	if (m_file->read(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		// We already did bounds checking, but we might hit EOF on
		// a 256 B PCI config space.  That's still valid, since the
		// 4 KB space conceptually exists, but is all 0xff.
		if (!m_file->is_eof()) {
			do_io_error(to_string(
			    boost::format("error reading register 0x%x")
			    %address));
		}
	}

	return pp_value(bb);
}

void
pci_io::write(const pp_value &address, const pp_bitwidth width,
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
		// We already did bounds checking, but we might hit EOF on
		// a 256 B PCI config space.  That's still valid, since the
		// 4 KB space conceptually exists, but is all 0xff.
		if (!m_file->is_eof()) {
			do_io_error(to_string(
			    boost::format("error writing register 0x%x")
			    %address));
		}
	}
}

void
pci_io::enumerate(std::vector<pci_address> *addresses)
{
	if (fs::direntry::is_dir(PCI_SYSFS_DIR)) {
		enumerate_sysfs(addresses);
	} else if (fs::direntry::is_dir(PCI_PROCFS_DIR)) {
		enumerate_procfs(addresses);
	}
	std::sort(addresses->begin(), addresses->end());
}

void
pci_io::do_io_error(const string &str) const
{
	throw pp_driver::io_error(to_string(m_address) + ": " + str);
}

void
pci_io::open_device(string devdir)
{
	bool use_standard_devdirs = 0;

	if (devdir == "") {
		devdir = PCI_SYSFS_DIR;
		use_standard_devdirs = 1;
	}

	string filename;

	/* try to open it through /sys */
	filename = to_string(
	    boost::format("%s/%04x:%02x:%02x.%d/config")
	    %devdir %m_address.segment %m_address.bus
	    %m_address.device %m_address.function);
	try {
		m_file = fs::file::open(filename, O_RDONLY);
		return;
	} catch (std::exception &e) {
		/* do nothing yet */
	}

	if (use_standard_devdirs) {
		devdir = PCI_PROCFS_DIR;
	}

	/* fall back on /proc */
	filename = to_string(
	    boost::format("%s/%02x/%02x.%x")
	    %devdir %m_address.bus %m_address.device
	    %m_address.function);
	try {
		m_file = fs::file::open(filename, O_RDONLY);
		return;
	} catch (std::exception &e) {
		/* nothing seems to have worked */
		do_io_error("can't open PCI");
	}
}

void
pci_io::check_width(pp_bitwidth width) const
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
pci_io::check_bounds(const pp_value &offset, size_t bytes) const
{
	/* we support 4 KB config space */
	if (offset < 0 || (offset+bytes) > 4096) {
		do_io_error(to_string(
		    boost::format("invalid register: %d bytes @ 0x%x")
		    %bytes %offset));
	}
}

void
pci_io::seek(const pp_value &offset) const
{
	m_file->seek(offset.get_uint(), SEEK_SET);
}

static void
enumerate_sysfs(std::vector<pci_address> *addresses)
{
	fs::directory_ptr dir = fs::directory::open(PCI_SYSFS_DIR);

	fs::direntry_ptr de;
	while ((de = dir->read())) {
		/* skip dotfiles */
		if (de->name()[0] == '.')
			continue;

		/* parse the file name */
		std::istringstream iss(de->name());
		char c;
		unsigned seg, bus, dev, func;
		// name comes in as "ssss:bb:dd.f"
		iss >> std::hex
		    >> seg
		    >> c
		    >> bus
		    >> c
		    >> dev
		    >> c
		    >> func;

		addresses->push_back(pci_address(seg, bus, dev, func));
	}
}

static void
enumerate_procfs(std::vector<pci_address> *addresses)
{
	fs::directory_ptr dir = fs::directory::open(PCI_PROCFS_DIR);

	fs::direntry_ptr de;
	while ((de = dir->read())) {
		/* skip dotfiles */
		if (de->name()[0] == '.')
			continue;

		if (de->is_dir()) {
			string name(string(PCI_PROCFS_DIR)
			    + "/" + de->name());

			fs::directory_ptr subdir
			    = fs::directory::open(name);

			fs::direntry_ptr subde;
			while ((subde = subdir->read())) {
				/* skip dotfiles */
				if (subde->name()[0] == '.')
					continue;

				/* parse the file name */
				std::istringstream bus_iss(de->name());
				// name comes in as "bb/dd.f"
				unsigned bus;
				bus_iss >> std::hex >> bus;
				std::istringstream devfn_iss(subde->name());
				char c;
				unsigned dev, func;
				devfn_iss >> std::hex
				    >> dev
				    >> c
				    >> func;

				addresses->push_back(
					pci_address(bus, dev, func));
			}
		}
	}
}
