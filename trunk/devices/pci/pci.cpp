#include "pp.h"
#include "printfxx.h"
#include <vector>
#include "fake_language.h"
#include "generic_device.h"

void
pci_datatypes_init()
{
	/* pci_vendor_t */
	ENUM("pci_vendor_t",
		KV("unknown", 0),
		KV("intel", 0x8086),
		KV("amd", 0x1022),
		KV("nvidia", 0x10de),
		KV("broadcom", 0x14e4),
		KV("silicon_image", 0x1095),
		KV("marvell", 0x11ab),
		KV("google", 0x1ae0),
		KV("plx", 0x10b5));
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
			KV("non_vga_dev", 0x00),
			KV("vga_dev", 0x01));
	ENUM("pci_subclass_mass_storage",
			KV("scsi_ctrlr", 0x00),
			KV("ide_ctrlr", 0x01),
			KV("floppy_ctrlr", 0x02),
			KV("ipi_bus_ctrlr", 0x03),
			KV("raid_ctrlr", 0x04),
			KV("ata_ctrlr", 0x05),
			KV("sata_ctrlr", 0x06),
			KV("sas_ctrlr", 0x06),
			KV("other", 0x80));
	ENUM("pci_subclass_network",
			KV("ethernet_ctrlr", 0x00),
			KV("token_ring_ctrlr", 0x01),
			KV("fddi_ctrlr", 0x02),
			KV("atm_ctrlr", 0x03),
			KV("isdn_ctrlr", 0x04),
			KV("other", 0x80));
	ENUM("pci_subclass_display",
			KV("vga_ctrlr", 0x00),
			KV("xga_ctrlr", 0x01),
			KV("three_d_ctrlr", 0x02),
			KV("other", 0x80));
	ENUM("pci_subclass_multimedia",
			KV("video_dev", 0x00),
			KV("audio_dev", 0x01),
			KV("computer_telephony_dev", 0x02),
			KV("other", 0x80));
	ENUM("pci_subclass_memory",
			KV("ram", 0x00),
			KV("flash", 0x01),
			KV("other", 0x80));
	ENUM("pci_subclass_bridge",
			KV("host_bridge", 0x00),
			KV("isa_bridge", 0x01),
			KV("eisa_bridge", 0x02),
			KV("mca_bridge", 0x03),
			KV("pci_bridge", 0x04),
			KV("pcmcia_bridge", 0x05),
			KV("nubus_bridge", 0x06),
			KV("cardbus_bridge", 0x07),
			KV("raceway_bridge", 0x08),
			KV("other", 0x80));
	//FIXME: classes 0x07-0x11

	// pci_bar_t
	ENUM("pci_bar_t",
			KV("bar0", 0),
			KV("bar1", 1),
			KV("bar2", 2),
			KV("bar3", 3),
			KV("bar4", 4),
			KV("bar5", 5));

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
			KV("msix", 0x11),
			KV("sata", 0x12),
			KV("af", 0X13));

	// pcie_capability_t
	ENUM("pcie_capability_t",
			KV("unknown", 0),
			KV("aer", 0x01),
			KV("vchannel", 0x02),
			KV("serial_num", 0x03),
			KV("power_budget", 0x04),
			KV("root_link_decl", 0x05),
			KV("root_internal_link_ctrl", 0x06),
			KV("root_event_collector_endpoint_assoc", 0x07),
			KV("multi_func_vchannel", 0x08),
			KV("vc2", 0x09),
			KV("rcrb_header", 0x0a),
			KV("vendor_specific", 0x0b),
			KV("acs", 0x0d));
}

static void
pci_discovered(const std::vector<pp_value> &args)
{
	pp_value seg = args[0];
	pp_value bus = args[1];
	pp_value dev = args[2];
	pp_value func = args[3];

	string name = sprintfxx("pci.%d.%d.%d.%d", seg, bus, dev, func);
	PCI_SCOPE(name, seg, bus, dev, func);
	CLOSE_SCOPE();
}

class pci_discovery {
    public:
	explicit
	pci_discovery()
	{
		// register a catch-all discovery rule
		std::vector<pp_value> args;
		pp_register_discovery("pci", args, pci_discovered);
	}
};

static pci_discovery the_pci_discovery;
