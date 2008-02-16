#include "pp.h"
#include <vector>
#include "device_init.h"
#include "utils.h"
#include "drivers.h"
#include "pp_scope.h"
#include "pp_driver.h"
#include "pp_datatypes.h"
#include "generic_device.h"

void
pci_datatypes_init()
{
	/* pci_vendor_t */
	ENUM("pci_vendor_t",
		KV("unknown", 0),
		KV("Intel", 0x8086),
		KV("AMD", 0x1022),
		KV("NVidia", 0x10de),
		KV("Broadcom", 0x14e4),
		KV("Silicon Image", 0x1095));
		//FIXME: default?

	/* pci_class_t */
	ENUM("pci_class_t",
		//TODO: incomplete list
		KV("pre_classcode", 0x00),
		KV("mass_storage", 0x01),
		KV("network", 0x02),
		KV("display", 0x03),
		KV("multimedia", 0x04),
		KV("memory", 0x05),
		KV("bridge", 0x06),
		KV("simple_comm", 0x07),
		KV("base_system", 0x08),
		KV("input", 0x09),
		KV("docking", 0x0a),
		KV("processor", 0x0b),
		KV("serial", 0x0c),
		KV("wireless", 0x0d),
		KV("intelligent_io", 0x0e),
		KV("satellite", 0x0f),
		KV("crypto", 0x10),
		KV("dsp", 0x11),
		KV("other", 0xff));
		//FIXME: default?

	/* pci_subclass_*_t */
	//FIXME: use pci_subclass_t[$class] notation?  What about
	//unknown classes?
	//FIXME: _t notation?
	ENUM("pci_subclass_pre_classcode",
			KV("non-VGA device", 0x00),
			KV("VGA device", 0x01));
	ENUM("pci_subclass_mass_storage",
			KV("SCSI controller", 0x00),
			KV("IDE controller", 0x01),
			KV("floppy controller", 0x02),
			KV("IPI bus controller", 0x03),
			KV("RAID controller", 0x04),
			KV("ATA controller with ADMA", 0x05),
			KV("SATA controller", 0x06),
			KV("SAS controller", 0x06),
			KV("other", 0x80));
	ENUM("pci_subclass_network",
			KV("ethernet controller", 0x00),
			KV("token Ring controller", 0x01),
			KV("FDDI controller", 0x02),
			KV("ATM controller", 0x03),
			KV("ISDN controller", 0x04),
			KV("other", 0x80));
	ENUM("pci_subclass_display",
			KV("VGA controller", 0x00),
			KV("XGA controller", 0x01),
			KV("3D controller", 0x02),
			KV("other", 0x80));
	ENUM("pci_subclass_multimedia",
			KV("video device", 0x00),
			KV("audio device", 0x01),
			KV("computer telephony device", 0x02),
			KV("other", 0x80));
	ENUM("pci_subclass_memory",
			KV("RAM", 0x00),
			KV("flash", 0x01),
			KV("other", 0x80));
	ENUM("pci_subclass_bridge",
			KV("host bridge", 0x00),
			KV("ISA bridge", 0x01),
			KV("EISA bridge", 0x02),
			KV("MCA bridge", 0x03),
			KV("PCI bridge", 0x04),
			KV("PCMCIA bridge", 0x05),
			KV("NuBus bridge", 0x06),
			KV("CardBus bridge", 0x07),
			KV("RACEway bridge", 0x08),
			KV("other", 0x80));
	//FIXME: classes 0x07-0x11

	// pci_capability_t
	ENUM("pci_capability_t",
			KV("unknown", 0),
			KV("power_mgmt", 0x01),
			KV("agp", 0x02),
			KV("vpd", 0x03),
			KV("slot_id", 0x04),
			KV("msi", 0x05),
			KV("hot_swap", 0x06),
			KV("pcix", 0x07),
			KV("ht", 0x08),
			KV("vendor", 0x09),
			KV("usb2_dbg_port", 0x0a),
			KV("resource_ctrl", 0x0b),
			KV("hot_plug", 0x0c),
			KV("ssid", 0x0d),
			KV("agp8x", 0x0e),
			KV("secure", 0x0f),
			KV("pcie", 0x10),
			KV("msix", 0x11));
}

static void
pci_discovered(const std::vector<pp_value> &args)
{
	pp_value seg = args[0];
	pp_value bus = args[1];
	pp_value dev = args[2];
	pp_value func = args[3];

	string name = "pci."
	    + to_string(seg) + "."
	    + to_string(bus) + "."
	    + to_string(dev) + "."
	    + to_string(func);

	OPEN_SCOPE(name, BIND("pci", args));
	pci_generic_device();
	CLOSE_SCOPE();
}

class pci_discovery {
    public:
	explicit
	pci_discovery()
	{
		// register a catch-all discovery rule
		std::vector<pp_value> args;
		register_discovery("pci", args, pci_discovered);
	}
};

static pci_discovery the_pci_discovery;
