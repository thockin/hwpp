//
// This file has been developed purely from publicly-available documentation
// acquired from the following sources:
// http://download.intel.com/design/processor/applnots/24161832.pdf
// http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
// http://www.sandpile.org/ia32/cpuid.htm
//

import pp;

public type cpuid_vendor_t = ENUM(
	[0x6c65746e49656e69756e6547, "intel"],
	[0x444d416369746e6568747541, "amd"],
	[0x20434d5520434d5520434d55, "umc"],
	[0x64616574736e497869727943, "cyrix"],
	[0x6e65766972446e654778654e, "nexgen"],
	[0x736c756148727561746e6543, "centaur"],
	[0x657369526573695265736952, "rise"],
	[0x205369532053695320536953, "sis"],
	[0x3638784d54656e69756e6547, "transmeta"],
	[0x43534e2079622065646f6547, "nsc"]);

public type cpuid_features_t = BITMASK(
	[0, "fpu"],
	[1, "vme"],
	[2, "de"],
	[3, "pse"],
	[4, "tsc"],
	[5, "msr"],
	[6, "pae"],
	[7, "mce"],
	[8, "cx8"],
	[9, "apic"],
	[11, "sysenter"],
	[12, "mtrr"],
	[13, "pge"],
	[14, "mca"],
	[15, "cmov"],
	[16, "pat"],
	[17, "pse36"],
	[18, "psn"],
	[19, "clflsh"],
	[21, "ds"],
	[22, "acpi"],
	[23, "mmx"],
	[24, "fxsr"],
	[25, "sse"],
	[26, "sse2"],
	[27, "ss"],
	[28, "htt"],
	[29, "tm"],
	[30, "ia64"],
	[31, "pbe"],
	[32+0, "sse3"],
	[32+2, "dtes64"],
	[32+3, "monitor"],
	[32+4, "ds-cpl"],
	[32+5, "vmx"],
	[32+6, "smx"],
	[32+7, "est"],
	[32+8, "tm2"],
	[32+9, "ssse3"],
	[32+10, "cnxt-id"],
	[32+13, "cx16"],
	[32+14, "xtpr"],
	[32+15, "pdcm"],
	[32+18, "dca"],
	[32+19, "sse4_1"],
	[32+20, "sse4_2"]);

public type cpuid_ext_features_t = BITMASK(
	[0, "fpu"],
	[1, "vme"],
	[2, "de"],
	[3, "pse"],
	[4, "tsc"],
	[5, "msr"],
	[6, "pae"],
	[7, "mce"],
	[8, "cx8"],
	[9, "apic"],
	[11, "syscall"],
	[12, "mtrr"],
	[13, "pge"],
	[14, "mca"],
	[15, "cmov"],
	[16, "pat"],
	[17, "pse36"],
	[20, "nx"],
	[22, "mmx_ext"],
	[23, "mmx"],
	[24, "fxsr"],
	[25, "ffxsr"],
	[26, "page_1gb"],
	[27, "rdtscp"],
	[29, "64bit"],
	[30, "3dnow_ext"],
	[31, "3dnow"],
	[32+0, "lahf_sahf"],
	[32+1, "cmp_legacy"],
	[32+2, "svm"],
	[32+3, "ext_apic_space"],
	[32+4, "alt_mov_cr_8"],
	[32+5, "abm"],
	[32+6, "sse4a"],
	[32+7, "misalign_sse"],
	[32+8, "3dnow_prefetch"],
	[32+9, "osvw"],
	[32+10, "ibs"],
	[32+11, "sse5"],
	[32+12, "skinit"],
	[32+13, "wdt"]);

public type cache_descriptor_t = ENUM(
	[0x00, "null"],
	[0x01, "instr_tlb_4KB_pages_4way_assoc_32_entries"],
	[0x02, "instr_tlb_4MB_pages_fully_assoc_2_entries"],
	[0x03, "data_tlb_4KB_pages_4way_assoc_64_entries"],
	[0x04, "data_tlb_4MB_pages_4way_assoc_8_entries"],
	[0x05, "data_tlb_4MB_pages_4way_assoc_32_entries"],
	[0x06, "level1_instr_cache_8KB_4way_assoc_32B_line"],
	[0x08, "level1_instr_cache_16KB_4way_assoc_32B_line"],
	[0x0A, "level1_data_cache_8KB_2way_assoc_32B_line"],
	[0x0B, "instr_tlb_4MB_pages_4way_assoc_4_entries"],
	[0x0C, "level1_data_cache_16KB_4way_assoc_32B_line"],
	[0x0E, "level1_data_cache_24KB_6way_assoc_64B_line"],
	[0x10, "level1_data_cache_16KB_4way_assoc_32B_line_ia64"],
	[0x15, "level1_instr_cache_16KB_4way_assoc_32B_line_ia64"],
	[0x1A, "level2_cache_96KB_6way_assoc_64B_line_ia64"],
	[0x22, "level3_cache_512KB_4way_assoc_sectored_64B_line"],
	[0x23, "level3_cache_1MB_8way_assoc_sectored_64B_line"],
	[0x25, "level3_cache_2MB_8way_assoc_sectored_64B_line"],
	[0x29, "level3_cache_4MB_8way_assoc_sectored_64B_line"],
	[0x2C, "level1_data_cache_32KB_8way_assoc_64B_line"],
	[0x30, "level1_instr_cache_32KB_8way_assoc_64B_line"],
	[0x39, "level2_cache_128KB_4way_assoc_sectored_64B_line"],
	[0x3A, "level2_cache_192KB_6way_assoc_sectored_64B_line"],
	[0x3B, "level2_cache_128KB_2way_assoc_sectored_64B_line"],
	[0x3C, "level2_cache_256KB_4way_assoc_sectored_64B_line"],
	[0x3D, "level2_cache_384KB_6way_assoc_sectored_64B_line"],
	[0x3E, "level2_cache_512KB_4way_assoc_sectored_64B_line"],
	[0x40, "either_no_level2_or_no_level3_cache"],
	[0x41, "level2_cache_128KB_4way_assoc_32B_line"],
	[0x42, "level2_cache_256KB_4way_assoc_32B_line"],
	[0x43, "level2_cache_512KB_4way_assoc_32B_line"],
	[0x44, "level2_cache_1MB_4way_assoc_32B_line"],
	[0x45, "level2_cache_2MB_4way_assoc_32B_line"],
	[0x46, "level3_cache_4MB_4way_assoc_64B_line"],
	[0x47, "level3_cache_8MB_8way_assoc_64B_line"],
	[0x48, "level2_cache_3MB_12way_assoc_64B_line"],
	// different on Xeon MP, Family 0Fh, Model 06h
	[0x49, "level2_cache_4MB_16way_assoc_64B_line"],
	[0x4A, "level3_cache_6MB_12way_assoc_64B_line"],
	[0x4B, "level3_cache_8MB_16way_assoc_64B_line"],
	[0x4C, "level3_cache_12MB_12way_assoc_64B_line"],
	[0x4D, "level3_cache_16MB_16way_assoc_64B_line"],
	[0x4E, "level2_cache_6MB_24way_assoc_64B_line"],
	[0x4F, "instr_tlb_4KB_pages_32_entries"],
	[0x50, "instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_64_entries"],
	[0x51, "instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_128_entries"],
	[0x52, "instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_256_entries"],
	[0x56, "level1_data_tlb0_4MB_pages_4way_assoc_16_entries"],
	[0x57, "level1_data_tlb0_4KB_pages_4way_assoc_16_entries"],
	[0x59, "level1_data_tlb0_4KB_pages_fully_assoc_16_entries"],
	[0x5B, "data_tlb_4KB_or_4MB_pages_fully_assoc_64_entries"],
	[0x5C, "data_tlb_4KB_or_4MB_pages_fully_assoc_128_entries"],
	[0x5D, "data_tlb_4KB_or_4MB_pages_fully_assoc_256_entries"],
	[0x60, "level1_data_cache_16KB_8way_assoc_sectored_64B_line"],
	[0x66, "level1_data_cache_8KB_4way_assoc_sectored_64B_line"],
	[0x67, "level1_data_cache_16KB_4way_assoc_sectored_64B_line"],
	[0x68, "level1_data_cache_32KB_4way_assoc_sectored_64B_line"],
	[0x70, "trace_cache_12K_uops_8way_assoc"],
	[0x71, "trace_cache_16K_uops_8way_assoc"],
	[0x72, "trace_cache_32K_uops_8way_assoc"],
	[0x73, "trace_cache_64K_uops_8way_assoc"],
	[0x77, "level1_instr_cache_16KB_4way_assoc_64B_line_sectored_ia64"],
	[0x78, "level2_cache_1MB_4way_assoc_64B_line"],
	[0x79, "level2_cache_128KB_8way_assoc_sectored_64B_line"],
	[0x7A, "level2_cache_256KB_8way_assoc_sectored_64B_line"],
	[0x7B, "level2_cache_512KB_8way_assoc_sectored_64B_line"],
	[0x7C, "level2_cache_1MB_8way_assoc_sectored_64B_line"],
	[0x7D, "level2_cache_2MB_8way_assoc_sectored_64B_line"],
	[0x7E, "level2_cache_256KB_8way_assoc_128B_line_sectored_ia64"],
	[0x7F, "level2_cache_512KB_2way_assoc_64B_line"],
	[0x80, "level2_cache_512KB_8way_assoc_64B_line"],
	[0x81, "level2_cache_128KB_8way_assoc_32B_line"],
	[0x82, "level2_cache_256KB_8way_assoc_32B_line"],
	[0x83, "level2_cache_512KB_8way_assoc_32B_line"],
	[0x84, "level2_cache_1MB_8way_assoc_32B_line"],
	[0x85, "level2_cache_2MB_8way_assoc_32B_line"],
	[0x86, "level2_cache_512KB_4way_assoc_64B_line"],
	[0x87, "level2_cache_1MB_8way_assoc_64B_line"],
	[0x88, "level3_cache_2MB_4way_assoc_64B_line_ia64"],
	[0x89, "level3_cache_4MB_4way_assoc_64B_line_ia64"],
	[0x8A, "level3_cache_8MB_4way_assoc_64B_line_ia64"],
	[0x8D, "level3_cache_3MB_12way_assoc_128B_line_ia64"],
	[0x90, "instr_tlb_4KB_to_256MB_pages_fully_assoc_64_entries_ia64"],
	[0x96, "data_tlb1_4KB_to_256MB_pages_fully_assoc_32_entries_ia64"],
	[0x9B, "data_tlb2_4KB_to_256MB_pages_fully_assoc_96_entries_ia64"],
	[0xB0, "instr_tlb_4KB_pages_4way_assoc_128_entries"],
	[0xB1, "instr_tlb_4MB_or_2MB_pages_4way_assoc_4_or_8_entries"],
	[0xB3, "data_tlb_4KB_pages_4way_assoc_128_entries"],
	[0xB4, "data_tlb1_4KB_pages_4way_assoc_256_entries"],
	[0xBA, "data_tlb1_4KB_pages_4way_assoc_64_entries"],
	[0xC0, "data_tlb_4KB_4MB_pages_4way_assoc_8_entries"],
	[0xF0, "64B_prefetching"],
	[0xF1, "128B_prefetching"]);

// FIXM[E]: is there a better way to deal with the one single differing
// KV pair on Xeon than an entirely different enum?
public type cache_descriptor_xeon_t = ENUM(
	[0x00, "null"],
	[0x01, "instr_tlb_4KB_pages_4way_assoc_32_entries"],
	[0x02, "instr_tlb_4MB_pages_fully_assoc_2_entries"],
	[0x03, "data_tlb_4KB_pages_4way_assoc_64_entries"],
	[0x04, "data_tlb_4MB_pages_4way_assoc_8_entries"],
	[0x05, "data_tlb_4MB_pages_4way_assoc_32_entries"],
	[0x06, "level1_instr_cache_8KB_4way_assoc_32B_line"],
	[0x08, "level1_instr_cache_16KB_4way_assoc_32B_line"],
	[0x0A, "level1_data_cache_8KB_2way_assoc_32B_line"],
	[0x0B, "instr_tlb_4MB_pages_4way_assoc_4_entries"],
	[0x0C, "level1_data_cache_16KB_4way_assoc_32B_line"],
	[0x0E, "level1_data_cache_24KB_6way_assoc_64B_line"],
	[0x22, "level3_cache_512KB_4way_assoc_sectored_64B_line"],
	[0x23, "level3_cache_1MB_8way_assoc_sectored_64B_line"],
	[0x25, "level3_cache_2MB_8way_assoc_sectored_64B_line"],
	[0x29, "level3_cache_4MB_8way_assoc_sectored_64B_line"],
	[0x2C, "level1_data_cache_32KB_8way_assoc_64B_line"],
	[0x30, "level1_instr_cache_32KB_8way_assoc_64B_line"],
	[0x39, "level2_cache_128KB_4way_assoc_sectored_64B_line"],
	[0x3A, "level2_cache_192KB_6way_assoc_sectored_64B_line"],
	[0x3B, "level2_cache_128KB_2way_assoc_sectored_64B_line"],
	[0x3C, "level2_cache_256KB_4way_assoc_sectored_64B_line"],
	[0x3D, "level2_cache_384KB_6way_assoc_sectored_64B_line"],
	[0x3E, "level2_cache_512KB_4way_assoc_sectored_64B_line"],
	[0x40, "either_no_level2_or_no_level3_cache"],
	[0x41, "level2_cache_128KB_4way_assoc_32B_line"],
	[0x42, "level2_cache_256KB_4way_assoc_32B_line"],
	[0x43, "level2_cache_512KB_4way_assoc_32B_line"],
	[0x44, "level2_cache_1MB_4way_assoc_32B_line"],
	[0x45, "level2_cache_2MB_4way_assoc_32B_line"],
	[0x46, "level3_cache_4MB_4way_assoc_64B_line"],
	[0x47, "level3_cache_8MB_8way_assoc_64B_line"],
	[0x48, "level2_cache_3MB_12way_assoc_64B_line"],
	// special case for Xeon MP, Family 0Fh, Model 06h
	[0x49, "level3_cache_4MB_16way_assoc_64B_line"],
	[0x4A, "level3_cache_6MB_12way_assoc_64B_line"],
	[0x4B, "level3_cache_8MB_16way_assoc_64B_line"],
	[0x4C, "level3_cache_12MB_12way_assoc_64B_line"],
	[0x4D, "level3_cache_16MB_16way_assoc_64B_line"],
	[0x4E, "level2_cache_6MB_24way_assoc_64B_line"],
	[0x4F, "instr_tlb_4KB_pages_32_entries"],
	[0x50, "instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_64_entries"],
	[0x51, "instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_128_entries"],
	[0x52, "instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_256_entries"],
	[0x56, "level1_data_tlb0_4MB_pages_4way_assoc_16_entries"],
	[0x57, "level1_data_tlb0_4KB_pages_4way_assoc_16_entries"],
	[0x59, "level1_data_tlb0_4KB_pages_fully_assoc_16_entries"],
	[0x5B, "data_tlb_4KB_or_4MB_pages_fully_assoc_64_entries"],
	[0x5C, "data_tlb_4KB_or_4MB_pages_fully_assoc_128_entries"],
	[0x5D, "data_tlb_4KB_or_4MB_pages_fully_assoc_256_entries"],
	[0x60, "level1_data_cache_16KB_8way_assoc_sectored_64B_line"],
	[0x66, "level1_data_cache_8KB_4way_assoc_sectored_64B_line"],
	[0x67, "level1_data_cache_16KB_4way_assoc_sectored_64B_line"],
	[0x68, "level1_data_cache_32KB_4way_assoc_sectored_64B_line"],
	[0x70, "trace_cache_12K_uops_8way_assoc"],
	[0x71, "trace_cache_16K_uops_8way_assoc"],
	[0x72, "trace_cache_32K_uops_8way_assoc"],
	[0x73, "trace_cache_64K_uops_8way_assoc"],
	[0x78, "level2_cache_1MB_4way_assoc_64B_line"],
	[0x79, "level2_cache_128KB_8way_assoc_sectored_64B_line"],
	[0x7A, "level2_cache_256KB_8way_assoc_sectored_64B_line"],
	[0x7B, "level2_cache_512KB_8way_assoc_sectored_64B_line"],
	[0x7C, "level2_cache_1MB_8way_assoc_sectored_64B_line"],
	[0x7D, "level2_cache_2MB_8way_assoc_sectored_64B_line"],
	[0x7F, "level2_cache_512KB_2way_assoc_64B_line"],
	[0x80, "level2_cache_512KB_8way_assoc_64B_line"],
	[0x82, "level2_cache_256KB_8way_assoc_32B_line"],
	[0x83, "level2_cache_512KB_8way_assoc_32B_line"],
	[0x84, "level2_cache_1MB_8way_assoc_32B_line"],
	[0x85, "level2_cache_2MB_8way_assoc_32B_line"],
	[0x86, "level2_cache_512KB_4way_assoc_64B_line"],
	[0x87, "level2_cache_1MB_8way_assoc_64B_line"],
	[0xB0, "instr_tlb_4KB_pages_4way_assoc_128_entries"],
	[0xB1, "instr_tlb_4MB_or_2MB_pages_4way_assoc_4_or_8_entries"],
	[0xB3, "data_tlb_4KB_pages_4way_assoc_128_entries"],
	[0xB4, "data_tlb1_4KB_pages_4way_assoc_256_entries"],
	[0xBA, "data_tlb1_4KB_pages_4way_assoc_64_entries"],
	[0xC0, "data_tlb_4KB_4MB_pages_4way_assoc_8_entries"],
	[0xF0, "64B_prefetching"],
	[0xF1, "128B_prefetching"]);

//FIXM[E]: how do I do this in the language?  Maybe a 'bits' type?
// Functions for easy access to individual registers returned by cpuid
EAX(string name, int hi, int lo)
{
	return BITS(name, hi, lo);
}
EAX(string name, int bit)
{
	return BITS(name, bit);
}
EAX(string name)
{
	return BITS(name, 31, 0);
}
EBX(string name, int hi, int lo)
{
	return BITS(name, hi + 32, lo + 32);
}
EBX(string name, int bit)
{
	return BITS(name, bit + 32);
}
EBX(string name)
{
	return BITS(name, 63, 32);
}
ECX(string name, int hi, int lo)
{
	return BITS(name, hi + 64, lo + 64);
}
ECX(string name, int bit)
{
	return BITS(name, bit + 64);
}
ECX(string name)
{
	return BITS(name, 95, 64);
}
EDX(string name, int hi, int lo)
{
	return BITS(name, hi + 96, lo + 96);
}
EDX(string name, int bit)
{
	return BITS(name, bit + 96);
}
EDX(string name)
{
	return BITS(name, 127, 96);
}

cpuid_generic_device()
{
	BOOKMARK("cpuid");

	REG128("%function_0", 0x0);
	FIELD("largest_std_fn", hex_t, EAX("%function_0", 31, 0));
	FIELD("vendor", cpuid_vendor_t,
			ECX("%function_0") +
			EDX("%function_0") +
			EBX("%function_0"));
	FIELD("vendor_str", string_t,
			ECX("%function_0") +
			EDX("%function_0") +
			EBX("%function_0"));

	// Feature Information
	// CPUID Function 0x1
	if (EVAL("largest_std_fn") >= 1) {
		REG128("%function_1", 0x1);
		FIELD("family", int_t, {
				if (EVAL("vendor") == "intel") {
					int family = READ(EAX("%function_1", 11, 8))
							   + READ(EAX("%function_1", 27, 20));
					return family;
				} else if (EVAL("vendor") == "amd") {
					int base_family = READ(EAX("%function_1", 11, 8));
					if (base_family == 0xf) {
						return READ(EAX("%function_1", 27, 20)) + 0xf;
					}
					return base_family;
				}
				WARN("unknown CPUID vendor, using base family");
				return READ(EAX("%function_1", 11, 8));
			}, {
				//NOT[E]: if the 2nd fn = undef, it's an error to call it
				// Not supported.
				WARN("tried to write %d to CPUID fiel[d]: family", args[0]);
			});

		FIELD("model", int_t, {
				if (EVAL("vendor") == "intel") {
					return READ(EAX("%function_1", 19, 16)
					          + EAX("%function_1", 7, 4));
				} else if (EVAL("vendor") == "amd") {
					int base_family = READ(EAX("%function_1", 11, 8));
					if (base_family == 0xf) {
						return READ(EAX("%function_1", 19, 16)
						          + EAX("%function_1", 7, 4));
					}
					return READ(EAX("%function_1", 7, 4));
				}
				WARN("unknown CPUID vendor, using base_model");
				return READ(EAX("%function_1", 7, 4));
			}, {
				// Not supported.
				WARN("tried to write %d to CPUID fiel[d]: model", args[0]);
			});

		FIELD("stepping", int_t, EAX("%function_1", 3, 0));

		FIELD("type", ENUM(
				[0, "oem"],
				[1, "overdrive"],
				[2, "dual"]),
			EAX("%function_1", 13, 12));

		FIELD("std_features", cpuid_features_t,
				ECX("%function_1", 31, 0)
				+ EDX("%function_1", 31, 0));

		FIELD("brand_id", int_t,
				EBX("%function_1", 7, 0));
		FIELD("clflush_size", INT("QWORDs"),
				EBX("%function_1", 15, 8));
		//FIXM[E]: this was FIELD_TEST - should it be & or =~ 
		if (EVAL("std_features") & "htt") {
			FIELD("logical_proc_count", int_t,
					EBX("%function_1", 23, 16));
		} else {
			FIELD("logical_proc_count", int_t, 1);
		}
		FIELD("init_lapic_id", int_t,
				EBX("%function_1", 31, 24));
	} // CPUID Function 0x1

	// Cache Descriptors
	// CPUID Function 0x2
	if (EVAL("largest_std_fn") >= 0x2) {
		int queries_needed = 1; // updated on first call
		SCOPE("cache_descriptors", {
			// check for special Xeon case
			type my_cache_descriptor_t;
			if (EVAL("../vendor") == "intel"
			 && EVAL("../family") == 0x0F
			 && EVAL("../model") == 0x06) {
				my_cache_descriptor_t = cache_descriptor_xeon_t;
			} else {
				my_cache_descriptor_t = cache_descriptor_t;
			}

			for (int i = 0; i < queries_needed; i++) {
				REG128("%function_2[]", 0x2);

				if (i == 0) {
					FIELD("queries_needed", int_t,
							EAX("%function_2[-1]", 7, 0));
					queries_needed = READ("queries_needed");
				}

				if (READ(EAX("%function_2[-1]", 31)) == 0) {
					for (int i = 31; i > 7; i -= 8) {
						if (READ(EAX("%function_2[-1]",
						    i, i-7)) != 0) {
							FIELD("descriptor[]",
							      my_cache_descriptor_t,
							      EAX("%function_2[-1]",
								  i, i - 7));
						}
					}
				}
				if (READ(EBX("%function_2[-1]", 31)) == 0) {
					for (int i = 31; i > 0; i -= 8) {
						if (READ(EBX("%function_2[-1]",
						    i, i-7)) != 0) {
							FIELD("descriptor[]",
							      my_cache_descriptor_t,
							      EBX("%function_2[-1]",
								  i, i - 7));
						}
					}
				}
				if (READ(ECX("%function_2[-1]", 31)) == 0) {
					for (int i = 31; i > 0; i -= 8) {
						if (READ(ECX("%function_2[-1]",
						    i, i-7)) != 0) {
							FIELD("descriptor[]",
							      my_cache_descriptor_t,
							      ECX("%function_2[-1]",
								  i, i - 7));
						}
					}
				}
				if (READ(EDX("%function_2[-1]", 31)) == 0) {
					for (int i = 31; i > 0; i -= 8) {
						if (READ(EDX("%function_2[-1]",
						    i, i-7)) != 0) {
							FIELD("descriptor[]",
							      my_cache_descriptor_t,
							      EDX("%function_2[-1]",
								  i, i - 7));
						}
					}
				}
			}
		});
	} // CPUID Function 0x2

	// Processor Serial Number
	// CPUID Function 0x3 deprecated
	// Probably not worth implementing, only works on Pentium III

	// Deterministic Cache Parameters
	// CPUID Function 0x4
	if (EVAL("largest_std_fn") >= 0x4) {
		for (int i = 0; true; i++) {
			int addr = (i << 32) | 0x4;
			// read the next 'type' field to see if it is valid
			if (READ(BITS(REG128(addr), 4, 0)) == 0) {
				break;
			}
			SCOPE("cache_info[]", {
				REG128("%function_4", addr);
				FIELD("type", ENUM(
							[0, "null"],
							[1, "data_cache"],
							[2, "instr_cache"],
							[3, "unified_cache"]),
						EAX("%function_4", 4, 0));
				FIELD("max_cores_per_pkg",
				      XFORM(int_t, { return args[0] + 1; }, {}),
				      EAX("%function_4", 31, 26));
				FIELD("max_threads_sharing_cache",
				      XFORM(int_t, { return args[0] + 1; }, {}),
				      EAX("%function_4", 25, 14));
				FIELD("fully_assoc", yesno_t,
				      EAX("%function_4", 9));
				FIELD("self_init", yesno_t,
				      EAX("%function_4", 8));
				FIELD("level", int_t,
				      EAX("%function_4", 7, 5));
				FIELD("ways_of_assoc",
				      XFORM(int_t, { return args[0] + 1; }, {}),
				      EBX("%function_4", 31, 22));
				FIELD("phys_line_partitions",
				      XFORM(int_t, { return args[0] + 1; }, {}),
				      EBX("%function_4", 21, 12));
				FIELD("sys_coherency_line_size",
				      XFORM(INT("bytes"), { return args[0] + 1; }, {}),
				      EBX("%function_4", 11, 0));
				FIELD("num_sets",
				      XFORM(int_t, { return args[0] + 1; }, {}),
				      ECX("%function_4", 31, 0));
				FIELD("prefetch_stride",
				      XFORM(INT("bytes"), { return (!args[0])*64 + args[0]; }),
				      EDX("%function_4", 9, 0));
			}); // cache_info[]
		}
	} // CPUID Function 0x4

	// MONITOR / MWAIT Parameters
	// CPUID Function 0x5
	if (EVAL("largest_std_fn") >= 0x5) {
		SCOPE("monitor_mwait", {
				REG128("%function_5", 0x5);
				FIELD("mon_line_size_min", INT("bytes"),
						EAX("%function_5", 15, 0));
				FIELD("mon_line_size_max", INT("bytes"),
						EBX("%function_5", 15, 0));
				FIELD("ibe", yesno_t,
						ECX("%function_5", 1));
				FIELD("emx", yesno_t,
						EDX("%function_5", 0));
				FIELD("c0_sub_states", int_t,
						EDX("%function_5", 3, 0));
				FIELD("c1_sub_states", int_t,
						EDX("%function_5", 7, 4));
				FIELD("c2_sub_states", int_t,
						EDX("%function_5", 11, 8));
				FIELD("c3_sub_states", int_t,
						EDX("%function_5", 15, 12));
				FIELD("c4_sub_states", int_t,
						EDX("%function_5", 19, 16));
		});
	} // CPUID Function 0x5

	// Digital Thermal Sensor and Power Management Parameters
	// CPUID Function 0x6
	if (EVAL("largest_std_fn") >= 0x6) {
		SCOPE("therm_power_mgmt", {
				REG128("%function_6", 0x6);
				FIELD("digital_thermal_sensor_cap", yesno_t,
						EAX("%function_6", 0));
				FIELD("num_int_thresholds", int_t, EAX("%function_6", 3, 0));
				FIELD("hw_coord_feedback_cap", yesno_t,
						ECX("%function_6", 0));
		});
	} // CPUID Function 0x6

	// CPUID Functions 0x7 & 0x8 Reserved on both Intel and AMD

	// Direct Cache Access Parameters
	// CPUID Function 0x9
	if (EVAL("largest_std_fn") >= 0x9) {
		SCOPE("direct_cache_access", {
				REG128("%function_9", 0x9);
				FIELD("platform_dca_cap", hex32_t, EAX("%function_9"));
		});
	}

	// Architectural Performance Monitor Features
	// CPUID Function 0xA
	if (EVAL("largest_std_fn") >= 0xA) {
		SCOPE("perf_mon_features", {
				REG128("%function_A", 0xA);
				type not_available_t = BOOL("not available", "available");
				FIELD("ebx_bit_vector_len", int_t,
						EAX("%function_A", 31, 24));
				FIELD("genpurpose_perfmon_counter_width", int_t,
						EAX("%function_A", 23, 16));
				FIELD("genpurpose_perfmon_counter_num", int_t,
						EAX("%function_A", 15, 8));
				FIELD("arch_perfmon_version", int_t,
						EAX("%function_A", 7, 0));
				FIELD("event_branch_mispredict_retired", not_available_t,
						EBX("%function_A", 6));
				FIELD("event_branch_instr_retired", not_available_t,
						EBX("%function_A", 5));
				FIELD("event_last_level_cache_misses", not_available_t,
						EBX("%function_A", 4));
				FIELD("event_last_level_cache_reverence", not_available_t,
						EBX("%function_A", 3));
				FIELD("event_reference_cycles", not_available_t,
						EBX("%function_A", 2));
				FIELD("event_instr_retired", not_available_t,
						EBX("%function_A", 1));
				FIELD("event_core_cycle", not_available_t,
						EBX("%function_A", 0));
				if (EVAL("arch_perfmon_version") > 1) {
					FIELD("fixed_fn_perf_counter_width", int_t,
							EDX("%function_A", 12, 5));
					FIELD("fixed_fn_perf_counter_num" ,int_t,
							EDX("%function_A", 4, 0));
				}
		});
	} // CPUID Function 0xA

	// Topology Enumeration Information
	// CPUID Function 0xB
	if (EVAL("largest_std_fn") >= 0xB) {
		for (int i = 0; true; i++) {
			int addr = (i << 32) | 0xB;
			// check if we have run out of levels
			if (READ(BITS(REG128(addr), 63, 0)) == 0) {
				break;
			}
			SCOPE("topo_info[]", {
					REG128("%function_B", addr);
					FIELD("bits_to_shift_for_next_level_id", int_t,
							EAX("%function_B", 4, 0));
					FIELD("enabled_logical_procs", int_t,
							EBX("%function_B", 15, 0));
					FIELD("level_type", ENUM([1, "smt"],
									  [2, "core"]),
							ECX("%function_B", 15, 8));
					FIELD("level_num", int_t, ECX("%function_B", 7, 0));
					FIELD("x2apic_id", hex32_t, EDX("%function_B"));
			}); // topo_info[]
		}
	} // CPUID Function 0xB

	// Largest Extended Function #
	// CPUID Function 0x80000000
	REG128("%function_80000000", 0x80000000);
	FIELD("largest_ext_fn", hex32_t, EAX("%function_80000000", 31, 0));

	// Extended Feature Bits
	// CPUID Function 0x80000001
	if (EVAL("largest_ext_fn") >= 0x80000001) {
		REG128("%function_80000001", 0x80000001);
		if (EVAL("vendor") == "amd") {
			FIELD("brand_id_16bit", int_t,
					EBX("%function_80000001", 15, 0));
			if (EVAL("family") & 0xFF >= 0x10) {
				FIELD("pkg_type", hex_t,
						EBX("%function_80000001",
						    31, 28));
			}
		}
		FIELD("ext_features", cpuid_ext_features_t,
				ECX("%function_80000001")
				+ EDX("%function_80000001"));
	} // CPUID Function 0x80000001

	// Processor Name / Brand String
	// CPUID Extended Functions 0x80000002-0x80000004
	if (EVAL("largest_ext_fn") >= 0x80000004) {
		REG128("%function_80000002", 0x80000002);
		REG128("%function_80000003", 0x80000003);
		REG128("%function_80000004", 0x80000004);
		FIELD("processor_name_str", string_t,
				BITS("%function_80000004", 127, 0) +
				BITS("%function_80000003", 127, 0) +
				BITS("%function_80000002", 127, 0));
	} // CPUID Functions 0x80000002-0x80000004

	// L1 Cache and TLB Identifiers
	// CPUID Function 0x80000005
	if (EVAL("vendor") == "amd" &&
	    EVAL("largest_ext_fn") >= 0x80000005) {
		SCOPE("l1_cache_tlb_identifiers", {
				REG128("%function_80000005", 0x80000005);
				// FIXM[E]: Is there a better way to do this than a 255-KV enum?
				type assoc_t = ENUM(
					[0x1, "direct_mapped"],
					[0x2, "2_way_assoc"],
					[0x3, "3_way_assoc"],
					[0x4, "4_way_assoc"],
					[0x5, "5_way_assoc"],
					[0x6, "6_way_assoc"],
					[0x7, "7_way_assoc"],
					[0x8, "8_way_assoc"],
					[0x9, "9_way_assoc"],
					[0xa, "10_way_assoc"],
					[0xb, "11_way_assoc"],
					[0xc, "12_way_assoc"],
					[0xd, "13_way_assoc"],
					[0xe, "14_way_assoc"],
					[0xf, "15_way_assoc"],
					[0x10, "16_way_assoc"],
					[0x11, "17_way_assoc"],
					[0x12, "18_way_assoc"],
					[0x13, "19_way_assoc"],
					[0x14, "20_way_assoc"],
					[0x15, "21_way_assoc"],
					[0x16, "22_way_assoc"],
					[0x17, "23_way_assoc"],
					[0x18, "24_way_assoc"],
					[0x19, "25_way_assoc"],
					[0x1a, "26_way_assoc"],
					[0x1b, "27_way_assoc"],
					[0x1c, "28_way_assoc"],
					[0x1d, "29_way_assoc"],
					[0x1e, "30_way_assoc"],
					[0x1f, "31_way_assoc"],
					[0x20, "32_way_assoc"],
					[0x21, "33_way_assoc"],
					[0x22, "34_way_assoc"],
					[0x23, "35_way_assoc"],
					[0x24, "36_way_assoc"],
					[0x25, "37_way_assoc"],
					[0x26, "38_way_assoc"],
					[0x27, "39_way_assoc"],
					[0x28, "40_way_assoc"],
					[0x29, "41_way_assoc"],
					[0x2a, "42_way_assoc"],
					[0x2b, "43_way_assoc"],
					[0x2c, "44_way_assoc"],
					[0x2d, "45_way_assoc"],
					[0x2e, "46_way_assoc"],
					[0x2f, "47_way_assoc"],
					[0x30, "48_way_assoc"],
					[0x31, "49_way_assoc"],
					[0x32, "50_way_assoc"],
					[0x33, "51_way_assoc"],
					[0x34, "52_way_assoc"],
					[0x35, "53_way_assoc"],
					[0x36, "54_way_assoc"],
					[0x37, "55_way_assoc"],
					[0x38, "56_way_assoc"],
					[0x39, "57_way_assoc"],
					[0x3a, "58_way_assoc"],
					[0x3b, "59_way_assoc"],
					[0x3c, "60_way_assoc"],
					[0x3d, "61_way_assoc"],
					[0x3e, "62_way_assoc"],
					[0x3f, "63_way_assoc"],
					[0x40, "64_way_assoc"],
					[0x41, "65_way_assoc"],
					[0x42, "66_way_assoc"],
					[0x43, "67_way_assoc"],
					[0x44, "68_way_assoc"],
					[0x45, "69_way_assoc"],
					[0x46, "70_way_assoc"],
					[0x47, "71_way_assoc"],
					[0x48, "72_way_assoc"],
					[0x49, "73_way_assoc"],
					[0x4a, "74_way_assoc"],
					[0x4b, "75_way_assoc"],
					[0x4c, "76_way_assoc"],
					[0x4d, "77_way_assoc"],
					[0x4e, "78_way_assoc"],
					[0x4f, "79_way_assoc"],
					[0x50, "80_way_assoc"],
					[0x51, "81_way_assoc"],
					[0x52, "82_way_assoc"],
					[0x53, "83_way_assoc"],
					[0x54, "84_way_assoc"],
					[0x55, "85_way_assoc"],
					[0x56, "86_way_assoc"],
					[0x57, "87_way_assoc"],
					[0x58, "88_way_assoc"],
					[0x59, "89_way_assoc"],
					[0x5a, "90_way_assoc"],
					[0x5b, "91_way_assoc"],
					[0x5c, "92_way_assoc"],
					[0x5d, "93_way_assoc"],
					[0x5e, "94_way_assoc"],
					[0x5f, "95_way_assoc"],
					[0x60, "96_way_assoc"],
					[0x61, "97_way_assoc"],
					[0x62, "98_way_assoc"],
					[0x63, "99_way_assoc"],
					[0x64, "100_way_assoc"],
					[0x65, "101_way_assoc"],
					[0x66, "102_way_assoc"],
					[0x67, "103_way_assoc"],
					[0x68, "104_way_assoc"],
					[0x69, "105_way_assoc"],
					[0x6a, "106_way_assoc"],
					[0x6b, "107_way_assoc"],
					[0x6c, "108_way_assoc"],
					[0x6d, "109_way_assoc"],
					[0x6e, "110_way_assoc"],
					[0x6f, "111_way_assoc"],
					[0x70, "112_way_assoc"],
					[0x71, "113_way_assoc"],
					[0x72, "114_way_assoc"],
					[0x73, "115_way_assoc"],
					[0x74, "116_way_assoc"],
					[0x75, "117_way_assoc"],
					[0x76, "118_way_assoc"],
					[0x77, "119_way_assoc"],
					[0x78, "120_way_assoc"],
					[0x79, "121_way_assoc"],
					[0x7a, "122_way_assoc"],
					[0x7b, "123_way_assoc"],
					[0x7c, "124_way_assoc"],
					[0x7d, "125_way_assoc"],
					[0x7e, "126_way_assoc"],
					[0x7f, "127_way_assoc"],
					[0x80, "128_way_assoc"],
					[0x81, "129_way_assoc"],
					[0x82, "130_way_assoc"],
					[0x83, "131_way_assoc"],
					[0x84, "132_way_assoc"],
					[0x85, "133_way_assoc"],
					[0x86, "134_way_assoc"],
					[0x87, "135_way_assoc"],
					[0x88, "136_way_assoc"],
					[0x89, "137_way_assoc"],
					[0x8a, "138_way_assoc"],
					[0x8b, "139_way_assoc"],
					[0x8c, "140_way_assoc"],
					[0x8d, "141_way_assoc"],
					[0x8e, "142_way_assoc"],
					[0x8f, "143_way_assoc"],
					[0x90, "144_way_assoc"],
					[0x91, "145_way_assoc"],
					[0x92, "146_way_assoc"],
					[0x93, "147_way_assoc"],
					[0x94, "148_way_assoc"],
					[0x95, "149_way_assoc"],
					[0x96, "150_way_assoc"],
					[0x97, "151_way_assoc"],
					[0x98, "152_way_assoc"],
					[0x99, "153_way_assoc"],
					[0x9a, "154_way_assoc"],
					[0x9b, "155_way_assoc"],
					[0x9c, "156_way_assoc"],
					[0x9d, "157_way_assoc"],
					[0x9e, "158_way_assoc"],
					[0x9f, "159_way_assoc"],
					[0xa0, "160_way_assoc"],
					[0xa1, "161_way_assoc"],
					[0xa2, "162_way_assoc"],
					[0xa3, "163_way_assoc"],
					[0xa4, "164_way_assoc"],
					[0xa5, "165_way_assoc"],
					[0xa6, "166_way_assoc"],
					[0xa7, "167_way_assoc"],
					[0xa8, "168_way_assoc"],
					[0xa9, "169_way_assoc"],
					[0xaa, "170_way_assoc"],
					[0xab, "171_way_assoc"],
					[0xac, "172_way_assoc"],
					[0xad, "173_way_assoc"],
					[0xae, "174_way_assoc"],
					[0xaf, "175_way_assoc"],
					[0xb0, "176_way_assoc"],
					[0xb1, "177_way_assoc"],
					[0xb2, "178_way_assoc"],
					[0xb3, "179_way_assoc"],
					[0xb4, "180_way_assoc"],
					[0xb5, "181_way_assoc"],
					[0xb6, "182_way_assoc"],
					[0xb7, "183_way_assoc"],
					[0xb8, "184_way_assoc"],
					[0xb9, "185_way_assoc"],
					[0xba, "186_way_assoc"],
					[0xbb, "187_way_assoc"],
					[0xbc, "188_way_assoc"],
					[0xbd, "189_way_assoc"],
					[0xbe, "190_way_assoc"],
					[0xbf, "191_way_assoc"],
					[0xc0, "192_way_assoc"],
					[0xc1, "193_way_assoc"],
					[0xc2, "194_way_assoc"],
					[0xc3, "195_way_assoc"],
					[0xc4, "196_way_assoc"],
					[0xc5, "197_way_assoc"],
					[0xc6, "198_way_assoc"],
					[0xc7, "199_way_assoc"],
					[0xc8, "200_way_assoc"],
					[0xc9, "201_way_assoc"],
					[0xca, "202_way_assoc"],
					[0xcb, "203_way_assoc"],
					[0xcc, "204_way_assoc"],
					[0xcd, "205_way_assoc"],
					[0xce, "206_way_assoc"],
					[0xcf, "207_way_assoc"],
					[0xd0, "208_way_assoc"],
					[0xd1, "209_way_assoc"],
					[0xd2, "210_way_assoc"],
					[0xd3, "211_way_assoc"],
					[0xd4, "212_way_assoc"],
					[0xd5, "213_way_assoc"],
					[0xd6, "214_way_assoc"],
					[0xd7, "215_way_assoc"],
					[0xd8, "216_way_assoc"],
					[0xd9, "217_way_assoc"],
					[0xda, "218_way_assoc"],
					[0xdb, "219_way_assoc"],
					[0xdc, "220_way_assoc"],
					[0xdd, "221_way_assoc"],
					[0xde, "222_way_assoc"],
					[0xdf, "223_way_assoc"],
					[0xe0, "224_way_assoc"],
					[0xe1, "225_way_assoc"],
					[0xe2, "226_way_assoc"],
					[0xe3, "227_way_assoc"],
					[0xe4, "228_way_assoc"],
					[0xe5, "229_way_assoc"],
					[0xe6, "230_way_assoc"],
					[0xe7, "231_way_assoc"],
					[0xe8, "232_way_assoc"],
					[0xe9, "233_way_assoc"],
					[0xea, "234_way_assoc"],
					[0xeb, "235_way_assoc"],
					[0xec, "236_way_assoc"],
					[0xed, "237_way_assoc"],
					[0xee, "238_way_assoc"],
					[0xef, "239_way_assoc"],
					[0xf0, "240_way_assoc"],
					[0xf1, "241_way_assoc"],
					[0xf2, "242_way_assoc"],
					[0xf3, "243_way_assoc"],
					[0xf4, "244_way_assoc"],
					[0xf5, "245_way_assoc"],
					[0xf6, "246_way_assoc"],
					[0xf7, "247_way_assoc"],
					[0xf8, "248_way_assoc"],
					[0xf9, "249_way_assoc"],
					[0xfa, "250_way_assoc"],
					[0xfb, "251_way_assoc"],
					[0xfc, "252_way_assoc"],
					[0xfd, "253_way_assoc"],
					[0xfe, "254_way_assoc"],
					[0xff, "fully_assoc"]);
				type entries_t = INT("entries");
				FIELD("l1_d_tlb_2_and_4_m_assoc", assoc_t,
						EAX("%function_80000005", 31, 24));
				FIELD("l1_d_tlb_2_and_4_m_size", entries_t,
						EAX("%function_80000005", 23, 16));
				FIELD("l1_i_tlb_2_and_4_m_assoc", assoc_t,
						EAX("%function_80000005", 15, 8));
				FIELD("l1_i_tlb_2_and_4_m_size", entries_t,
						EAX("%function_80000005", 7, 0));
				FIELD("l1_d_tlb_4k_assoc", assoc_t,
						EBX("%function_80000005", 31, 24));
				FIELD("l1_d_tlb_4k_size", entries_t,
						EBX("%function_80000005", 23, 16));
				FIELD("l1_i_tlb_4k_assoc", assoc_t,
						EBX("%function_80000005", 15, 8));
				FIELD("l1_i_tlb_4k_size", entries_t,
						EBX("%function_80000005", 7, 0));
				FIELD("l1_d_c_size", INT("KB"),
						ECX("%function_80000005", 31, 24));
				FIELD("l1_d_c_assoc", assoc_t,
						ECX("%function_80000005", 23, 16));
				FIELD("l1_d_c_lines_per_tag", int_t,
						ECX("%function_80000005", 15, 8));
				FIELD("l1_d_c_line_size", INT("B"),
						ECX("%function_80000005", 7, 0));
				FIELD("l1_i_c_size", INT("KB"),
						EDX("%function_80000005", 31, 24));
				FIELD("l1_i_c_assoc", assoc_t,
						EDX("%function_80000005", 23, 16));
				FIELD("l1_i_c_lines_per_tag", int_t,
						EDX("%function_80000005", 15, 8));
				FIELD("l1_i_c_line_size", INT("B"),
						EDX("%function_80000005", 7, 0));
		}); // l1_cache_tlb_identifiers
	} // CPUID Function 0x80000005

	// L2/L3 Cache and L2 TLB Identifiers
	// CPUID Function 0x80000006
	if (EVAL("largest_ext_fn") >= 0x80000006) {
		SCOPE("l2_l3_cache_l2_tlb_identifiers", {
				REG128("%function_80000006", 0x80000006);
				type assoc_t = ENUM(
					[0x0, "disabled"],
					[0x1, "direct_mapped"],
					[0x2, "2_way_assoc"],
					[0x4, "4_way_assoc"],
					[0x6, "8_way_assoc"],
					[0x8, "16_way_assoc"],
					[0xA, "32_way_assoc"],
					[0xB, "48_way_assoc"],
					[0xC, "64_way_assoc"],
					[0xD, "96_way_assoc"],
					[0xE, "128_way_assoc"],
					[0xF, "fully_assoc"]);
				type entries_t = INT("entries");
				if (EVAL("../vendor") == "amd") {
					FIELD("l2_d_tlb_2_and_4_m_assoc", assoc_t,
							EAX("%function_80000006", 31, 28));
					FIELD("l2_d_tlb_2_and_4_m_size", entries_t,
							EAX("%function_80000006", 27, 16));
					FIELD("l2_i_tlb_2_and_4_m_assoc", assoc_t,
							EAX("%function_80000006", 15, 12));
					FIELD("l2_i_tlb_2_and_4_m_size", entries_t,
							EAX("%function_80000006", 11, 0));
					FIELD("l2_d_tlb_4k_assoc", assoc_t,
							EBX("%function_80000006", 31, 28));
					FIELD("l2_d_tlb_4k_size", entries_t,
							EBX("%function_80000006", 27, 16));
					FIELD("l2_i_tlb_4k_assoc", assoc_t,
							EBX("%function_80000006", 15, 12));
					FIELD("l2_i_tlb_4k_size", entries_t,
							EBX("%function_80000006", 11, 0));
					FIELD("l2_lines_per_tag", int_t,
							ECX("%function_80000006", 11, 8));
					FIELD("l3_size_min",
					      XFORM(INT("KB"), { return args[0] * 512;}),
					      EDX("%function_80000006", 31, 18));
					FIELD("l3_size_max",
					      XFORM(INT("KB"), {return args[0] * 512;}),
					      EDX("%function_80000006", 31, 18));
					FIELD("l3_assoc", assoc_t,
							EDX("%function_80000006", 15, 12));
					FIELD("l3_lines_per_tag", int_t,
							EDX("%function_80000006", 11, 8));
					FIELD("l3_line_size", INT("B"),
							EDX("%function_80000006", 7, 0));
				} // End non-Intel section
				FIELD("l2_size", INT("KB"),
						ECX("%function_80000006", 31, 16));
				FIELD("l2_assoc", assoc_t,
						ECX("%function_80000006", 15, 12));
				FIELD("l2_line_size", INT("B"),
						ECX("%function_80000006", 7, 0));
		}); // l2_l3_cache_l2_tlb_identifiers
	} // CPUID Function 0x80000006

	// Advanced Power Management Information
	// CPUID Function 0x80000007
	if (EVAL("vendor") == "amd" &&
	    EVAL("largest_ext_fn") >= 0x80000007) {
		SCOPE("adv_power_mgmt_info", {
				REG128("%function_80000007", 0x80000007);
				FIELD("ts", yesno_t, EDX("%function_80000007", 0));
				FIELD("fid", yesno_t, EDX("%function_80000007", 1));
				FIELD("vid", yesno_t, EDX("%function_80000007", 2));
				FIELD("ttp", yesno_t, EDX("%function_80000007", 3));
				FIELD("tm", yesno_t, EDX("%function_80000007", 4));
				FIELD("stc", yesno_t, EDX("%function_80000007", 5));
				FIELD("_100_mhz_steps", yesno_t,
						EDX("%function_80000007", 6));
				FIELD("hw_p_state", yesno_t, EDX("%function_80000007", 7));
				FIELD("tsc_invariant", yesno_t, EDX("%function_80000007", 8));
		}); // adv_power_mgmt_info
	} // CPUID Function 0x80000007

	// Address Size and Physical Core Count Information
	// CPUID Function 0x80000008
	if (EVAL("largest_ext_fn") >= 0x80000008) {
		SCOPE("addr_size_core_count_info", {
				REG128("%function_80000008", 0x80000008);
				type bits_t = INT("b");
				if (EVAL("../vendor") == "amd") {
					FIELD("guest_phys_addr_size", bits_t,
							EAX("%function_80000008", 23, 16));
					FIELD("apic_id_core_id_size", int_t,
							EAX("%function_80000008", 15, 12));
					FIELD("nc", int_t,
							EAX("%function_80000008", 7, 0));
					if (EVAL("apic_id_core_id_size") == 0) {
						FIELD("mnc", int_t,
							READ("nc") + 1);
					} else {
						FIELD("mnc", int_t,
							1 << READ("apic_id_core_id_size"));
					}
				}
				FIELD("lin_addr_size", bits_t,
						EAX("%function_80000008", 15, 8));
				FIELD("phys_addr_size", bits_t,
						EAX("%function_80000008", 7, 0));
		}); // addr_size_core_count_info
	}

	// Function 0x80000009 reserved on Intel and AMD

	// SVM Revision and Feature Identification
	// CPUID Function 0x8000000A
	if (EVAL("largest_ext_fn") >= 0x8000000A) {
		SCOPE("svn_rev_feature_ident", {
				REG128("%function_8000000A", 0x8000000A);
				FIELD("svm_rev", int_t, EAX("%function_8000000A", 7, 0));
				FIELD("nasid", int_t, EBX("%function_8000000A"));
				FIELD("ssse3_sse5_dis", yesno_t,
						EDX("%function_8000000A", 9));
				FIELD("nrips", yesno_t, EDX("%function_8000000A", 3));
				FIELD("svml", yesno_t, EDX("%function_8000000A", 2));
				FIELD("lbr_virt", yesno_t, EDX("%function_8000000A", 1));
				FIELD("np", yesno_t, EDX("%function_8000000A", 0));
		}); // svn_rev_feature_ident
	} // CPUID Function 0x8000000A

	// Functions 0x8000000B through 0x80000018 reserved on Intel and AMD

	// TLB 1GB Page Identifiers
	// CPUID Function 0x80000019
	if (EVAL("largest_ext_fn") >= 0x80000019) {
		SCOPE("tlb_1gb_page_identifiers", {
				REG128("%function_80000019", 0x80000019);
				type assoc_t = ENUM(
					[0x0, "disabled"],
					[0x1, "direct_mapped"],
					[0x2, "2_way_assoc"],
					[0x4, "4_way_assoc"],
					[0x6, "8_way_assoc"],
					[0x8, "16_way_assoc"],
					[0xA, "32_way_assoc"],
					[0xB, "48_way_assoc"],
					[0xC, "64_way_assoc"],
					[0xD, "96_way_assoc"],
					[0xE, "128_way_assoc"],
					[0xF, "fully_assoc"]);
				type entries_t = INT("entries");
				FIELD("l1_d_tlb_1g_assoc", assoc_t,
						EAX("%function_80000019", 31, 28));
				FIELD("l1_d_tlb_1g_size", entries_t,
						EAX("%function_80000019", 27, 16));
				FIELD("l1_i_tlb_1g_assoc", assoc_t,
						EAX("%function_80000019", 15, 12));
				FIELD("l1_i_tlb_1g_size", entries_t,
						EAX("%function_80000019", 11, 0));
				FIELD("l2_d_tlb_1g_assoc", assoc_t,
						EBX("%function_80000019", 31, 28));
				FIELD("l2_d_tlb_1g_size", entries_t,
						EBX("%function_80000019", 27, 16));
				FIELD("l2_i_tlb_1g_assoc", assoc_t,
						EBX("%function_80000019", 15, 12));
				FIELD("l2_i_tlb_1g_size", entries_t,
						EBX("%function_80000019", 11, 0));
		}); // tlb_1gb_page_identifiers
	} // CPUID Function 0x80000019

	// Performance Optimization Identifiers
	// CPUID Function 0x8000001A
	if (EVAL("largest_ext_fn") >= 0x8000001A) {
		SCOPE("perf_opt_identifiers", {
				REG128("%function_8000001A", 0x8000001A);
				FIELD("movu", yesno_t, EAX("%function_8000001A", 1));
				FIELD("fp128", yesno_t, EAX("%function_8000001A", 0));
		}); // perf_opt_identifiers
	} // CPUID Function 0x8000001A
}

public
CPUID_SCOPE(string name, int cpu, func tail_fn)
{
	SCOPE(name, BIND("cpuid", cpu), {
		cpuid_generic_device();
		tail_fn();
	});
}
