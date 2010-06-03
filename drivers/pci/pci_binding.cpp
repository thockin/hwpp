#include "pp.h"
#include "util/printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <sstream>

#include "pci_binding.h"
#include "driver.h"
#include "util/filesystem.h"
#include "util/bit_buffer.h"

namespace pp { 

// helpers, local to this file
static void
enumerate_sysfs(std::vector<PciAddress> *addresses);
static void
enumerate_procfs(std::vector<PciAddress> *addresses);

#define PCI_SYSFS_DIR	"/sys/bus/pci/devices"
#define PCI_PROCFS_DIR	"/proc/bus/pci"

/* constructor */
PciIo::PciIo(const PciAddress &address, const string &devdir)
    : m_address(address)
{
	open_device(devdir);
}

/* destructor */
PciIo::~PciIo()
{
	// m_file will close() when it's last reference goes away
}

const PciAddress &
PciIo::address() const
{
	return m_address;
}

Value
PciIo::read(const Value &address, const BitWidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	seek(address);
	util::BitBuffer bb(width, 0xff);
	if (m_file->read(bb.get(), bb.size_bytes()) != bb.size_bytes()) {
		// We already did bounds checking, but we might hit EOF on
		// a 256 B PCI config space.  That's still valid, since the
		// 4 KB space conceptually exists, but is all 0xff.
		if (!m_file->is_eof()) {
			do_io_error(sprintfxx("error reading register 0x%x: %s",
			                      address, strerror(errno)));
		}
	}

	return Value(bb);
}

void
PciIo::write(const Value &address, const BitWidth width,
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
		// We already did bounds checking, but we might hit EOF on
		// a 256 B PCI config space.  That's still valid, since the
		// 4 KB space conceptually exists, but is all 0xff.
		if (!m_file->is_eof()) {
			do_io_error(sprintfxx("error writing register 0x%x: %s",
			                      address, strerror(errno)));
		}
	}
}

void
PciIo::enumerate(std::vector<PciAddress> *addresses)
{
	if (filesystem::Direntry::is_dir(PCI_SYSFS_DIR)) {
		enumerate_sysfs(addresses);
	} else if (filesystem::Direntry::is_dir(PCI_PROCFS_DIR)) {
		enumerate_procfs(addresses);
	}
	std::sort(addresses->begin(), addresses->end());
}

void
PciIo::do_io_error(const string &str) const
{
	throw Driver::IoError(to_string(m_address) + ": " + str);
}

void
PciIo::open_device(string devdir)
{
	bool use_standard_devdirs = 0;

	if (devdir == "") {
		devdir = PCI_SYSFS_DIR;
		use_standard_devdirs = 1;
	}

	string filename;

	/* try to open it through /sys */
	filename = sprintfxx("%s/%04x:%02x:%02x.%d/config", devdir,
	                     m_address.segment, m_address.bus,
	                     m_address.device, m_address.function);
	try {
		m_file = filesystem::File::open(filename, O_RDONLY);
		return;
	} catch (std::exception &e) {
		/* do nothing yet */
	}

	if (use_standard_devdirs) {
		devdir = PCI_PROCFS_DIR;
	}

	/* fall back on /proc */
	filename = sprintfxx("%s/%02x/%02x.%x", devdir, m_address.bus,
	                     m_address.device, m_address.function);
	m_file = filesystem::File::open(filename, O_RDONLY);
	return;
}

void
PciIo::check_width(BitWidth width) const
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
PciIo::check_bounds(const Value &offset, size_t bytes) const
{
	/* we support 4 KB config space */
	if (offset < 0 || (offset+bytes) > 4096) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}

void
PciIo::seek(const Value &offset) const
{
	m_file->seek(offset.get_uint(), SEEK_SET);
}

static void
enumerate_sysfs(std::vector<PciAddress> *addresses)
{
	filesystem::DirectoryPtr dir
	    = filesystem::Directory::open(PCI_SYSFS_DIR);

	filesystem::DirentryPtr de;
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

		addresses->push_back(PciAddress(seg, bus, dev, func));
	}
}

static void
enumerate_procfs(std::vector<PciAddress> *addresses)
{
	filesystem::DirectoryPtr dir
	    = filesystem::Directory::open(PCI_PROCFS_DIR);

	filesystem::DirentryPtr de;
	while ((de = dir->read())) {
		/* skip dotfiles */
		if (de->name()[0] == '.')
			continue;

		if (de->is_dir()) {
			string name(string(PCI_PROCFS_DIR)
			    + "/" + de->name());

			filesystem::DirectoryPtr subdir
			     = filesystem::Directory::open(name);

			filesystem::DirentryPtr subde;
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
					PciAddress(bus, dev, func));
			}
		}
	}
}

}  // namespace pp
