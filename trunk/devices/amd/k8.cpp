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
#include "devices/cpuid/generic_device.h"

static void
k8_cpuid(const pp_value &node, const pp_value &ncores, const pp_value &cpu)
{
	cpuid_generic_device(node * ncores + cpu);
}

static void
k8_ht_route(const string &name, const pp_value &address)
{
	OPEN_SCOPE(name);

	REG32("%ht_route", address);
	FIELD("RQRoute", "ht_route_t", BITS("%ht_route", 3, 0));
	FIELD("RPRoute", "ht_route_t", BITS("%ht_route", 11, 8));
	FIELD("BCRoute", "ht_route_t", BITS("%ht_route", 19, 16));

	CLOSE_SCOPE();
}

static void
k8_ht_buffer_count(const pp_value &address)
{
	REG32("%ldt_buf_count", address);
	FIELD("request_count", "int_t", BITS("%ldt_buf_count", 3, 0));
	FIELD("posted_request_count", "int_t", BITS("%ldt_buf_count", 7, 4));
	FIELD("response_count", "int_t", BITS("%ldt_buf_count", 11, 8));
	FIELD("probe_count", "int_t", BITS("%ldt_buf_count", 15, 12));
	FIELD("request_data_count", "int_t", BITS("%ldt_buf_count", 18, 16));
	FIELD("posted_request_data_count", "int_t",
			BITS("%ldt_buf_count", 22, 20));
	FIELD("response_data_count", "int_t", BITS("%ldt_buf_count", 26, 24));
}

static void
k8_ht_bus_nums(const pp_value &address)
{
	REG32("%ldt_bus_nums", address);
	FIELD("primary_bus", "int_t", BITS("%ldt_bus_nums", 7, 0));
	FIELD("secondary_bus", "int_t", BITS("%ldt_bus_nums", 15, 8));
	FIELD("subordinate_bus", "int_t", BITS("%ldt_bus_nums", 23, 16));
}

static void
k8_ht_type(const pp_value &address)
{
	REG32("%ldt_type", address);
	FIELD("link_connected", "yesno_t", BITS("%ldt_type", 0));
	FIELD("init_complete", "yesno_t", BITS("%ldt_type", 1));
	FIELD("coherency", ANON_ENUM(
				KV("coherent", 0),
				KV("noncoherent", 1),
				KV("uniproc_coherent_nb", 2)),
			BITS("%ldt_type", 3, 2));
	FIELD("link_connect_pending", "yesno_t", BITS("%ldt_type", 4));
}

static void
k8_ht_regs(const string &name, const pp_value &address)
{
	OPEN_SCOPE(name);

	k8_ht_buffer_count(address);
	k8_ht_bus_nums(address+4);
	k8_ht_type(address+8);

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

	//
	// Link transaction control register
	//
	REG32("%link_txn_ctrl", 0x68);
	FIELD("DisRdBP", "yesno_t", BITS("%link_txn_ctrl", 0));
	FIELD("DisRdDwP", "yesno_t", BITS("%link_txn_ctrl", 1));
	FIELD("DisWrBP", "yesno_t", BITS("%link_txn_ctrl", 2));
	FIELD("DisWrDwP", "yesno_t", BITS("%link_txn_ctrl", 3));
	FIELD("DisMTS", "yesno_t", BITS("%link_txn_ctrl", 4));
	FIELD("Cpu1En", "yesno_t", BITS("%link_txn_ctrl", 5));
	FIELD("CPUReqPassPW", "yesno_t", BITS("%link_txn_ctrl", 6));
	FIELD("CPURdRspPassPW", "yesno_t", BITS("%link_txn_ctrl", 7));
	FIELD("DisPMemC", "yesno_t", BITS("%link_txn_ctrl", 8));
	FIELD("DisRmtPMemC", "yesno_t", BITS("%link_txn_ctrl", 9));
	FIELD("DisFillP", "yesno_t", BITS("%link_txn_ctrl", 10));
	FIELD("RespPassPW", "yesno_t", BITS("%link_txn_ctrl", 11));
	FIELD("BufRelPri", ANON_ENUM(
				KV("64", 0),
				KV("16", 1),
				KV("8",  2),
				KV("2",  3)),
			BITS("%link_txn_ctrl", 14, 13));
	FIELD("LimitCldtCfg", "yesno_t", BITS("%link_txn_ctrl", 15));
	FIELD("LintEn", "yesno_t", BITS("%link_txn_ctrl", 16));
	FIELD("ApicExtBrdCst", ANON_ENUM(
				KV("0x0f", 0),
				KV("0xff", 1)),
			BITS("%link_txn_ctrl", 17));
	FIELD("ApicExtId", "yesno_t", BITS("%link_txn_ctrl", 18));
	FIELD("ApicExtSpur", "yesno_t", BITS("%link_txn_ctrl", 19));
	FIELD("SeqIdSrcNodeEn", "yesno_t", BITS("%link_txn_ctrl", 20));
	FIELD("DsNpReqLmt", ANON_ENUM(
				KV("no limit", 0),
				KV("1", 1),
				KV("4", 2),
				KV("8", 3)),
			BITS("%link_txn_ctrl", 22, 21));
	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("InstallStateS", "yesno_t", BITS("%link_txn_ctrl", 23));
	}
	FIELD("DispRefModeEn", "yesno_t", BITS("%link_txn_ctrl", 24));
	FIELD("CHtExtAddrEn", "yesno_t", BITS("%link_txn_ctrl", 25));
	FIELD("EnPReqHiPriTblWlk", "yesno_t", BITS("%link_txn_ctrl", 31));

	//
	// Link initialization control register
	//
	REG32("%link_init_ctrl", 0x68);
	FIELD("RouteTblDis", "yesno_t", BITS("%link_init_ctrl", 0));
	FIELD("ReqDis", "yesno_t", BITS("%link_init_ctrl", 1));
	FIELD("DefLnk", ANON_ENUM(
				KV("link0", 0),
				KV("link1", 1),
				KV("link2", 2),
				KV("link3", 3),
				KV("self", 4)),
			BITS("%link_init_ctrl", 3, 2));
	FIELD("ColdRstDet", "yesno_t", BITS("%link_init_ctrl", 4));
	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("BiosRstDet", "hex_t",
				BITS("%link_init_ctrl", 5) +
				BITS("%link_init_ctrl", 10, 9));
	} else  {
		FIELD("BiosRstDet", "hex_t", BITS("%link_init_ctrl", 5));
	}
	FIELD("InitDet", "yesno_t", BITS("%link_init_ctrl", 6));
	FIELD("DefSubLnk", "int_t", BITS("%link_init_ctrl", 8));
	FIELD("Tr0Double", "yesno_t", BITS("%link_init_ctrl", 20));

	// The HyperTransport capability blocks are handled by the generic
	// PCI code.

	//
	// k8-specific HT registers
	//
	k8_ht_regs("ldt0", 0x90);
	k8_ht_regs("ldt1", 0xb0);
	k8_ht_regs("ldt2", 0xd0);
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

	pp_value node = dev-24;

	OPEN_SCOPE("k8[]");
	BOOKMARK("k8");

	pp_value ncores;

	OPEN_SCOPE("core[]");
	BOOKMARK("core");

	// Figure out how many cores on on each node, and explore CPUID
	// for each core.
	//
	// FIXME: This is kind of fugly.  It might be better to move CPUID
	// out of the k8 device itself?  We need to know
	// family/model/stepping for PCI stuff, though.
	if (node == 0) {
		k8_cpuid(node, 1, 0);
		ncores = READ("cpuid.1/logical_proc_count");
	} else {
		//FIXME: this assumes all k8s are the same
		//FIXME: uggh.  Maybe allow ALIAS in the node0 case?
		ncores = READ("/k8[0]/core[0]/cpuid.1/logical_proc_count");
		k8_cpuid(node, ncores, 0);
	}

	CLOSE_SCOPE(); // core[]
	for (unsigned i = 1; i < ncores; i++) {
		OPEN_SCOPE("core[]");
		BOOKMARK("core");
		k8_cpuid(node, ncores, i);
		CLOSE_SCOPE();
	}

	// for simplicity
	ENUM("k8_rev_t",
		KV("rev_e", 'e'),
		KV("rev_f", 'f'),
		KV("unknown", 0));
	if (FIELD_EQ("core[0]/cpuid.1/family", 0xf)) {
		if (FIELD_GE("core[0]/cpuid.1/model", 0x40)) {
			FIELD("k8_rev", "k8_rev_t", 'f');
		} else if (FIELD_GE("core[0]/cpuid.1/model", 0x20)) {
			FIELD("k8_rev", "k8_rev_t", 'e');
		} else {
			FIELD("k8_rev", "k8_rev_t", pp_value(0));
		}
	} else {
		FIELD("k8_rev", "k8_rev_t", pp_value(0));
	}

	/* function 0 */
	PCI_SCOPE("ht_config", seg, bus, dev, func);
	k8_ht_config();
	CLOSE_SCOPE();

	/* function 1 */
	PCI_SCOPE("address_map", seg, bus, dev, func+1);
	k8_address_map();
	CLOSE_SCOPE();

	/* function 2 */
	PCI_SCOPE("dram_ctrl", seg, bus, dev, func+2);
	k8_dram_controller();
	CLOSE_SCOPE();

	/* function 3 */
	PCI_SCOPE("misc_ctrl", seg, bus, dev, func+3);
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
		// ignore other sub-devices in this same device, we'll
		// handle them in k8_discovered()
		pp_register_discovery("pci", ARGS(0x1022, 0x1101));
		pp_register_discovery("pci", ARGS(0x1022, 0x1102));
		pp_register_discovery("pci", ARGS(0x1022, 0x1103));
		pp_register_discovery("cpuid",     // rev E
				ARGS(pp_value("0x444d416369746e6568747541"),
				     0x0f, 0x0f,   // family range
				     0x20, 0x39,   // model range
				     0x00, 0xFF)); // stepping range
		pp_register_discovery("cpuid",     // rev F
				ARGS(pp_value("0x444d416369746e6568747541"),
				     0x0f, 0x0f,   // family range
				     0x40, 0xFF,   // model range
				     0x00, 0xFF)); // stepping range
		//FIXME: register other devices this owns
	}
};

static k8_discovery the_k8_discovery;
