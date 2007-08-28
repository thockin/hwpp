#include "device_init.h"
#include "utils.h"
#include "pp_platform.h"
#include "pp_datatypes.h"

void
pci_datatypes_init(pp_platform *platform)
{
	pp_enum_ptr e;

	//FIXME: for now these will do.  a global add_datatype() template
	//function would be better.

	/* pci_vendor_t */
	e = ANON_ENUM(
		{"Intel", 0x8086},
		{"AMD", 0x1022},
		{"NVidia", 0x10de},
		{"Broadcom", 0x14e4},
		{"Silicon Image", 0x1095});
	e->set_default("Unknown");
	platform->add_datatype("pci_vendor_t", e);

	/* pci_class_t */
	e = ANON_ENUM(
		//TODO: incomplete list
		{"pre-classcode device", 0x00},
		{"mass storage controller", 0x01},
		{"network controller", 0x02},
		{"display controller", 0x03},
		{"multimedia device", 0x04},
		{"memory controller", 0x05},
		{"bridge device", 0x06},
		{"simple communication controller", 0x07},
		{"base system peripheral", 0x08},
		{"input device", 0x09},
		{"docking station", 0x0a},
		{"processor", 0x0b},
		{"serial bus controller", 0x0c},
		{"wireless controller", 0x0d},
		{"intelligent I/O controller", 0x0e},
		{"satellite communication controller", 0x0f},
		{"encryption/decryption controller", 0x10},
		{"data acquisition / signal processing controller", 0x11});
	e->set_default("unknown class");
	platform->add_datatype("pci_class_t", e);
}
