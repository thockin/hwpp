/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_PCI_PCI_BINDING_H__
#define PP_DRIVERS_PCI_PCI_BINDING_H__

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

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

inline bool
operator<(const pci_address &left, const pci_address &right)
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
 * pci_io - Linux-specific PCI IO
 */
class pci_io
{
    public:
	pci_io(const pci_address &address, const string &devdir = "");
	~pci_io();

	pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const;

	void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const;

	const pci_address &
	address() const;

	static void
	enumerate(std::vector<pci_address> *addresses);

    private:
	pci_address m_address;
	fs::file_ptr m_file;

	pp_driver_io_error
	do_io_error(const string &str) const;

	void
	open_device(string devdir);

	void
	seek(const pp_regaddr offset) const;

	template<typename Tdata>
	pp_value
	do_read(const pp_regaddr offset) const;

	template<typename Tdata>
	void
	do_write(const pp_regaddr offset, const pp_value value) const;
};

/*
 * pci_binding - PCI binding for register spaces
 *
 * Constructors:
 *	(pci_address address)
 */
typedef simple_binding<pci_io, pci_address> pci_binding;

#define new_pci_binding(...) pp_binding_ptr(new pci_binding(__VA_ARGS__))

#endif // PP_DRIVERS_PCI_PCI_BINDING_H__
