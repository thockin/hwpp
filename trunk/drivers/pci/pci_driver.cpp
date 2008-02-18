#include "pp.h"
#include "drivers.h"
#include "pp_datatypes.h"
#include "pci_driver.h"
#include "pci_binding.h"

// this forces linkage and avoids the static initialization order fiasco
void
load_pci_driver()
{
	static pci_driver the_pci_driver;
}

pci_driver::pci_driver()
{
	pp_register_driver(this);
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
pci_driver::new_binding(const std::vector<pp_value> &args) const
{
	if (args.size() < 3 || args.size() > 4) {
		throw pp_driver_args_error(
		    "pci<>: <seg=0, bus, dev, func>");
	}

	pp_value seg;
	int i = 0;
	if (args.size() == 3) {
		seg = 0;
	} else {
		seg = args[i++];
	}
	pp_value bus = args[i++];
	pp_value dev = args[i++];
	pp_value func = args[i++];

	if (seg < 0 || seg > UINT32_MAX) {
		throw pp_driver_args_error("pci<>: invalid segment");
	}
	if (bus < 0 || bus >= 256) {
		throw pp_driver_args_error("pci<>: invalid bus");
	}
	if (dev < 0 || dev >= 32) {
		throw pp_driver_args_error("pci<>: invalid device");
	}
	if (func < 0 || func >= 8) {
		throw pp_driver_args_error("pci<>: invalid function");
	}
	return new_pci_binding(pci_address(seg.get_uint(), bus.get_uint(),
		dev.get_uint(), func.get_uint()));
}

void
pci_driver::discover(pp_scope *platform) const
{
	std::vector<pci_address> addresses;

	/* find all PCI addresses */
	pci_io::enumerate(&addresses);

	/* for each PCI device in the system */
	std::vector<pci_address>::iterator it;
	for (it = addresses.begin(); it != addresses.end(); it++) {
		/* check if anyone registered for this vendor/device */
		const discovery_request *dr = find_discovery_request(*it);
		std::vector<pp_value> args;
		args.push_back(it->segment);
		args.push_back(it->bus);
		args.push_back(it->device);
		args.push_back(it->function);
		if (dr) {
			/* call the callback */
			dr->function(args);
		} else if (m_catchall) {
			/* call the catchall */
			m_catchall(args);
		}
	}
}

void
pci_driver::register_discovery(const std::vector<pp_value> &args,
    discovery_callback function)
{
	if (args.size() == 0) {
		if (m_catchall) {
			throw pp_driver_args_error(
			    "pci discovery: catchall already defined");
		}
		m_catchall = function;
		return;
	}

	if (args.size() != 2) {
		throw pp_driver_args_error(
		    "pci discovery: <vendor, device>");
	}

	discovery_request dr;
	dr.vendor = args[0].get_uint();
	dr.device = args[1].get_uint();
	dr.function = function;
	m_callbacks.push_back(dr);
}

const pci_driver::discovery_request *
pci_driver::find_discovery_request(const pci_address &addr) const
{
	pci_io dev(addr);
	uint16_t vid = dev.read(0, BITS16).get_uint();
	uint16_t did = dev.read(2, BITS16).get_uint();

	for (size_t i = 0; i < m_callbacks.size(); i++) {
		if (m_callbacks[i].vendor == vid
		 && m_callbacks[i].device == did) {
			return &m_callbacks[i];
		}
	}
	return NULL;
}
