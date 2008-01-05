/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_PCI_PCI_DRIVER_H__
#define PP_DRIVERS_PCI_PCI_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"
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
	pci_driver();
	virtual ~pci_driver();

	/*
	 * pci_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * pci_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_value> &args) const;

	/*
	 * pci_driver::discover(platform)
	 *
	 * Discover devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual void
	discover(pp_scope *platform) const;

	/*
	 * pci_driver::register_discovery(args, function)
	 *
	 * Register a discovery_callback which will be called when the
	 * driver's discover() routine finds a device that matches args.
	 * The contents af the args vector depends on the specific driver.
	 */
	virtual void
	register_discovery(const std::vector<pp_value> &args,
			discovery_callback function);

    private:
	struct discovery_request {
		uint16_t vendor;
		uint16_t device;
		discovery_callback function;
	};

	const discovery_request *
	find_discovery_request(const pci_address &addr) const;

	std::vector<discovery_request> m_callbacks;
	discovery_callback m_catchall;
};

#define new_pci_driver(...) pp_driver_ptr(new pci_driver(__VA_ARGS__))

#endif // PP_DRIVERS_PCI_PCI_DRIVER_H__
