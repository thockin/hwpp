#include "pp.h"
#include "pp_scope.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "utils.h"
#include "drivers.h"

// All standard BARs look like this.
static void
BAR(const string &name, const pp_value &address)
{
	OPEN_SCOPE(name);

	REG32("%lower", address);
	ONE_BIT_FIELD("type", ANON_ENUM(
				KV("mem", 0),
				KV("io", 1)),
			"%lower", 0);

	if (FIELD_EQ("type", "mem")) {
		SIMPLE_FIELD("width", ANON_ENUM(
					KV("bits32", 0),
					KV("bits20", 1),
					KV("bits64", 2)),
				"%lower", 2, 1);
		ONE_BIT_FIELD("prefetch", "yesno_t", "%lower", 3);
	}

	if (FIELD_EQ("type", "io")) {
		COMPLEX_FIELD("address", "addr16_t",
				BITS("0", 1, 0),
				BITS("%lower", 15, 2));
	} else if (FIELD_EQ("width", "bits32")
	    || FIELD_EQ("width", "bits20")) {
		COMPLEX_FIELD("address", "addr32_t",
				BITS("0", 3, 0),
				BITS("%lower", 31, 4));
	} else {
		REG32("%upper", address+4);
		COMPLEX_FIELD("address", "addr64_t",
				BITS("0", 3, 0),
				BITS("%lower", 31, 4),
				BITS("%upper", 31, 0));
	}
	CLOSE_SCOPE();
}

static void
ht_link_control(const pp_value &address)
{
	REG16("%control", address);
	OPEN_SCOPE("control");

	ONE_BIT_FIELD("SrcIdEn", "yesno_t", "../%control", 0);
	ONE_BIT_FIELD("CFIEn", "yesno_t", "../%control", 1);
	ONE_BIT_FIELD("CST", "yesno_t", "../%control", 2);
	ONE_BIT_FIELD("CFEr", "yesno_t", "../%control", 3);
	ONE_BIT_FIELD("LinkFail", "yesno_t", "../%control", 4);
	ONE_BIT_FIELD("InitCmp", "yesno_t", "../%control", 5);
	ONE_BIT_FIELD("EOC", "yesno_t", "../%control", 6);
	ONE_BIT_FIELD("TXOff", "yesno_t", "../%control", 7);
	SIMPLE_FIELD("CRCErr", "hex4_t", "../%control", 11, 8);
	ONE_BIT_FIELD("IsocEn", "yesno_t", "../%control", 12);
	ONE_BIT_FIELD("LSEn", "yesno_t", "../%control", 13);
	ONE_BIT_FIELD("ExtCTL", "yesno_t", "../%control", 14);
	ONE_BIT_FIELD("64bEn", "yesno_t", "../%control", 15);

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

	SIMPLE_FIELD("MaxLinkWidthIn", "ht_link_width_t", "../%config", 2, 0);
	ONE_BIT_FIELD("DwFcIn", "yesno_t", "../%config", 3);
	SIMPLE_FIELD("MaxLinkWidthOut", "ht_link_width_t", "../%config", 6, 4);
	ONE_BIT_FIELD("DwFcOut", "yesno_t", "../%config", 7);

	SIMPLE_FIELD("LinkWidthIn", "ht_link_width_t", "../%config", 10, 8);
	ONE_BIT_FIELD("DwFcInEn", "yesno_t", "../%config", 11);
	SIMPLE_FIELD("LinkWidthOut", "ht_link_width_t", "../%config", 14, 12);
	ONE_BIT_FIELD("DwFcOutEn", "yesno_t", "../%config", 15);

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
			KV("mhz2600", 14));

	REG8("%freq_err", address);
	SIMPLE_FIELD("Freq", "ht_link_freq_t", "%freq_err", 3, 0);
	ONE_BIT_FIELD("ProtoErr", "yesno_t", "%freq_err", 4);
	ONE_BIT_FIELD("OverErr", "yesno_t", "%freq_err", 5);
	ONE_BIT_FIELD("EOCErr", "yesno_t", "%freq_err", 6);
	ONE_BIT_FIELD("CTLTimeout", "yesno_t", "%freq_err", 7);
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
	SIMPLE_FIELD("FreqCap", "ht_link_freq_cap_t", "%freq_cap", 14, 0);
}

static void
ht_error_handling(const pp_value &address)
{
	REG16("%error", address);
	OPEN_SCOPE("error");

	ONE_BIT_FIELD("proto_flood", "yesno_t", "../%error", 0);
	ONE_BIT_FIELD("over_flood", "yesno_t", "../%error", 1);
	ONE_BIT_FIELD("proto_fatal", "yesno_t", "../%error", 2);
	ONE_BIT_FIELD("over_fatal", "yesno_t", "../%error", 3);
	ONE_BIT_FIELD("eoc_fatal", "yesno_t", "../%error", 4);
	ONE_BIT_FIELD("resp_fatal", "yesno_t", "../%error", 5);
	ONE_BIT_FIELD("crc_fatal", "yesno_t", "../%error", 6);
	ONE_BIT_FIELD("serr_fatal", "yesno_t", "../%error", 7);
	ONE_BIT_FIELD("chain_fail", "yesno_t", "../%error", 8);
	ONE_BIT_FIELD("resp_err", "yesno_t", "../%error", 9);
	ONE_BIT_FIELD("proto_nonfatal", "yesno_t", "../%error", 10);
	ONE_BIT_FIELD("over_nonfatal", "yesno_t", "../%error", 11);
	ONE_BIT_FIELD("eoc_nonfatal", "yesno_t", "../%error", 12);
	ONE_BIT_FIELD("resp_nonfatal", "yesno_t", "../%error", 13);
	ONE_BIT_FIELD("crc_nonfatal", "yesno_t", "../%error", 14);
	ONE_BIT_FIELD("serr_nonfatal", "yesno_t", "../%error", 15);
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
	ONE_BIT_FIELD("iso_flow_ctrl", "yesno_t", "../%feature", 0);
	ONE_BIT_FIELD("ldtstop", "yesno_t", "../%feature", 1);
	ONE_BIT_FIELD("crc_test_mode", "yesno_t", "../%feature", 2);
	ONE_BIT_FIELD("extended_ctl_time", "yesno_t", "../%feature", 3);
	ONE_BIT_FIELD("64bit_addressing", "yesno_t", "../%feature", 4);
	ONE_BIT_FIELD("unit_id_reorder_dis", "yesno_t", "../%feature", 5);
	ONE_BIT_FIELD("source_id", "yesno_t", "../%feature", 6);
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
	ONE_BIT_FIELD("iso_flow_ctrl", "yesno_t", "../%feature", 0);
	ONE_BIT_FIELD("ldtstop", "yesno_t", "../%feature", 1);
	ONE_BIT_FIELD("crc_test_mode", "yesno_t", "../%feature", 2);
	ONE_BIT_FIELD("extended_ctl_time", "yesno_t", "../%feature", 3);
	ONE_BIT_FIELD("64bit_addressing", "yesno_t", "../%feature", 4);
	ONE_BIT_FIELD("unit_id_reorder_dis", "yesno_t", "../%feature", 5);
	ONE_BIT_FIELD("source_id", "yesno_t", "../%feature", 6);
	ONE_BIT_FIELD("ext_regs", "yesno_t", "../%feature", 8);
	ONE_BIT_FIELD("upstream_cfg", "yesno_t", "../%feature", 9);
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
	SIMPLE_FIELD("major_rev", "int_t", "%rev", 7, 5);
	SIMPLE_FIELD("minor_rev", "int_t", "%rev", 4, 0);
}

static void
ht_extended_config_capability(const pp_value &address)
{
	REG32("%address", address+4);
	ONE_BIT_FIELD("type", ANON_ENUM(
				KV("type0", 0),
				KV("type1", 1)),
			"%address", 28);
	SIMPLE_FIELD("bus", "int_t", "%address", 27, 20);
	SIMPLE_FIELD("device", "int_t", "%address", 19, 15);
	SIMPLE_FIELD("function", "int_t", "%address", 14, 12);
	COMPLEX_FIELD("register", "hex_t",
			BITS("0", 1, 0),
			BITS("%address", 11, 2));
	REGFIELD32("data", address+8, "hex32_t");
}

static void
ht_address_mapping_capability(const pp_value &address)
{
	REG16("%command", address + 0x02);
	SIMPLE_FIELD("num_dma", "int_t", "%command", 3, 0);
	SIMPLE_FIELD("io_size", "int_t", "%command", 8, 4);
	SIMPLE_FIELD("map_type", ANON_ENUM(
				KV("bits40", 0),
				KV("bits64", 1)),
			"%command", 10, 9);

	if (FIELD_EQ("map_type", "bits40")) {
		REG32("%sec_non_prefetch", address + 0x04);
		OPEN_SCOPE("sec_non_prefetch");
		ONE_BIT_FIELD("enable", "yesno_t", "../%sec_non_prefetch", 31);
		ONE_BIT_FIELD("isoch", "yesno_t", "../%sec_non_prefetch", 30);
		ONE_BIT_FIELD("noncoherent", "yesno_t",
				"../%sec_non_prefetch", 29);
		ONE_BIT_FIELD("compat", "yesno_t", "../%sec_non_prefetch", 28);
		COMPLEX_FIELD("base", "addr64_t",
				BITS("0", 19, 0),
				BITS("../%sec_non_prefetch", 19, 0));
		CLOSE_SCOPE();

		REG32("%sec_prefetch", address + 0x08);
		OPEN_SCOPE("sec_prefetch");
		ONE_BIT_FIELD("enable", "yesno_t", "../%sec_prefetch", 31);
		ONE_BIT_FIELD("isochronous", "yesno_t", "../%sec_prefetch", 30);
		ONE_BIT_FIELD("noncoherent", "yesno_t", "../%sec_prefetch", 29);
		ONE_BIT_FIELD("compat", "yesno_t", "../%sec_prefetch", 28);
		COMPLEX_FIELD("base", "addr64_t",
				BITS("0", 19, 0),
				BITS("../%sec_prefetch", 19, 0));
		CLOSE_SCOPE();

		pp_value value = GET_FIELD("num_dma")->read();
		for (unsigned i = 0; i < value; i++) {
			OPEN_SCOPE("dma[" + to_string(i+1) + "]");
			REG32("%lower", address + 0x0c + (8*i));
			REG32("%upper", address + 0x10 + (8*i));

			ONE_BIT_FIELD("enable", "yesno_t", "%lower", 31);
			ONE_BIT_FIELD("isochronous", "yesno_t", "%lower", 30);
			ONE_BIT_FIELD("noncoherent", "yesno_t", "%lower", 29);

			COMPLEX_FIELD("pri_base", "addr64_t",
					BITS("0", 23, 0),
					BITS("%lower", 15, 0));
			COMPLEX_FIELD("sec_base", "addr64_t",
					BITS("0", 23, 0),
					BITS("%lower", 31, 16));
			COMPLEX_FIELD("sec_limit", "addr64_t",
					BITS("1", 23, 0),
					BITS("%lower", 15, 0));
			CLOSE_SCOPE();
		}
	} else if (FIELD_EQ("map_type", "bits64")) {
		//FIXME: this requires index/data pair access within PCI
	}
}

static void
ht_msi_mapping_capability(const pp_value &address)
{
	REG8("%flags", address+2);
	ONE_BIT_FIELD("en", "yesno_t", "%flags", 0);
	ONE_BIT_FIELD("fixed", "yesno_t", "%flags", 1);
	if (!FIELD_BOOL("fixed")) {
		REG32("%lower", address+4);
		REG32("%upper", address+8);
		COMPLEX_FIELD("address", "addr64_t",
				BITS("0", 19, 0),
				BITS("%lower", 31, 20),
				BITS("%upper", 31, 0));
	}
}

static void
power_mgmt_capability(const pp_value &address)
{
	REG16("%pmc", address+2);
	SIMPLE_FIELD("version", ANON_ENUM(
				KV("pcipm_v1_0", 1),
				KV("pcipm_v1_1", 2),
				KV("pcipm_v1_2", 3)),
			"%pmc", 2, 0);
	ONE_BIT_FIELD("clock", "yesno_t", "%pmc", 3);
	ONE_BIT_FIELD("dsi", "yesno_t", "%pmc", 5);
	SIMPLE_FIELD("aux_current", ANON_ENUM(
				KV("self", 0),
				KV("mA_55", 1),
				KV("mA_100", 2),
				KV("mA_160", 3),
				KV("mA_220", 4),
				KV("mA_270", 5),
				KV("mA_320", 6),
				KV("mA_375", 7)),
			"%pmc", 8, 6);
	ONE_BIT_FIELD("d1_support", "yesno_t", "%pmc", 9);
	ONE_BIT_FIELD("d2_support", "yesno_t", "%pmc", 10);
	SIMPLE_FIELD("pme_support", ANON_BITMASK(
				KV("d0", 0),
				KV("d1", 1),
				KV("d2", 2),
				KV("d3hot", 3),
				KV("d3cold", 4)),
			"%pmc", 15, 11);

	REG16("%pmcsr", address+4);
	SIMPLE_FIELD("power_state", ANON_ENUM(
				KV("d0", 0),
				KV("d1", 1),
				KV("d2", 2),
				KV("d3hot", 3)),
			"%pmcsr", 1, 0);
	ONE_BIT_FIELD("no_soft_reset", "yesno_t", "%pmcsr", 3);
	ONE_BIT_FIELD("pme_en", "yesno_t", "%pmcsr", 8);
	ONE_BIT_FIELD("pme_status", "yesno_t", "%pmcsr", 15);

	REG8("%pmcsr_bse", address+6);
	ONE_BIT_FIELD("bpcc_en", "yesno_t", "%pmcsr_bse", 7);
	if (FIELD_BOOL("bpcc_en")) {
		ONE_BIT_FIELD("b2_b3", "yesno_t", "%pmcsr_bse", 6);
	}

	//FIXME: this is really an index/data pair set of regs
	SIMPLE_FIELD("data_select", "hex4_t", "%pmcsr", 12, 9);
	SIMPLE_FIELD("data_scale", "hex4_t", "%pmcsr", 14, 13);
	REG8("%data", address+7);
	SIMPLE_FIELD("data", "hex8_t", "%data", 7, 0);
}

static void
slot_id_capability(const pp_value &address)
{
	REG8("%slot", address+2);
	SIMPLE_FIELD("nslots", "int_t", "%slot", 4, 0);
	ONE_BIT_FIELD("first", "yesno_t", "%slot", 5);
	REGFIELD8("chassis", address+3, "int_t");
}

static void
msi_capability(const pp_value &address)
{
	// message control
	REG16("%msg_ctrl", address + 2);
	ONE_BIT_FIELD("msi_enable", "yesno_t", "%msg_ctrl", 0);
	SIMPLE_FIELD("multi_msg_cap", ANON_ENUM(
				KV("vec1", 0),
				KV("vec2", 1),
				KV("vec4", 2),
				KV("vec8", 3),
				KV("vec16", 4),
				KV("vec32", 5)),
			"%msg_ctrl", 3, 1);
	SIMPLE_FIELD("multi_msg_en", ANON_ENUM(
				KV("vec1", 0),
				KV("vec2", 1),
				KV("vec4", 2),
				KV("vec8", 3),
				KV("vec16", 4),
				KV("vec32", 5)),
			"%msg_ctrl", 6, 4);
	ONE_BIT_FIELD("cap64", "yesno_t", "%msg_ctrl", 7);
	ONE_BIT_FIELD("mask_per_vec", "yesno_t", "%msg_ctrl", 8);

	// message address
	REG32("%msg_addr", address + 4);

	// is this 64 bit capable?
	if (!FIELD_BOOL("cap64")) {
		// no, just use the low address and define the data
		COMPLEX_FIELD("msg_addr", "addr32_t",
				BITS("0", 1, 0),
				BITS("%msg_addr", 31, 2));
		REGFIELD16("msg_data", address + 8, "hex16_t");

		if (FIELD_BOOL("mask_per_vec")) {
			// mask and pending bits
			REG32("%mask", address + 12);
			REG32("%pending", address + 16);
		}
	} else {
		// add the high address and define the data
		REG32("%msg_addr_hi",  address + 8);
		COMPLEX_FIELD("msg_addr", "addr64_t",
				BITS("0", 1, 0),
				BITS("%msg_addr", 31, 2),
				BITS("%msg_addr_hi", 31, 0));
		REGFIELD16("msg_data", address + 12, "hex16_t");

		if (FIELD_BOOL("mask_per_vec")) {
			// mask and pending bits
			REG32("%mask", address + 16);
			REG32("%pending", address + 20);
		}
	}

	if (FIELD_BOOL("mask_per_vec")) {
		pp_value vecs = 1 << GET_FIELD("multi_msg_cap")->read();
		for (unsigned i = 0; i < vecs; i++) {
			ONE_BIT_FIELD("mask["+to_string(i)+"]",
					"yesno_t", "%mask", i);
			ONE_BIT_FIELD("pend["+to_string(i)+"]",
					"yesno_t", "%pending", i);
		}
	}
}

static void
msix_capability(const pp_value &address)
{
	REG16("%msg_ctrl", address + 2);
	ONE_BIT_FIELD("msix_enable", "yesno_t", "%msg_ctrl", 15);
	//FIXME: procfield?  should be +1
	SIMPLE_FIELD("table_size", "int_t", "%msg_ctrl", 10, 0);
	ONE_BIT_FIELD("func_mask", "yesno_t", "%msg_ctrl", 14);

	// these will be used a bit later
	string bar;
	pp_value base;
	std::vector<pp_value> args;
	pp_const_binding_ptr bind;
	pp_value table_size = GET_FIELD("table_size")->read() + 1;

	// the table is memory mapped through a BAR
	REG32("%table_ptr", address + 4);
	SIMPLE_FIELD("table_bir", ANON_ENUM(
				KV("bar0", 0),
				KV("bar1", 1),
				KV("bar2", 2),
				KV("bar3", 3),
				KV("bar4", 4),
				KV("bar5", 5)),
			"%table_ptr", 2, 0);
	COMPLEX_FIELD("table_offset", "hex_t",
			BITS("0", 2, 0),
			BITS("%table_ptr", 31, 3));

	//FIXME: a better way to do this?
	bar = "^/" + GET_FIELD("table_bir")->evaluate() + "/address";
	base = GET_FIELD(bar)->read() + GET_FIELD("table_offset")->read();
	args.push_back(base);
	args.push_back(table_size * 16);
	bind = find_driver("mem")->new_binding(args);
	OPEN_SCOPE("table", bind); {
		for (unsigned i = 0; i < table_size; i++) {
			OPEN_SCOPE("entry[" + to_string(i) + "]"); {
				REG32("%msg_addr", i*16 + 0);
				REG32("%msg_upper_addr", i*16 + 4);
				COMPLEX_FIELD("address", "addr64_t",
						BITS("0", 1, 0),
						BITS("%msg_addr", 31, 2),
						BITS("%msg_upper_addr", 31, 0));
				REGFIELD32("msg_data", i*16 + 8, "hex_t");
				REG32("%vector_ctrl", i*16 + 12);
				ONE_BIT_FIELD("mask", "yesno_t",
						"%vector_ctrl", 0);
			} CLOSE_SCOPE();
		}
	} CLOSE_SCOPE();

	// the pending bit array is memory mapped through a BAR
	REG32("%pba_ptr", address + 8);
	SIMPLE_FIELD("pba_bir", ANON_ENUM(
				KV("bar0", 0),
				KV("bar1", 1),
				KV("bar2", 2),
				KV("bar3", 3),
				KV("bar4", 4),
				KV("bar5", 5)),
			"%pba_ptr", 2, 0);
	COMPLEX_FIELD("pba_offset", "hex_t",
			BITS("0", 2, 0),
			BITS("%pba_ptr", 31, 3));

	args.clear();
	bar = "^/" + GET_FIELD("pba_bir")->evaluate() + "/address";
	base = GET_FIELD(bar)->read() + GET_FIELD("table_offset")->read();
	args.push_back(base);
	args.push_back(((table_size+63)/64) * 8);
	bind = find_driver("mem")->new_binding(args);
	OPEN_SCOPE("pba", bind); {
		pp_value tmp_size = table_size;
		// loop for each PBA QWORD
		for (unsigned i = 0; i < (table_size+63)/64; i++) {
			string regname = "%pending[" + to_string(i) + "]";
			REG64(regname, i);
			for (size_t j = 0; j < 64; j++) {
				if (j >= tmp_size)
					break;
				ONE_BIT_FIELD("pending" + to_string(i*64 + j),
						"yesno_t", regname, j);
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
	SIMPLE_FIELD("is_interface", ANON_BOOL("no", "yes"), "%subcap", 7, 6);
	if (FIELD_EQ("is_interface", "yes")) {
		// subtype is a 3 bit field
		SIMPLE_FIELD("subtype", ANON_ENUM(
					KV("slave_primary", 0),
					KV("host_secondary", 1)),
				"%subcap", 7, 5);

		if (FIELD_EQ("subtype", "slave_primary")) {
			ht_slave_capability(address);
		} else if (FIELD_EQ("subtype", "host_secondary")) {
			ht_host_capability(address);
		}
	} else {
		// subtype is a 5 bit field
		SIMPLE_FIELD("subtype", ANON_ENUM(
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
				"%subcap", 7, 3);

		if (FIELD_EQ("subtype", "switch")) {
			//FIXME: not implemented yet
		} else if (FIELD_EQ("subtype", "intr_discovery")) {
			//FIXME: not implemented yet
			//This requires index/data accesses within PCI space
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
			//this requires index/data pairs in PCI space
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
	SIMPLE_FIELD("version", "int_t", "%pcie_caps", 3, 0);
	SIMPLE_FIELD("type", ANON_ENUM(
				KV("endpoint", 0),
				KV("legacy_endpoint", 1),
				KV("root_port", 4),
				KV("upstream_switch_port", 5),
				KV("downstream_switch_port", 6),
				KV("pcie_pci_bridge", 7),
				KV("pci_pcie_bridge", 8),
				KV("root_integrated_endpoint", 9),
				KV("root_event_collector", 10)),
			"%pcie_caps", 7, 4);
	if (FIELD_EQ("type", "root_port")
	 || FIELD_EQ("type", "downstream_switch_port")) {
		ONE_BIT_FIELD("slot_impl", "yesno_t", "%pcie_caps", 8);
	}
	SIMPLE_FIELD("intr_msg_num", "int_t", "%pcie_caps", 13, 9);
	ONE_BIT_FIELD("tcs", "yesno_t", "%pcie_caps", 14);

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
			KV("GTs_25", 1),
			KV("GTs_50", 2));


	// all PCI-E devices implement this block
	OPEN_SCOPE("device");
	REG32("%dev_caps", address + 0x04);
	REG16("%dev_control", address + 0x08);
	REG16("%dev_status", address + 0x0a);

	ENUM("pcie_payload_size_t",
			KV("b128", 0),
			KV("b256", 1),
			KV("b512", 2),
			KV("b1024", 3),
			KV("b2048", 4),
			KV("b4096", 5));
	SIMPLE_FIELD("max_payload_cap", "pcie_payload_size_t",
			"%dev_caps", 2, 0);
	SIMPLE_FIELD("max_payload", "pcie_payload_size_t",
			"%dev_control", 7, 5);
	SIMPLE_FIELD("max_read", "pcie_payload_size_t",
			"%dev_control", 14, 12);

	SIMPLE_FIELD("phantom_funcs_cap", ANON_ENUM(
				KV("phantom_0", 0),
				KV("phantom_4", 1),
				KV("phantom_6", 2),
				KV("phantom_8", 3)),
			"%dev_caps", 4, 3);
	ONE_BIT_FIELD("phantom_funcs", "yesno_t", "%dev_control", 9);

	ONE_BIT_FIELD("ext_tag_field_cap", ANON_ENUM(
				KV("b5", 0),
				KV("b8", 1)),
			"%dev_caps", 5);
	ONE_BIT_FIELD("ext_tag_field", "yesno_t", "%dev_control", 8);

	if (FIELD_EQ("../type", "endpoint")
	 || FIELD_EQ("../type", "legacy_endpoint")
	 || FIELD_EQ("../type", "root_integrated_endpoint")) {
		SIMPLE_FIELD("max_l0s_lat", "pcie_l0s_latency_t",
				"%dev_caps", 8, 6);
		SIMPLE_FIELD("max_l1_lat", "pcie_l1_latency_t",
				"%dev_caps", 11, 9);
		ONE_BIT_FIELD("func_reset_cap", "yesno_t", "%dev_caps", 28);
		ONE_BIT_FIELD("func_reset", "yesno_t", "%dev_control", 15);
	}

	if (FIELD_EQ("../type", "upstream_switch_port")) {
		SIMPLE_FIELD("power_limit_value", "int_t", "%dev_caps", 25, 18);
		SIMPLE_FIELD("power_limit_scale", ANON_ENUM(
					KV("x1_0", 0),
					KV("x0_1", 1),
					KV("x0_01", 2),
					KV("x0_001", 3)),
				"%dev_caps", 27, 26);
	}
	ONE_BIT_FIELD("role_based_err_cap", "yesno_t", "%dev_caps", 15);
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
	if (!DEFINED("func_reset") && FIELD_EQ("^/class", "bridge")) {
		ONE_BIT_FIELD("bridge_retry_en", "yesno_t", "%dev_control", 15);
	}
	ONE_BIT_FIELD("txn_pend", "yesno_t", "%dev_status", 5);

	if (FIELD_GE("../version", 2)) {
		REG32("%dev_caps2", address + 0x24);
		REG16("%dev_control2", address + 0x28);
		REG16("%dev_status2", address + 0x2a);

		BITMASK_DFLT("pcie_completion_timeout_t", "us_50_ms_50",
				KV("us_50_ms_10", 0),
				KV("ms_10_ms_250", 1),
				KV("ms_250_s_4", 2),
				KV("s_4_s_64", 3));

		SIMPLE_FIELD("completion_timeout_ranges",
				"pcie_completion_timeout_t",
				"%dev_caps2", 3, 0);
		SIMPLE_FIELD("completion_timeout",
				"pcie_completion_timeout_t",
				"%dev_control2", 3, 0);

		ONE_BIT_FIELD("completion_timeout_disable_en", "yesno_t",
				"%dev_caps2", 4);
		ONE_BIT_FIELD("completion_timeout_disable", "yesno_t",
				"%dev_control2", 4);
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

		SIMPLE_FIELD("port_number", "int_t", "%link_caps", 31, 24);

		SIMPLE_FIELD("supported_speeds", ANON_ENUM(
					KV("GTs_25", 1),
					KV("GTs_50_25", 2)),
				"%link_caps", 3, 0);
		SIMPLE_FIELD("speed", "pcie_link_speed_t",
				"%link_status", 3, 0);
		SIMPLE_FIELD("max_width", "pcie_width_t", "%link_caps", 9, 4);
		SIMPLE_FIELD("width", "pcie_width_t", "%link_status", 9, 4);

		SIMPLE_FIELD("aspm_support", ANON_BITMASK(
					KV("l0s", 0),
					KV("l1", 1)),
				"%link_caps", 11, 10);
		SIMPLE_FIELD("aspm_ctl", ANON_BITMASK(
					KV("l0s", 0),
					KV("l1", 1)),
				"%link_control", 1, 0);

		SIMPLE_FIELD("l0s_exit_latency", "pcie_l0s_latency_t",
				"%link_caps", 14, 12);
		SIMPLE_FIELD("l1_exit_latency", "pcie_l1_latency_t",
				"%link_caps", 17, 15);

		ONE_BIT_FIELD("clock_pm_cap", "yesno_t", "%link_caps", 18);
		ONE_BIT_FIELD("clock_pm_en", "yesno_t", "%link_control", 8);

		ONE_BIT_FIELD("surprise_down_err_report_cap", "yesno_t",
				"%link_caps", 19);
		ONE_BIT_FIELD("data_link_active_report_cap", "yesno_t",
				"%link_caps", 20);
		if (FIELD_BOOL("data_link_active_report_cap")) {
			ONE_BIT_FIELD("data_link_active", "yesno_t",
					"%link_status", 13);
		}
		ONE_BIT_FIELD("link_bw_notify_cap", "yesno_t",
				"%link_caps", 21);
		if (FIELD_BOOL("link_bw_notify_cap")) {
			ONE_BIT_FIELD("link_bw_status", "yesno_t",
					"%link_status", 14);
			ONE_BIT_FIELD("link_auto_bw_status", "yesno_t",
					"%link_status", 15);
		}

		ONE_BIT_FIELD("rcb", ANON_BOOL("b64", "b128"),
				"%link_control", 3);
		if (FIELD_EQ("../type", "root_port")
		 || FIELD_EQ("../type", "downstream_switch_port")
		 || FIELD_EQ("../type", "pci_pcie_bridge")) {
			ONE_BIT_FIELD("link_disable", "yesno_t",
					"%link_control", 4);
			ONE_BIT_FIELD("link_retrain", "yesno_t",
					"%link_control", 5);
			ONE_BIT_FIELD("link_bw_mgmt_intr_en", "yesno_t",
					"%link_control", 10);
			ONE_BIT_FIELD("link_auto_bw_mgmt_intr_en", "yesno_t",
					"%link_control", 11);
		}
		ONE_BIT_FIELD("common_clock", "yesno_t", "%link_control", 6);
		ONE_BIT_FIELD("extended_sync", "yesno_t", "%link_control", 7);
		ONE_BIT_FIELD("hw_auto_width_dis", "yesno_t",
				"%link_control", 9);

		if (FIELD_EQ("../type", "root_port")
		 || FIELD_EQ("../type", "downstream_switch_port")
		 || FIELD_EQ("../type", "pci_pcie_bridge")) {
			ONE_BIT_FIELD("link_training", "yesno_t",
					"%link_status", 11);
		}
		ONE_BIT_FIELD("slot_clock", "yesno_t", "%link_status", 12);

		if (FIELD_GE("../version", 2)) {
			REG32("%link_caps2", address + 0x2c);
			REG16("%link_control2", address + 0x30);
			REG16("%link_status2", address + 0x32);

			SIMPLE_FIELD("target_link_speed", "pcie_link_speed_t",
					"%link_control2", 3, 0);
			ONE_BIT_FIELD("enter_compliance", "yesno_t",
					"%link_control2", 4);
			ONE_BIT_FIELD("hw_auto_speed_dis", "yesno_t",
					"%link_control2", 5);
			ONE_BIT_FIELD("selecatble_deemphasis",
					ANON_BOOL("dB_neg_6_0", "dB_neg_3_5"),
					"%link_control2", 6);
			SIMPLE_FIELD("transit_margin", "hex4_t",
					"%link_control2", 9, 7);
			ONE_BIT_FIELD("enter_mod_compliance", "yesno_t",
					"%link_control2", 10);
			ONE_BIT_FIELD("compliance_sos", "yesno_t",
					"%link_control2", 11);
			ONE_BIT_FIELD("compliance_deemphasis",
					ANON_BOOL("dB_neg_6_0", "dB_neg_3_5"),
					"%link_control2", 12);

			ONE_BIT_FIELD("current_deemphasis",
					ANON_BOOL("dB_neg_6_0", "dB_neg_3_5"),
					"%link_status2", 0);
		}

		CLOSE_SCOPE(); // link
	}

	if (DEFINED("slot_impl") && FIELD_BOOL("slot_impl")) {
		OPEN_SCOPE("slot");
		REG32("%slot_caps", address + 0x14);
		REG16("%slot_control", address + 0x18);
		REG16("%slot_status", address + 0x1a);

		SIMPLE_FIELD("slot_number", "int_t", "%slot_caps", 31, 19);

		ONE_BIT_FIELD("attn_button", "yesno_t", "%slot_caps", 0);
		if (FIELD_BOOL("attn_button")) {
			ONE_BIT_FIELD("attn_button_en", "yesno_t",
					"%slot_control", 0);
			ONE_BIT_FIELD("attn_button_press", "yesno_t",
					"%slot_status", 0);
		}

		ONE_BIT_FIELD("power_controller", "yesno_t", "%slot_caps", 1);
		if (FIELD_BOOL("power_controller")) {
			ONE_BIT_FIELD("power_controller_ctrl",
					ANON_BOOL("on", "off"),
					"%slot_control", 10);
			ONE_BIT_FIELD("power_fault_en", "yesno_t",
					"%slot_control", 1);
			ONE_BIT_FIELD("power_fault_det", "yesno_t",
					"%slot_status", 1);
		}

		ONE_BIT_FIELD("mrl_sensor", "yesno_t", "%slot_caps", 2);
		if (FIELD_BOOL("mrl_sensor")) {
			ONE_BIT_FIELD("mrl_sensor_en", "yesno_t",
					"%slot_control", 2);
			ONE_BIT_FIELD("mrl_sensor_change", "yesno_t",
					"%slot_status", 2);
			ONE_BIT_FIELD("mrl_sensor_state",
					ANON_BOOL("closed", "open"),
					"%slot_status", 5);
		}

		ONE_BIT_FIELD("attn_indicator", "yesno_t", "%slot_caps", 3);
		if (FIELD_BOOL("attn_indicator")) {
			SIMPLE_FIELD("attn_indicator_ctrl", ANON_ENUM(
						KV("on", 1),
						KV("blink", 2),
						KV("off", 3)),
					"%slot_control", 7, 6);
		}

		ONE_BIT_FIELD("power_indicator", "yesno_t", "%slot_caps", 4);
		if (FIELD_BOOL("power_indicator")) {
			SIMPLE_FIELD("power_indicator_ctrl", ANON_ENUM(
						KV("on", 1),
						KV("blink", 2),
						KV("off", 3)),
					"%slot_control", 9, 8);
		}

		ONE_BIT_FIELD("hotplug_capable", "yesno_t", "%slot_caps", 6);
		if (FIELD_BOOL("hotplug_capable")) {
			ONE_BIT_FIELD("hotplug_surprise", "yesno_t",
					"%slot_caps", 5);
			ONE_BIT_FIELD("presence_detect_en", "yesno_t",
					"%slot_control", 3);
			ONE_BIT_FIELD("presence_detect_change", "yesno_t",
					"%slot_status", 3);
			ONE_BIT_FIELD("presence_detect_state",
					ANON_BOOL("empty", "present"),
					"%slot_status", 6);
			ONE_BIT_FIELD("hot_plug_intr_en", "yesno_t",
					"%slot_control", 5);
			ONE_BIT_FIELD("no_cmd_complete", "yesno_t",
					"%slot_caps", 18);
			if (FIELD_BOOL("no_cmd_complete") == 0) {
				ONE_BIT_FIELD("cmd_complete_intr_en",
						"yesno_t", "%slot_control", 4);
				ONE_BIT_FIELD("cmd_completed", "yesno_t",
						"%slot_status", 4);

			}
		}

		ONE_BIT_FIELD("electromech_lock", "yesno_t", "%slot_caps", 17);
		if (FIELD_BOOL("electromech_lock")) {
			ONE_BIT_FIELD("electromech_lock_ctrl", "int_t",
					"%slot_control", 11);
			ONE_BIT_FIELD("electromech_lock_status",
					ANON_BOOL("disengaged", "engaged"),
					"%slot_status", 7);
		}

		//FIXME: better as a procfield?
		SIMPLE_FIELD("slot_power_limit", "int_t", "%slot_caps", 14, 7);
		SIMPLE_FIELD("slot_power_scale", "int_t", "%slot_caps", 16, 15);

		if (FIELD_BOOL("../link/data_link_active_report_cap")) {
			ONE_BIT_FIELD("data_link_state_change_en", "yesno_t",
					"%slot_control", 12);
			ONE_BIT_FIELD("data_link_state_change", "yesno_t",
					"%slot_status", 8);
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

		ONE_BIT_FIELD("serr_on_cerr", "yesno_t", "%root_control", 0);
		ONE_BIT_FIELD("serr_on_nferr", "yesno_t", "%root_control", 1);
		ONE_BIT_FIELD("serr_on_ferr", "yesno_t", "%root_control", 2);
		ONE_BIT_FIELD("pme_intr_en", "yesno_t", "%root_control", 3);

		ONE_BIT_FIELD("crs_visible_cap", "yesno_t", "%root_caps", 0);
		ONE_BIT_FIELD("crs_visible_en", "yesno_t", "%root_control", 4);

		ONE_BIT_FIELD("pme_status", "yesno_t", "%root_status", 16);
		ONE_BIT_FIELD("pme_pending", "yesno_t", "%root_status", 17);
		SIMPLE_FIELD("pme_requester", "hex16_t", "%root_status", 15, 0);

		CLOSE_SCOPE(); // root
	}
}

// Handle the PCI capabilities linked-list.
static void
explore_capabilities()
{
	if (FIELD_BOOL("status/caps")) {
		REGFIELD8("capptr", 0x34, "hex8_t");

		pp_value ptr = GET_FIELD("capptr")->read();
		unsigned i = 0;
		while (ptr != 0 && ptr != 0xff) {
			OPEN_SCOPE("capability[" + to_string(i) + "]");

			CONSTANT_FIELD("offset", "hex8_t", ptr);
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
		ONE_BIT_FIELD("cap66", "yesno_t", "../%sec_status", 5);
		ONE_BIT_FIELD("fbb", "yesno_t", "../%sec_status", 7);
		ONE_BIT_FIELD("mdperr", "yesno_t", "../%sec_status", 8);
		SIMPLE_FIELD("devsel", ANON_ENUM(
					KV("FAST", 0),
					KV("MEDIUM", 1),
					KV("SLOW", 2)),
				"../%sec_status", 10, 9);
		ONE_BIT_FIELD("sigtabort", "yesno_t", "../%sec_status", 11);
		ONE_BIT_FIELD("rcvtabort", "yesno_t", "../%sec_status", 12);
		ONE_BIT_FIELD("rcvmabort", "yesno_t", "../%sec_status", 13);
		ONE_BIT_FIELD("rcvserr", "yesno_t", "../%sec_status", 14);
		ONE_BIT_FIELD("perr", "yesno_t", "../%sec_status", 15);
	CLOSE_SCOPE();

	// IO decode window
	OPEN_SCOPE("io_window");
		REG8("%base_lo", 0x1c);
		REG16("%base_hi", 0x30);
		REG8("%limit_lo", 0x1d);
		REG16("%limit_hi", 0x32);

		SIMPLE_FIELD("width", ANON_ENUM(
					KV("bits16", 0),
					KV("bits32", 1)),
				"%base_lo", 3, 0);

		if (FIELD_EQ("width", "bits16")) {
			COMPLEX_FIELD("base", "addr16_t",
					BITS("0", 11, 0),
					BITS("%base_lo", 7, 4));
			COMPLEX_FIELD("limit", "addr16_t",
					BITS("1", 11, 0),
					BITS("%limit_lo", 7, 4));
		} else if (FIELD_EQ("width", "bits32")) {
			COMPLEX_FIELD("base", "addr32_t",
					BITS("0", 11, 0),
					BITS("%base_lo", 7, 4),
					BITS("%base_hi", 15, 0));
			COMPLEX_FIELD("limit", "addr32_t",
					BITS("1", 11, 0),
					BITS("%limit_lo", 7, 4),
					BITS("%limit_hi", 15, 0));
		}
	CLOSE_SCOPE();

	// MMIO decode window
	OPEN_SCOPE("mem_window");
		REG16("%base", 0x20);
		REG16("%limit", 0x22);

		COMPLEX_FIELD("base", "addr32_t",
				BITS("0", 19, 0),
				BITS("%base", 15, 4));
		COMPLEX_FIELD("limit", "addr32_t",
				BITS("1", 19, 0),
				BITS("%limit", 15, 4));
	CLOSE_SCOPE();

	// Prefetched memory decode window
	OPEN_SCOPE("prefetch_window");
		REG16("%base_lo", 0x24);
		REG32("%base_hi", 0x28);
		REG16("%limit_lo", 0x26);
		REG32("%limit_hi", 0x2c);

		SIMPLE_FIELD("width", ANON_ENUM(
					KV("bits32", 0),
					KV("bits64", 1)),
				"%base_lo", 3, 0);

		if (FIELD_EQ("width", "bits32")) {
			COMPLEX_FIELD("base", "addr32_t",
					BITS("0", 19, 0),
					BITS("%base_lo", 15, 4));
			COMPLEX_FIELD("limit", "addr32_t",
					BITS("1", 19, 0),
					BITS("%limit_lo", 15, 4));
		} else if (FIELD_EQ("width", "bits64")) {
			COMPLEX_FIELD("base", "addr64_t",
					BITS("0", 19, 0),
					BITS("%base_lo", 15, 4),
					BITS("%base_hi", 31, 0));
			COMPLEX_FIELD("limit", "addr64_t",
					BITS("1", 19, 0),
					BITS("%limit_lo", 15, 4),
					BITS("%limit_hi", 31, 0));
		}
	CLOSE_SCOPE();

	// Capabilities
	explore_capabilities();

	// Expansion ROM
	REG32("%rombase", 0x38);
	OPEN_SCOPE("rombase");
		ONE_BIT_FIELD("enabled", "yesno_t", "../%rombase", 0);
		COMPLEX_FIELD("address", "addr32_t",
				BITS("0", 10, 0),
				BITS("../%rombase", 31, 11));
	CLOSE_SCOPE();

	// Bridge control
	REG16("%bridge_ctrl", 0x3e);
	OPEN_SCOPE("bridge_ctrl");
		ONE_BIT_FIELD("perr", "yesno_t", "../%bridge_ctrl", 0);
		ONE_BIT_FIELD("serr", "yesno_t", "../%bridge_ctrl", 1);
		ONE_BIT_FIELD("isa", "yesno_t", "../%bridge_ctrl", 2);
		ONE_BIT_FIELD("vga", "yesno_t", "../%bridge_ctrl", 3);
		ONE_BIT_FIELD("vga16", "yesno_t", "../%bridge_ctrl", 4);
		ONE_BIT_FIELD("mst_abort", "yesno_t", "../%bridge_ctrl", 5);
		ONE_BIT_FIELD("sec_reset", "yesno_t", "../%bridge_ctrl", 6);
		ONE_BIT_FIELD("fbb", "yesno_t", "../%bridge_ctrl", 7);
		ONE_BIT_FIELD("pri_discard", "yesno_t", "../%bridge_ctrl", 8);
		ONE_BIT_FIELD("sec_discard", "yesno_t", "../%bridge_ctrl", 9);
		ONE_BIT_FIELD("discard_status", "yesno_t", "../%bridge_ctrl", 10);
		ONE_BIT_FIELD("discard_serr", "yesno_t", "../%bridge_ctrl", 11);
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
		ONE_BIT_FIELD("enabled", "yesno_t", "../%rombase", 0);
		COMPLEX_FIELD("address", "addr32_t",
				BITS("0", 10, 0),
				BITS("../%rombase", 31, 11));
	CLOSE_SCOPE();

	REGFIELD8("mingnt", 0x3e, ANON_INT("1/4 usecs"));
	REGFIELD8("maxlat", 0x3f, ANON_INT("1/4 usecs"));
}

/* populate the current scope with generic PCI device fields */
void
pci_generic_device()
{
	for (unsigned i = 0; i < 4096; i += 4) {
		REG32(to_string(boost::format("%%PCI[%04x]") %i), i);
	}

	REGFIELD16("vendor", 0x00, "pci_vendor_t");
	REGFIELD16("device", 0x02, "hex16_t");

	REG16("%command", 0x04);
	OPEN_SCOPE("command");
		ONE_BIT_FIELD("io", "yesno_t", "../%command", 0);
		ONE_BIT_FIELD("mem", "yesno_t", "../%command", 1);
		ONE_BIT_FIELD("bm", "yesno_t", "../%command", 2);
		ONE_BIT_FIELD("special", "yesno_t", "../%command", 3);
		ONE_BIT_FIELD("mwinv", "yesno_t", "../%command", 4);
		ONE_BIT_FIELD("vgasnoop", "yesno_t", "../%command", 5);
		ONE_BIT_FIELD("perr", "yesno_t", "../%command", 6);
		ONE_BIT_FIELD("step", "yesno_t", "../%command", 7);
		ONE_BIT_FIELD("serr", "yesno_t", "../%command", 8);
		ONE_BIT_FIELD("fbb", "yesno_t", "../%command", 9);
		ONE_BIT_FIELD("intr", "yesno_t", "../%command", 10);
	CLOSE_SCOPE();

	REG16("%status", 0x06);
	OPEN_SCOPE("status");
		ONE_BIT_FIELD("intr", "yesno_t", "../%status", 3);
		ONE_BIT_FIELD("caps", "yesno_t", "../%status", 4);
		ONE_BIT_FIELD("cap66", "yesno_t", "../%status", 5);
		ONE_BIT_FIELD("user", "yesno_t", "../%status", 6);
		ONE_BIT_FIELD("fbb", "yesno_t", "../%status", 7);
		ONE_BIT_FIELD("mdperr", "yesno_t", "../%status", 8);
		SIMPLE_FIELD("devsel", ANON_ENUM(
					KV("FAST", 0),
					KV("MEDIUM", 1),
					KV("SLOW", 2)),
				"../%status", 10, 9);
		ONE_BIT_FIELD("sigtabort", "yesno_t", "../%status", 11);
		ONE_BIT_FIELD("rcvtabort", "yesno_t", "../%status", 12);
		ONE_BIT_FIELD("rcvmabort", "yesno_t", "../%status", 13);
		ONE_BIT_FIELD("sigserr", "yesno_t", "../%status", 14);
		ONE_BIT_FIELD("perr", "yesno_t", "../%status", 15);
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
	} else if (FIELD_EQ("class", "simple_comm")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_simple_comm");
	} else if (FIELD_EQ("class", "base_system")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_base_system");
	} else if (FIELD_EQ("class", "input")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_input");
	} else if (FIELD_EQ("class", "docking")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_docking");
	} else if (FIELD_EQ("class", "processor")) {
		REGFIELD8("subclass", 0x0a, "pci_subclass_processor");
	//FIXME: not implemented yet
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
	COMPLEX_FIELD("hdrtype", ANON_ENUM(
				KV("device", 0),
				KV("pci_bridge", 1),
				KV("cardbus_bridge", 2)),
			BITS("%hdrtype", 6, 0));
	ONE_BIT_FIELD("multifunc", "yesno_t", "%hdrtype", 7);

	REG8("%bist", 0x0f);
	OPEN_SCOPE("bist");
		ONE_BIT_FIELD("capable", "yesno_t", "../%bist", 7);
		ONE_BIT_FIELD("start", "yesno_t", "../%bist", 6);
		COMPLEX_FIELD("code", "hex8_t", BITS("../%bist", 3, 0));
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
