#include "pp.h"
#include "pp_scope.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "utils.h"

// All standard BARs look like this.
static void
BAR(const string &name, pp_regaddr address)
{
	OPEN_SCOPE(name);

	REG32("%lower", address);
	ONE_BIT_FIELD("type", ANON_ENUM(
				{"mem", 0},
				{"io", 1}),
			"%lower", 0);

	if (GET_FIELD("type")->compare("mem")) {
		SIMPLE_FIELD("width", ANON_ENUM(
					{"bits32", 0},
					{"bits20", 1},
					{"bits64", 2}),
				"%lower", 2, 1);
		ONE_BIT_FIELD("prefetch", "yesno_t", "%lower", 3);
	}

	if (GET_FIELD("type")->compare("io")) {
		COMPLEX_FIELD("address", "addr16_t",
				{"0", 1, 0},
				{"%lower", 15, 2});
	} else if (GET_FIELD("width")->compare("bits32")
	    || GET_FIELD("width")->compare("bits20")) {
		COMPLEX_FIELD("address", "addr32_t",
				{"0", 3, 0},
				{"%lower", 31, 4});
	} else {
		REG32("%upper", address+4);
		COMPLEX_FIELD("address", "addr64_t",
				{"0", 3, 0},
				{"%lower", 31, 4},
				{"%upper", 31, 0});
	}
	CLOSE_SCOPE();
}

static void
ht_link_control(pp_value address)
{
	REG16("%control", address);
	OPEN_SCOPE("control");

	ONE_BIT_FIELD("SrcIdEn", "yesno_t", "%control", 0);
	ONE_BIT_FIELD("CFIEn", "yesno_t", "%control", 1);
	ONE_BIT_FIELD("CST", "yesno_t", "%control", 2);
	ONE_BIT_FIELD("CFEr", "yesno_t", "%control", 3);
	ONE_BIT_FIELD("LinkFail", "yesno_t", "%control", 4);
	ONE_BIT_FIELD("InitCmp", "yesno_t", "%control", 5);
	ONE_BIT_FIELD("EOC", "yesno_t", "%control", 6);
	ONE_BIT_FIELD("TXOff", "yesno_t", "%control", 7);
	SIMPLE_FIELD("CRCErr", "hex4_t", "%control", 11, 8);
	ONE_BIT_FIELD("IsocEn", "yesno_t", "%control", 12);
	ONE_BIT_FIELD("LSEn", "yesno_t", "%control", 13);
	ONE_BIT_FIELD("ExtCTL", "yesno_t", "%control", 14);
	ONE_BIT_FIELD("64bEn", "yesno_t", "%control", 15);

	CLOSE_SCOPE();
}

static void
ht_link_config(pp_value address)
{
	ENUM("ht_link_width_t",
			{"bits8", 0},
			{"bits16", 1},
			{"bits32", 3},
			{"bits2", 4},
			{"bits4", 5},
			{"nc", 7});

	REG16("%config", address);
	OPEN_SCOPE("config");

	SIMPLE_FIELD("MaxLinkWidthIn", "ht_link_width_t", "%config", 2, 0);
	ONE_BIT_FIELD("DwFcIn", "yesno_t", "%config", 3);
	SIMPLE_FIELD("MaxLinkWidthOut", "ht_link_width_t", "%config", 6, 4);
	ONE_BIT_FIELD("DwFcOut", "yesno_t", "%config", 7);

	SIMPLE_FIELD("LinkWidthIn", "ht_link_width_t", "%config", 10, 8);
	ONE_BIT_FIELD("DwFcInEn", "yesno_t", "%config", 11);
	SIMPLE_FIELD("LinkWidthOut", "ht_link_width_t", "%config", 14, 12);
	ONE_BIT_FIELD("DwFcOutEn", "yesno_t", "%config", 15);

	CLOSE_SCOPE();
}

static void
ht_link_freq_err(pp_value address)
{
	ENUM("ht_link_freq_t",
			{"mhz200", 0},
			{"mhz300", 1},
			{"mhz400", 2},
			{"mhz500", 3},
			{"mhz600", 4},
			{"mhz800", 5},
			{"mhz1000", 6},
			{"mhz1200", 7},
			{"mhz1400", 8},
			{"mhz1600", 9},
			{"mhz1800", 1},
			{"mhz2000", 11},
			{"mhz2200", 12},
			{"mhz2400", 13},
			{"mhz2600", 14});

	REG8("%freq_err", address);
	SIMPLE_FIELD("Freq", "ht_link_freq_t", "%freq_err", 3, 0);
	ONE_BIT_FIELD("ProtoErr", "yesno_t", "%freq_err", 4);
	ONE_BIT_FIELD("OverErr", "yesno_t", "%freq_err", 5);
	ONE_BIT_FIELD("EOCErr", "yesno_t", "%freq_err", 6);
	ONE_BIT_FIELD("CTLTimeout", "yesno_t", "%freq_err", 7);
}

static void
ht_link_freq_cap(pp_value address)
{
	BITMASK("ht_link_freq_cap_t",
			{"mhz200", 0},
			{"mhz300", 1},
			{"mhz400", 2},
			{"mhz500", 3},
			{"mhz600", 4},
			{"mhz800", 5},
			{"mhz1000", 6},
			{"mhz1200", 7},
			{"mhz1400", 8},
			{"mhz1600", 9},
			{"mhz1800", 1},
			{"mhz2000", 11},
			{"mhz2200", 12},
			{"mhz2400", 13},
			{"mhz2600", 14});

	REG16("%freq_cap", address);
	SIMPLE_FIELD("FreqCap", "ht_link_freq_cap_t", "%freq_cap", 15, 0);
}

static void
ht_error_handling(pp_value address)
{
	REG16("%error", address);

	OPEN_SCOPE("error");
	ONE_BIT_FIELD("proto_flood", "yesno_t", "%err", 0);
	ONE_BIT_FIELD("over_flood", "yesno_t", "%err", 1);
	ONE_BIT_FIELD("proto_fatal", "yesno_t", "%err", 2);
	ONE_BIT_FIELD("over_fatal", "yesno_t", "%err", 3);
	ONE_BIT_FIELD("eoc_fatal", "yesno_t", "%err", 4);
	ONE_BIT_FIELD("resp_fatal", "yesno_t", "%err", 5);
	ONE_BIT_FIELD("crc_fatal", "yesno_t", "%err", 6);
	ONE_BIT_FIELD("serr_fatal", "yesno_t", "%err", 7);
	ONE_BIT_FIELD("chain_fail", "yesno_t", "%err", 8);
	ONE_BIT_FIELD("resp_err", "yesno_t", "%err", 9);
	ONE_BIT_FIELD("proto_nonfatal", "yesno_t", "%err", 10);
	ONE_BIT_FIELD("over_nonfatal", "yesno_t", "%err", 11);
	ONE_BIT_FIELD("eoc_nonfatal", "yesno_t", "%err", 12);
	ONE_BIT_FIELD("resp_nonfatal", "yesno_t", "%err", 13);
	ONE_BIT_FIELD("crc_nonfatal", "yesno_t", "%err", 14);
	ONE_BIT_FIELD("serr_nonfatal", "yesno_t", "%err", 15);
	CLOSE_SCOPE();
}

static void
ht_slave_link(const string &name, pp_value address)
{
	OPEN_SCOPE(name);
	ht_link_control(address);
	ht_link_config(address+2);
	ht_link_freq_err(address+0x09);
	ht_link_freq_cap(address+0x0a);
	CLOSE_SCOPE();
}

// HyperTransport primary/slave
static void
ht_slave_capability(pp_value address)
{
	REG16("%command", address + 2);
	SIMPLE_FIELD("base_unit_id", "int_t", "%command", 4, 0);
	SIMPLE_FIELD("unit_count", "int_t", "%command", 9, 5);
	ONE_BIT_FIELD("master", "yesno_t", "%command", 10);
	ONE_BIT_FIELD("default_direction", "yesno_t", "%command", 11);
	ONE_BIT_FIELD("drop_on_uninit", "yesno_t", "%command", 12);

	ht_slave_link("link0", address + 0x04);
	ht_slave_link("link1", address + 0x08);

	//FIXME: clean up HT field names all over - inconsistent BumpyCaps
	REG8("%rev", address + 0x0c);
	SIMPLE_FIELD("MajorRev", "int_t", "%rev", 7, 5);
	SIMPLE_FIELD("MinorRev", "int_t", "%rev", 4, 0);

	REG8("%feature", address + 0x10);
	OPEN_SCOPE("feature");
	ONE_BIT_FIELD("iso_flow_ctrl", "yesno_t", "%feature", 0);
	ONE_BIT_FIELD("ldtstop", "yesno_t", "%feature", 1);
	ONE_BIT_FIELD("crc_test_mode", "yesno_t", "%feature", 2);
	ONE_BIT_FIELD("extended_ctl_time", "yesno_t", "%feature", 3);
	ONE_BIT_FIELD("64bit_addressing", "yesno_t", "%feature", 4);
	ONE_BIT_FIELD("unit_id_reorder_dis", "yesno_t", "%feature", 5);
	ONE_BIT_FIELD("source_id", "yesno_t", "%feature", 6);
	CLOSE_SCOPE();

	REGFIELD16("scratchpad", address + 0x14, "hex16_t");

	ht_error_handling(address + 0x16);

	REGFIELD8("mem_base_hi", address + 0x18, "addr16_t");
	REGFIELD8("mem_limit_hi", address + 0x19, "addr16_t");
	REGFIELD8("bus", address + 0x1a, "int_t");
}

static void
ht_host_link(const string &name, pp_value address)
{
	OPEN_SCOPE(name);
	ht_link_control(address);
	ht_link_config(address+2);
	ht_link_freq_err(address+0x05);
	ht_link_freq_cap(address+0x06);
	CLOSE_SCOPE();
}

static void
ht_host_capability(pp_value address)
{
	REG16("%command", address + 2);
	ONE_BIT_FIELD("warm_reset", "yesno_t", "%command", 0);
	ONE_BIT_FIELD("dbl_ended", "yesno_t", "%command", 1);
	SIMPLE_FIELD("device", "int_t", "%command", 6, 2);
	ONE_BIT_FIELD("chain_side", "yesno_t", "%command", 7);
	ONE_BIT_FIELD("host_hide", "yesno_t", "%command", 8);
	ONE_BIT_FIELD("act_slave", "yesno_t", "%command", 10);
	ONE_BIT_FIELD("in_eoc_err", "yesno_t", "%command", 11);
	ONE_BIT_FIELD("drop_on_uninit", "yesno_t", "%command", 12);

	ht_host_link("link", address + 0x04);

	REG8("%rev", address + 0x08);
	SIMPLE_FIELD("major_rev", "int_t", "%rev", 7, 5);
	SIMPLE_FIELD("minor_rev", "int_t", "%rev", 4, 0);

	REG16("%feature", address + 0x0c);
	OPEN_SCOPE("feature");
	ONE_BIT_FIELD("iso_flow_ctrl", "yesno_t", "%feature", 0);
	ONE_BIT_FIELD("ldtstop", "yesno_t", "%feature", 1);
	ONE_BIT_FIELD("crc_test_mode", "yesno_t", "%feature", 2);
	ONE_BIT_FIELD("extended_ctl_time", "yesno_t", "%feature", 3);
	ONE_BIT_FIELD("64bit_addressing", "yesno_t", "%feature", 4);
	ONE_BIT_FIELD("unit_id_reorder_dis", "yesno_t", "%feature", 5);
	ONE_BIT_FIELD("source_id", "yesno_t", "%feature", 6);
	ONE_BIT_FIELD("ext_regs", "yesno_t", "%feature", 8);
	ONE_BIT_FIELD("upstream_cfg", "yesno_t", "%feature", 9);
	CLOSE_SCOPE();

	if (GET_FIELD("feature/ext_regs")->read()) {
		REGFIELD16("scratchpad", address + 0x10, "hex16_t");
		ht_error_handling(address + 0x12);
		REGFIELD8("mem_base_hi", address + 0x14, "addr16_t");
		REGFIELD8("mem_limit_hi", address + 0x15, "addr16_t");
	}
}

static void
ht_revision_capability(pp_value address)
{
	REG8("%rev", address+2);
	SIMPLE_FIELD("major_rev", "int_t", "%rev", 7, 5);
	SIMPLE_FIELD("minor_rev", "int_t", "%rev", 4, 0);
}

static void
ht_extended_config_capability(pp_value address)
{
	REG32("%address", address+4);
	ONE_BIT_FIELD("type", ANON_ENUM(
				{"type0", 0},
				{"type1", 1}),
			"%address", 28);
	SIMPLE_FIELD("bus", "int_t", "%address", 27, 20);
	SIMPLE_FIELD("device", "int_t", "%address", 19, 15);
	SIMPLE_FIELD("function", "int_t", "%address", 14, 12);
	COMPLEX_FIELD("register", "hex_t",
			{"0", 1, 0},
			{"%address", 11, 2});
	REGFIELD32("data", address+8, "hex32_t");
}

static void
ht_address_mapping_capability(pp_value address)
{
	REG16("%command", address + 0x02);
	SIMPLE_FIELD("num_dma", "int_t", "%command", 3, 0);
	SIMPLE_FIELD("io_size", "int_t", "%command", 8, 4);
	SIMPLE_FIELD("map_type", ANON_ENUM(
				{"bits40", 0},
				{"bits64", 1}),
			"%command", 10, 9);

	if (GET_FIELD("map_type")->compare("bits40")) {
		REG32("%sec_non_prefetch", address + 0x04);
		OPEN_SCOPE("sec_non_prefetch");
		ONE_BIT_FIELD("enable", "yesno_t", "%sec_non_prefetch", 31);
		ONE_BIT_FIELD("isoch", "yesno_t", "%sec_non_prefetch", 30);
		ONE_BIT_FIELD("noncoherent", "yesno_t",
				"%sec_non_prefetch", 29);
		ONE_BIT_FIELD("compat", "yesno_t", "%sec_non_prefetch", 28);
		COMPLEX_FIELD("base", "addr64_t",
				{"0", 19, 0},
				{"%sec_non_prefetch", 19, 0});
		CLOSE_SCOPE();

		REG32("%sec_prefetch", address + 0x08);
		OPEN_SCOPE("sec_prefetch");
		ONE_BIT_FIELD("enable", "yesno_t", "%sec_prefetch", 31);
		ONE_BIT_FIELD("isochronous", "yesno_t", "%sec_prefetch", 30);
		ONE_BIT_FIELD("noncoherent", "yesno_t", "%sec_prefetch", 29);
		ONE_BIT_FIELD("compat", "yesno_t", "%sec_prefetch", 28);
		COMPLEX_FIELD("base", "addr64_t",
				{"0", 19, 0},
				{"%sec_prefetch", 19, 0});
		CLOSE_SCOPE();

		pp_value value = GET_FIELD("num_dma")->read();
		pp_value i = 0;
		for (i = 0; i < value; i++) {
			OPEN_SCOPE("dma[" + to_string(i+1) + "]");
			REG32("%lower", address + 0x0c + (8*i));
			REG32("%upper", address + 0x10 + (8*i));

			ONE_BIT_FIELD("enable", "yesno_t", "%lower", 31);
			ONE_BIT_FIELD("isochronous", "yesno_t", "%lower", 30);
			ONE_BIT_FIELD("noncoherent", "yesno_t", "%lower", 29);

			COMPLEX_FIELD("pri_base", "addr64_t",
					{"0", 23, 0},
					{"%lower", 15, 0});
			COMPLEX_FIELD("sec_base", "addr64_t",
					{"0", 23, 0},
					{"%lower", 31, 16});
			COMPLEX_FIELD("sec_limit", "addr64_t",
					{"1", 23, 0},
					{"%lower", 15, 0});
			CLOSE_SCOPE();
		}
	} else if (GET_FIELD("map_type")->compare("bits64")) {
		//FIXME: this requires index/data pair access within PCI
	}
}

static void
ht_msi_mapping_capability(pp_value address)
{
	REG8("%flags", address+2);
	ONE_BIT_FIELD("en", "yesno_t", "%flags", 0);
	ONE_BIT_FIELD("fixed", "yesno_t", "%flags", 1);
	if (!GET_FIELD("fixed")->read()) {
		REG32("%lower", address+4);
		REG32("%upper", address+8);
		COMPLEX_FIELD("address", "addr64_t",
				{"0", 19, 0},
				{"%lower", 31, 20},
				{"%upper", 31, 0});
	}
}

static void
slot_id_capability(pp_value address)
{
	REG8("%slot", address+2);
	SIMPLE_FIELD("nslots", "int_t", "%slot", 4, 0);
	ONE_BIT_FIELD("first", "yesno_t", "%slot", 5);
	REGFIELD8("chassis", address+3, "int_t");
}

static void
msi_capability(pp_value address)
{
	// message control
	REG16("%msg_ctrl", address + 2);
	ONE_BIT_FIELD("msi_enable", "yesno_t", "%msg_ctrl", 0);
	SIMPLE_FIELD("multi_msg_cap", ANON_ENUM(
				{"vec1",  0},
				{"vec2",  1},
				{"vec4",  2},
				{"vec8",  3},
				{"vec16", 4},
				{"vec32", 5}),
			"%msg_ctrl", 3, 1);
	SIMPLE_FIELD("multi_msg_en", ANON_ENUM(
				{"vec1",  0},
				{"vec2",  1},
				{"vec4",  2},
				{"vec8",  3},
				{"vec16", 4},
				{"vec32", 5}),
			"%msg_ctrl", 6, 4);
	ONE_BIT_FIELD("cap64", "yesno_t", "%msg_ctrl", 7);
	ONE_BIT_FIELD("mask_per_vec", "yesno_t", "%msg_ctrl", 8);

	// message address
	REG32("%msg_addr", address + 4);

	// is this 64 bit capable?
	if (!GET_FIELD("cap64")->read()) {
		// no, just use the low address and define the data
		COMPLEX_FIELD("msg_addr", "addr32_t",
				{"0", 1, 0},
				{"%msg_addr", 31, 2});
		REGFIELD16("msg_data", address + 8, "hex16_t");

		if (GET_FIELD("mask_per_vec")->read()) {
			// mask and pending bits
			REG32("%mask", address + 12);
			REG32("%pending", address + 16);
		}
	} else {
		// add the high address and define the data
		REG32("%msg_addr_hi",  address + 8);
		COMPLEX_FIELD("msg_addr", "addr64_t",
				{"0", 1, 0},
				{"%msg_addr", 31, 2},
				{"%msg_addr_hi", 31, 0});
		REGFIELD16("msg_data", address + 12, "hex16_t");

		if (GET_FIELD("mask_per_vec")->read()) {
			// mask and pending bits
			REG32("%mask", address + 16);
			REG32("%pending", address + 20);
		}
	}

	if (GET_FIELD("mask_per_vec")->read()) {
		pp_value vecs = 1 << GET_FIELD("multi_msg_cap")->read();
		pp_value i;
		for (i = 0; i < vecs; i++) {
			ONE_BIT_FIELD("mask["+to_string(i)+"]",
					"yesno_t", "%mask", i);
			ONE_BIT_FIELD("pend["+to_string(i)+"]",
					"yesno_t", "%pending", i);
		}
	}
}

static void
msix_capability(pp_value address)
{
	//FIXME:  This gets complicated.  The PCI registers refer to other
	//registers that exist behind a BAR.  Maybe we need to allow
	//nested bindings, so we can define this.
}

static void
ht_capability(pp_value address)
{
	REG8("%subcap", address + 3);
	// Check the upper two bits - if they are zero, then we have
	// a slave/primary interface block or a host/secondary interface
	// block.  If they are non-zero, we have some other HT capability.
	// This matters because they use different encodings of the
	// capability type (3 bits vs 5 bits).
	SIMPLE_FIELD("is_interface", ANON_ENUM(
				{"yes", 0},
				{"no", 1},
				{"no", 2},
				{"no", 3}),
			"%sub_cap", 7, 6);
	if (GET_FIELD("is_interface")->compare("yes")) {
		// subtype is a 3 bit field
		SIMPLE_FIELD("subtype", ANON_ENUM(
					{"slave_primary", 0},
					{"host_secondary", 1}),
				"%subcap", 15, 13);

		if (GET_FIELD("subtype")->compare("slave_primary")) {
			ht_slave_capability(address);
		} else if (GET_FIELD("subtype")->compare("host_secondary")) {
			ht_host_capability(address);
		}
	} else {
		// subtype is a 5 bit field
		SIMPLE_FIELD("subtype", ANON_ENUM(
					{"switch", 8},
					{"intr_discovery", 16},
					{"revision", 17},
					{"unit_id_clump", 18},
					{"extended_config", 19},
					{"address_mapping", 20},
					{"msi_mapping", 21},
					{"direct_route", 22},
					{"vc_set", 23},
					{"retry_mode", 24},
					{"x86_encode", 25},
					{"gen3", 26},
					{"function_extend", 27},
					{"power_mgmt", 28}),
				"%subcap", 7, 3);

		if (GET_FIELD("subtype")->compare("switch")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("intr_discovery")) {
			//FIXME: not implemented yet
			//This requires index/data accesses within PCI space
		} else if (GET_FIELD("subtype")->compare("revision")) {
			ht_revision_capability(address);
		} else if (GET_FIELD("subtype")->compare("unit_id_clump")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("extended_config")) {
			ht_extended_config_capability(address);
		} else if (GET_FIELD("subtype")->compare("address_mapping")) {
			ht_address_mapping_capability(address);
		} else if (GET_FIELD("subtype")->compare("msi_mapping")) {
			ht_msi_mapping_capability(address);
		} else if (GET_FIELD("subtype")->compare("direct_route")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("vc_set")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("retry_mode")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("x86_encode")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("gen3")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("function_extend")) {
			//FIXME: not implemented yet
		} else if (GET_FIELD("subtype")->compare("power_mgmt")) {
			//FIXME: not implemented yet
			//this requires index/data pairs in PCI space
		}
	}
}

static void
ssid_capability(pp_value address)
{
	REGFIELD16("ssvid", address + 0x04, "pci_vendor_t");
	REGFIELD16("ssid", address + 0x06, "hex16_t");
}

static void
pcie_capability(pp_value address)
{
	REG16("%pcie_caps", address + 0x02);
	SIMPLE_FIELD("version", "int_t", "%pcie_caps", 3, 0);
	SIMPLE_FIELD("type", ANON_ENUM(
				{"endpoint", 0},
				{"legacy_endpoint", 1},
				{"root_port", 4},
				{"upstream_switch_port", 5},
				{"dnstream_switch_port", 6},
				{"pcie_pci_bridge", 7},
				{"pci_pcie_bridge", 8},
				{"integrated_endpoint", 9},
				{"event_collector", 10}),
			"%pcie_caps", 7, 4);
	ONE_BIT_FIELD("slot", "yesno_t", "%pcie_caps", 8);
	SIMPLE_FIELD("intr_msg_num", "int_t", "%pcie_caps", 13, 9);
	ONE_BIT_FIELD("tcs", "yesno_t", "%pcie_caps", 14);

	REG32("%dev_caps", address + 0x04);
	REG16("%dev_control", address + 0x08);
	REG16("%dev_status", address + 0x0a);

	ENUM("pcie_payload_size_t",
			{"b128", 0},
			{"b256", 1},
			{"b512", 2},
			{"b1024", 3},
			{"b2048", 4},
			{"b4096", 5});
	SIMPLE_FIELD("max_payload_cap", "pcie_payload_size_t", "%dev_caps", 2, 0);
	SIMPLE_FIELD("max_payload", "pcie_payload_size_t", "%dev_control", 7, 5);
	SIMPLE_FIELD("max_read", "pcie_payload_size_t", "%dev_control", 14, 12);
	SIMPLE_FIELD("phantom_funcs_cap", ANON_ENUM(
				{"phantom_0", 0},
				{"phantom_4", 1},
				{"phantom_6", 2},
				{"phantom_8", 3}),
			"%dev_caps", 4, 3);
	ONE_BIT_FIELD("phantom_funcs", "yesno_t", "%dev_control", 9);
	ONE_BIT_FIELD("ext_tag_field_cap", ANON_ENUM(
				{"b5", 0},
				{"b8", 1}),
			"%dev_caps", 5);
	ONE_BIT_FIELD("ext_tag_field", "yesno_t", "%dev_control", 8);
	if (GET_FIELD("type")->compare("endpoint")
	 || GET_FIELD("type")->compare("legacy_endpoint")
	 || GET_FIELD("type")->compare("integrated_endpoint")) {
		SIMPLE_FIELD("max_l0s_lat", ANON_ENUM(
					{"max_64ns", 0},
					{"max_128ns", 1},
					{"max_256ns", 2},
					{"max_512ns", 3},
					{"max_1us", 4},
					{"max_2us", 5},
					{"max_4us", 6},
					{"no_limit", 7}),
				"%dev_caps", 8, 6);
		SIMPLE_FIELD("max_l1_lat", ANON_ENUM(
					{"max_1us", 0},
					{"max_2us", 1},
					{"max_4us", 2},
					{"max_8us", 3},
					{"max_16us", 4},
					{"max_32us", 5},
					{"max_64us", 6},
					{"no_limit", 7}),
				"%dev_caps", 11, 9);
		ONE_BIT_FIELD("func_reset_cap", "yesno_t", "%dev_caps", 28);
		ONE_BIT_FIELD("func_reset", "yesno_t", "%dev_control", 15);
	}
	ONE_BIT_FIELD("attn_button", "yesno_t", "%dev_caps", 12);
	ONE_BIT_FIELD("attn_indicator", "yesno_t", "%dev_caps", 13);
	ONE_BIT_FIELD("power_indicator", "yesno_t", "%dev_caps", 14);
	ONE_BIT_FIELD("role_based_err_cap", "yesno_t", "%dev_caps", 15);
	if (GET_FIELD("type")->compare("upstream_switch_port")) {
		SIMPLE_FIELD("power_limit_value", "int_t", "%dev_caps", 25, 18);
		SIMPLE_FIELD("power_limit_scale", ANON_ENUM(
					{"x1_0", 0},
					{"x0_1", 1},
					{"x0_01", 2},
					{"x0_001", 3}),
				"%dev_caps", 27, 26);
	}
	ONE_BIT_FIELD("corr_err_report", "yesno_t", "%dev_control", 0);
	ONE_BIT_FIELD("corr_err_det", "yesno_t", "%dev_status", 0);
	ONE_BIT_FIELD("nonfatal_err_report", "yesno_t", "%dev_control", 1);
	ONE_BIT_FIELD("nonfatal_err_det", "yesno_t", "%dev_status", 1);
	ONE_BIT_FIELD("fatal_err_report", "yesno_t", "%dev_control", 2);
	ONE_BIT_FIELD("fatal_err_det", "yesno_t", "%dev_status", 2);
	ONE_BIT_FIELD("unsup_req_report", "yesno_t", "%dev_control", 3);
	ONE_BIT_FIELD("unsup_req_det", "yesno_t", "%dev_status", 3);
	ONE_BIT_FIELD("relax", "yesno_t", "%dev_control", 4);
	ONE_BIT_FIELD("aux_pm", "yesno_t", "%dev_control", 10);
	ONE_BIT_FIELD("aux_pm_det", "yesno_t", "%dev_status", 4);
	ONE_BIT_FIELD("en_no_snoop", "yesno_t", "%dev_control", 11);
	if (!DEFINED("func_reset") && GET_FIELD("class")->compare("bridge")) {
		ONE_BIT_FIELD("bridge_retry_en", "yesno_t", "%dev_control", 15);
	}
	ONE_BIT_FIELD("txn_pend", "yesno_t", "%dev_status", 5);
	
	//FIXME: left off here PCIe v2 pg 458
}

// Handle the PCI capabilities linked-list.
static void
explore_capabilities()
{
	if (GET_FIELD("status/caps")->read()) {
		REGFIELD8("capptr", 0x34, "hex8_t");

		pp_value ptr = GET_FIELD("capptr")->read();
		pp_value i = 0;
		while (ptr != 0 && ptr != 0xff) {
			OPEN_SCOPE("capability[" + to_string(i) + "]");

			//FIXME: add a "literal" fieldtype:  offset=$ptr?
			REGFIELD8("id", ptr, "pci_capability_t");
			REGFIELD8("next", ptr+1, "hex8_t");

			if (GET_FIELD("id")->compare("power_mgmt")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("agp")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("agp8x")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("vpd")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("slot_id")) {
				// PCI-PCI bridge spec
				slot_id_capability(ptr);
			} else if (GET_FIELD("id")->compare("msi")) {
				// PCI spec
				msi_capability(ptr);
			} else if (GET_FIELD("id")->compare("msix")) {
				// PCI spec
				msix_capability(ptr);
			} else if (GET_FIELD("id")->compare("resource_ctrl")) {
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("hot_swap")) {
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("hot_plug")) {
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("ht")) {
				// HT spec
				ht_capability(ptr);
			} else if (GET_FIELD("id")->compare("usb2_dbg_port")) {
				// EHCI spec
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("ssid")) {
				// PCI-PCI bridge spec
				ssid_capability(ptr);
			} else if (GET_FIELD("id")->compare("pcix")) {
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("pcie")) {
				// PCI-E spec
				pcie_capability(ptr);
			} else if (GET_FIELD("id")->compare("vendor")) {
				//FIXME: not implemented yet
			} else if (GET_FIELD("id")->compare("secure")) {
				//FIXME: not implemented yet
			}
			CLOSE_SCOPE();

			ptr = GET_FIELD("capability["
					+ to_string(i) + "]/next")->read();
			i++;
		}
	}
	//FIXME: scan EHCI extended capabilities
	//FIXME: PCIE Extended caps! { 16 bits ID, 4 bits version, 12 bits next
}

static void
create_pci_bridge()
{
	REGFIELD8("pri_bus", 0x18, "int_t");
	REGFIELD8("sec_bus", 0x19, "int_t");
	REGFIELD8("sub_bus", 0x1a, "int_t");
	REGFIELD8("sec_latency", 0x1b, ANON_INT("clocks"));

	// Secondary status
	REG16("%sec_status", 0x1e);
	OPEN_SCOPE("sec_status");
		ONE_BIT_FIELD("cap66", "yesno_t", "%status", 5);
		ONE_BIT_FIELD("fbb", "yesno_t", "%status", 7);
		ONE_BIT_FIELD("mdperr", "yesno_t", "%status", 8);
		SIMPLE_FIELD("devsel", ANON_ENUM(
					{"FAST", 0},
					{"MEDIUM", 1},
					{"SLOW", 2}),
				"%status", 10, 9);
		ONE_BIT_FIELD("sigtabort", "yesno_t", "%status", 11);
		ONE_BIT_FIELD("rcvtabort", "yesno_t", "%status", 12);
		ONE_BIT_FIELD("rcvmabort", "yesno_t", "%status", 13);
		ONE_BIT_FIELD("rcvserr", "yesno_t", "%status", 14);
		ONE_BIT_FIELD("perr", "yesno_t", "%status", 15);
	CLOSE_SCOPE();

	// IO decode window
	OPEN_SCOPE("io_window");
		REG8("%base_lo", 0x1c);
		REG8("%base_hi", 0x30);
		REG8("%limit_lo", 0x1d);
		REG8("%limit_hi", 0x32);

		SIMPLE_FIELD("width", ANON_ENUM(
					{"bits16", 0},
					{"bits32", 1}),
				"%base_lo", 3, 0);

		if (GET_FIELD("width")->compare("bits16")) {
			COMPLEX_FIELD("base", "addr16_t",
					{"0", 11, 0},
					{"%base_lo", 7, 4});
			COMPLEX_FIELD("limit", "addr16_t",
					{"1", 11, 0},
					{"%limit_lo", 7, 4});
		} else if (GET_FIELD("width")->compare("bits32")) {
			COMPLEX_FIELD("base", "addr32_t",
					{"0", 11, 0},
					{"%base_lo", 7, 4},
					{"%base_hi", 15, 0});
			COMPLEX_FIELD("limit", "addr32_t",
					{"1", 11, 0},
					{"%limit_lo", 7, 4},
					{"%limit_hi", 15, 0});
		}
	CLOSE_SCOPE();

	// MMIO decode window
	OPEN_SCOPE("mem_window");
		REG16("%base", 0x20);
		REG16("%limit", 0x22);

		COMPLEX_FIELD("base", "addr32_t",
				{"0", 19, 0},
				{"%base", 15, 4});
		COMPLEX_FIELD("limit", "addr32_t",
				{"1", 19, 0},
				{"%limit", 15, 4});
	CLOSE_SCOPE();

	// Prefetched memory decode window
	OPEN_SCOPE("prefetch_window");
		REG16("%base_lo", 0x24);
		REG32("%base_hi", 0x28);
		REG16("%limit_lo", 0x26);
		REG32("%limit_hi", 0x2c);

		SIMPLE_FIELD("width", ANON_ENUM(
					{"bits32", 0},
					{"bits64", 1}),
				"%base_lo", 3, 0);

		if (GET_FIELD("width")->compare("bits32")) {
			COMPLEX_FIELD("base", "addr32_t",
					{"0", 19, 0},
					{"%base_lo", 15, 4});
			COMPLEX_FIELD("limit", "addr32_t",
					{"1", 19, 0},
					{"%limit_lo", 15, 4});
		} else if (GET_FIELD("width")->compare("bits64")) {
			COMPLEX_FIELD("base", "addr64_t",
					{"0", 19, 0},
					{"%base_lo", 15, 4},
					{"%base_hi", 31, 0});
			COMPLEX_FIELD("limit", "addr64_t",
					{"1", 19, 0},
					{"%limit_lo", 15, 4},
					{"%limit_hi", 31, 0});
		}
	CLOSE_SCOPE();

	// Capabilities
	explore_capabilities();

	// Expansion ROM
	REG32("%rombase", 0x38);
	OPEN_SCOPE("rombase");
		ONE_BIT_FIELD("enabled", "yesno_t", "%rombase", 0);
		COMPLEX_FIELD("address", "addr32_t",
				{"0", 10, 0},
				{"%rombase", 31, 11});
	CLOSE_SCOPE();

	// Bridge control
	REG16("%bridge_ctrl", 0x3e);
	OPEN_SCOPE("bridge_ctrl");
		ONE_BIT_FIELD("perr", "yesno_t", "%bridge_ctrl", 0);
		ONE_BIT_FIELD("serr", "yesno_t", "%bridge_ctrl", 1);
		ONE_BIT_FIELD("isa", "yesno_t", "%bridge_ctrl", 2);
		ONE_BIT_FIELD("vga", "yesno_t", "%bridge_ctrl", 3);
		ONE_BIT_FIELD("vga16", "yesno_t", "%bridge_ctrl", 4);
		ONE_BIT_FIELD("vga16", "yesno_t", "%bridge_ctrl", 4);
		ONE_BIT_FIELD("mst_abort", "yesno_t", "%bridge_ctrl", 5);
		ONE_BIT_FIELD("sec_reset", "yesno_t", "%bridge_ctrl", 6);
		ONE_BIT_FIELD("fbb", "yesno_t", "%bridge_ctrl", 7);
		ONE_BIT_FIELD("pri_discard", "yesno_t", "%bridge_ctrl", 8);
		ONE_BIT_FIELD("sec_discard", "yesno_t", "%bridge_ctrl", 9);
		ONE_BIT_FIELD("discard_status", "yesno_t", "%bridge_ctrl", 10);
		ONE_BIT_FIELD("discard_serr", "yesno_t", "%bridge_ctrl", 11);
	CLOSE_SCOPE();
}

static void
create_device()
{
	// if BAR1 is not defined, BAR0 must be 64 bit, BAR2 is ok.
	// if BAR1 is not 64 bit memory, BAR2 is ok.
	if (!DEFINED("bar1")
	 || !(GET_FIELD("bar1/type")->compare("mem")
	   && GET_FIELD("bar1/width")->compare("bits64"))) {
		BAR("bar2", 0x18);
	}

	// if BAR2 is not defined, BAR1 must be 64 bit, BAR3 is ok.
	// if BAR2 is not 64 bit memory, BAR3 is ok.
	if (!DEFINED("bar2")
	 || !(GET_FIELD("bar2/type")->compare("mem")
	   && GET_FIELD("bar2/width")->compare("bits64"))) {
		BAR("bar3", 0x1c);
	}

	// if BAR3 is not defined, BAR2 must be 64 bit, BAR4 is ok.
	// if BAR3 is not 64 bit memory, BAR4 is ok.
	if (!DEFINED("bar3")
	 || !(GET_FIELD("bar3/type")->compare("mem")
	   && GET_FIELD("bar3/width")->compare("bits64"))) {
		BAR("bar4", 0x20);
	}

	// if BAR4 is not defined, BAR3 must be 64 bit, BAR5 is ok.
	// if BAR4 is not 64 bit memory, BAR5 is ok.
	if (!DEFINED("bar4")
	 || !(GET_FIELD("bar4/type")->compare("mem")
	   && GET_FIELD("bar4/width")->compare("bits64"))) {
		BAR("bar5", 0x24);
	}

	REGFIELD32("cisptr", 0x28, "addr32_t");

	// Subsystem IDs
	REGFIELD16("subvendor", 0x2c, "pci_vendor_t");
	REGFIELD16("subsystem", 0x2e, "hex16_t");

	// Capabilities
	explore_capabilities();

	// Expansion ROM
	REG32("%rombase", 0x30);
	OPEN_SCOPE("rombase");
		ONE_BIT_FIELD("enabled", "yesno_t", "%rombase", 0);
		COMPLEX_FIELD("address", "addr32_t",
				{"0", 10, 0},
				{"%rombase", 31, 11});
	CLOSE_SCOPE();

	REGFIELD8("mingnt", 0x3e, ANON_INT("1/4 usecs"));
	REGFIELD8("maxlat", 0x3f, ANON_INT("1/4 usecs"));
}

/* populate the current scope with generic PCI device fields */
void
pci_generic_device()
{
	for (int i = 0; i < 4096; i += 4) {
		REG32(to_string(boost::format("%%PCI[%04x]") %i), i);
	}

	REGFIELD16("vendor", 0x00, "pci_vendor_t");
	REGFIELD16("device", 0x02, "hex16_t");

	REG16("%command", 0x04);
	OPEN_SCOPE("command");
		ONE_BIT_FIELD("io", "yesno_t", "%command", 0);
		ONE_BIT_FIELD("mem", "yesno_t", "%command", 1);
		ONE_BIT_FIELD("bm", "yesno_t", "%command", 2);
		ONE_BIT_FIELD("special", "yesno_t", "%command", 3);
		ONE_BIT_FIELD("mwinv", "yesno_t", "%command", 4);
		ONE_BIT_FIELD("vgasnoop", "yesno_t", "%command", 5);
		ONE_BIT_FIELD("perr", "yesno_t", "%command", 6);
		ONE_BIT_FIELD("step", "yesno_t", "%command", 7);
		ONE_BIT_FIELD("serr", "yesno_t", "%command", 8);
		ONE_BIT_FIELD("fbb", "yesno_t", "%command", 9);
		ONE_BIT_FIELD("intr", "yesno_t", "%command", 10);
	CLOSE_SCOPE();

	REG16("%status", 0x06);
	OPEN_SCOPE("status");
		ONE_BIT_FIELD("intr", "yesno_t", "%status", 3);
		ONE_BIT_FIELD("caps", "yesno_t", "%status", 4);
		ONE_BIT_FIELD("cap66", "yesno_t", "%status", 5);
		ONE_BIT_FIELD("user", "yesno_t", "%status", 6);
		ONE_BIT_FIELD("fbb", "yesno_t", "%status", 7);
		ONE_BIT_FIELD("mdperr", "yesno_t", "%status", 8);
		SIMPLE_FIELD("devsel", ANON_ENUM(
					{"FAST", 0},
					{"MEDIUM", 1},
					{"SLOW", 2}),
				"%status", 10, 9);
		ONE_BIT_FIELD("sigtabort", "yesno_t", "%status", 11);
		ONE_BIT_FIELD("rcvtabort", "yesno_t", "%status", 12);
		ONE_BIT_FIELD("rcvmabort", "yesno_t", "%status", 13);
		ONE_BIT_FIELD("sigserr", "yesno_t", "%status", 14);
		ONE_BIT_FIELD("perr", "yesno_t", "%status", 15);
	CLOSE_SCOPE();

	REGFIELD8("class", 0x0b, "pci_class_t");

	//FIXME: figure the best way to use the types in pci.c
	if (GET_FIELD("class")->compare("pre_classcode")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_pre_classcode");
	} else if (GET_FIELD("class")->compare("mass_storage")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_mass_storage");
	} else if (GET_FIELD("class")->compare("network")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_network");
	} else if (GET_FIELD("class")->compare("display")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_display");
	} else if (GET_FIELD("class")->compare("multimedia")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_multimedia");
	} else if (GET_FIELD("class")->compare("memory")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_memory");
	} else if (GET_FIELD("class")->compare("bridge")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_bridge");
	} else if (GET_FIELD("class")->compare("simple_comm")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_simple_comm");
	} else if (GET_FIELD("class")->compare("base_system")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_base_system");
	} else if (GET_FIELD("class")->compare("input")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_input");
	} else if (GET_FIELD("class")->compare("docking")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_docking");
	} else if (GET_FIELD("class")->compare("processor")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_processor");
	} else if (GET_FIELD("class")->compare("serial")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_serial");
	} else if (GET_FIELD("class")->compare("wireless")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_wireless");
	} else if (GET_FIELD("class")->compare("intelligent_io")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_intelligent_io");
	} else if (GET_FIELD("class")->compare("satellite")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_satellite");
	} else if (GET_FIELD("class")->compare("crypto")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_crypto");
	} else if (GET_FIELD("class")->compare("dsp")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_dsp");
	} else {
		REGFIELD8("subclass", 0x0a, "hex8_t");
	}

	//FIXME: it would be better to have detailed progintfs
	REGFIELD8("progintf", 0x09, "hex8_t");

	REGFIELD8("revision", 0x08, "hex8_t");

	REGFIELD8("cacheline", 0x0c, ANON_INT("DWORDs"));

	REG8("%hdrtype", 0x0e);
	COMPLEX_FIELD("hdrtype", ANON_ENUM(
				{"device", 0},
				{"pci_bridge", 1},
				{"cardbus_bridge", 2}),
			{"%hdrtype", 6, 0});
	ONE_BIT_FIELD("multifunc", "yesno_t", "%hdrtype", 7);

	REG8("%bist", 0x0f);
	OPEN_SCOPE("bist");
		ONE_BIT_FIELD("capable", "yesno_t", "%bist", 7);
		ONE_BIT_FIELD("start", "yesno_t", "%bist", 6);
		COMPLEX_FIELD("code", "hex8_t", {"%bist", 3, 0});
	CLOSE_SCOPE();

	REGFIELD8("intline", 0x3c, "int_t");

	REGFIELD8("intpin", 0x3d, ANON_ENUM(
				{"none", 0},
				{"inta", 1},
				{"intb", 2},
				{"intc", 3},
				{"intd", 4}));

	// Base Address Registers - all devices have at least 2 BARs.
	BAR("bar0", 0x10);

	// if BAR0 is not 64 bit memory, declare BAR1
	if (!(GET_FIELD("bar0/type")->compare("mem")
	   && GET_FIELD("bar0/width")->compare("bits64"))) {
		BAR("bar1", 0x14);
	}

	if (GET_FIELD("hdrtype")->compare("device")) {
		create_device();
	} else if (GET_FIELD("hdrtype")->compare("pci_bridge")) {
		create_pci_bridge();
	} else if (GET_FIELD("hdrtype")->compare("cardbus_bridge")) {
		//TODO: need a spec
	}
}
