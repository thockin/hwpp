/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_PCI_PCI_DRIVER_H__
#define PP_DRIVERS_PCI_PCI_DRIVER_H__

#include "pp.h"
#include "driver.h"
#include "pci_binding.h"

namespace pp { 

/*
 * PciDriver - PCI driver plugin.
 */
class PciDriver: public Driver
{
    public:
	PciDriver();
	virtual ~PciDriver();

	/*
	 * PciDriver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * PciDriver::new_binding(args)
	 *
	 * Create a new Binding.
	 *
	 * Throws: Driver::ArgsError
	 */
	virtual BindingPtr
	new_binding(const std::vector<Value> &args) const;

	/*
	 * PciDriver::discover()
	 *
	 * Discover devices owned by this driver, and add them to the
	 * platform.
	 */
	virtual void
	discover() const;

	/*
	 * PciDriver::register_discovery(args, function)
	 *
	 * Register a DiscoveryCallback which will be called when the
	 * driver's discover() routine finds a device that matches args.
	 * The contents af the args vector depends on the specific driver.
	 */
	virtual void
	register_discovery(const std::vector<Value> &args,
			DiscoveryCallback function);

    private:
	struct DiscoveryRequest {
		uint16_t vendor;
		uint16_t device;
		DiscoveryCallback function;
	};

	const DiscoveryRequest *
	find_discovery_request(const PciAddress &addr) const;

	std::vector<DiscoveryRequest> m_callbacks;
	DiscoveryCallback m_catchall;
};

#define new_pci_driver(...) DriverPtr(new PciDriver(__VA_ARGS__))

}  // namespace pp

#endif // PP_DRIVERS_PCI_PCI_DRIVER_H__
