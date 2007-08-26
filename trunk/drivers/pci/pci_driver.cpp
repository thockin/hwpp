#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "pci_driver.h"
#include "pci_io.h"
#include "pci_binding.h"

int force_pci_driver_linkage;
static const pci_driver the_pci_driver;

pci_driver::pci_driver()
{
	register_driver(this);
}

pci_driver::~pci_driver()
{
}

string
pci_driver::name() const
{
	return "pci";
}

pp_binding_ptr
pci_driver::new_binding(const std::vector<pp_regaddr> &args) const
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

//FIXME: move this out of drivers/pci to devices/pci
extern pp_space_ptr pci_generic_space(pp_const_binding_ptr binding_ptr,
    const pp_platform *platform);

int
pci_driver::discover(pp_platform *platform) const
{
	int ndevs = 0;
	std::vector<pci_address> addresses;
	std::vector<pci_address>::iterator it;

	/* find all PCI addresses */
	pci_io::enumerate(&addresses);

	/* for each PCI device in the system */
	it = addresses.begin();
	while (it != addresses.end()) {
		/* check if anyone registered for this vendor/device */
		const discovery_request *dr = find_discovery_request(*it);
		if (dr) {
			/* call the callback */
			std::vector<pp_regaddr> args;
			args.push_back(it->segment);
			args.push_back(it->bus);
			args.push_back(it->device);
			args.push_back(it->function);
			dr->function(platform, this, args);
		} else {
			/* create a generic PCI device */
			pp_binding_ptr binding = new_pci_binding(*it);
			pp_space_ptr space = pci_generic_space(
			    binding, platform);

			/* add it to the platform */
			platform->add_space(to_string(*it), space);
		}
		it++;
		ndevs++;
	}

	return ndevs;
}

void
pci_driver::register_discovery(const std::vector<pp_regaddr> &args,
    discovery_callback function)
{
	if (args.size() != 2) {
		throw pp_driver_args_error(
		    "PCI discovery: <vendor, device>");
	}

	discovery_request dr;
	dr.vendor = args[0];
	dr.device = args[1];
	dr.function = function;
	m_callbacks.push_back(dr);
}

const pci_driver::discovery_request *
pci_driver::find_discovery_request(const pci_address &addr) const
{
	pci_io dev(addr);
	uint16_t vid = dev.read(0, BITS16);
	uint16_t did = dev.read(2, BITS16);

	for (size_t i = 0; i < m_callbacks.size(); i++) {
		if (m_callbacks[i].vendor == vid
		 && m_callbacks[i].device == did) {
			return &m_callbacks[i];
		}
	}
	return NULL;
}
