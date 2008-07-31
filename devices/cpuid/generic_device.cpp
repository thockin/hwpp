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
			ECX("%function_0", 31, 0) +
			EDX("%function_0", 31, 0) +
			EBX("%function_0", 31, 0));

	// Feature Information
	// CPUID Function 0x1
	if (FIELD_GE("largest_std_fn", 1)) {
		REG128("%function_1", 0x1);
		FIELD("family", "int_t", PROCS(cpuid_family_procs));
		FIELD("model", "int_t", PROCS(cpuid_model_procs));
		FIELD("stepping", "int_t", EAX("%function_1", 3, 0));

		if (FIELD_EQ("vendor", "intel")) {
			FIELD("type", ANON_ENUM(
					KV("oem", 0),
					KV("overdrive", 1),
					KV("dual", 2)),
				EAX("%function_1", 13, 12));
		}

		if (FIELD_EQ("vendor", "amd")) {
			FIELD("std_features", "cpuid_features_t",
					ECX("%function_1", 31, 0)
					+ EDX("%function_1", 31, 0));

			FIELD("brand_id_8bit", "int_t",
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
		}
	} // CPUID Function 0x1

	// Cache Descriptors
	// CPUID Function 0x2
	// I was only able to find documentation of this function from
	// Intel, so adding a vendor check until docs from another vendor
	// can be found.
	if (FIELD_GE("largest_std_fn", 0x2) &&
	    FIELD_EQ("vendor", "intel")) {
		uint8_t queries_needed = 1; // updated on first call
		OPEN_SCOPE("cache_descriptors");

		// check for special Xeon case
		string cache_descriptor_t;
		if (FIELD_EQ("../family", 0x0F) &&
		    FIELD_EQ("../model", 0x06)) {
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
			CLOSE_SCOPE();
		}
	} // CPUID Function 0x2
	
	// Processor Serial Number
	// CPUID Function 0x3 deprecated
	// Probably not worth implementing, only works on Pentium III
	
	// Deterministic Cache Parameters
	// CPUID Function 0x4
	// I was only able to find documentation of this function from
	// Intel, so adding a vendor check until docs from another vendor
	// can be found.
	if (FIELD_GE("largest_std_fn", 0x4) &&
	    FIELD_EQ("vendor", "intel")) {
		for (int i = 0; true; i++) {
			pp_value addr = (pp_value(i) << 32) | pp_value(0x4);
			// read the next 'type' field to see if it is valid
			if (READ(BITS(REG128(addr), 4, 0)) == 0) {
				break;
			}
			OPEN_SCOPE("cache_info[]");
			REG128("%function_4[]", addr);
			FIELD("type", ANON_ENUM(KV("null", 0),
						KV("data_cache", 1),
						KV("instr_cache", 2),
						KV("unified_cache", 3)),
					EAX("%function_4[$]", 4, 0));
			FIELD("max_cores_per_pkg", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				EAX("%function_4[$]", 31, 26));
			FIELD("max_threads_sharing_cache", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				EAX("%function_4[$]", 25, 14));
			FIELD("fully_assoc", "yesno_t",
			      EAX("%function_4[$]", 9));
			FIELD("self_init", "yesno_t",
			      EAX("%function_4[$]", 8));
			FIELD("level", "int_t",
			      EAX("%function_4[$]", 7, 5));
			FIELD("ways_of_assoc", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				EBX("%function_4[$]", 31, 22));
			FIELD("phys_line_partitions", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				EBX("%function_4[$]", 21, 12));
			FIELD("sys_coherency_line_size", "int_t",
			      EBX("%function_4[$]", 11, 0));
			FIELD("num_sets", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				ECX("%function_4[$]", 31, 0));
			FIELD("prefetch_stride", /*ANON_XFORM( */
					ANON_INT("bytes"),
				// FIXME: why doesn't this work?
				// this causes huge amounts of nonsense
				// numbers to be printed as output
				/*LAMBDA(if_then_else_return(_1 == 0, 64, _1)),
				LAMBDA(if_then_else_return(_1 == 64, 0, _1))),*/
				EDX("%function_4[$]", 9, 0));
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
		if (FIELD_NE("../vendor", "amd")) {
			// reserved on AMD
			for (int i = 0; i < 20; i += 4) {
				FIELD("sub_states_supported[]", "int_t",
						EDX("%function_5", i + 3, i));
			}
		}
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
	
	// FIXME: add function 0x9 for Intel

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

	// "Largest Extended Function #"
	// CPUID Extended Function 0x0
	REG128("%function_80000000", 0x80000000);
	FIELD("largest_ext_fn", "hex32_t", EAX("%function_80000000", 31, 0));

	// "Processor Name / Brand String"
	// CPUID Extended Function 0x2-0x4
	REG128("%function_80000002", 0x80000002);
	REG128("%function_80000003", 0x80000003);
	REG128("%function_80000004", 0x80000004);
	FIELD("processor_name_str", ANON_STRING(),
			BITS("%function_80000004", 127, 0) +
			BITS("%function_80000003", 127, 0) +
			BITS("%function_80000002", 127, 0));

	// FIXME: add extended functions
}
