#include "hwpp.h"
#include "util/printfxx.h"
#include "datatype_types.h"
#include "pci_driver.h"
#include "pci_binding.h"

namespace hwpp { 

// this forces linkage and avoids the static initialization order fiasco
Driver *
load_pci_driver()
{
	static PciDriver the_driver;
	return &the_driver;
}

PciDriver::PciDriver()
{
}

PciDriver::~PciDriver()
{
}

string
PciDriver::name() const
{
	return "pci";
}

BindingPtr
PciDriver::new_binding(const std::vector<Value> &args) const
{
	if (args.size() < 3 || args.size() > 4) {
		throw Driver::ArgsError(
		    "pci<>: <seg=0, bus, dev, func>");
	}

	Value seg;
	int i = 0;
	if (args.size() == 3) {
		seg = 0;
	} else {
		seg = args[i++];
	}
	Value bus = args[i++];
	Value dev = args[i++];
	Value func = args[i++];

	if (seg < 0 || seg > UINT32_MAX) {
		throw Driver::ArgsError("pci<>: invalid segment");
	}
	if (bus < 0 || bus >= 256) {
		throw Driver::ArgsError("pci<>: invalid bus");
	}
	if (dev < 0 || dev >= 32) {
		throw Driver::ArgsError("pci<>: invalid device");
	}
	if (func < 0 || func >= 8) {
		throw Driver::ArgsError("pci<>: invalid function");
	}
	return new_pci_binding(PciAddress(seg.as_uint(), bus.as_uint(),
		dev.as_uint(), func.as_uint()));
}

void
PciDriver::discover() const
{
	std::vector<PciAddress> addresses;

	/* find all PCI addresses */
	PciIo::enumerate(&addresses);

	/* for each PCI device in the system */
	std::vector<PciAddress>::iterator it;
	for (it = addresses.begin(); it != addresses.end(); it++) {
		/* check if anyone registered for this vendor/device */
		const DiscoveryRequest *dr = find_discovery_request(*it);
		if (dr && dr->function == NULL) {
			continue;
		}

		/* call the callback */
		std::vector<Value> args;
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
PciDriver::register_discovery(const std::vector<Value> &args,
    DiscoveryCallback function)
{
	if (args.size() == 0) {
		if (m_catchall) {
			throw Driver::ArgsError(
			    "pci discovery: catchall already defined");
		}
		m_catchall = function;
		return;
	}

	if (args.size() != 2) {
		throw Driver::ArgsError("pci discovery: <vendor, device>");
	}

	DiscoveryRequest dr;
	dr.vendor = args[0].as_uint();
	dr.device = args[1].as_uint();
	dr.function = function;
	m_callbacks.push_back(dr);
}

const PciDriver::DiscoveryRequest *
PciDriver::find_discovery_request(const PciAddress &addr) const
{
	PciIo dev(addr);
	uint16_t vid = dev.read(0, BITS16).as_uint();
	uint16_t did = dev.read(2, BITS16).as_uint();

	DTRACE(TRACE_DISCOVERY, sprintfxx("discovery: pci 0x%04x 0x%04x",
	                                  vid, did));

	for (size_t i = 0; i < m_callbacks.size(); i++) {
		if (m_callbacks[i].vendor == vid
		 && m_callbacks[i].device == did) {
			DTRACE(TRACE_DISCOVERY,
			       sprintfxx("discovery: pci found match for %s",
			                 addr));
			return &m_callbacks[i];
		}
	}

	DTRACE(TRACE_DISCOVERY, sprintfxx("discovery: pci no match for %s",
	                                  addr));
	return NULL;
}

}  // namespace hwpp
