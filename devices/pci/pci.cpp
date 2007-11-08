#include "device_init.h"
#include "utils.h"
#include "drivers.h"
#include "pp_scope.h"
#include "pp_driver.h"
#include "pp_datatypes.h"
#include "generic_device.h"

void
pci_datatypes_init(pp_scope *platform)
{
	/* pci_vendor_t */
	ENUM("pci_vendor_t",
		{"unknown", 0},
		{"Intel", 0x8086},
		{"AMD", 0x1022},
		{"NVidia", 0x10de},
		{"Broadcom", 0x14e4},
		{"Silicon Image", 0x1095});
		//FIXME: default?

	/* pci_class_t */
	ENUM("pci_class_t",
		//TODO: incomplete list
		{"pre_classcode", 0x00},
		{"mass_storage", 0x01},
		{"network", 0x02},
		{"display", 0x03},
		{"multimedia", 0x04},
		{"memory", 0x05},
		{"bridge", 0x06},
		{"simple_comm", 0x07},
		{"base_system", 0x08},
		{"input", 0x09},
		{"docking", 0x0a},
		{"processor", 0x0b},
		{"serial", 0x0c},
		{"wireless", 0x0d},
		{"intelligent_io", 0x0e},
		{"satellite", 0x0f},
		{"crypto", 0x10},
		{"dsp", 0x11},
		{"other", 0xff});
		//FIXME: default?

	/* pci_subclass_*_t */
	//FIXME: use pci_subclass_t[$class] notation?  What about
	//unknown classes?
	//FIXME: _t notation?
	ENUM("pci_subclass_pre_classcode",
			{"non-VGA device", 0x00},
			{"VGA device", 0x01});
	ENUM("pci_subclass_mass_storage",
			{"SCSI controller", 0x00},
			{"IDE controller", 0x01},
			{"floppy controller", 0x02},
			{"IPI bus controller", 0x03},
			{"RAID controller", 0x04},
			{"ATA controller with ADMA", 0x05},
			{"SATA controller", 0x06},
			{"SAS controller", 0x06},
			{"other", 0x80});
	ENUM("pci_subclass_network",
			{"ethernet controller", 0x00},
			{"token Ring controller", 0x01},
			{"FDDI controller", 0x02},
			{"ATM controller", 0x03},
			{"ISDN controller", 0x04},
			{"other", 0x80});
	ENUM("pci_subclass_display",
			{"VGA controller", 0x00},
			{"XGA controller", 0x01},
			{"3D controller", 0x02},
			{"other", 0x80});
	ENUM("pci_subclass_multimedia",
			{"video device", 0x00},
			{"audio device", 0x01},
			{"computer telephony device", 0x02},
			{"other", 0x80});
	ENUM("pci_subclass_memory",
			{"RAM", 0x00},
			{"flash", 0x01},
			{"other", 0x80});
	ENUM("pci_subclass_bridge",
			{"host bridge", 0x00},
			{"ISA bridge", 0x01},
			{"EISA bridge", 0x02},
			{"MCA bridge", 0x03},
			{"PCI bridge", 0x04},
			{"PCMCIA bridge", 0x05},
			{"NuBus bridge", 0x06},
			{"CardBus bridge", 0x07},
			{"RACEway bridge", 0x08},
			{"other", 0x80});
	//FIXME: classes 0x07-0x11

	// pci_capability_t
	ENUM("pci_capability_t",
			{"unknown", 0},
			{"power_mgmt", 0x01},
			{"agp", 0x02},
			{"vpd", 0x03},
			{"slot_id", 0x04},
			{"msi", 0x05},
			{"hot_swap", 0x06},
			{"pcix", 0x07},
			{"ht", 0x08},
			{"vendor", 0x09},
			{"usb2_dbg_port", 0x0a},
			{"resource_ctrl", 0x0b},
			{"hot_plug", 0x0c},
			{"ssid", 0x0d},
			{"agp8x", 0x0e},
			{"secure", 0x0f},
			{"pcie", 0x10},
			{"msix", 0x11});
}

static void
pci_discovered(pp_scope *platform, const pp_driver *driver,
		const std::vector<pp_regaddr> &args)
{
	pp_const_binding_ptr bind_ptr = driver->new_binding(args);
	OPEN_SCOPE(string("@")+bind_ptr->to_string(), bind_ptr);
	pci_generic_device();
}

class pci_discovery {
    public:
	explicit
	pci_discovery()
	{
		std::vector<pp_regaddr> args;
		register_discovery("pci", args, pci_discovered);
	}
};

static pci_discovery the_pci_discovery;
