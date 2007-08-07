/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PCI_DRIVER_H__
#define PP_PCI_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
#include "pci_io.h"
#include "pci_binding.h"

/*
 * pci_driver - PCI driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class pci_driver: public pp_driver
{
    public:
    	//FIXME: should be pp_global or pp_environ or something
	explicit pci_driver(pp_platform *platform) {}
	virtual ~pci_driver() {}

	/*
	 * pci_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const
	{
		return "pci";
	}

	/*
	 * pci_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_regaddr> &args) const
	{
		int seg, bus, dev, func;
		int i = 0;

		if (args.size() < 3 || args.size() > 4) {
			throw pp_driver_args_error(
			    "PCI binding: <seg?, bus, dev, func>");
		}

		if (args.size() == 3) {
			seg = 0;
		} else {
			seg = args[i++];
		}
		bus = args[i++];
		dev = args[i++];
		func = args[i++];

		return new_pci_binding(pci_address(seg, bus, dev, func));
	}

	/*
	 * pci_driver::enumerate(platform)
	 *
	 * Enumerate devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual int
	enumerate(pp_platform *platform) const
	{
		int ndevs = 0;
		std::vector<pci_address> addresses;
		std::vector<pci_address>::iterator it;

		/* find all PCI addresses */
		addresses = pci_io::enumerate();

		/* for each PCI device in the system */
		it = addresses.begin();
		while (it != addresses.end()) {
			std::cout << it->segment << ":"
			     << it->bus << ":"
			     << it->device << "."
			     << it->function << std::endl;
			/* check if anyone registered for this vendor/device */
			//FIXME:
			//if (something) {
				/* call the callback */
			//} else {
				/* create a generic PCI device */
				pp_binding_ptr binding = new_pci_binding(*it);
				//pp_space_ptr space = pci_generic_space(binding);
				pp_space_ptr space = new_pp_space(binding);

				/* add it to the platform */
				platform->add_space(to_string(*it), space);
			//}
			it++;
			ndevs++;
		}

		return ndevs;
	}
};

#define new_pci_driver(...) pp_driver_ptr(new pci_driver(__VA_ARGS__))

#endif // PP_PCI_DRIVER_H__
