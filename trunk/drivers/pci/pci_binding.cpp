#include <stdint.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "pp.h"
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
	m_file->close();
}

const pci_address &
pci_io::address() const
{
	return m_address;
}

pp_value
pci_io::read(const pp_regaddr &address, const pp_bitwidth width) const
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
	    boost::format("unsupported register width %d")
	    %width));
}

void
pci_io::write(const pp_regaddr &address, const pp_bitwidth width,
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
	    boost::format("unsupported register width %d")
	    %width));
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

pp_driver_io_error
pci_io::do_io_error(const string &str) const
{
	return pp_driver_io_error(to_string(m_address) + ": " + str);
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
		throw do_io_error("can't open PCI");
	}
}

void
pci_io::seek(const pp_regaddr &offset) const
{
	/* we only support 4KB config space */
	if (offset >= 4096) {
		throw do_io_error(to_string(
		    boost::format("can't access register 0x%x")
		    %offset));
	}

	m_file->seek(offset, SEEK_SET);
}

template<typename Tdata>
pp_value
pci_io::do_read(const pp_regaddr &offset) const
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
pci_io::do_write(const pp_regaddr &offset, const pp_value &value) const
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
		pci_address addr;
		std::istringstream iss(de->name());
		char c;
		iss >> std::hex
		    >> addr.segment
		    >> c
		    >> addr.bus
		    >> c
		    >> addr.device
		    >> c
		    >> addr.function;

		addresses->push_back(addr);
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
				pci_address addr;
				addr.segment = 0;

				std::istringstream iss(subde->name());
				char c;
				iss >> std::hex
				    >> addr.bus
				    >> c
				    >> addr.device
				    >> c
				    >> addr.function;

				addresses->push_back(addr);
			}
		}
	}
}
