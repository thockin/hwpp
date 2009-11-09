/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_PCI_PCI_BINDING_H__
#define PP_DRIVERS_PCI_PCI_BINDING_H__

#include "pp/pp.h"
#include "pp/binding.h"
#include "pp/driver.h"
#include "pp/util/filesystem.h"
#include <iostream>

namespace pp { 

/*
 * PciAddress
 */
struct PciAddress
{
	/* constructors */
	PciAddress(unsigned s, unsigned b, unsigned d, unsigned f)
	    : segment(s), bus(b), device(d), function(f)
	{
	}
	PciAddress(unsigned b, unsigned d, unsigned f)
	    : segment(0), bus(b), device(d), function(f)
	{
	}
	unsigned segment;
	unsigned bus;
	unsigned device;
	unsigned function;
};

inline bool
operator<(const PciAddress &left, const PciAddress &right)
{
	if (left.segment != right.segment) {
		return (left.segment < right.segment);
	}
	if (left.bus != right.bus) {
		return (left.bus < right.bus);
	}
	if (left.device != right.device) {
		return (left.device < right.device);
	}
	return (left.function < right.function);
}

inline std::ostream &
operator<<(std::ostream& out, const PciAddress &addr)
{
	out << "pci<"
	  << addr.segment << ","
	  << addr.bus << ","
	  << addr.device << ","
	  << addr.function << ">";

	return out;
}

/*
 * PciIo - Linux-specific PCI IO
 */
class PciIo
{
    public:
	PciIo(const PciAddress &address, const string &devdir = "");
	~PciIo();

	Value
	read(const Value &address, const BitWidth width) const;

	void
	write(const Value &address, const BitWidth width,
	    const Value &value) const;

	const PciAddress &
	address() const;

	static void
	enumerate(std::vector<PciAddress> *addresses);

    private:
	PciAddress m_address;
	filesystem::FilePtr m_file;

	void
	do_io_error(const string &str) const;

	void
	open_device(string devdir);

	void
	check_width(BitWidth width) const;

	void
	check_bounds(const Value &offset, size_t bytes) const;

	void
	seek(const Value &offset) const;
};

/*
 * PciBinding - PCI binding for register spaces
 */
typedef SimpleBinding<PciIo, PciAddress> PciBinding;

#define new_pci_binding(...) BindingPtr(new PciBinding(__VA_ARGS__))

}  // namespace pp

#endif // PP_DRIVERS_PCI_PCI_BINDING_H__
