/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PCI_IO_H__
#define PP_PCI_IO_H__

#include <stdint.h>
#include <vector>

#include "pp.h"
#include "pp_binding.h"
#include "filesystem.h"

#define PCI_SYSFS_DIR	"/sys/bus/pci/devices"
#define PCI_PROCFS_DIR	"/proc/bus/pci"

class pci_io_error: public pp_binding_error
{
    public:
	explicit pci_io_error(const string &arg): pp_binding_error(arg) {}
};

class pci_nodev_error: public pp_binding_error
{
    public:
	explicit pci_nodev_error(const string &arg): pp_binding_error(arg) {}
};

/*
 * pci_address
 */
struct pci_address
{
	/* constructors */
	pci_address()
	    : segment(-1), bus(-1), device(-1), function(-1)
	{
	}
	pci_address(int s, int b, int d, int f)
	    : segment(s), bus(b), device(d), function(f)
	{
	}
	pci_address(int b, int d, int f)
	    : segment(0), bus(b), device(d), function(f)
	{
	}

	int segment;
	int bus;
	int device;
	int function;
};

inline std::ostream &
operator<<(std::ostream& out, const pci_address &addr)
{
	out << "pci<"
	  << addr.segment << ","
	  << addr.bus << ","
	  << addr.device << ","
	  << addr.function << ">";

	return out;
}

/*
 * Linux-specific PCI IO
 */
class pci_io
{
    public:
	/* constructors */
	explicit pci_io(int seg, int bus, int dev, int func)
	{
		open_device(seg, bus, dev, func);
	}
	explicit pci_io(const pci_address &addr)
	{
		open_device(addr.segment, addr.bus, addr.device, addr.function);
	}

	/* destructor */
	~pci_io()
	{
		m_file->close();
	}

	pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		switch (width) {
		    case BITS8:
			return do_read<uint8_t>(address);
		    case BITS16:
			return do_read<uint16_t>(address);
		    case BITS32:
			return do_read<uint32_t>(address);
		    default:
			break;
		}
		//FIXME: width should have been validated already
		//FIXME: format, more specific type
		throw pci_io_error("bad register width");
	}

	void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value)
	{
		switch (width) {
		    case BITS8:
			return do_write<uint8_t>(address, value);
		    case BITS16:
			return do_write<uint16_t>(address, value);
		    case BITS32:
			return do_write<uint32_t>(address, value);
		    default:
			break;
		}
		//FIXME: format, more specific type
		throw pci_io_error("bad register width");
	}

	static std::vector<pci_address>
	enumerate()
	{
		struct stat s;

		if (fs::direntry::is_dir(PCI_SYSFS_DIR)) {
			return enumerate_sysfs();
		}

		if (fs::direntry::is_dir(PCI_PROCFS_DIR)) {
			return enumerate_procfs();
		}

		return std::vector<pci_address>();
	}

    private:
	fs::file_ptr m_file;

	void
	open_device(int seg, int bus, int dev, int func)
	{
		string filename;

		/* try to open it through /sys */
		filename = to_string(
		    boost::format("%s/%04x:%02x:%02x.%d/config")
		    %PCI_SYSFS_DIR %seg %bus %dev %func);
		try {
			m_file = fs::file::open(filename, O_RDONLY);
			return;
		} catch (fs::not_found_error e) {
			/* do nothing yet */
		}

		/* fall back on /proc */
		filename = to_string(
		    boost::format("%s/%02x/%02x.%x")
		    %PCI_PROCFS_DIR %bus %dev %func);
		try {
			m_file = fs::file::open(filename, O_RDONLY);
			return;
		} catch (fs::not_found_error e) {
			/* the device seems to not exist */
			throw pci_nodev_error(to_string(
			    boost::format(
			    "device pci<%d,%d,%d,%d> does not exist")
			    %seg %bus %dev %func));
		}
	}

	void
	seek(const pp_regaddr address) const
	{
		int r;

		/* we only support 4KB config space */
		if (address >= 4096) {
			//FIXME: format, more specific type
			throw pci_io_error("bad address");
		}

		m_file->seek(address, SEEK_SET);
	}

	template<typename Tdata>
	pp_value
	do_read(const pp_regaddr address) const
	{
		seek(address);

		int r;
		Tdata data;
		r = m_file->read(&data, sizeof(data));
		if (r != sizeof(data)) {
			if (r == 0) {
				/*
				 * Work around /sys and /proc files which do
				 * not return proper errors when you read past
				 * the end of the "file".
				 */
				//FIXME: is this wrong?  throw EPERM?
				return PP_MASK(sizeof(Tdata)*CHAR_BIT);
			}
			//FIXME: format, more specific type
			throw pci_io_error("can't read value");
		}
		return data;
	}

	template<typename Tdata>
	void
	do_write(const pp_regaddr address, const pp_value value)
	{
		/* see if we are already open RW or can change to RW */
		if (m_file->mode() == O_RDONLY) {
			m_file->reopen(O_RDWR);
		}

		seek(address);
		Tdata data = value;
		//FIXME: is this wrong?  throw EPERM? like read()
		if (m_file->write(&data, sizeof(data)) != sizeof(data)) {
			//FIXME: format, more specific type
			throw pci_io_error("can't write value");
		}
	}

	static std::vector<pci_address>
	enumerate_sysfs()
	{
		std::vector<pci_address> result;

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

			result.push_back(addr);
		}

		return result;
	}

	static std::vector<pci_address>
	enumerate_procfs()
	{
		std::vector<pci_address> result;

		fs::directory_ptr dir = fs::directory::open(PCI_PROCFS_DIR);

		fs::direntry_ptr de;
		while ((de = dir->read())) {
			/* skip dotfiles */
			if (de->name()[0] == '.')
				continue;

			if (de->is_dir()) {
				string name(string(PCI_PROCFS_DIR)
				    + de->name());

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

					result.push_back(addr);
				}
			}
		}

		return result;
	}
};

#endif // PP_PCI_IO_H__
