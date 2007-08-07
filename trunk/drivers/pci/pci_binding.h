/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PCI_BINDING_H__
#define PP_PCI_BINDING_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

#include "pp_binding.h"
#include "pci_io.h"

/*
 * pci_binding - PCI binding for register spaces
 *
 * Constructors:
 *	(pci_address address, pci_io *io?)
 *
 * Notes:
 */
class pci_binding: public pp_binding
{
    public:
	explicit pci_binding(pci_address address, pci_io *io = NULL)
	    : m_address(address), m_io(io ? io : new pci_io(address))
	{
	}

	virtual ~pci_binding()
	{
	}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		return m_io->read(address, width);
	}

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const
	{
		return m_io->write(address, width, value);
	}

    private:
	pci_address m_address;
	boost::shared_ptr<pci_io> m_io;
};
typedef boost::shared_ptr<pci_binding> pci_binding_ptr;

#define new_pci_binding(...) pci_binding_ptr(new pci_binding(__VA_ARGS__))

#endif // PP_PCI_BINDING_H__
