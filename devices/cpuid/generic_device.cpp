//
// This file has been developed purely from publicly-available documentation
// acquired from the following sources:
// http://download.intel.com/design/processor/applnots/24161832.pdf
// http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
// http://www.sandpile.org/ia32/cpuid.htm
//

#include "pp.h"
#include "generic_device.h"
#include "fake_language.h"

// Functions for easy access to individual registers returned by cpuid
inline pp_regbits
EAX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi, lo);
}
inline pp_regbits
EAX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit);
}
inline pp_regbits
EAX(const string &name)
{
	return BITS(name, 31, 0);
}
inline pp_regbits
EBX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi + 32, lo + 32);
}
inline pp_regbits
EBX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit + 32);
}
inline pp_regbits
EBX(const string &name)
{
	return BITS(name, 63, 32);
}
inline pp_regbits
ECX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi + 64, lo + 64);
}
inline pp_regbits
ECX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit + 64);
}
inline pp_regbits
ECX(const string &name)
{
	return BITS(name, 95, 64);
}
inline pp_regbits
EDX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi + 96, lo + 96);
}
inline pp_regbits
EDX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit + 96);
}
inline pp_regbits
EDX(const string &name)
{
	return BITS(name, 127, 96);
}

void
CPUID_SCOPE(const string &name, const pp_value &cpu)
{
	OPEN_SCOPE(name, BIND("cpuid", ARGS(cpu)));
	BOOKMARK("cpuid");
	cpuid_generic_device();
}

// create a scope and simple fields for an "opaque" MSR
void
CPUID_REGSCOPE(const string &name, const pp_value &address)
{
	string regname = "%" + name;
	REG64(regname, address);
	OPEN_SCOPE(name);
	FIELD("addr", "hex64_t", address);
	FIELD("eax", "hex32_t", BITS(regname, 31, 0));
	FIELD("ebx", "hex32_t", BITS(regname, 63, 32));
	FIELD("ecx", "hex32_t", BITS(regname, 95, 64));
	FIELD("edx", "hex32_t", BITS(regname, 127, 96));
	CLOSE_SCOPE();
}

/* populate the current scope with generic CPUID fields */
class cpuid_family_procs: public pp_rwprocs
{
	pp_value
	read() const
	{
		if (FIELD_EQ("vendor", "intel")) {
			pp_value family = READ(EAX("%function_1", 11, 8))
			                + READ(EAX("%function_1", 27, 20));
			return family;
		} else if (FIELD_EQ("vendor", "amd")) {
			pp_value base_family = READ(EAX("%function_1", 11, 8));
			if (base_family == 0xf) {
				return READ(EAX("%function_1", 27, 20)) + 0xf;
			}
			return base_family;
		}
		WARN("unknown CPUID vendor, using base family");
		return READ(EAX("%function_1", 11, 8));
	}
	void
	write(const pp_value &value) const
	{
		(void)value;
		// not supported
	}
};
class cpuid_model_procs: public pp_rwprocs
{
	pp_value
	read() const
	{
		if (FIELD_EQ("vendor", "intel")) {
			return READ(EAX("%function_1", 19, 16)
				    + EAX("%function_1", 7, 4));
		} else if (FIELD_EQ("vendor", "amd")) {
			pp_value base_family = READ(EAX("%function_1", 11, 8));
			if (base_family == 0xf) {
				return READ(EAX("%function_1", 19, 16)
					    + EAX("%function_1", 7, 4));
			}
			return READ(EAX("%function_1", 7, 4));
		}
		WARN("unknown CPUID vendor, using base_model");
		return READ(EAX("%function_1", 7, 4));
	}
	void
	write(const pp_value &value) const
	{
		(void)value;
		// not supported
	}
};
void
cpuid_generic_device()
{
	REG128("%function_0", 0x0);
	FIELD("largest_std_fn", "hex_t", EAX("%function_0", 31, 0));
	FIELD("vendor", "cpuid_vendor_t",
			ECX("%function_0") +
			EDX("%function_0") +
			EBX("%function_0"));
	FIELD("vendor_str", ANON_STRING(),
			ECX("%function_0") +
			EDX("%function_0") +
			EBX("%function_0"));

	// Feature Information
	// CPUID Function 0x1
	if (FIELD_GE("largest_std_fn", 1)) {
		REG128("%function_1", 0x1);
		FIELD("family", "int_t", PROCS(cpuid_family_procs));
		FIELD("model", "int_t", PROCS(cpuid_model_procs));
		FIELD("stepping", "int_t", EAX("%function_1", 3, 0));

		FIELD("type", ANON_ENUM(
				KV("oem", 0),
				KV("overdrive", 1),
				KV("dual", 2)),
			EAX("%function_1", 13, 12));

		FIELD("std_features", "cpuid_features_t",
				ECX("%function_1", 31, 0)
				+ EDX("%function_1", 31, 0));

		FIELD("brand_id", "int_t",
				EBX("%function_1", 7, 0));
		FIELD("clflush_size", ANON_INT("QWORDs"),
				EBX("%function_1", 15, 8));
		if (FIELD_TEST("std_features", "htt")) {
			FIELD("logical_proc_count", "int_t",
					EBX("%function_1", 23, 16));
		} else {
			FIELD("logical_proc_count", "int_t", 1);
		}
		FIELD("init_lapic_id", "int_t",
				EBX("%function_1", 31, 24));
	} // CPUID Function 0x1

	// Cache Descriptors
	// CPUID Function 0x2
	if (FIELD_GE("largest_std_fn", 0x2)) {
		uint8_t queries_needed = 1; // updated on first call
		OPEN_SCOPE("cache_descriptors");

		// check for special Xeon case
		string cache_descriptor_t;
		if (FIELD_EQ("../vendor", "intel")
		 && FIELD_EQ("../family", 0x0F)
		 && FIELD_EQ("../model", 0x06)) {
			cache_descriptor_t = "cache_descriptor_xeon_t";
		} else {
			cache_descriptor_t = "cache_descriptor_t";
		}

		for (uint8_t i = 0; i < queries_needed; i++) {
			REG128("%function_2[]", 0x2);

			if (i == 0) {
				FIELD("queries_needed", "int_t",
						EAX("%function_2[$]", 7, 0));
				queries_needed =
					READ("queries_needed").get_uint();
			}

			if (READ(EAX("%function_2[$]", 31)) == pp_value(0)) {
				for (int i = 31; i > 7; i -= 8) {
					if (READ(EAX("%function_2[$]",
					    i, i-7)) != 0) {
						FIELD("descriptor[]",
						      cache_descriptor_t,
						      EAX("%function_2[$]",
						          i, i - 7));
					}
				}
			}
			if (READ(EBX("%function_2[$]", 31)) == pp_value(0)) {
				for (int i = 31; i > 0; i -= 8) {
					if (READ(EBX("%function_2[$]",
					    i, i-7)) != 0) {
						FIELD("descriptor[]",
						      cache_descriptor_t,
						      EBX("%function_2[$]",
						          i, i - 7));
					}
				}
			}
			if (READ(ECX("%function_2[$]", 31)) == pp_value(0)) {
				for (int i = 31; i > 0; i -= 8) {
					if (READ(ECX("%function_2[$]",
					    i, i-7)) != 0) {
						FIELD("descriptor[]",
						      cache_descriptor_t,
						      ECX("%function_2[$]",
						          i, i - 7));
					}
				}
			}
			if (READ(EDX("%function_2[$]", 31)) == pp_value(0)) {
				for (int i = 31; i > 0; i -= 8) {
					if (READ(EDX("%function_2[$]",
					    i, i-7)) != 0) {
						FIELD("descriptor[]",
						      cache_descriptor_t,
						      EDX("%function_2[$]",
						          i, i - 7));
					}
				}
			}
		}
		CLOSE_SCOPE();
	} // CPUID Function 0x2

	// Processor Serial Number
	// CPUID Function 0x3 deprecated
	// Probably not worth implementing, only works on Pentium III

	// Deterministic Cache Parameters
	// CPUID Function 0x4
	if (FIELD_GE("largest_std_fn", 0x4)) {
		for (int i = 0; true; i++) {
			pp_value addr = (pp_value(i) << 32) | pp_value(0x4);
			// read the next 'type' field to see if it is valid
			if (READ(BITS(REG128(addr), 4, 0)) == 0) {
				break;
			}
			OPEN_SCOPE("cache_info[]");
			REG128("%function_4", addr);
			FIELD("type", ANON_ENUM(KV("null", 0),
						KV("data_cache", 1),
						KV("instr_cache", 2),
						KV("unified_cache", 3)),
					EAX("%function_4", 4, 0));
			FIELD("max_cores_per_pkg", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1)),
				EAX("%function_4", 31, 26));
			FIELD("max_threads_sharing_cache", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1)),
				EAX("%function_4", 25, 14));
			FIELD("fully_assoc", "yesno_t",
			      EAX("%function_4", 9));
			FIELD("self_init", "yesno_t",
			      EAX("%function_4", 8));
			FIELD("level", "int_t",
			      EAX("%function_4", 7, 5));
			FIELD("ways_of_assoc", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1)),
				EBX("%function_4", 31, 22));
			FIELD("phys_line_partitions", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1)),
				EBX("%function_4", 21, 12));
			FIELD("sys_coherency_line_size", ANON_XFORM(
				ANON_INT("bytes"), LAMBDA(_1 + 1), LAMBDA(_1)),
				EBX("%function_4", 11, 0));
			FIELD("num_sets", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1)),
				ECX("%function_4", 31, 0));
			FIELD("prefetch_stride", ANON_XFORM(ANON_INT("bytes"),
					LAMBDA((!_1)*64 + _1), LAMBDA(_1)),
				EDX("%function_4", 9, 0));
			CLOSE_SCOPE(); // cache_info[]
		}
	} // CPUID Function 0x4

	// MONITOR / MWAIT Parameters
	// CPUID Function 0x5
	if (FIELD_GE("largest_std_fn", 0x5)) {
		OPEN_SCOPE("monitor_mwait");
		REG128("%function_5", 0x5);
		FIELD("mon_line_size_min", ANON_INT("bytes"),
				EAX("%function_5", 15, 0));
		FIELD("mon_line_size_max", ANON_INT("bytes"),
				EBX("%function_5", 15, 0));
		FIELD("ibe", "yesno_t",
				ECX("%function_5", 1));
		FIELD("emx", "yesno_t",
				EDX("%function_5", 0));
		FIELD("c0_sub_states", "int_t",
				EDX("%function_5", 3, 0));
		FIELD("c1_sub_states", "int_t",
				EDX("%function_5", 7, 4));
		FIELD("c2_sub_states", "int_t",
				EDX("%function_5", 11, 8));
		FIELD("c3_sub_states", "int_t",
				EDX("%function_5", 15, 12));
		FIELD("c4_sub_states", "int_t",
				EDX("%function_5", 19, 16));
		CLOSE_SCOPE();
	} // CPUID Function 0x5

	// Digital Thermal Sensor and Power Management Parameters
	// CPUID Function 0x6
	if (FIELD_GE("largest_std_fn", 0x6)) {
		OPEN_SCOPE("therm_power_mgmt");
		REG128("%function_6", 0x6);
		FIELD("digital_thermal_sensor_cap", "yesno_t",
				EAX("%function_6", 0));
		FIELD("num_int_thresholds", "int_t", EAX("%function_6", 3, 0));
		FIELD("hw_coord_feedback_cap", "yesno_t",
				ECX("%function_6", 0));
		CLOSE_SCOPE();
	} // CPUID Function 0x6

	// CPUID Functions 0x7 & 0x8 Reserved on both Intel and AMD

	// Direct Cache Access Parameters
	// CPUID Function 0x9
	if (FIELD_GE("largest_std_fn", 0x9)) {
		OPEN_SCOPE("direct_cache_access");
		REG128("%function_9", 0x9);
		FIELD("platform_dca_cap", "hex32_t", EAX("%function_9"));
		CLOSE_SCOPE();
	}

	// Architectural Performance Monitor Features
	// CPUID Function 0xA
	if (FIELD_GE("largest_std_fn", 0xA)) {
		OPEN_SCOPE("perf_mon_features");
		REG128("%function_A", 0xA);
		BOOL("not_available_t", "not available", "available");
		FIELD("ebx_bit_vector_len", "int_t",
				EAX("%function_A", 31, 24));
		FIELD("genpurpose_perfmon_counter_width", "int_t",
				EAX("%function_A", 23, 16));
		FIELD("genpurpose_perfmon_counter_num", "int_t",
				EAX("%function_A", 15, 8));
		FIELD("arch_perfmon_version", "int_t",
				EAX("%function_A", 7, 0));
		FIELD("event_branch_mispredict_retired", "not_available_t",
				EBX("%function_A", 6));
		FIELD("event_branch_instr_retired", "not_available_t",
				EBX("%function_A", 5));
		FIELD("event_last_level_cache_misses", "not_available_t",
				EBX("%function_A", 4));
		FIELD("event_last_level_cache_reverence", "not_available_t",
				EBX("%function_A", 3));
		FIELD("event_reference_cycles", "not_available_t",
				EBX("%function_A", 2));
		FIELD("event_instr_retired", "not_available_t",
				EBX("%function_A", 1));
		FIELD("event_core_cycle", "not_available_t",
				EBX("%function_A", 0));
		if (FIELD_GT("arch_perfmon_version", 1)) {
			FIELD("fixed_fn_perf_counter_width", "int_t",
					EDX("%function_A", 12, 5));
			FIELD("fixed_fn_perf_counter_num" ,"int_t",
					EDX("%function_A", 4, 0));
		}
		CLOSE_SCOPE();
	} // CPUID Function 0xA

	// Topology Enumeration Information
	// CPUID Function 0xB
	if (FIELD_GE("largest_std_fn", 0xB)) {
		for (int i = 0; true; i++) {
			pp_value addr = (pp_value(i) << 32) | pp_value(0xB);
			// check if we have run out of levels
			if (READ(BITS(REG128(addr), 63, 0)) == 0) {
				break;
			}
			OPEN_SCOPE("topo_info[]");
			REG128("%function_B", addr);
			FIELD("bits_to_shift_for_next_level_id", "int_t",
					EAX("%function_B", 4, 0));
			FIELD("enabled_logical_procs", "int_t",
					EBX("%function_B", 15, 0));
			FIELD("level_type", ANON_ENUM(KV("smt", 1),
						      KV("core", 2)),
					ECX("%function_B", 15, 8));
			FIELD("level_num", "int_t", ECX("%function_B", 7, 0));
			FIELD("x2apic_id", "hex32_t", EDX("%function_B"));
			CLOSE_SCOPE(); // topo_info[]
		}
	} // CPUID Function 0xB

	// Largest Extended Function #
	// CPUID Function 0x80000000
	REG128("%function_80000000", 0x80000000);
	FIELD("largest_ext_fn", "hex32_t", EAX("%function_80000000", 31, 0));

	// Extended Feature Bits
	// CPUID Function 0x80000001
	if (FIELD_GE("largest_ext_fn", 0x80000001)) {
		REG128("%function_80000001", 0x80000001);
		if (FIELD_EQ("vendor", "amd")) {
			FIELD("brand_id_16bit", "int_t",
					EBX("%function_80000001", 15, 0));
			if (FIELD_AND("family", 0xFF) >= 0x10) {
				FIELD("pkg_type", "hex_t",
						EBX("%function_80000001",
						    31, 28));
			}
		}
		FIELD("ext_features", "cpuid_ext_features_t",
				ECX("%function_80000001")
				+ EDX("%function_80000001"));
	} // CPUID Function 0x80000001

	// Processor Name / Brand String
	// CPUID Extended Functions 0x80000002-0x80000004
	if (FIELD_GE("largest_ext_fn", 0x80000004)) {
		REG128("%function_80000002", 0x80000002);
		REG128("%function_80000003", 0x80000003);
		REG128("%function_80000004", 0x80000004);
		FIELD("processor_name_str", ANON_STRING(),
				BITS("%function_80000004", 127, 0) +
				BITS("%function_80000003", 127, 0) +
				BITS("%function_80000002", 127, 0));
	} // CPUID Functions 0x80000002-0x80000004

	// L1 Cache and TLB Identifiers
	// CPUID Function 0x80000005
	if (FIELD_EQ("vendor", "amd") &&
	    FIELD_GE("largest_ext_fn", 0x80000005)) {
		OPEN_SCOPE("l1_cache_tlb_identifiers");
		REG128("%function_80000005", 0x80000005);
		// FIXME: Is there a better way to do this than a 255-KV enum?
		ENUM("assoc_t",
			KV("direct_mapped", 0x1),
			KV("2_way_assoc", 0x2),
			KV("3_way_assoc", 0x3),
			KV("4_way_assoc", 0x4),
			KV("5_way_assoc", 0x5),
			KV("6_way_assoc", 0x6),
			KV("7_way_assoc", 0x7),
			KV("8_way_assoc", 0x8),
			KV("9_way_assoc", 0x9),
			KV("10_way_assoc", 0xa),
			KV("11_way_assoc", 0xb),
			KV("12_way_assoc", 0xc),
			KV("13_way_assoc", 0xd),
			KV("14_way_assoc", 0xe),
			KV("15_way_assoc", 0xf),
			KV("16_way_assoc", 0x10),
			KV("17_way_assoc", 0x11),
			KV("18_way_assoc", 0x12),
			KV("19_way_assoc", 0x13),
			KV("20_way_assoc", 0x14),
			KV("21_way_assoc", 0x15),
			KV("22_way_assoc", 0x16),
			KV("23_way_assoc", 0x17),
			KV("24_way_assoc", 0x18),
			KV("25_way_assoc", 0x19),
			KV("26_way_assoc", 0x1a),
			KV("27_way_assoc", 0x1b),
			KV("28_way_assoc", 0x1c),
			KV("29_way_assoc", 0x1d),
			KV("30_way_assoc", 0x1e),
			KV("31_way_assoc", 0x1f),
			KV("32_way_assoc", 0x20),
			KV("33_way_assoc", 0x21),
			KV("34_way_assoc", 0x22),
			KV("35_way_assoc", 0x23),
			KV("36_way_assoc", 0x24),
			KV("37_way_assoc", 0x25),
			KV("38_way_assoc", 0x26),
			KV("39_way_assoc", 0x27),
			KV("40_way_assoc", 0x28),
			KV("41_way_assoc", 0x29),
			KV("42_way_assoc", 0x2a),
			KV("43_way_assoc", 0x2b),
			KV("44_way_assoc", 0x2c),
			KV("45_way_assoc", 0x2d),
			KV("46_way_assoc", 0x2e),
			KV("47_way_assoc", 0x2f),
			KV("48_way_assoc", 0x30),
			KV("49_way_assoc", 0x31),
			KV("50_way_assoc", 0x32),
			KV("51_way_assoc", 0x33),
			KV("52_way_assoc", 0x34),
			KV("53_way_assoc", 0x35),
			KV("54_way_assoc", 0x36),
			KV("55_way_assoc", 0x37),
			KV("56_way_assoc", 0x38),
			KV("57_way_assoc", 0x39),
			KV("58_way_assoc", 0x3a),
			KV("59_way_assoc", 0x3b),
			KV("60_way_assoc", 0x3c),
			KV("61_way_assoc", 0x3d),
			KV("62_way_assoc", 0x3e),
			KV("63_way_assoc", 0x3f),
			KV("64_way_assoc", 0x40),
			KV("65_way_assoc", 0x41),
			KV("66_way_assoc", 0x42),
			KV("67_way_assoc", 0x43),
			KV("68_way_assoc", 0x44),
			KV("69_way_assoc", 0x45),
			KV("70_way_assoc", 0x46),
			KV("71_way_assoc", 0x47),
			KV("72_way_assoc", 0x48),
			KV("73_way_assoc", 0x49),
			KV("74_way_assoc", 0x4a),
			KV("75_way_assoc", 0x4b),
			KV("76_way_assoc", 0x4c),
			KV("77_way_assoc", 0x4d),
			KV("78_way_assoc", 0x4e),
			KV("79_way_assoc", 0x4f),
			KV("80_way_assoc", 0x50),
			KV("81_way_assoc", 0x51),
			KV("82_way_assoc", 0x52),
			KV("83_way_assoc", 0x53),
			KV("84_way_assoc", 0x54),
			KV("85_way_assoc", 0x55),
			KV("86_way_assoc", 0x56),
			KV("87_way_assoc", 0x57),
			KV("88_way_assoc", 0x58),
			KV("89_way_assoc", 0x59),
			KV("90_way_assoc", 0x5a),
			KV("91_way_assoc", 0x5b),
			KV("92_way_assoc", 0x5c),
			KV("93_way_assoc", 0x5d),
			KV("94_way_assoc", 0x5e),
			KV("95_way_assoc", 0x5f),
			KV("96_way_assoc", 0x60),
			KV("97_way_assoc", 0x61),
			KV("98_way_assoc", 0x62),
			KV("99_way_assoc", 0x63),
			KV("100_way_assoc", 0x64),
			KV("101_way_assoc", 0x65),
			KV("102_way_assoc", 0x66),
			KV("103_way_assoc", 0x67),
			KV("104_way_assoc", 0x68),
			KV("105_way_assoc", 0x69),
			KV("106_way_assoc", 0x6a),
			KV("107_way_assoc", 0x6b),
			KV("108_way_assoc", 0x6c),
			KV("109_way_assoc", 0x6d),
			KV("110_way_assoc", 0x6e),
			KV("111_way_assoc", 0x6f),
			KV("112_way_assoc", 0x70),
			KV("113_way_assoc", 0x71),
			KV("114_way_assoc", 0x72),
			KV("115_way_assoc", 0x73),
			KV("116_way_assoc", 0x74),
			KV("117_way_assoc", 0x75),
			KV("118_way_assoc", 0x76),
			KV("119_way_assoc", 0x77),
			KV("120_way_assoc", 0x78),
			KV("121_way_assoc", 0x79),
			KV("122_way_assoc", 0x7a),
			KV("123_way_assoc", 0x7b),
			KV("124_way_assoc", 0x7c),
			KV("125_way_assoc", 0x7d),
			KV("126_way_assoc", 0x7e),
			KV("127_way_assoc", 0x7f),
			KV("128_way_assoc", 0x80),
			KV("129_way_assoc", 0x81),
			KV("130_way_assoc", 0x82),
			KV("131_way_assoc", 0x83),
			KV("132_way_assoc", 0x84),
			KV("133_way_assoc", 0x85),
			KV("134_way_assoc", 0x86),
			KV("135_way_assoc", 0x87),
			KV("136_way_assoc", 0x88),
			KV("137_way_assoc", 0x89),
			KV("138_way_assoc", 0x8a),
			KV("139_way_assoc", 0x8b),
			KV("140_way_assoc", 0x8c),
			KV("141_way_assoc", 0x8d),
			KV("142_way_assoc", 0x8e),
			KV("143_way_assoc", 0x8f),
			KV("144_way_assoc", 0x90),
			KV("145_way_assoc", 0x91),
			KV("146_way_assoc", 0x92),
			KV("147_way_assoc", 0x93),
			KV("148_way_assoc", 0x94),
			KV("149_way_assoc", 0x95),
			KV("150_way_assoc", 0x96),
			KV("151_way_assoc", 0x97),
			KV("152_way_assoc", 0x98),
			KV("153_way_assoc", 0x99),
			KV("154_way_assoc", 0x9a),
			KV("155_way_assoc", 0x9b),
			KV("156_way_assoc", 0x9c),
			KV("157_way_assoc", 0x9d),
			KV("158_way_assoc", 0x9e),
			KV("159_way_assoc", 0x9f),
			KV("160_way_assoc", 0xa0),
			KV("161_way_assoc", 0xa1),
			KV("162_way_assoc", 0xa2),
			KV("163_way_assoc", 0xa3),
			KV("164_way_assoc", 0xa4),
			KV("165_way_assoc", 0xa5),
			KV("166_way_assoc", 0xa6),
			KV("167_way_assoc", 0xa7),
			KV("168_way_assoc", 0xa8),
			KV("169_way_assoc", 0xa9),
			KV("170_way_assoc", 0xaa),
			KV("171_way_assoc", 0xab),
			KV("172_way_assoc", 0xac),
			KV("173_way_assoc", 0xad),
			KV("174_way_assoc", 0xae),
			KV("175_way_assoc", 0xaf),
			KV("176_way_assoc", 0xb0),
			KV("177_way_assoc", 0xb1),
			KV("178_way_assoc", 0xb2),
			KV("179_way_assoc", 0xb3),
			KV("180_way_assoc", 0xb4),
			KV("181_way_assoc", 0xb5),
			KV("182_way_assoc", 0xb6),
			KV("183_way_assoc", 0xb7),
			KV("184_way_assoc", 0xb8),
			KV("185_way_assoc", 0xb9),
			KV("186_way_assoc", 0xba),
			KV("187_way_assoc", 0xbb),
			KV("188_way_assoc", 0xbc),
			KV("189_way_assoc", 0xbd),
			KV("190_way_assoc", 0xbe),
			KV("191_way_assoc", 0xbf),
			KV("192_way_assoc", 0xc0),
			KV("193_way_assoc", 0xc1),
			KV("194_way_assoc", 0xc2),
			KV("195_way_assoc", 0xc3),
			KV("196_way_assoc", 0xc4),
			KV("197_way_assoc", 0xc5),
			KV("198_way_assoc", 0xc6),
			KV("199_way_assoc", 0xc7),
			KV("200_way_assoc", 0xc8),
			KV("201_way_assoc", 0xc9),
			KV("202_way_assoc", 0xca),
			KV("203_way_assoc", 0xcb),
			KV("204_way_assoc", 0xcc),
			KV("205_way_assoc", 0xcd),
			KV("206_way_assoc", 0xce),
			KV("207_way_assoc", 0xcf),
			KV("208_way_assoc", 0xd0),
			KV("209_way_assoc", 0xd1),
			KV("210_way_assoc", 0xd2),
			KV("211_way_assoc", 0xd3),
			KV("212_way_assoc", 0xd4),
			KV("213_way_assoc", 0xd5),
			KV("214_way_assoc", 0xd6),
			KV("215_way_assoc", 0xd7),
			KV("216_way_assoc", 0xd8),
			KV("217_way_assoc", 0xd9),
			KV("218_way_assoc", 0xda),
			KV("219_way_assoc", 0xdb),
			KV("220_way_assoc", 0xdc),
			KV("221_way_assoc", 0xdd),
			KV("222_way_assoc", 0xde),
			KV("223_way_assoc", 0xdf),
			KV("224_way_assoc", 0xe0),
			KV("225_way_assoc", 0xe1),
			KV("226_way_assoc", 0xe2),
			KV("227_way_assoc", 0xe3),
			KV("228_way_assoc", 0xe4),
			KV("229_way_assoc", 0xe5),
			KV("230_way_assoc", 0xe6),
			KV("231_way_assoc", 0xe7),
			KV("232_way_assoc", 0xe8),
			KV("233_way_assoc", 0xe9),
			KV("234_way_assoc", 0xea),
			KV("235_way_assoc", 0xeb),
			KV("236_way_assoc", 0xec),
			KV("237_way_assoc", 0xed),
			KV("238_way_assoc", 0xee),
			KV("239_way_assoc", 0xef),
			KV("240_way_assoc", 0xf0),
			KV("241_way_assoc", 0xf1),
			KV("242_way_assoc", 0xf2),
			KV("243_way_assoc", 0xf3),
			KV("244_way_assoc", 0xf4),
			KV("245_way_assoc", 0xf5),
			KV("246_way_assoc", 0xf6),
			KV("247_way_assoc", 0xf7),
			KV("248_way_assoc", 0xf8),
			KV("249_way_assoc", 0xf9),
			KV("250_way_assoc", 0xfa),
			KV("251_way_assoc", 0xfb),
			KV("252_way_assoc", 0xfc),
			KV("253_way_assoc", 0xfd),
			KV("254_way_assoc", 0xfe),
			KV("fully_assoc", 0xff));
		INT("entries_t", "entries");
		FIELD("l1_d_tlb_2_and_4_m_assoc", "assoc_t",
				EAX("%function_80000005", 31, 24));
		FIELD("l1_d_tlb_2_and_4_m_size", "entries_t",
				EAX("%function_80000005", 23, 16));
		FIELD("l1_i_tlb_2_and_4_m_assoc", "assoc_t",
				EAX("%function_80000005", 15, 8));
		FIELD("l1_i_tlb_2_and_4_m_size", "entries_t",
				EAX("%function_80000005", 7, 0));
		FIELD("l1_d_tlb_4k_assoc", "assoc_t",
				EBX("%function_80000005", 31, 24));
		FIELD("l1_d_tlb_4k_size", "entries_t",
				EBX("%function_80000005", 23, 16));
		FIELD("l1_i_tlb_4k_assoc", "assoc_t",
				EBX("%function_80000005", 15, 8));
		FIELD("l1_i_tlb_4k_size", "entries_t",
				EBX("%function_80000005", 7, 0));
		FIELD("l1_d_c_size", ANON_INT("KB"),
				ECX("%function_80000005", 31, 24));
		FIELD("l1_d_c_assoc", "assoc_t",
				ECX("%function_80000005", 23, 16));
		FIELD("l1_d_c_lines_per_tag", "int_t",
				ECX("%function_80000005", 15, 8));
		FIELD("l1_d_c_line_size", ANON_INT("B"),
				ECX("%function_80000005", 7, 0));
		FIELD("l1_i_c_size", ANON_INT("KB"),
				EDX("%function_80000005", 31, 24));
		FIELD("l1_i_c_assoc", "assoc_t",
				EDX("%function_80000005", 23, 16));
		FIELD("l1_i_c_lines_per_tag", "int_t",
				EDX("%function_80000005", 15, 8));
		FIELD("l1_i_c_line_size", ANON_INT("B"),
				EDX("%function_80000005", 7, 0));
		CLOSE_SCOPE(); // l1_cache_tlb_identifiers
	} // CPUID Function 0x80000005

	// L2/L3 Cache and L2 TLB Identifiers
	// CPUID Function 0x80000006
	if (FIELD_GE("largest_ext_fn", 0x80000006)) {
		OPEN_SCOPE("l2_l3_cache_l2_tlb_identifiers");
		REG128("%function_80000006", 0x80000006);
		ENUM("assoc_t",
			KV("disabled", 0x0),
			KV("direct_mapped", 0x1),
			KV("2_way_assoc", 0x2),
			KV("4_way_assoc", 0x4),
			KV("8_way_assoc", 0x6),
			KV("16_way_assoc", 0x8),
			KV("32_way_assoc", 0xA),
			KV("48_way_assoc", 0xB),
			KV("64_way_assoc", 0xC),
			KV("96_way_assoc", 0xD),
			KV("128_way_assoc", 0xE),
			KV("fully_assoc", 0xF));
		INT("entries_t", "entries");
		if (FIELD_EQ("../vendor", "amd")) {
			FIELD("l2_d_tlb_2_and_4_m_assoc", "assoc_t",
					EAX("%function_80000006", 31, 28));
			FIELD("l2_d_tlb_2_and_4_m_size", "entries_t",
					EAX("%function_80000006", 27, 16));
			FIELD("l2_i_tlb_2_and_4_m_assoc", "assoc_t",
					EAX("%function_80000006", 15, 12));
			FIELD("l2_i_tlb_2_and_4_m_size", "entries_t",
					EAX("%function_80000006", 11, 0));
			FIELD("l2_d_tlb_4k_assoc", "assoc_t",
					EBX("%function_80000006", 31, 28));
			FIELD("l2_d_tlb_4k_size", "entries_t",
					EBX("%function_80000006", 27, 16));
			FIELD("l2_i_tlb_4k_assoc", "assoc_t",
					EBX("%function_80000006", 15, 12));
			FIELD("l2_i_tlb_4k_size", "entries_t",
					EBX("%function_80000006", 11, 0));
			FIELD("l2_lines_per_tag", "int_t",
					ECX("%function_80000006", 11, 8));
			FIELD("l3_size_min", ANON_XFORM(ANON_INT("KB"),
						LAMBDA(_1 * 512),
						LAMBDA(_1)),
					EDX("%function_80000006", 31, 18));
			FIELD("l3_size_max", ANON_XFORM(ANON_INT("KB"),
						LAMBDA((_1 + 1) * 512),
						LAMBDA(_1)),
					EDX("%function_80000006", 31, 18));
			FIELD("l3_assoc", "assoc_t",
					EDX("%function_80000006", 15, 12));
			FIELD("l3_lines_per_tag", "int_t",
					EDX("%function_80000006", 11, 8));
			FIELD("l3_line_size", ANON_INT("B"),
					EDX("%function_80000006", 7, 0));
		} // End non-Intel section
		FIELD("l2_size", ANON_INT("KB"),
				ECX("%function_80000006", 31, 16));
		FIELD("l2_assoc", "assoc_t",
				ECX("%function_80000006", 15, 12));
		FIELD("l2_line_size", ANON_INT("B"),
				ECX("%function_80000006", 7, 0));
		CLOSE_SCOPE(); // l2_l3_cache_l2_tlb_identifiers
	} // CPUID Function 0x80000006

	// Advanced Power Management Information
	// CPUID Function 0x80000007
	if (FIELD_EQ("vendor", "amd") &&
	    FIELD_GE("largest_ext_fn", 0x80000007)) {
		OPEN_SCOPE("adv_power_mgmt_info");
		REG128("%function_80000007", 0x80000007);
		FIELD("ts", "yesno_t", EDX("%function_80000007", 0));
		FIELD("fid", "yesno_t", EDX("%function_80000007", 1));
		FIELD("vid", "yesno_t", EDX("%function_80000007", 2));
		FIELD("ttp", "yesno_t", EDX("%function_80000007", 3));
		FIELD("tm", "yesno_t", EDX("%function_80000007", 4));
		FIELD("stc", "yesno_t", EDX("%function_80000007", 5));
		FIELD("_100_mhz_steps", "yesno_t",
				EDX("%function_80000007", 6));
		FIELD("hw_p_state", "yesno_t", EDX("%function_80000007", 7));
		FIELD("tsc_invariant", "yesno_t", EDX("%function_80000007", 8));
		CLOSE_SCOPE(); // adv_power_mgmt_info
	} // CPUID Function 0x80000007

	// Address Size and Physical Core Count Information
	// CPUID Function 0x80000008
	if (FIELD_GE("largest_ext_fn", 0x80000008)) {
		OPEN_SCOPE("addr_size_core_count_info");
		REG128("%function_80000008", 0x80000008);
		INT("bits_t", "b");
		if (FIELD_EQ("../vendor", "amd")) {
			FIELD("guest_phys_addr_size", "bits_t",
					EAX("%function_80000008", 23, 16));
			FIELD("apic_id_core_id_size", "int_t",
					EAX("%function_80000008", 15, 12));
			FIELD("nc", "int_t",
					EAX("%function_80000008", 7, 0));
			if (FIELD_EQ("apic_id_core_id_size", pp_value(0))) {
				FIELD("mnc", "int_t",
					READ("nc") + 1);
			} else {
				FIELD("mnc", "int_t",
					1 << READ("apic_id_core_id_size"));
			}
		}
		FIELD("lin_addr_size", "bits_t",
				EAX("%function_80000008", 15, 8));
		FIELD("phys_addr_size", "bits_t",
				EAX("%function_80000008", 7, 0));
		CLOSE_SCOPE(); // addr_size_core_count_info
	}

	// Function 0x80000009 reserved on Intel and AMD

	// SVM Revision and Feature Identification
	// CPUID Function 0x8000000A
	if (FIELD_GE("largest_ext_fn", 0x8000000A)) {
		OPEN_SCOPE("svn_rev_feature_ident");
		REG128("%function_8000000A", 0x8000000A);
		FIELD("svm_rev", "int_t", EAX("%function_8000000A", 7, 0));
		FIELD("nasid", "int_t", EBX("%function_8000000A"));
		FIELD("ssse3_sse5_dis", "yesno_t",
				EDX("%function_8000000A", 9));
		FIELD("nrips", "yesno_t", EDX("%function_8000000A", 3));
		FIELD("svml", "yesno_t", EDX("%function_8000000A", 2));
		FIELD("lbr_virt", "yesno_t", EDX("%function_8000000A", 1));
		FIELD("np", "yesno_t", EDX("%function_8000000A", 0));
		CLOSE_SCOPE(); // svn_rev_feature_ident
	} // CPUID Function 0x8000000A

	// Functions 0x8000000B through 0x80000018 reserved on Intel and AMD

	// TLB 1GB Page Identifiers
	// CPUID Function 0x80000019
	if (FIELD_GE("largest_ext_fn", 0x80000019)) {
		OPEN_SCOPE("tlb_1gb_page_identifiers");
		REG128("%function_80000019", 0x80000019);
		ENUM("assoc_t",
			KV("disabled", 0x0),
			KV("direct_mapped", 0x1),
			KV("2_way_assoc", 0x2),
			KV("4_way_assoc", 0x4),
			KV("8_way_assoc", 0x6),
			KV("16_way_assoc", 0x8),
			KV("32_way_assoc", 0xA),
			KV("48_way_assoc", 0xB),
			KV("64_way_assoc", 0xC),
			KV("96_way_assoc", 0xD),
			KV("128_way_assoc", 0xE),
			KV("fully_assoc", 0xF));
		INT("entries_t", "entries");
		FIELD("l1_d_tlb_1g_assoc", "assoc_t",
				EAX("%function_80000019", 31, 28));
		FIELD("l1_d_tlb_1g_size", "entries_t",
				EAX("%function_80000019", 27, 16));
		FIELD("l1_i_tlb_1g_assoc", "assoc_t",
				EAX("%function_80000019", 15, 12));
		FIELD("l1_i_tlb_1g_size", "entries_t",
				EAX("%function_80000019", 11, 0));
		FIELD("l2_d_tlb_1g_assoc", "assoc_t",
				EBX("%function_80000019", 31, 28));
		FIELD("l2_d_tlb_1g_size", "entries_t",
				EBX("%function_80000019", 27, 16));
		FIELD("l2_i_tlb_1g_assoc", "assoc_t",
				EBX("%function_80000019", 15, 12));
		FIELD("l2_i_tlb_1g_size", "entries_t",
				EBX("%function_80000019", 11, 0));
		CLOSE_SCOPE(); // tlb_1gb_page_identifiers
	} // CPUID Function 0x80000019

	// Performance Optimization Identifiers
	// CPUID Function 0x8000001A
	if (FIELD_GE("largest_ext_fn", 0x8000001A)) {
		OPEN_SCOPE("perf_opt_identifiers");
		REG128("%function_8000001A", 0x8000001A);
		FIELD("movu", "yesno_t", EAX("%function_8000001A", 1));
		FIELD("fp128", "yesno_t", EAX("%function_8000001A", 0));
		CLOSE_SCOPE(); // perf_opt_identifiers
	} // CPUID Function 0x8000001A
}
