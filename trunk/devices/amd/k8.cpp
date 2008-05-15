//
// This file has been developed purely from publicly available
// documentation.
//
// Tim Hockin
// May, 2008
//
#include "pp.h"
#include "fake_language.h"
#include "devices/pci/generic_device.h"

static void
k8_ht_route(const string &name, pp_value address)
{
	OPEN_SCOPE(name);

	REG32("%ht_route", address);
	FIELD("RQRoute", "ht_route_t", BITS("%ht_route", 3, 0));
	FIELD("RPRoute", "ht_route_t", BITS("%ht_route", 11, 8));
	FIELD("BCRoute", "ht_route_t", BITS("%ht_route", 19, 16));

	CLOSE_SCOPE();
}

static void
k8_ht_config()
{
	//
	// HT routing table registers
	//

	OPEN_SCOPE("ht_route");

	// each HT route is a 4 bit bitmask
	BITMASK("ht_route_t",
			KV("self",  0),
			KV("link0", 1),
			KV("link1", 2),
			KV("link2", 3));

	k8_ht_route("node0", 0x40);
	k8_ht_route("node1", 0x44);
	k8_ht_route("node2", 0x48);
	k8_ht_route("node3", 0x4c);
	k8_ht_route("node4", 0x50);
	k8_ht_route("node5", 0x54);
	k8_ht_route("node6", 0x58);
	k8_ht_route("node7", 0x5c);

	CLOSE_SCOPE();

	//
	// Node ID register
	//
	REG32("%node_id", 0x60);
	FIELD("NodeId", "int_t", BITS("%node_id", 2, 0));
	//FIXME: this should be a lambda +1
	FIELD("NodeCnt", "int_t", BITS("%node_id", 6, 4));
	FIELD("SbNode", "int_t", BITS("%node_id", 10, 8));
	FIELD("LkNode", "int_t", BITS("%node_id", 14, 12));
	//FIXME: this should be a lambda +1
	FIELD("CpuCnt", "int_t", BITS("%node_id", 19, 16));

	//
	// Unit ID register
	//
	REG32("%unit_id", 0x64);
	FIELD("C0Unit", "int_t", BITS("%unit_id", 1, 0));
	FIELD("C1Unit", "int_t", BITS("%unit_id", 3, 2));
	FIELD("MctUnit", "int_t", BITS("%unit_id", 5, 4));
	FIELD("HbUnit", "int_t", BITS("%unit_id", 7, 6));
	FIELD("SbLink", "int_t", BITS("%unit_id", 10, 8));

}

static void
k8_address_map()
{
}

static void
k8_dram_controller()
{
}

static void
k8_misc_control()
{
}

static void
k8_discovered(const std::vector<pp_value> &args)
{
	pp_value seg = args[0];
	pp_value bus = args[1];
	pp_value dev = args[2];
	pp_value func = args[3];

	OPEN_SCOPE("k8[]");

	/* function 0 */
	OPEN_SCOPE("ht_config", BIND("pci", ARGS(seg, bus, dev, func)));
	pci_generic_device();
	k8_ht_config();
	CLOSE_SCOPE();

	/* function 1 */
	OPEN_SCOPE("address_map", BIND("pci", ARGS(seg, bus, dev, func+1)));
	pci_generic_device();
	k8_address_map();
	CLOSE_SCOPE();

	/* function 2 */
	OPEN_SCOPE("dram_ctrl", BIND("pci", ARGS(seg, bus, dev, func+2)));
	pci_generic_device();
	k8_dram_controller();
	CLOSE_SCOPE();

	/* function 3 */
	OPEN_SCOPE("misc_ctrl", BIND("pci", ARGS(seg, bus, dev, func+3)));
	pci_generic_device();
	k8_misc_control();
	CLOSE_SCOPE();

	CLOSE_SCOPE(); // k8[]
}

class k8_discovery {
    public:
	explicit
	k8_discovery()
	{
		// register our 'keystone' device
		pp_register_discovery("pci", ARGS(0x1022, 0x1100),
				k8_discovered);
		// ignore other devices in this same device, we'll handle
		// them in k8_discovered()
		pp_register_discovery("pci", ARGS(0x1022, 0x1101));
		pp_register_discovery("pci", ARGS(0x1022, 0x1102));
		pp_register_discovery("pci", ARGS(0x1022, 0x1103));
	}
};

static k8_discovery the_k8_discovery;
