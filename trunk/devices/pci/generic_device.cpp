#include "pp.h"
#include "generic_device.h"
#include "fake_language.h"

//FIXME unify cap, sts, en, dis, msk, svr names everywhere.  Look at aer

void
PCI_SCOPE(const string &name, const pp_value &seg, const pp_value &bus,
		const pp_value &dev, const pp_value &func)
{
	OPEN_SCOPE(name, BIND("pci", ARGS(seg, bus, dev, func)));
	BOOKMARK("pci");
	pci_generic_device();
}

// All standard BARs look like this.
static void
BAR(const string &name, const pp_value &address)
{
	OPEN_SCOPE(name);

	REG32("%lower", address);
	FIELD("type", ANON_ENUM(
				KV("mem", 0),
				KV("io", 1)),
			BITS("%lower", 0));

	if (FIELD_EQ("type", "mem")) {
		FIELD("width", ANON_ENUM(
					KV("bits32", 0),
					KV("bits20", 1),
					KV("bits64", 2)),
				BITS("%lower", 2, 1));
		FIELD("prefetch", "yesno_t", BITS("%lower", 3));
	}

	if (FIELD_EQ("type", "io")) {
		FIELD("address", "addr16_t",
				BITS("%lower", 15, 2) +
				BITS("%0", 1, 0));
	} else if (FIELD_EQ("width", "bits32")
	    || FIELD_EQ("width", "bits20")) {
		FIELD("address", "addr32_t",
				BITS("%lower", 31, 4) +
				BITS("%0", 3, 0));
	} else {
		REG32("%upper", address+4);
		FIELD("address", "addr64_t",
				BITS("%upper", 31, 0) +
				BITS("%lower", 31, 4) +
				BITS("%0", 3, 0));
	}
	CLOSE_SCOPE();
}

static void
ht_link_control(const pp_value &address)
{
	REG16("%control", address);
	OPEN_SCOPE("control");

	FIELD("SrcIdEn", "yesno_t", BITS("../%control", 0));
	FIELD("CFIEn", "yesno_t", BITS("../%control", 1));
	FIELD("CST", "yesno_t", BITS("../%control", 2));
	FIELD("CFEr", "yesno_t", BITS("../%control", 3));
	FIELD("LinkFail", "yesno_t", BITS("../%control", 4));
	FIELD("InitCmp", "yesno_t", BITS("../%control", 5));
	FIELD("EOC", "yesno_t", BITS("../%control", 6));
	FIELD("TXOff", "yesno_t", BITS("../%control", 7));
	FIELD("CRCErr", "hex4_t", BITS("../%control", 11, 8));
	FIELD("IsocEn", "yesno_t", BITS("../%control", 12));
	FIELD("LSEn", "yesno_t", BITS("../%control", 13));
	FIELD("ExtCTL", "yesno_t", BITS("../%control", 14));
	FIELD("b64En", "yesno_t", BITS("../%control", 15));

	CLOSE_SCOPE();
}

static void
ht_link_config(const pp_value &address)
{
	ENUM("ht_link_width_t",
			KV("bits8", 0),
			KV("bits16", 1),
			KV("bits32", 3),
			KV("bits2", 4),
			KV("bits4", 5),
			KV("nc", 7));

	REG16("%config", address);
	OPEN_SCOPE("config");

	FIELD("MaxLinkWidthIn", "ht_link_width_t", BITS("../%config", 2, 0));
	FIELD("DwFcIn", "yesno_t", BITS("../%config", 3));
	FIELD("MaxLinkWidthOut", "ht_link_width_t", BITS("../%config", 6, 4));
	FIELD("DwFcOut", "yesno_t", BITS("../%config", 7));

	FIELD("LinkWidthIn", "ht_link_width_t", BITS("../%config", 10, 8));
	FIELD("DwFcInEn", "yesno_t", BITS("../%config", 11));
	FIELD("LinkWidthOut", "ht_link_width_t", BITS("../%config", 14, 12));
	FIELD("DwFcOutEn", "yesno_t", BITS("../%config", 15));

	CLOSE_SCOPE();
}

static void
ht_link_freq_err(const pp_value &address)
{
	ENUM("ht_link_freq_t",
			KV("mhz200", 0),
			KV("mhz300", 1),
			KV("mhz400", 2),
			KV("mhz500", 3),
			KV("mhz600", 4),
			KV("mhz800", 5),
			KV("mhz1000", 6),
			KV("mhz1200", 7),
			KV("mhz1400", 8),
			KV("mhz1600", 9),
			KV("mhz1800", 1),
			KV("mhz2000", 11),
			KV("mhz2200", 12),
			KV("mhz2400", 13),
			KV("mhz2600", 14),
			KV("mhz100", 15));

	REG8("%freq_err", address);
	FIELD("Freq", "ht_link_freq_t", BITS("%freq_err", 3, 0));
	FIELD("ProtoErr", "yesno_t", BITS("%freq_err", 4));
	FIELD("OverErr", "yesno_t", BITS("%freq_err", 5));
	FIELD("EOCErr", "yesno_t", BITS("%freq_err", 6));
	FIELD("CTLTimeout", "yesno_t", BITS("%freq_err", 7));
}

static void
ht_link_freq_cap(const pp_value &address)
{
	BITMASK("ht_link_freq_cap_t",
			KV("mhz200", 0),
			KV("mhz300", 1),
			KV("mhz400", 2),
			KV("mhz500", 3),
			KV("mhz600", 4),
			KV("mhz800", 5),
			KV("mhz1000", 6),
			KV("mhz1200", 7),
			KV("mhz1400", 8),
			KV("mhz1600", 9),
			KV("mhz1800", 10),
			KV("mhz2000", 11),
			KV("mhz2200", 12),
			KV("mhz2400", 13),
			KV("mhz2600", 14));

	REG16("%freq_cap", address);
	FIELD("FreqCap", "ht_link_freq_cap_t", BITS("%freq_cap", 14, 0));
}

static void
ht_error_handling(const pp_value &address)
{
	REG16("%error", address);
	OPEN_SCOPE("error");

	FIELD("proto_flood", "yesno_t", BITS("../%error", 0));
	FIELD("over_flood", "yesno_t", BITS("../%error", 1));
	FIELD("proto_fatal", "yesno_t", BITS("../%error", 2));
	FIELD("over_fatal", "yesno_t", BITS("../%error", 3));
	FIELD("eoc_fatal", "yesno_t", BITS("../%error", 4));
	FIELD("resp_fatal", "yesno_t", BITS("../%error", 5));
	FIELD("crc_fatal", "yesno_t", BITS("../%error", 6));
	FIELD("serr_fatal", "yesno_t", BITS("../%error", 7));
	FIELD("chain_fail", "yesno_t", BITS("../%error", 8));
	FIELD("resp_err", "yesno_t", BITS("../%error", 9));
	FIELD("proto_nonfatal", "yesno_t", BITS("../%error", 10));
	FIELD("over_nonfatal", "yesno_t", BITS("../%error", 11));
	FIELD("eoc_nonfatal", "yesno_t", BITS("../%error", 12));
	FIELD("resp_nonfatal", "yesno_t", BITS("../%error", 13));
	FIELD("crc_nonfatal", "yesno_t", BITS("../%error", 14));
	FIELD("serr_nonfatal", "yesno_t", BITS("../%error", 15));
	CLOSE_SCOPE();
}

static void
ht_slave_link(const string &name, const pp_value &address)
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
ht_slave_capability(const pp_value &address)
{
	REG16("%command", address + 2);
	FIELD("base_unit_id", "int_t", BITS("%command", 4, 0));
	FIELD("unit_count", "int_t", BITS("%command", 9, 5));
	FIELD("master", "yesno_t", BITS("%command", 10));
	FIELD("default_direction", "yesno_t", BITS("%command", 11));
	FIELD("drop_on_uninit", "yesno_t", BITS("%command", 12));

	ht_slave_link("link0", address + 0x04);
	ht_slave_link("link1", address + 0x08);

	//FIXME: clean up HT field names all over - inconsistent BumpyCaps
	REG8("%rev", address + 0x0c);
	FIELD("MajorRev", "int_t", BITS("%rev", 7, 5));
	FIELD("MinorRev", "int_t", BITS("%rev", 4, 0));

	REG8("%feature", address + 0x10);
	OPEN_SCOPE("feature");
	FIELD("iso_flow_ctrl", "yesno_t", BITS("../%feature", 0));
	FIELD("ldtstop", "yesno_t", BITS("../%feature", 1));
	FIELD("crc_test_mode", "yesno_t", BITS("../%feature", 2));
	FIELD("extended_ctl_time", "yesno_t", BITS("../%feature", 3));
	FIELD("bits64_addressing", "yesno_t", BITS("../%feature", 4));
	FIELD("unit_id_reorder_dis", "yesno_t", BITS("../%feature", 5));
	FIELD("source_id", "yesno_t", BITS("../%feature", 6));
	CLOSE_SCOPE();

	REGFIELD16("scratchpad", address + 0x14, "hex16_t");

	ht_error_handling(address + 0x16);

	REGFIELD8("mem_base_hi", address + 0x18, "addr16_t");
	REGFIELD8("mem_limit_hi", address + 0x19, "addr16_t");
	REGFIELD8("bus", address + 0x1a, "int_t");
}

static void
ht_host_link(const string &name, const pp_value &address)
{
	OPEN_SCOPE(name);
	ht_link_control(address);
	ht_link_config(address+2);
	ht_link_freq_err(address+0x05);
	ht_link_freq_cap(address+0x06);
	CLOSE_SCOPE();
}

static void
ht_host_capability(const pp_value &address)
{
	REG16("%command", address + 2);
	FIELD("warm_reset", "yesno_t", BITS("%command", 0));
	FIELD("dbl_ended", "yesno_t", BITS("%command", 1));
	FIELD("device", "int_t", BITS("%command", 6, 2));
	FIELD("chain_side", "yesno_t", BITS("%command", 7));
	FIELD("host_hide", "yesno_t", BITS("%command", 8));
	FIELD("act_slave", "yesno_t", BITS("%command", 10));
	FIELD("in_eoc_err", "yesno_t", BITS("%command", 11));
	FIELD("drop_on_uninit", "yesno_t", BITS("%command", 12));

	ht_host_link("link", address + 0x04);

	REG8("%rev", address + 0x08);
	FIELD("major_rev", "int_t", BITS("%rev", 7, 5));
	FIELD("minor_rev", "int_t", BITS("%rev", 4, 0));

	//FIXME: some of these only apply to ht3, if you want to be strict
	REG16("%feature", address + 0x0c);
	OPEN_SCOPE("feature");
	FIELD("iso_flow_ctrl", "yesno_t", BITS("../%feature", 0));
	FIELD("ldtstop", "yesno_t", BITS("../%feature", 1));
	FIELD("crc_test_mode", "yesno_t", BITS("../%feature", 2));
	FIELD("extended_ctl_time", "yesno_t", BITS("../%feature", 3));
	FIELD("bits64_addressing", "yesno_t", BITS("../%feature", 4));
	FIELD("unit_id_reorder_dis", "yesno_t", BITS("../%feature", 5));
	FIELD("source_id", "yesno_t", BITS("../%feature", 6));
	FIELD("ext_regs", "yesno_t", BITS("../%feature", 8));
	FIELD("upstream_cfg", "yesno_t", BITS("../%feature", 9));
	CLOSE_SCOPE();

	if (FIELD_BOOL("feature/ext_regs")) {
		REGFIELD16("scratchpad", address + 0x10, "hex16_t");
		ht_error_handling(address + 0x12);
		REGFIELD8("mem_base_hi", address + 0x14, "addr16_t");
		REGFIELD8("mem_limit_hi", address + 0x15, "addr16_t");
	}
}

static void
ht_revision_capability(const pp_value &address)
{
	REG8("%rev", address+2);
	FIELD("major_rev", "int_t", BITS("%rev", 7, 5));
	FIELD("minor_rev", "int_t", BITS("%rev", 4, 0));
}

static void
ht_extended_config_capability(const pp_value &address)
{
	REG32("%address", address+4);
	FIELD("type", ANON_ENUM(
				KV("type0", 0),
				KV("type1", 1)),
			BITS("%address", 28));
	FIELD("bus", "int_t", BITS("%address", 27, 20));
	FIELD("device", "int_t", BITS("%address", 19, 15));
	FIELD("function", "int_t", BITS("%address", 14, 12));
	FIELD("register", "hex_t",
			BITS("%address", 11, 2) +
			BITS("%0", 1, 0));
	REGFIELD32("data", address+8, "hex32_t");
}

static void
ht_address_mapping_capability(const pp_value &address)
{
	REG16("%command", address + 0x02);
	FIELD("num_dma", "int_t", BITS("%command", 3, 0));
	FIELD("io_size", "int_t", BITS("%command", 8, 4));
	FIELD("map_type", ANON_ENUM(
				KV("bits40", 0),
				KV("bits64", 1)),
			BITS("%command", 10, 9));

	if (FIELD_EQ("map_type", "bits40")) {
		REG32("%sec_non_prefetch", address + 0x04);
		OPEN_SCOPE("sec_non_prefetch");
		FIELD("enable", "yesno_t", BITS("../%sec_non_prefetch", 31));
		FIELD("isoch", "yesno_t", BITS("../%sec_non_prefetch", 30));
		FIELD("noncoherent", "yesno_t",
			 BITS("../%sec_non_prefetch", 29));
		FIELD("compat", "yesno_t", BITS("../%sec_non_prefetch", 28));
		FIELD("base", "addr64_t",
				BITS("../%sec_non_prefetch", 19, 0) +
				BITS("%0", 19, 0));
		CLOSE_SCOPE();

		REG32("%sec_prefetch", address + 0x08);
		OPEN_SCOPE("sec_prefetch");
		FIELD("enable", "yesno_t", BITS("../%sec_prefetch", 31));
		FIELD("isochronous", "yesno_t", BITS("../%sec_prefetch", 30));
		FIELD("noncoherent", "yesno_t", BITS("../%sec_prefetch", 29));
		FIELD("compat", "yesno_t", BITS("../%sec_prefetch", 28));
		FIELD("base", "addr64_t",
				BITS("../%sec_prefetch", 19, 0) +
				BITS("%0", 19, 0));
		CLOSE_SCOPE();

		pp_value value = READ("num_dma");
		for (unsigned i = 0; i < value; i++) {
			OPEN_SCOPE("dma[]");
			REG32("%lower", address + 0x0c + (8*i));
			REG32("%upper", address + 0x10 + (8*i));

			FIELD("enable", "yesno_t", BITS("%lower", 31));
			FIELD("isochronous", "yesno_t", BITS("%lower", 30));
			FIELD("noncoherent", "yesno_t", BITS("%lower", 29));

			FIELD("pri_base", "addr64_t",
					BITS("%lower", 15, 0) +
					BITS("%0", 23, 0));
			FIELD("sec_base", "addr64_t",
					BITS("%lower", 31, 16) +
					BITS("%0", 23, 0));
			FIELD("sec_limit", "addr64_t",
					BITS("%lower", 15, 0) +
					BITS("%1", 23, 0));
			CLOSE_SCOPE();
		}
	} else if (FIELD_EQ("map_type", "bits64")) {
		//FIXME: procfields index/data pair access within PCI
	}
}

static void
ht_msi_mapping_capability(const pp_value &address)
{
	REG8("%flags", address+2);
	FIELD("en", "yesno_t", BITS("%flags", 0));
	FIELD("fixed", "yesno_t", BITS("%flags", 1));
	if (!FIELD_BOOL("fixed")) {
		REG32("%lower", address+4);
		REG32("%upper", address+8);
		FIELD("address", "addr64_t",
				BITS("%upper", 31, 0) +
				BITS("%lower", 31, 20) +
				BITS("%0", 19, 0));
	}
}

static void
power_mgmt_capability(const pp_value &address)
{
	REG16("%pmc", address+2);
	FIELD("version", ANON_ENUM(
				KV("pcipm_v1_0", 1),
				KV("pcipm_v1_1", 2),
				KV("pcipm_v1_2", 3)),
			BITS("%pmc", 2, 0));
	FIELD("clock", "yesno_t", BITS("%pmc", 3));
	FIELD("dsi", "yesno_t", BITS("%pmc", 5));
	FIELD("aux_current", ANON_ENUM(
				KV("self", 0),
				KV("mA_55", 1),
				KV("mA_100", 2),
				KV("mA_160", 3),
				KV("mA_220", 4),
				KV("mA_270", 5),
				KV("mA_320", 6),
				KV("mA_375", 7)),
			BITS("%pmc", 8, 6));
	FIELD("d1_support", "yesno_t", BITS("%pmc", 9));
	FIELD("d2_support", "yesno_t", BITS("%pmc", 10));
	FIELD("pme_support", ANON_BITMASK(
				KV("d0", 0),
				KV("d1", 1),
				KV("d2", 2),
				KV("d3hot", 3),
				KV("d3cold", 4)),
			BITS("%pmc", 15, 11));

	REG16("%pmcsr", address+4);
	FIELD("power_state", ANON_ENUM(
				KV("d0", 0),
				KV("d1", 1),
				KV("d2", 2),
				KV("d3hot", 3)),
			BITS("%pmcsr", 1, 0));
	FIELD("no_soft_reset", "yesno_t", BITS("%pmcsr", 3));
	FIELD("pme_en", "yesno_t", BITS("%pmcsr", 8));
	FIELD("pme_status", "yesno_t", BITS("%pmcsr", 15));

	REG8("%pmcsr_bse", address+6);
	FIELD("bpcc_en", "yesno_t", BITS("%pmcsr_bse", 7));
	if (FIELD_BOOL("bpcc_en")) {
		FIELD("b2_b3", "yesno_t", BITS("%pmcsr_bse", 6));
	}

	//FIXME: procfield: this is really an index/data pair set of regs
	FIELD("data_select", "hex4_t", BITS("%pmcsr", 12, 9));
	FIELD("data_scale", "hex4_t", BITS("%pmcsr", 14, 13));
	REG8("%data", address+7);
	FIELD("data", "hex8_t", BITS("%data", 7, 0));
}

static void
slot_id_capability(const pp_value &address)
{
	REG8("%slot", address+2);
	FIELD("nslots", "int_t", BITS("%slot", 4, 0));
	FIELD("first", "yesno_t", BITS("%slot", 5));
	REGFIELD8("chassis", address+3, "int_t");
}

static void
msi_capability(const pp_value &address)
{
	// message control
	REG16("%msg_ctrl", address + 2);
	FIELD("msi_enable", "yesno_t", BITS("%msg_ctrl", 0));
	FIELD("multi_msg_cap", ANON_ENUM(
				KV("vec1", 0),
				KV("vec2", 1),
				KV("vec4", 2),
				KV("vec8", 3),
				KV("vec16", 4),
				KV("vec32", 5)),
			BITS("%msg_ctrl", 3, 1));
	FIELD("multi_msg_en", ANON_ENUM(
				KV("vec1", 0),
				KV("vec2", 1),
				KV("vec4", 2),
				KV("vec8", 3),
				KV("vec16", 4),
				KV("vec32", 5)),
			BITS("%msg_ctrl", 6, 4));
	FIELD("cap64", "yesno_t", BITS("%msg_ctrl", 7));
	FIELD("mask_per_vec", "yesno_t", BITS("%msg_ctrl", 8));

	// message address
	REG32("%msg_addr", address + 4);

	// is this 64 bit capable?
	if (!FIELD_BOOL("cap64")) {
		// no, just use the low address and define the data
		FIELD("msg_addr", "addr32_t",
				BITS("%msg_addr", 31, 2) +
				BITS("%0", 1, 0));
		REGFIELD16("msg_data", address + 8, "hex16_t");

		if (FIELD_BOOL("mask_per_vec")) {
			// mask and pending bits
			REG32("%mask", address + 12);
			REG32("%pending", address + 16);
		}
	} else {
		// add the high address and define the data
		REG32("%msg_addr_hi",  address + 8);
		FIELD("msg_addr", "addr64_t",
				BITS("%msg_addr_hi", 31, 0) +
				BITS("%msg_addr", 31, 2) +
				BITS("%0", 1, 0));
		REGFIELD16("msg_data", address + 12, "hex16_t");

		if (FIELD_BOOL("mask_per_vec")) {
			// mask and pending bits
			REG32("%mask", address + 16);
			REG32("%pending", address + 20);
		}
	}

	if (FIELD_BOOL("mask_per_vec")) {
		pp_value vecs = 1 << READ("multi_msg_cap");
		for (unsigned i = 0; i < vecs; i++) {
			FIELD("mask[]", "yesno_t", BITS("%mask", i));
			FIELD("pend[]", "yesno_t", BITS("%pending", i));
		}
	}
}

static void
msix_capability(const pp_value &address)
{
	REG16("%msg_ctrl", address + 2);
	FIELD("msix_enable", "yesno_t", BITS("%msg_ctrl", 15));
	FIELD("table_size", ANON_XFORM("int_t", LAMBDA(_1+1), LAMBDA(_1-1)),
			BITS("%msg_ctrl", 9, 0));
	FIELD("func_mask", "yesno_t", BITS("%msg_ctrl", 14));

	// the table is memory mapped through a BAR
	REG32("%table_ptr", address + 4);
	FIELD("table_bir", ANON_ENUM(
				KV("bar0", 0),
				KV("bar1", 1),
				KV("bar2", 2),
				KV("bar3", 3),
				KV("bar4", 4),
				KV("bar5", 5)),
			BITS("%table_ptr", 2, 0));
	FIELD("table_offset", "hex_t",
			BITS("%table_ptr", 31, 3) +
			BITS("%0", 2, 0));

	pp_value table_size = READ("table_size") + 1;

	//FIXME: a better way to do this?
	string bar;
	pp_value base, size;

	bar = "$pci/" + GET_FIELD("table_bir")->evaluate() + "/address";
	base = READ(bar) + READ("table_offset");
	size = table_size * 16;
	OPEN_SCOPE("table", BIND("mem", ARGS(base, size))); {
		for (unsigned i = 0; i < table_size; i++) {
			OPEN_SCOPE("entry[]"); {
				REG32("%msg_addr", i*16 + 0);
				REG32("%msg_upper_addr", i*16 + 4);
				FIELD("address", "addr64_t",
						BITS("%msg_upper_addr", 31, 0) +
						BITS("%msg_addr", 31, 2) +
						BITS("%0", 1, 0));
				REGFIELD32("msg_data", i*16 + 8, "hex_t");
				REG32("%vector_ctrl", i*16 + 12);
				FIELD("mask", "yesno_t",
						BITS("%vector_ctrl", 0));
			} CLOSE_SCOPE();
		}
	} CLOSE_SCOPE();

	// the pending bit array is memory mapped through a BAR
	REG32("%pba_ptr", address + 8);
	FIELD("pba_bir", ANON_ENUM(
				KV("bar0", 0),
				KV("bar1", 1),
				KV("bar2", 2),
				KV("bar3", 3),
				KV("bar4", 4),
				KV("bar5", 5)),
			BITS("%pba_ptr", 2, 0));
	FIELD("pba_offset", "hex_t",
			BITS("%pba_ptr", 31, 3) +
			BITS("%0", 2, 0));

	//FIXME: a better way to do this?
	bar = "$pci/" + GET_FIELD("pba_bir")->evaluate() + "/address";
	base = READ(bar) + READ("table_offset");
	size = (table_size+63) / 64;
	OPEN_SCOPE("pba", BIND("mem", ARGS(base, size * 8))); {
		pp_value tmp_size = table_size;
		// loop for each PBA QWORD
		for (unsigned i = 0; i < table_size; i++) {
			REG64("%pending[]", i);
			for (size_t j = 0; j < 64; j++) {
				if (j >= tmp_size)
					break;
				FIELD("pending" + to_string(i*64 + j),
				    "yesno_t", BITS("%pending[$]", j));
			}
			tmp_size -= 64;
		}
	} CLOSE_SCOPE();
}

static void
ht_capability(const pp_value &address)
{
	REG8("%subcap", address + 3);
	// Check the upper two bits - if they are zero, then we have
	// a slave/primary interface block or a host/secondary interface
	// block.  If they are non-zero, we have some other HT capability.
	// This matters because they use different encodings of the
	// capability type (3 bits vs 5 bits).
	FIELD("is_interface", ANON_BOOL("no", "yes"), BITS("%subcap", 7, 6));
	if (FIELD_EQ("is_interface", "yes")) {
		// subtype is a 3 bit field
		FIELD("subtype", ANON_ENUM(
					KV("slave_primary", 0),
					KV("host_secondary", 1)),
				BITS("%subcap", 7, 5));

		if (FIELD_EQ("subtype", "slave_primary")) {
			ht_slave_capability(address);
		} else if (FIELD_EQ("subtype", "host_secondary")) {
			ht_host_capability(address);
		}
	} else {
		// subtype is a 5 bit field
		FIELD("subtype", ANON_ENUM(
					KV("switch", 8),
					KV("intr_discovery", 16),
					KV("revision", 17),
					KV("unit_id_clump", 18),
					KV("extended_config", 19),
					KV("address_mapping", 20),
					KV("msi_mapping", 21),
					KV("direct_route", 22),
					KV("vc_set", 23),
					KV("retry_mode", 24),
					KV("x86_encode", 25),
					KV("gen3", 26),
					KV("function_extend", 27),
					KV("power_mgmt", 28)),
				BITS("%subcap", 7, 3));

		if (FIELD_EQ("subtype", "switch")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "intr_discovery")) {
			//FIXME: not implemented yet
			//procfields: index/data accesses within PCI space
		} else if (FIELD_EQ("subtype", "revision")) {
			ht_revision_capability(address);
		} else if (FIELD_EQ("subtype", "unit_id_clump")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "extended_config")) {
			ht_extended_config_capability(address);
		} else if (FIELD_EQ("subtype", "address_mapping")) {
			ht_address_mapping_capability(address);
		} else if (FIELD_EQ("subtype", "msi_mapping")) {
			ht_msi_mapping_capability(address);
		} else if (FIELD_EQ("subtype", "direct_route")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "vc_set")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "retry_mode")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "x86_encode")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "gen3")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "function_extend")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "power_mgmt")) {
			//FIXME: not implemented yet
			//procfields: index/data pairs in PCI space
		}
	}
}

static void
ssid_capability(const pp_value &address)
{
	REGFIELD16("ssvid", address + 0x04, "pci_vendor_t");
	REGFIELD16("ssid", address + 0x06, "hex16_t");
}

static void
pcie_capability(const pp_value &address)
{
	// all PCI-E devices implement this block
	REG16("%pcie_caps", address + 0x02);
	FIELD("version", "int_t", BITS("%pcie_caps", 3, 0));
	FIELD("type", ANON_ENUM(
				KV("endpoint", 0),
				KV("legacy_endpoint", 1),
				KV("root_port", 4),
				KV("upstream_switch_port", 5),
				KV("downstream_switch_port", 6),
				KV("pcie_pci_bridge", 7),
				KV("pci_pcie_bridge", 8),
				KV("root_integrated_endpoint", 9),
				KV("root_event_collector", 10)),
			BITS("%pcie_caps", 7, 4));
	if (FIELD_EQ("type", "root_port")
	 || FIELD_EQ("type", "downstream_switch_port")) {
		FIELD("slot_impl", "yesno_t", BITS("%pcie_caps", 8));
	}
	FIELD("intr_msg_num", "int_t", BITS("%pcie_caps", 13, 9));
	FIELD("tcs", "yesno_t", BITS("%pcie_caps", 14));

	// common datatypes
	ENUM("pcie_l0s_latency_t",
			KV("ns_64", 0),
			KV("ns_128", 1),
			KV("ns_256", 2),
			KV("ns_512", 3),
			KV("us_1", 4),
			KV("us_2", 5),
			KV("us_4", 6),
			KV("us_4_plus", 7));
	ENUM("pcie_l1_latency_t",
			KV("us_1", 0),
			KV("us_2", 1),
			KV("us_4", 2),
			KV("us_8", 3),
			KV("us_16", 4),
			KV("us_32", 5),
			KV("us_64", 6),
			KV("us_64_plus", 7));
	ENUM("pcie_width_t",
			KV("x0", 0),
			KV("x1", 1),
			KV("x2", 2),
			KV("x4", 4),
			KV("x8", 8),
			KV("x12", 12),
			KV("x16", 16),
			KV("x32", 32));
	ENUM("pcie_link_speed_t",
			KV("GTs_25_only", 0),
			KV("GTs_25", 1),
			KV("GTs_50", 2));


	// all PCI-E devices implement this block
	OPEN_SCOPE("device");
	REG32("%dev_caps", address + 0x04);
	REG16("%dev_control", address + 0x08);
	REG16("%dev_status", address + 0x0a);

	ENUM("pcie_payload_size_t",
			KV("bytes128", 0),
			KV("bytes256", 1),
			KV("bytes512", 2),
			KV("bytes1024", 3),
			KV("bytes2048", 4),
			KV("bytes4096", 5));
	FIELD("max_payload_cap", "pcie_payload_size_t",
			BITS("%dev_caps", 2, 0));
	FIELD("max_payload", "pcie_payload_size_t",
			BITS("%dev_control", 7, 5));
	FIELD("max_read", "pcie_payload_size_t",
			BITS("%dev_control", 14, 12));

	FIELD("phantom_funcs_cap", ANON_ENUM(
				KV("phantom_0", 0),
				KV("phantom_4", 1),
				KV("phantom_6", 2),
				KV("phantom_8", 3)),
			BITS("%dev_caps", 4, 3));
	FIELD("phantom_funcs", "yesno_t", BITS("%dev_control", 9));

	FIELD("ext_tag_field_cap", ANON_ENUM(
				KV("b5", 0),
				KV("b8", 1)),
			BITS("%dev_caps", 5));
	FIELD("ext_tag_field", "yesno_t", BITS("%dev_control", 8));

	if (FIELD_EQ("../type", "endpoint")
	 || FIELD_EQ("../type", "legacy_endpoint")
	 || FIELD_EQ("../type", "root_integrated_endpoint")) {
		FIELD("max_l0s_lat", "pcie_l0s_latency_t",
				BITS("%dev_caps", 8, 6));
		FIELD("max_l1_lat", "pcie_l1_latency_t",
				BITS("%dev_caps", 11, 9));
		FIELD("func_reset_cap", "yesno_t", BITS("%dev_caps", 28));
		FIELD("func_reset", "yesno_t", BITS("%dev_control", 15));
	}

	if (FIELD_EQ("../type", "upstream_switch_port")) {
		FIELD("power_limit_value", "int_t", BITS("%dev_caps", 25, 18));
		FIELD("power_limit_scale", ANON_ENUM(
					KV("x1_0", 0),
					KV("x0_1", 1),
					KV("x0_01", 2),
					KV("x0_001", 3)),
				BITS("%dev_caps", 27, 26));
	}
	FIELD("role_based_err_cap", "yesno_t", BITS("%dev_caps", 15));
	FIELD("corr_err_report", "yesno_t", BITS("%dev_control", 0));
	FIELD("corr_err_det", "yesno_t", BITS("%dev_status", 0));
	FIELD("nonfatal_err_report", "yesno_t", BITS("%dev_control", 1));
	FIELD("nonfatal_err_det", "yesno_t", BITS("%dev_status", 1));
	FIELD("fatal_err_report", "yesno_t", BITS("%dev_control", 2));
	FIELD("fatal_err_det", "yesno_t", BITS("%dev_status", 2));
	FIELD("unsup_req_report", "yesno_t", BITS("%dev_control", 3));
	FIELD("unsup_req_det", "yesno_t", BITS("%dev_status", 3));
	FIELD("relax", "yesno_t", BITS("%dev_control", 4));
	FIELD("aux_pm", "yesno_t", BITS("%dev_control", 10));
	FIELD("aux_pm_det", "yesno_t", BITS("%dev_status", 4));
	FIELD("en_no_snoop", "yesno_t", BITS("%dev_control", 11));
	if (!DEFINED("func_reset") && FIELD_EQ("$pci/class", "bridge")) {
		FIELD("bridge_retry_en", "yesno_t", BITS("%dev_control", 15));
	}
	FIELD("txn_pend", "yesno_t", BITS("%dev_status", 5));

	if (FIELD_GE("../version", 2)) {
		REG32("%dev_caps2", address + 0x24);
		REG16("%dev_control2", address + 0x28);
		REG16("%dev_status2", address + 0x2a);

		ENUM("pcie_completion_timeout_t",
				KV("us_50_ms_50", 0),
				KV("us_50_ms_10", 1),
				KV("ms_10_ms_250", 2),
				KV("us_50_ms_250", 3),
				KV("ms_10_s_4", 6),
				KV("us_50_s_4", 7),
				KV("ms_10_s_64", 14),
				KV("us_50_s_64", 15));

		FIELD("completion_timeout_ranges",
				"pcie_completion_timeout_t",
				BITS("%dev_caps2", 3, 0));
		FIELD("completion_timeout",
				"pcie_completion_timeout_t",
				BITS("%dev_control2", 3, 0));

		FIELD("completion_timeout_disable_en", "yesno_t",
				BITS("%dev_caps2", 4));
		FIELD("completion_timeout_disable", "yesno_t",
				BITS("%dev_control2", 4));
	}

	CLOSE_SCOPE(); // device

	//
	// not all devices implement the remaining blocks
	//
	if (FIELD_EQ("type", "endpoint")
	 || FIELD_EQ("type", "legacy_endpoint")
	 || FIELD_EQ("type", "root_port")
	 || FIELD_EQ("type", "upstream_switch_port")
	 || FIELD_EQ("type", "downstream_switch_port")
	 || FIELD_EQ("type", "pcie_pci_bridge")
	 || FIELD_EQ("type", "pci_pcie_bridge")) {
		OPEN_SCOPE("link");
		REG32("%link_caps", address + 0x0c);
		REG16("%link_control", address + 0x10);
		REG16("%link_status", address + 0x12);

		FIELD("port_number", "int_t", BITS("%link_caps", 31, 24));

		FIELD("supported_speeds", ANON_ENUM(
					KV("GTs_25", 1),
					KV("GTs_50_25", 2)),
				BITS("%link_caps", 3, 0));
		FIELD("speed", "pcie_link_speed_t",
				BITS("%link_status", 3, 0));
		FIELD("max_width", "pcie_width_t", BITS("%link_caps", 9, 4));
		FIELD("width", "pcie_width_t", BITS("%link_status", 9, 4));

		FIELD("aspm_support", ANON_BITMASK(
					KV("l0s", 0),
					KV("l1", 1)),
				BITS("%link_caps", 11, 10));
		FIELD("aspm_ctl", ANON_BITMASK(
					KV("l0s", 0),
					KV("l1", 1)),
				BITS("%link_control", 1, 0));

		FIELD("l0s_exit_latency", "pcie_l0s_latency_t",
				BITS("%link_caps", 14, 12));
		FIELD("l1_exit_latency", "pcie_l1_latency_t",
				BITS("%link_caps", 17, 15));

		FIELD("clock_pm_cap", "yesno_t", BITS("%link_caps", 18));
		FIELD("clock_pm_en", "yesno_t", BITS("%link_control", 8));

		FIELD("surprise_down_err_report_cap", "yesno_t",
				BITS("%link_caps", 19));
		FIELD("data_link_active_report_cap", "yesno_t",
				BITS("%link_caps", 20));
		if (FIELD_BOOL("data_link_active_report_cap")) {
			FIELD("data_link_active", "yesno_t",
					BITS("%link_status", 13));
		}
		FIELD("link_bw_notify_cap", "yesno_t",
				BITS("%link_caps", 21));
		if (FIELD_BOOL("link_bw_notify_cap")) {
			FIELD("link_bw_status", "yesno_t",
					BITS("%link_status", 14));
			FIELD("link_auto_bw_status", "yesno_t",
					BITS("%link_status", 15));
		}

		FIELD("rcb", ANON_BOOL("b64", "b128"),
				BITS("%link_control", 3));
		if (FIELD_EQ("../type", "root_port")
		 || FIELD_EQ("../type", "downstream_switch_port")
		 || FIELD_EQ("../type", "pci_pcie_bridge")) {
			FIELD("link_disable", "yesno_t",
					BITS("%link_control", 4));
			FIELD("link_retrain", "yesno_t",
					BITS("%link_control", 5));
			FIELD("link_bw_mgmt_intr_en", "yesno_t",
					BITS("%link_control", 10));
			FIELD("link_auto_bw_mgmt_intr_en", "yesno_t",
					BITS("%link_control", 11));
		}
		FIELD("common_clock", "yesno_t", BITS("%link_control", 6));
		FIELD("extended_sync", "yesno_t", BITS("%link_control", 7));
		FIELD("hw_auto_width_dis", "yesno_t",
				BITS("%link_control", 9));

		if (FIELD_EQ("../type", "root_port")
		 || FIELD_EQ("../type", "downstream_switch_port")
		 || FIELD_EQ("../type", "pci_pcie_bridge")) {
			FIELD("link_training", "yesno_t",
					BITS("%link_status", 11));
		}
		FIELD("slot_clock", "yesno_t", BITS("%link_status", 12));

		if (FIELD_GE("../version", 2)) {
			REG32("%link_caps2", address + 0x2c);
			REG16("%link_control2", address + 0x30);
			REG16("%link_status2", address + 0x32);

			FIELD("target_link_speed", "pcie_link_speed_t",
					BITS("%link_control2", 3, 0));
			FIELD("enter_compliance", "yesno_t",
					BITS("%link_control2", 4));
			FIELD("hw_auto_speed_dis", "yesno_t",
					BITS("%link_control2", 5));
			FIELD("selecatble_deemphasis",
					ANON_BOOL("dB_neg_6_0", "dB_neg_3_5"),
					BITS("%link_control2", 6));
			FIELD("transit_margin", "hex4_t",
					BITS("%link_control2", 9, 7));
			FIELD("enter_mod_compliance", "yesno_t",
					BITS("%link_control2", 10));
			FIELD("compliance_sos", "yesno_t",
					BITS("%link_control2", 11));
			FIELD("compliance_deemphasis",
					ANON_BOOL("dB_neg_6_0", "dB_neg_3_5"),
					BITS("%link_control2", 12));

			FIELD("current_deemphasis",
					ANON_BOOL("dB_neg_6_0", "dB_neg_3_5"),
					BITS("%link_status2", 0));
		}

		CLOSE_SCOPE(); // link
	}

	if (DEFINED("slot_impl") && FIELD_BOOL("slot_impl")) {
		OPEN_SCOPE("slot");
		REG32("%slot_caps", address + 0x14);
		REG16("%slot_control", address + 0x18);
		REG16("%slot_status", address + 0x1a);

		FIELD("slot_number", "int_t", BITS("%slot_caps", 31, 19));

		FIELD("attn_button", "yesno_t", BITS("%slot_caps", 0));
		if (FIELD_BOOL("attn_button")) {
			FIELD("attn_button_en", "yesno_t",
					BITS("%slot_control", 0));
			FIELD("attn_button_press", "yesno_t",
					BITS("%slot_status", 0));
		}

		FIELD("power_controller", "yesno_t", BITS("%slot_caps", 1));
		if (FIELD_BOOL("power_controller")) {
			FIELD("power_controller_ctrl",
					ANON_BOOL("on", "off"),
					BITS("%slot_control", 10));
			FIELD("power_fault_en", "yesno_t",
					BITS("%slot_control", 1));
			FIELD("power_fault_det", "yesno_t",
					BITS("%slot_status", 1));
		}

		FIELD("mrl_sensor", "yesno_t", BITS("%slot_caps", 2));
		if (FIELD_BOOL("mrl_sensor")) {
			FIELD("mrl_sensor_en", "yesno_t",
					BITS("%slot_control", 2));
			FIELD("mrl_sensor_change", "yesno_t",
					BITS("%slot_status", 2));
			FIELD("mrl_sensor_state",
					ANON_BOOL("closed", "open"),
					BITS("%slot_status", 5));
		}

		FIELD("attn_indicator", "yesno_t", BITS("%slot_caps", 3));
		if (FIELD_BOOL("attn_indicator")) {
			FIELD("attn_indicator_ctrl", ANON_ENUM(
						KV("on", 1),
						KV("blink", 2),
						KV("off", 3)),
					BITS("%slot_control", 7, 6));
		}

		FIELD("power_indicator", "yesno_t", BITS("%slot_caps", 4));
		if (FIELD_BOOL("power_indicator")) {
			FIELD("power_indicator_ctrl", ANON_ENUM(
						KV("on", 1),
						KV("blink", 2),
						KV("off", 3)),
					BITS("%slot_control", 9, 8));
		}

		FIELD("hotplug_capable", "yesno_t", BITS("%slot_caps", 6));
		if (FIELD_BOOL("hotplug_capable")) {
			FIELD("hotplug_surprise", "yesno_t",
					BITS("%slot_caps", 5));
			FIELD("presence_detect_en", "yesno_t",
					BITS("%slot_control", 3));
			FIELD("presence_detect_change", "yesno_t",
					BITS("%slot_status", 3));
			FIELD("presence_detect_state",
					ANON_BOOL("empty", "present"),
					BITS("%slot_status", 6));
			FIELD("hot_plug_intr_en", "yesno_t",
					BITS("%slot_control", 5));
			FIELD("no_cmd_complete", "yesno_t",
					BITS("%slot_caps", 18));
			if (FIELD_BOOL("no_cmd_complete") == 0) {
				FIELD("cmd_complete_intr_en",
						"yesno_t", BITS("%slot_control", 4));
				FIELD("cmd_completed", "yesno_t",
						BITS("%slot_status", 4));

			}
		}

		FIELD("electromech_lock", "yesno_t", BITS("%slot_caps", 17));
		if (FIELD_BOOL("electromech_lock")) {
			FIELD("electromech_lock_ctrl", "int_t",
					BITS("%slot_control", 11));
			FIELD("electromech_lock_status",
					ANON_BOOL("disengaged", "engaged"),
					BITS("%slot_status", 7));
		}

		//FIXME: better as a procfield?
		FIELD("slot_power_limit", "int_t", BITS("%slot_caps", 14, 7));
		FIELD("slot_power_scale", "int_t", BITS("%slot_caps", 16, 15));

		if (FIELD_BOOL("../link/data_link_active_report_cap")) {
			FIELD("data_link_state_change_en", "yesno_t",
					BITS("%slot_control", 12));
			FIELD("data_link_state_change", "yesno_t",
					BITS("%slot_status", 8));
		}

		if (FIELD_GE("../version", 2)) {
			REG32("%link_caps2", address + 0x34);
			REG16("%link_control2", address + 0x38);
			REG16("%link_status2", address + 0x3a);
		}
		CLOSE_SCOPE(); // slot
	}

	if (FIELD_EQ("type", "root_event_collector")
	 || FIELD_EQ("type", "root_port")) {
		OPEN_SCOPE("root");
		REG16("%root_control", address + 0x1c);
		REG16("%root_caps", address + 0x1e);
		REG32("%root_status", address + 0x20);

		FIELD("serr_on_cerr", "yesno_t", BITS("%root_control", 0));
		FIELD("serr_on_nferr", "yesno_t", BITS("%root_control", 1));
		FIELD("serr_on_ferr", "yesno_t", BITS("%root_control", 2));
		FIELD("pme_intr_en", "yesno_t", BITS("%root_control", 3));

		FIELD("crs_visible_cap", "yesno_t", BITS("%root_caps", 0));
		FIELD("crs_visible_en", "yesno_t", BITS("%root_control", 4));

		FIELD("pme_status", "yesno_t", BITS("%root_status", 16));
		FIELD("pme_pending", "yesno_t", BITS("%root_status", 17));
		FIELD("pme_requester", "hex16_t", BITS("%root_status", 15, 0));

		CLOSE_SCOPE(); // root
	}
}

static void
aer_ecapability(const pp_value &address)
{
	BOOL("fatal_t", "fatal", "nonfatal");

	// uncorrectable errors
	OPEN_SCOPE("uncorrectable");

	REG32("%error_status", address + 0x4);
	REG32("%error_mask", address + 0x8);
	REG32("%error_severity", address + 0xc);

	OPEN_SCOPE("data_link_proto_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 4));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 4));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 4));
	CLOSE_SCOPE();

	OPEN_SCOPE("surprise_down_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 5));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 5));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 5));
	CLOSE_SCOPE();

	OPEN_SCOPE("poisoned_tlp");
	FIELD("status", "yesno_t", BITS("../%error_status", 12));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 12));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 12));
	CLOSE_SCOPE();

	OPEN_SCOPE("flow_ctrl_proto_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 13));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 13));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 13));
	CLOSE_SCOPE();

	OPEN_SCOPE("completion_timeout");
	FIELD("status", "yesno_t", BITS("../%error_status", 14));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 14));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 14));
	CLOSE_SCOPE();

	OPEN_SCOPE("completer_abort");
	FIELD("status", "yesno_t", BITS("../%error_status", 15));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 15));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 15));
	CLOSE_SCOPE();

	OPEN_SCOPE("unexpected_completion");
	FIELD("status", "yesno_t", BITS("../%error_status", 16));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 16));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 16));
	CLOSE_SCOPE();

	OPEN_SCOPE("receiver_overflow");
	FIELD("status", "yesno_t", BITS("../%error_status", 17));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 17));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 17));
	CLOSE_SCOPE();

	OPEN_SCOPE("malformed_tlp");
	FIELD("status", "yesno_t", BITS("../%error_status", 18));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 18));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 18));
	CLOSE_SCOPE();

	OPEN_SCOPE("ecrc_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 19));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 19));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 19));
	CLOSE_SCOPE();

	OPEN_SCOPE("unsupported_request_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 20));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 20));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 20));
	CLOSE_SCOPE();

	OPEN_SCOPE("acs_violation");
	FIELD("status", "yesno_t", BITS("../%error_status", 21));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 21));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 21));
	CLOSE_SCOPE();

	CLOSE_SCOPE(); // uncorrectable

	// correctable errors
	OPEN_SCOPE("correctable");

	REG32("%error_status", address + 0x10);
	REG32("%error_mask", address + 0x14);

	OPEN_SCOPE("receiver_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 0));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 0));
	CLOSE_SCOPE();

	OPEN_SCOPE("bad_tlp");
	FIELD("status", "yesno_t", BITS("../%error_status", 6));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 6));
	CLOSE_SCOPE();

	OPEN_SCOPE("bad_dllp");
	FIELD("status", "yesno_t", BITS("../%error_status", 7));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 7));
	CLOSE_SCOPE();

	OPEN_SCOPE("replay_num_rollover");
	FIELD("status", "yesno_t", BITS("../%error_status", 8));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 8));
	CLOSE_SCOPE();

	OPEN_SCOPE("replay_timer_timeout");
	FIELD("status", "yesno_t", BITS("../%error_status", 12));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 12));
	CLOSE_SCOPE();

	OPEN_SCOPE("advisory_nf_err");
	FIELD("status", "yesno_t", BITS("../%error_status", 21));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 13));
	CLOSE_SCOPE();

	CLOSE_SCOPE(); // correctable

	// capabilities and control
	REG32("%caps_control", address + 0x18);
	FIELD("first_err_ptr", "hex8_t",
			BITS("%caps_control", 4, 0));
	FIELD("ecrc_generation_cap", "yesno_t",
			BITS("%caps_control", 5));
	FIELD("ecrc_generation_en", "yesno_t",
			BITS("%caps_control", 6));
	FIELD("ecrc_check_cap", "yesno_t",
			BITS("%caps_control", 7));
	FIELD("ecrc_check_en", "yesno_t",
			BITS("%caps_control", 8));

	// header log
	REG32("%header_log_0", address + 0x1c);
	REG32("%header_log_1", address + 0x20);
	REG32("%header_log_2", address + 0x24);
	REG32("%header_log_3", address + 0x28);
	FIELD("header_log", "hex128_t",
			BITS("%header_log_3", 31, 0) +
			BITS("%header_log_2", 31, 0) +
			BITS("%header_log_1", 31, 0) +
			BITS("%header_log_0", 31, 0));

	// if this device is a root port or root complex event collector
	if (FIELD_EQ("$pci/capability.pcie[0]/type", "root_event_collector")
	 || FIELD_EQ("$pci/capability.pcie[0]/type", "root_port")) {
		// root error command and status
		REG32("%root_error_command", address + 0x2c);
		REG32("%root_error_status", address + 0x30);

		FIELD("correctable_err_en", "yesno_t",
				BITS("%root_error_command", 0));
		FIELD("correctable_err_rcvd", "yesno_t",
				BITS("%root_error_status", 0));
		FIELD("correctable_err_multi", "yesno_t",
				BITS("%root_error_status", 1));

		FIELD("nonfatal_err_en", "yesno_t",
				BITS("%root_error_command", 1));
		FIELD("nonfatal_rcvd", "yesno_t",
				BITS("%root_error_status", 5));

		FIELD("fatal_err_en", "yesno_t",
				BITS("%root_error_command", 2));
		FIELD("fatal_rcvd", "yesno_t",
				BITS("%root_error_status", 6));

		FIELD("uncorrectable_err_rcvd", "yesno_t",
				BITS("%root_error_status", 2));
		FIELD("uncorrectable_err_multi", "yesno_t",
				BITS("%root_error_status", 3));
		FIELD("first_uncorrectable", "fatal_t",
				BITS("%root_error_status", 4));

		FIELD("intr_msg_num", "hex8_t",
				BITS("%root_error_status", 31, 27));

		REG32("%error_src_id", address + 0x34);
		FIELD("correctable_src_id", "hex16_t",
				BITS("%error_src_id", 15, 0));
		FIELD("uncorrectable_src_id", "hex16_t",
				BITS("%error_src_id", 31, 16));
	}
}

// Handle the PCI capabilities linked-list.
static void
explore_capabilities()
{
	// Walk the PCI capabilities linked-list.
	if (FIELD_BOOL("status/caps")) {
		REGFIELD8("capptr", 0x34, "hex8_t");

		pp_value ptr = READ("capptr");
		while (ptr != 0 && ptr != 0xff) {
			OPEN_SCOPE("capability");

			FIELD("offset", "hex8_t", ptr);
			REGFIELD8("id", ptr, "pci_capability_t");
			REGFIELD8("next", ptr+1, "hex8_t");

			if (FIELD_EQ("id", "power_mgmt")) {
				// PCI spec
				power_mgmt_capability(ptr);
			} else if (FIELD_EQ("id", "agp")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "agp8x")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "vpd")) {
				// PCI spec
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "slot_id")) {
				// PCI-PCI bridge spec
				slot_id_capability(ptr);
			} else if (FIELD_EQ("id", "msi")) {
				// PCI spec
				msi_capability(ptr);
			} else if (FIELD_EQ("id", "msix")) {
				// PCI spec
				msix_capability(ptr);
			} else if (FIELD_EQ("id", "resource_ctrl")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "hot_swap")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "hot_plug")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "ht")) {
				// HT spec
				ht_capability(ptr);
			} else if (FIELD_EQ("id", "usb2_dbg_port")) {
				// EHCI spec
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "ssid")) {
				// PCI-PCI bridge spec
				ssid_capability(ptr);
			} else if (FIELD_EQ("id", "pcix")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "pcie")) {
				// PCI-E spec
				pcie_capability(ptr);
			} else if (FIELD_EQ("id", "vendor")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "secure")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "sata")) {
				//FIXME: not implemented yet, what spec??
			} else if (FIELD_EQ("id", "af")) {
				//FIXME: not implemented yet, ECN
			} else if (FIELD_EQ("id", pp_value(0))) {
				// This should not be needed, except that
				// some hardware vendors make the
				// capability pointer link to a
				// capability block with id=0 and next=0.
			} else {
				WARN("unknown PCI capability 0x%02x @ 0x%02x",
						READ("id"), ptr);
			}

			ptr = READ("next");
			CLOSE_SCOPE("capability."
			    + GET_FIELD("id")->evaluate() + "[]");
			//FIXME: need to return a valid path_element, or be sanitized
		}
	}

	// Only PCI-X and PCI-Express devices should have extended
	// capabilities.  Those devices self identify through the
	// existence of a traditional PCI capability block.  If we have a
	// non-zero capability header, walk the extended capabilities
	// linked-list.
	// FIXME: need RCRB support
	if ((DEFINED("$pci/capability.pcie") || DEFINED("$pci/capability.pcix"))
	 && (READ("%PCI.100") != 0)) {
		pp_value ptr = 0x100;
		while (ptr != 0 && ptr != 0xfff) {
			OPEN_SCOPE("ecapability");

			FIELD("offset", "hex16_t", ptr);
			REGFIELD16("id", ptr, "pcie_capability_t");
			REG16("%ver_next", ptr+2);
			FIELD("version", "hex4_t", BITS("%ver_next", 3, 0));
			FIELD("next", "hex12_t", BITS("%ver_next", 15, 4));

			if (FIELD_EQ("id", "aer")) {
				// PCI-E spec
				aer_ecapability(ptr);
			} else if (FIELD_EQ("id", "vchannel")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "serial_num")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "power_budget")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "root_link_decl")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "root_internal_link_ctrl")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id",
				   "root_event_collector_endpoint_assoc")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "multi_func_vchannel")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "vc2")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "rcrb_header")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "vendor_specific")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", "acs")) {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", pp_value(0))) {
				// This should not be needed, except that
				// some hardware vendors make the
				// capability pointer link to a
				// capability block with id=0 and next=0.
			} else {
				WARN("unknown PCIE capability 0x%04x @ 0x%03x",
						READ("id"), ptr);
			}

			ptr = READ("next");
			CLOSE_SCOPE("ecapability."
			    + GET_FIELD("id")->evaluate() + "[]");
			//FIXME: need to return a valid path_element, or be sanitized
		}
	}
	//FIXME: scan EHCI extended capabilities
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
		FIELD("cap66", "yesno_t", BITS("../%sec_status", 5));
		FIELD("fbb", "yesno_t", BITS("../%sec_status", 7));
		FIELD("mdperr", "yesno_t", BITS("../%sec_status", 8));
		FIELD("devsel", ANON_ENUM(
					KV("FAST", 0),
					KV("MEDIUM", 1),
					KV("SLOW", 2)),
			 BITS("../%sec_status", 10, 9));
		FIELD("sigtabort", "yesno_t", BITS("../%sec_status", 11));
		FIELD("rcvtabort", "yesno_t", BITS("../%sec_status", 12));
		FIELD("rcvmabort", "yesno_t", BITS("../%sec_status", 13));
		FIELD("rcvserr", "yesno_t", BITS("../%sec_status", 14));
		FIELD("perr", "yesno_t", BITS("../%sec_status", 15));
	CLOSE_SCOPE();

	// IO decode window
	OPEN_SCOPE("io_window");
		REG8("%base_lo", 0x1c);
		REG16("%base_hi", 0x30);
		REG8("%limit_lo", 0x1d);
		REG16("%limit_hi", 0x32);

		FIELD("width", ANON_ENUM(
					KV("bits16", 0),
					KV("bits32", 1)),
				BITS("%base_lo", 3, 0));

		if (FIELD_EQ("width", "bits16")) {
			FIELD("base", "addr16_t",
					BITS("%base_lo", 7, 4) +
					BITS("%0", 11, 0));
			FIELD("limit", "addr16_t",
					BITS("%limit_lo", 7, 4) +
					BITS("%1", 11, 0));
		} else if (FIELD_EQ("width", "bits32")) {
			FIELD("base", "addr32_t",
					BITS("%base_hi", 15, 0) +
					BITS("%base_lo", 7, 4) +
					BITS("%0", 11, 0));
			FIELD("limit", "addr32_t",
					BITS("%limit_hi", 15, 0) +
					BITS("%limit_lo", 7, 4) +
					BITS("%1", 11, 0));
		}
	CLOSE_SCOPE();

	// MMIO decode window
	OPEN_SCOPE("mem_window");
		REG16("%base", 0x20);
		REG16("%limit", 0x22);

		FIELD("base", "addr32_t",
				BITS("%base", 15, 4) +
				BITS("%0", 19, 0));
		FIELD("limit", "addr32_t",
				BITS("%limit", 15, 4) +
				BITS("%1", 19, 0));
	CLOSE_SCOPE();

	// Prefetched memory decode window
	OPEN_SCOPE("prefetch_window");
		REG16("%base_lo", 0x24);
		REG32("%base_hi", 0x28);
		REG16("%limit_lo", 0x26);
		REG32("%limit_hi", 0x2c);

		FIELD("width", ANON_ENUM(
					KV("bits32", 0),
					KV("bits64", 1)),
				BITS("%base_lo", 3, 0));

		if (FIELD_EQ("width", "bits32")) {
			FIELD("base", "addr32_t",
					BITS("%base_lo", 15, 4) +
					BITS("%0", 19, 0));
			FIELD("limit", "addr32_t",
					BITS("%limit_lo", 15, 4) +
					BITS("%1", 19, 0));
		} else if (FIELD_EQ("width", "bits64")) {
			FIELD("base", "addr64_t",
					BITS("%base_hi", 31, 0) +
					BITS("%base_lo", 15, 4) +
					BITS("%0", 19, 0));
			FIELD("limit", "addr64_t",
					BITS("%limit_hi", 31, 0) +
					BITS("%limit_lo", 15, 4) +
					BITS("%1", 19, 0));
		}
	CLOSE_SCOPE();

	// Capabilities
	explore_capabilities();

	// Expansion ROM
	REG32("%rombase", 0x38);
	OPEN_SCOPE("rombase");
		FIELD("enabled", "yesno_t", BITS("../%rombase", 0));
		FIELD("address", "addr32_t",
				BITS("../%rombase", 31, 11) +
				BITS("%0", 10, 0));
	CLOSE_SCOPE();

	// Bridge control
	REG16("%bridge_ctrl", 0x3e);
	OPEN_SCOPE("bridge_ctrl");
		BOOL("discard_timer_t", "short", "long");
		FIELD("perr", "yesno_t", BITS("../%bridge_ctrl", 0));
		FIELD("serr", "yesno_t", BITS("../%bridge_ctrl", 1));
		FIELD("isa", "yesno_t", BITS("../%bridge_ctrl", 2));
		FIELD("vga", "yesno_t", BITS("../%bridge_ctrl", 3));
		FIELD("vga16", "yesno_t", BITS("../%bridge_ctrl", 4));
		FIELD("mst_abort", "yesno_t", BITS("../%bridge_ctrl", 5));
		FIELD("sec_reset", "yesno_t", BITS("../%bridge_ctrl", 6));
		FIELD("fbb", "yesno_t", BITS("../%bridge_ctrl", 7));
		FIELD("pri_discard", "discard_timer_t",
		    BITS("../%bridge_ctrl", 8));
		FIELD("sec_discard", "discard_timer_t",
		    BITS("../%bridge_ctrl", 9));
		FIELD("discard_status", "yesno_t", BITS("../%bridge_ctrl", 10));
		FIELD("discard_serr", "yesno_t", BITS("../%bridge_ctrl", 11));
	CLOSE_SCOPE();
}

static void
create_device()
{
	// if BAR1 is not defined, BAR0 must be 64 bit, BAR2 is ok.
	// if BAR1 is not 64 bit memory, BAR2 is ok.
	if (!DEFINED("bar1")
	 || !(FIELD_EQ("bar1/type", "mem")
	   && FIELD_EQ("bar1/width", "bits64"))) {
		BAR("bar2", 0x18);
	}

	// if BAR2 is not defined, BAR1 must be 64 bit, BAR3 is ok.
	// if BAR2 is not 64 bit memory, BAR3 is ok.
	if (!DEFINED("bar2")
	 || !(FIELD_EQ("bar2/type", "mem")
	   && FIELD_EQ("bar2/width", "bits64"))) {
		BAR("bar3", 0x1c);
	}

	// if BAR3 is not defined, BAR2 must be 64 bit, BAR4 is ok.
	// if BAR3 is not 64 bit memory, BAR4 is ok.
	if (!DEFINED("bar3")
	 || !(FIELD_EQ("bar3/type", "mem")
	   && FIELD_EQ("bar3/width", "bits64"))) {
		BAR("bar4", 0x20);
	}

	// if BAR4 is not defined, BAR3 must be 64 bit, BAR5 is ok.
	// if BAR4 is not 64 bit memory, BAR5 is ok.
	if (!DEFINED("bar4")
	 || !(FIELD_EQ("bar4/type", "mem")
	   && FIELD_EQ("bar4/width", "bits64"))) {
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
		FIELD("enabled", "yesno_t", BITS("../%rombase", 0));
		FIELD("address", "addr32_t",
				BITS("../%rombase", 31, 11) +
				BITS("%0", 10, 0));
	CLOSE_SCOPE();

	REGFIELD8("mingnt", 0x3e, ANON_INT("1/4 usecs"));
	REGFIELD8("maxlat", 0x3f, ANON_INT("1/4 usecs"));
}

/* populate the current scope with generic PCI device fields */
void
pci_generic_device()
{
	// all PCI devices have a 256 Byte config space
	for (unsigned i = 0; i < 256; i += 4) {
		REG32(to_string(boost::format("%%PCI.%03x") %i), i);
	}
	// if %PCI.100 is not FFFFFFFF, we have a 4 KByte config space
	REG32("%PCI.100", 0x100);
	if (READ("%PCI.100") != 0xffffffff) {
		for (unsigned i = 256+4; i < 4096; i += 4) {
			REG32(to_string(boost::format("%%PCI.%03x") %i), i);
		}
	}

	REGFIELD16("vendor", 0x00, "pci_vendor_t");
	REGFIELD16("device", 0x02, "hex16_t");

	REG16("%command", 0x04);
	OPEN_SCOPE("command");
		FIELD("io", "yesno_t", BITS("../%command", 0));
		FIELD("mem", "yesno_t", BITS("../%command", 1));
		FIELD("bm", "yesno_t", BITS("../%command", 2));
		FIELD("special", "yesno_t", BITS("../%command", 3));
		FIELD("mwinv", "yesno_t", BITS("../%command", 4));
		FIELD("vgasnoop", "yesno_t", BITS("../%command", 5));
		FIELD("perr", "yesno_t", BITS("../%command", 6));
		FIELD("step", "yesno_t", BITS("../%command", 7));
		FIELD("serr", "yesno_t", BITS("../%command", 8));
		FIELD("fbb", "yesno_t", BITS("../%command", 9));
		FIELD("intr", "yesno_t", BITS("../%command", 10));
	CLOSE_SCOPE();

	REG16("%status", 0x06);
	OPEN_SCOPE("status");
		FIELD("intr", "yesno_t", BITS("../%status", 3));
		FIELD("caps", "yesno_t", BITS("../%status", 4));
		FIELD("cap66", "yesno_t", BITS("../%status", 5));
		FIELD("user", "yesno_t", BITS("../%status", 6));
		FIELD("fbb", "yesno_t", BITS("../%status", 7));
		FIELD("mdperr", "yesno_t", BITS("../%status", 8));
		FIELD("devsel", ANON_ENUM(
					KV("FAST", 0),
					KV("MEDIUM", 1),
					KV("SLOW", 2)),
			 BITS("../%status", 10, 9));
		FIELD("sigtabort", "yesno_t", BITS("../%status", 11));
		FIELD("rcvtabort", "yesno_t", BITS("../%status", 12));
		FIELD("rcvmabort", "yesno_t", BITS("../%status", 13));
		FIELD("sigserr", "yesno_t", BITS("../%status", 14));
		FIELD("perr", "yesno_t", BITS("../%status", 15));
	CLOSE_SCOPE();

	REGFIELD8("class", 0x0b, "pci_class_t");

	//FIXME: figure the best way to use the types in pci.c
	if (FIELD_EQ("class", "pre_classcode")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_pre_classcode");
	} else if (FIELD_EQ("class", "mass_storage")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_mass_storage");
	} else if (FIELD_EQ("class", "network")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_network");
	} else if (FIELD_EQ("class", "display")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_display");
	} else if (FIELD_EQ("class", "multimedia")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_multimedia");
	} else if (FIELD_EQ("class", "memory")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_memory");
	} else if (FIELD_EQ("class", "bridge")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_bridge");
	//FIXME: not implemented yet
	//} else if (FIELD_EQ("class", "simple_comm")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_simple_comm");
	//} else if (FIELD_EQ("class", "base_system")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_base_system");
	//} else if (FIELD_EQ("class", "input")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_input");
	//} else if (FIELD_EQ("class", "docking")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_docking");
	//} else if (FIELD_EQ("class", "processor")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_processor");
	//} else if (FIELD_EQ("class", "serial")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_serial");
	//} else if (FIELD_EQ("class", "wireless")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_wireless");
	//} else if (FIELD_EQ("class", "intelligent_io")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_intelligent_io");
	//} else if (FIELD_EQ("class", "satellite")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_satellite");
	//} else if (FIELD_EQ("class", "crypto")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_crypto");
	//} else if (FIELD_EQ("class", "dsp")) {
		//REGFIELD8("subclass", 0x0a, "pci_subclass_dsp");
	} else {
		REGFIELD8("subclass", 0x0a, "hex8_t");
	}

	//FIXME: it would be better to have detailed progintfs
	REGFIELD8("progintf", 0x09, "hex8_t");

	REGFIELD8("revision", 0x08, "hex8_t");

	REGFIELD8("cacheline", 0x0c, ANON_INT("DWORDs"));

	REG8("%hdrtype", 0x0e);
	FIELD("hdrtype", ANON_ENUM(
				KV("device", 0),
				KV("pci_bridge", 1),
				KV("cardbus_bridge", 2)),
			BITS("%hdrtype", 6, 0));
	FIELD("multifunc", "yesno_t", BITS("%hdrtype", 7));

	REG8("%bist", 0x0f);
	OPEN_SCOPE("bist");
		FIELD("capable", "yesno_t", BITS("../%bist", 7));
		FIELD("start", "yesno_t", BITS("../%bist", 6));
		FIELD("code", "hex8_t", BITS("../%bist", 3, 0));
	CLOSE_SCOPE();

	REGFIELD8("intline", 0x3c, "int_t");

	REGFIELD8("intpin", 0x3d, ANON_ENUM(
				KV("none", 0),
				KV("inta", 1),
				KV("intb", 2),
				KV("intc", 3),
				KV("intd", 4)));

	// Base Address Registers - all devices have at least 2 BARs.
	BAR("bar0", 0x10);

	// if BAR0 is not 64 bit memory, declare BAR1
	if (!(FIELD_EQ("bar0/type", "mem")
	   && FIELD_EQ("bar0/width", "bits64"))) {
		BAR("bar1", 0x14);
	}

	if (FIELD_EQ("hdrtype", "device")) {
		create_device();
	} else if (FIELD_EQ("hdrtype", "pci_bridge")) {
		create_pci_bridge();
	} else if (FIELD_EQ("hdrtype", "cardbus_bridge")) {
		//TODO: need a spec
	}
}
