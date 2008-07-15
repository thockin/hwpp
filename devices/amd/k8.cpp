//
// This file includes support for AMD K8 family processors.  The following
// revisions are supported:
// 	- Rev E
// 	- Rev F (NPT, Family 0F)
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
#include "devices/msr/generic_device.h"


static void
k8_cpuid(const pp_value &cpu)
{
	cpuid_generic_device(cpu);
}

static void
k8_msr(const pp_value &cpu)
{
	MSR_SCOPE("msr", cpu);

	//FIXME: more

	CLOSE_SCOPE(); // msr
}

static void
k8_cpu(const pp_value &node, const pp_value &ncores, const pp_value &cpu)
{
	k8_cpuid(node * ncores + cpu);
	k8_msr(node * ncores + cpu);
}

static void
k8_ht_config(const pp_value &seg, const pp_value &bus,
             const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("ht_config", seg, bus, dev, func);

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

	for (int addr = 0x40; addr < 0x60; addr += 4) {
		OPEN_SCOPE("node[]");

		REG32("%ht_route", addr);
		FIELD("RQRoute", "ht_route_t", BITS("%ht_route", 3, 0));
		FIELD("RPRoute", "ht_route_t", BITS("%ht_route", 11, 8));
		FIELD("BCRoute", "ht_route_t", BITS("%ht_route", 19, 16));

		CLOSE_SCOPE();
	}

	CLOSE_SCOPE();

	//
	// Node ID register
	//
	REG32("%node_id", 0x60);
	FIELD("NodeId", "int_t", BITS("%node_id", 2, 0));
	FIELD("NodeCnt", ANON_XFORM("int_t", LAMBDA(_1+1), LAMBDA(_1-1)),
			BITS("%node_id", 6, 4));
	FIELD("SbNode", "int_t", BITS("%node_id", 10, 8));
	FIELD("LkNode", "int_t", BITS("%node_id", 14, 12));
	FIELD("CpuCnt", ANON_XFORM("int_t", LAMBDA(_1+1), LAMBDA(_1-1)),
			BITS("%node_id", 19, 16));

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
	for (int addr = 0x90; addr < 0x100; addr += 0x20) {
		OPEN_SCOPE("ldt[]");

		//
		// HT buffer count
		//
		REG32("%ldt_buf_count", addr);
		FIELD("Req", "int_t", BITS("%ldt_buf_count", 3, 0));
		FIELD("PReq", "int_t", BITS("%ldt_buf_count", 7, 4));
		FIELD("Rsp", "int_t", BITS("%ldt_buf_count", 11, 8));
		FIELD("Probe", "int_t", BITS("%ldt_buf_count", 15, 12));
		FIELD("ReqD", "int_t", BITS("%ldt_buf_count", 18, 16));
		FIELD("PReqD", "int_t", BITS("%ldt_buf_count", 22, 20));
		FIELD("RspD", "int_t", BITS("%ldt_buf_count", 26, 24));

		//
		// HT bus nums
		//
		REG32("%ldt_bus_nums", addr+4);
		FIELD("PriBusNum", "int_t", BITS("%ldt_bus_nums", 7, 0));
		FIELD("SecBusNum", "int_t", BITS("%ldt_bus_nums", 15, 8));
		FIELD("SubBusNum", "int_t", BITS("%ldt_bus_nums", 23, 16));

		//
		// HT type
		//
		REG32("%ldt_type", addr+8);
		FIELD("LinkCon", "yesno_t", BITS("%ldt_type", 0));
		FIELD("InitComplete", "yesno_t", BITS("%ldt_type", 1));
		FIELD("NC", ANON_BOOL("noncoherent", "coherent"),
				BITS("%ldt_type", 2));
		FIELD("UniP_cLDT", ANON_BOOL("uniproc_coherent_nb", "normal"),
				BITS("%ldt_type", 3));
		FIELD("LinkConPend", "yesno_t", BITS("%ldt_type", 4));

		CLOSE_SCOPE();
	}

	CLOSE_SCOPE(); // ht_config
}

static void
k8_address_map(const pp_value &seg, const pp_value &bus,
               const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("address_map", seg, bus, dev, func+1);

	//
	// DRAM base/limit registers
	//
	for (int addr = 0x40; addr < 0x80; addr += 0x08) {
		OPEN_SCOPE("dram[]");

		REG32("%dram_base", addr);
		REG32("%dram_limit", addr + 0x04);

		FIELD("RE", "enabledisable_t", BITS("%dram_base", 0));
		FIELD("WE", "enabledisable_t", BITS("%dram_base", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("DRAMBase", "addr64_t",
			    BITS("%dram_base", 31, 16) + BITS("%0", 23, 0));
			FIELD("DRAMLimit", "addr64_t",
			    BITS("%dram_limit", 31, 16) + BITS("%1", 23, 0));
			FIELD("DstNode", "int_t", BITS("%dram_limit", 2, 0));
			FIELD("IntlvEn", ANON_ENUM(KV("none", 0),
					KV("a12", 1),
					KV("a12_a13", 3),
					KV("a12_a13_a14", 7)),
				BITS("%dram_base", 10, 8));
			FIELD("IntlvSel", ANON_BITMASK(
					KV("a12", 0),
					KV("a13", 1),
					KV("a14", 2)),
				BITS("%dram_limit", 10, 8));
		}

		CLOSE_SCOPE();
	}

	//
	// Memory-mapped I/O base/limit registers
	//
	for (int addr = 0x80; addr < 0xc0; addr += 0x08) {
		OPEN_SCOPE("mmio[]");

		REG32("%mmio_base", addr);
		REG32("%mmio_limit", addr + 0x04);

		FIELD("RE", "enabledisable_t", BITS("%mmio_base", 0));
		FIELD("WE", "enabledisable_t", BITS("%mmio_base", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("MMIOBase", "addr64_t",
			    BITS("%mmio_base", 31, 8) + BITS("%0", 15, 0));
			FIELD("MMIOLimit", "addr64_t",
			    BITS("%mmio_limit", 31, 8) + BITS("%1", 15, 0));
			FIELD("DstNode", "int_t", BITS("%mmio_limit", 2, 0));
			FIELD("DstLink", "int_t", BITS("%mmio_limit", 5, 4));
			FIELD("NP", "yesno_t", BITS("%mmio_limit", 7));
			FIELD("CpuDis", "yesno_t", BITS("%mmio_base", 2));
			FIELD("Lock", "yesno_t", BITS("%mmio_base", 3));
		}

		CLOSE_SCOPE();
	}

	//
	// PCI I/O base/limit registers
	//
	for (int addr = 0xc0; addr < 0xe0; addr += 0x08) {
		OPEN_SCOPE("pciio[]");

		REG32("%pciio_base", addr);
		REG32("%pciio_limit", addr + 0x04);

		FIELD("RE", "enabledisable_t", BITS("%pciio_base", 0));
		FIELD("WE", "enabledisable_t", BITS("%pciio_base", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("PCIIOBase", "addr64_t",
			    BITS("%pciio_base", 24, 12) + BITS("%0", 11, 0));
			FIELD("PCIIOLimit", "addr64_t",
			    BITS("%pciio_limit", 24, 12) + BITS("%1", 11, 0));
			FIELD("DstNode", "int_t", BITS("%pciio_limit", 2, 0));
			FIELD("DstLink", "int_t", BITS("%pciio_limit", 5, 4));
			FIELD("VE", "enabledisable_t", BITS("%pciio_base", 4));
			FIELD("IE", "enabledisable_t", BITS("%pciio_base", 5));
		}

		CLOSE_SCOPE();
	}

	//
	// Configuration base/limit registers
	//
	for (int addr = 0xe0; addr < 0xf0; addr += 0x04) {
		OPEN_SCOPE("config[]");

		REG32("%config", addr);
		FIELD("RE", "enabledisable_t", BITS("%config", 0));
		FIELD("WE", "enabledisable_t", BITS("%config", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("DstNode", "int_t", BITS("%config", 6, 4));
			FIELD("DstLink", "int_t", BITS("%config", 9, 8));
			FIELD("BusNumBase", "int_t", BITS("%config", 23, 16));
			FIELD("BusNumLimit", "int_t", BITS("%config", 31, 24));
			FIELD("DevCmpEn", "enabledisable_t",
					BITS("%config", 2));
		}

		CLOSE_SCOPE();
	}

	//
	// DRAM hole address register
	//
	REG32("%DRAMHole", 0xf0);
	OPEN_SCOPE("dram_hole");

	FIELD("DramHoleValid", "truefalse_t", BITS("../%DRAMHole", 0));
	if (FIELD_BOOL("DramHoleValid")) {
		FIELD("DramHoleOffset", "hex32_t",
			BITS("../%DRAMHole", 15, 8) + BITS("%0", 23, 0));
		FIELD("DramHoleBase", "hex32_t",
			BITS("../%DRAMHole", 31, 24) + BITS("%0", 23, 0));
	}

	CLOSE_SCOPE();

	CLOSE_SCOPE(); // address_map
}

static void
k8_dram_controller(const pp_value &seg, const pp_value &bus,
                   const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("dram_ctrl", seg, bus, dev, func+2);
	CLOSE_SCOPE(); // dram_ctrl
}

static void
k8_misc_control(const pp_value &seg, const pp_value &bus,
                const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("misc_ctrl", seg, bus, dev, func+3);
	CLOSE_SCOPE(); //misc_ctrl
}

static void
k8_discovered(const std::vector<pp_value> &args)
{
	pp_value seg = args[0];
	pp_value bus = args[1];
	pp_value dev = args[2];
	pp_value func = args[3];

	pp_value node = dev-24;
	pp_value ncores;

	OPEN_SCOPE("k8[]");
	BOOKMARK("k8");

	OPEN_SCOPE("core[]");
	BOOKMARK("core");

	// Figure out how many cores on on each node, and explore CPUID
	// for each core.
	//
	// FIXME: This is kind of fugly.  It might be better to move CPUID
	// out of the k8 device itself?  We need to know
	// family/model/stepping for PCI stuff, though.
	if (node == 0) {
		k8_cpu(node, 1, 0);
		ncores = READ("cpuid.1/logical_proc_count");
	} else {
		//FIXME: this assumes all k8s are the same
		//FIXME: uggh.  Maybe allow ALIAS in the node0 case?
		ncores = READ("/k8[0]/core[0]/cpuid.1/logical_proc_count");
		k8_cpu(node, ncores, 0);
	}

	CLOSE_SCOPE(); // core[]
	for (unsigned i = 1; i < ncores; i++) {
		OPEN_SCOPE("core[]");
		BOOKMARK("core");
		k8_cpu(node, ncores, i);
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
	k8_ht_config(seg, bus, dev, func);

	/* function 1 */
	k8_address_map(seg, bus, dev, func);

	/* function 2 */
	k8_dram_controller(seg, bus, dev, func);

	/* function 3 */
	k8_misc_control(seg, bus, dev, func);

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
