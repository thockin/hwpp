#include "lang_stuff.h"
#include "utils.h"
#include "pp_platform.h"
#include "pp_datatypes.h"

void
pci_datatypes_init(pp_platform *platform)
{
	pp_enum_ptr e;

	/* pci_vendor_t */
	e = new_pp_enum();
	e->add_value("Intel", 0x8086);
	e->add_value("AMD", 0x1022);
	e->add_value("NVidia", 0x10de);
	e->add_value("Broadcom", 0x14e4);
	e->add_value("Silicon Image", 0x1095);
	e->set_default("Unknown");
	platform->add_datatype("pci_vendor_t", e);

	/* pci_class_t */
	e = new_pp_enum();
	e->add_value("pre-classcode device", 0x00);
	e->add_value("mass storage controller", 0x01);
	e->add_value("network controller", 0x02);
	e->add_value("display controller", 0x03);
	e->add_value("multimedia device", 0x04);
	e->add_value("memory controller", 0x05);
	e->add_value("bridge device", 0x06);
	e->add_value("simple communication controller", 0x07);
	e->add_value("base system peripheral", 0x08);
	e->add_value("input device", 0x09);
	e->add_value("docking station", 0x0a);
	e->add_value("processor", 0x0b);
	e->add_value("serial bus controller", 0x0c);
	e->add_value("wireless controller", 0x0d);
	e->add_value("intelligent I/O controller", 0x0e);
	e->add_value("satellite communication controller", 0x0f);
	e->add_value("encryption/decryption controller", 0x10);
	e->add_value("data acquisition / signal processing controller", 0x11);
	//TODO: incomplete list
	e->set_default("unknown class");
	platform->add_datatype("pci_class_t", e);
}
