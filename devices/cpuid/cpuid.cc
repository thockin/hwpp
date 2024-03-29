#include "hwpp.h"
#include <vector>
#include "fake_language.h"
#include "devices/cpuid/generic_device.h"

namespace hwpp {
namespace device {

void
cpuid_datatypes_init()
{
	/* cpuid_vendor_t */
	ENUM("cpuid_vendor_t",
		KV("intel",     Value("0x6c65746e49656e69756e6547")),
		KV("amd",       Value("0x444d416369746e6568747541")),
		KV("umc",       Value("0x20434d5520434d5520434d55")),
		KV("cyrix",     Value("0x64616574736e497869727943")),
		KV("nexgen",    Value("0x6e65766972446e654778654e")),
		KV("centaur",   Value("0x736c756148727561746e6543")),
		KV("rise",      Value("0x657369526573695265736952")),
		KV("sis",       Value("0x205369532053695320536953")),
		KV("transmeta", Value("0x3638784d54656e69756e6547")),
		KV("nsc",       Value("0x43534e2079622065646f6547")));

	/* cpuid_features_t */
	BITMASK("cpuid_features_t",
		KV("fpu", 0),
		KV("vme", 1),
		KV("de", 2),
		KV("pse", 3),
		KV("tsc", 4),
		KV("msr", 5),
		KV("pae", 6),
		KV("mce", 7),
		KV("cx8", 8),
		KV("apic", 9),
		KV("sysenter", 11),
		KV("mtrr", 12),
		KV("pge", 13),
		KV("mca", 14),
		KV("cmov", 15),
		KV("pat", 16),
		KV("pse36", 17),
		KV("psn", 18),
		KV("clflsh", 19),
		KV("ds", 21),
		KV("acpi", 22),
		KV("mmx", 23),
		KV("fxsr", 24),
		KV("sse", 25),
		KV("sse2", 26),
		KV("ss", 27),
		KV("htt", 28),
		KV("tm", 29),
		KV("ia64", 30),
		KV("pbe", 31),
		KV("sse3", 32+0),
		KV("dtes64", 32+2),
		KV("monitor", 32+3),
		KV("ds-cpl", 32+4),
		KV("vmx", 32+5),
		KV("smx", 32+6),
		KV("est", 32+7),
		KV("tm2", 32+8),
		KV("ssse3", 32+9),
		KV("cnxt-id", 32+10),
		KV("cx16", 32+13),
		KV("xtpr", 32+14),
		KV("pdcm", 32+15),
		KV("dca", 32+18),
		KV("sse4_1", 32+19),
		KV("sse4_2", 32+20));

	/* cpuid_ext_features_t */
	BITMASK("cpuid_ext_features_t",
		KV("fpu", 0),
		KV("vme", 1),
		KV("de", 2),
		KV("pse", 3),
		KV("tsc", 4),
		KV("msr", 5),
		KV("pae", 6),
		KV("mce", 7),
		KV("cx8", 8),
		KV("apic", 9),
		KV("syscall", 11),
		KV("mtrr", 12),
		KV("pge", 13),
		KV("mca", 14),
		KV("cmov", 15),
		KV("pat", 16),
		KV("pse36", 17),
		KV("nx", 20),
		KV("mmx_ext", 22),
		KV("mmx", 23),
		KV("fxsr", 24),
		KV("ffxsr", 25),
		KV("page_1gb", 26),
		KV("rdtscp", 27),
		KV("64bit", 29),
		KV("3dnow_ext", 30),
		KV("3dnow", 31),
		KV("lahf_sahf", 32+0),
		KV("cmp_legacy", 32+1),
		KV("svm", 32+2),
		KV("ext_apic_space", 32+3),
		KV("alt_mov_cr_8", 32+4),
		KV("abm", 32+5),
		KV("sse4a", 32+6),
		KV("misalign_sse", 32+7),
		KV("3dnow_prefetch", 32+8),
		KV("osvw", 32+9),
		KV("ibs", 32+10),
		KV("sse5", 32+11),
		KV("skinit", 32+12),
		KV("wdt", 32+13));

	/* cache_descriptor_t */
	ENUM("cache_descriptor_t",
		KV("null", 0x00),
		KV("instr_tlb_4KB_pages_4way_assoc_32_entries", 0x01),
		KV("instr_tlb_4MB_pages_fully_assoc_2_entries", 0x02),
		KV("data_tlb_4KB_pages_4way_assoc_64_entries", 0x03),
		KV("data_tlb_4MB_pages_4way_assoc_8_entries", 0x04),
		KV("data_tlb_4MB_pages_4way_assoc_32_entries", 0x05),
		KV("level1_instr_cache_8KB_4way_assoc_32B_line", 0x06),
		KV("level1_instr_cache_16KB_4way_assoc_32B_line", 0x08),
		KV("level1_data_cache_8KB_2way_assoc_32B_line", 0x0A),
		KV("instr_tlb_4MB_pages_4way_assoc_4_entries", 0x0B),
		KV("level1_data_cache_16KB_4way_assoc_32B_line", 0x0C),
		KV("level1_data_cache_24KB_6way_assoc_64B_line", 0x0E),
		KV("level1_data_cache_16KB_4way_assoc_32B_line_ia64", 0x10),
		KV("level1_instr_cache_16KB_4way_assoc_32B_line_ia64", 0x15),
		KV("level2_cache_96KB_6way_assoc_64B_line_ia64", 0x1A),
		KV("level3_cache_512KB_4way_assoc_sectored_64B_line", 0x22),
		KV("level3_cache_1MB_8way_assoc_sectored_64B_line", 0x23),
		KV("level3_cache_2MB_8way_assoc_sectored_64B_line", 0x25),
		KV("level3_cache_4MB_8way_assoc_sectored_64B_line", 0x29),
		KV("level1_data_cache_32KB_8way_assoc_64B_line", 0x2C),
		KV("level1_instr_cache_32KB_8way_assoc_64B_line", 0x30),
		KV("level2_cache_128KB_4way_assoc_sectored_64B_line", 0x39),
		KV("level2_cache_192KB_6way_assoc_sectored_64B_line", 0x3A),
		KV("level2_cache_128KB_2way_assoc_sectored_64B_line", 0x3B),
		KV("level2_cache_256KB_4way_assoc_sectored_64B_line", 0x3C),
		KV("level2_cache_384KB_6way_assoc_sectored_64B_line", 0x3D),
		KV("level2_cache_512KB_4way_assoc_sectored_64B_line", 0x3E),
		KV("either_no_level2_or_no_level3_cache", 0x40),
		KV("level2_cache_128KB_4way_assoc_32B_line", 0x41),
		KV("level2_cache_256KB_4way_assoc_32B_line", 0x42),
		KV("level2_cache_512KB_4way_assoc_32B_line", 0x43),
		KV("level2_cache_1MB_4way_assoc_32B_line", 0x44),
		KV("level2_cache_2MB_4way_assoc_32B_line", 0x45),
		KV("level3_cache_4MB_4way_assoc_64B_line", 0x46),
		KV("level3_cache_8MB_8way_assoc_64B_line", 0x47),
		KV("level2_cache_3MB_12way_assoc_64B_line", 0x48),
		// different on Xeon MP, Family 0Fh, Model 06h
		KV("level2_cache_4MB_16way_assoc_64B_line", 0x49),
		KV("level3_cache_6MB_12way_assoc_64B_line", 0x4A),
		KV("level3_cache_8MB_16way_assoc_64B_line", 0x4B),
		KV("level3_cache_12MB_12way_assoc_64B_line", 0x4C),
		KV("level3_cache_16MB_16way_assoc_64B_line", 0x4D),
		KV("level2_cache_6MB_24way_assoc_64B_line", 0x4E),
		KV("instr_tlb_4KB_pages_32_entries", 0x4F),
		KV("instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_64_entries",
				0x50),
		KV("instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_128_entries",
				0x51),
		KV("instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_256_entries",
				0x52),
		KV("level1_data_tlb0_4MB_pages_4way_assoc_16_entries", 0x56),
		KV("level1_data_tlb0_4KB_pages_4way_assoc_16_entries", 0x57),
		KV("level1_data_tlb0_4KB_pages_fully_assoc_16_entries", 0x59),
		KV("data_tlb_4KB_or_4MB_pages_fully_assoc_64_entries", 0x5B),
		KV("data_tlb_4KB_or_4MB_pages_fully_assoc_128_entries", 0x5C),
		KV("data_tlb_4KB_or_4MB_pages_fully_assoc_256_entries", 0x5D),
		KV("level1_data_cache_16KB_8way_assoc_sectored_64B_line", 0x60),
		KV("level1_data_cache_8KB_4way_assoc_sectored_64B_line", 0x66),
		KV("level1_data_cache_16KB_4way_assoc_sectored_64B_line", 0x67),
		KV("level1_data_cache_32KB_4way_assoc_sectored_64B_line", 0x68),
		KV("trace_cache_12K_uops_8way_assoc", 0x70),
		KV("trace_cache_16K_uops_8way_assoc", 0x71),
		KV("trace_cache_32K_uops_8way_assoc", 0x72),
		KV("trace_cache_64K_uops_8way_assoc", 0x73),
		KV("level1_instr_cache_16KB_4way_assoc_64B_line_sectored_ia64",
				0x77),
		KV("level2_cache_1MB_4way_assoc_64B_line", 0x78),
		KV("level2_cache_128KB_8way_assoc_sectored_64B_line", 0x79),
		KV("level2_cache_256KB_8way_assoc_sectored_64B_line", 0x7A),
		KV("level2_cache_512KB_8way_assoc_sectored_64B_line", 0x7B),
		KV("level2_cache_1MB_8way_assoc_sectored_64B_line", 0x7C),
		KV("level2_cache_2MB_8way_assoc_sectored_64B_line", 0x7D),
		KV("level2_cache_256KB_8way_assoc_128B_line_sectored_ia64",
				0x7E),
		KV("level2_cache_512KB_2way_assoc_64B_line", 0x7F),
		KV("level2_cache_512KB_8way_assoc_64B_line", 0x80),
		KV("level2_cache_128KB_8way_assoc_32B_line", 0x81),
		KV("level2_cache_256KB_8way_assoc_32B_line", 0x82),
		KV("level2_cache_512KB_8way_assoc_32B_line", 0x83),
		KV("level2_cache_1MB_8way_assoc_32B_line", 0x84),
		KV("level2_cache_2MB_8way_assoc_32B_line", 0x85),
		KV("level2_cache_512KB_4way_assoc_64B_line", 0x86),
		KV("level2_cache_1MB_8way_assoc_64B_line", 0x87),
		KV("level3_cache_2MB_4way_assoc_64B_line_ia64", 0x88),
		KV("level3_cache_4MB_4way_assoc_64B_line_ia64", 0x89),
		KV("level3_cache_8MB_4way_assoc_64B_line_ia64", 0x8A),
		KV("level3_cache_3MB_12way_assoc_128B_line_ia64", 0x8D),
		KV("instr_tlb_4KB_to_256MB_pages_fully_assoc_64_entries_ia64",
				0x90),
		KV("data_tlb1_4KB_to_256MB_pages_fully_assoc_32_entries_ia64",
				0x96),
		KV("data_tlb2_4KB_to_256MB_pages_fully_assoc_96_entries_ia64",
				0x9B),
		KV("instr_tlb_4KB_pages_4way_assoc_128_entries", 0xB0),
		KV("instr_tlb_4MB_or_2MB_pages_4way_assoc_4_or_8_entries",
				0xB1),
		KV("data_tlb_4KB_pages_4way_assoc_128_entries", 0xB3),
		KV("data_tlb1_4KB_pages_4way_assoc_256_entries", 0xB4),
		KV("data_tlb1_4KB_pages_4way_assoc_64_entries", 0xBA),
		KV("data_tlb_4KB_4MB_pages_4way_assoc_8_entries", 0xC0),
		KV("64B_prefetching", 0xF0),
		KV("128B_prefetching", 0xF1));

	/* cache_descriptor_xeon_t */
	// FIXME: is there a better way to deal with the one single differing
	// KV pair on Xeon than an entirely different enum?
	ENUM("cache_descriptor_xeon_t",
		KV("null", 0x00),
		KV("instr_tlb_4KB_pages_4way_assoc_32_entries", 0x01),
		KV("instr_tlb_4MB_pages_fully_assoc_2_entries", 0x02),
		KV("data_tlb_4KB_pages_4way_assoc_64_entries", 0x03),
		KV("data_tlb_4MB_pages_4way_assoc_8_entries", 0x04),
		KV("data_tlb_4MB_pages_4way_assoc_32_entries", 0x05),
		KV("level1_instr_cache_8KB_4way_assoc_32B_line", 0x06),
		KV("level1_instr_cache_16KB_4way_assoc_32B_line", 0x08),
		KV("level1_data_cache_8KB_2way_assoc_32B_line", 0x0A),
		KV("instr_tlb_4MB_pages_4way_assoc_4_entries", 0x0B),
		KV("level1_data_cache_16KB_4way_assoc_32B_line", 0x0C),
		KV("level1_data_cache_24KB_6way_assoc_64B_line", 0x0E),
		KV("level3_cache_512KB_4way_assoc_sectored_64B_line", 0x22),
		KV("level3_cache_1MB_8way_assoc_sectored_64B_line", 0x23),
		KV("level3_cache_2MB_8way_assoc_sectored_64B_line", 0x25),
		KV("level3_cache_4MB_8way_assoc_sectored_64B_line", 0x29),
		KV("level1_data_cache_32KB_8way_assoc_64B_line", 0x2C),
		KV("level1_instr_cache_32KB_8way_assoc_64B_line", 0x30),
		KV("level2_cache_128KB_4way_assoc_sectored_64B_line", 0x39),
		KV("level2_cache_192KB_6way_assoc_sectored_64B_line", 0x3A),
		KV("level2_cache_128KB_2way_assoc_sectored_64B_line", 0x3B),
		KV("level2_cache_256KB_4way_assoc_sectored_64B_line", 0x3C),
		KV("level2_cache_384KB_6way_assoc_sectored_64B_line", 0x3D),
		KV("level2_cache_512KB_4way_assoc_sectored_64B_line", 0x3E),
		KV("either_no_level2_or_no_level3_cache", 0x40),
		KV("level2_cache_128KB_4way_assoc_32B_line", 0x41),
		KV("level2_cache_256KB_4way_assoc_32B_line", 0x42),
		KV("level2_cache_512KB_4way_assoc_32B_line", 0x43),
		KV("level2_cache_1MB_4way_assoc_32B_line", 0x44),
		KV("level2_cache_2MB_4way_assoc_32B_line", 0x45),
		KV("level3_cache_4MB_4way_assoc_64B_line", 0x46),
		KV("level3_cache_8MB_8way_assoc_64B_line", 0x47),
		KV("level2_cache_3MB_12way_assoc_64B_line", 0x48),
		// special case for Xeon MP, Family 0Fh, Model 06h
		KV("level3_cache_4MB_16way_assoc_64B_line", 0x49),
		KV("level3_cache_6MB_12way_assoc_64B_line", 0x4A),
		KV("level3_cache_8MB_16way_assoc_64B_line", 0x4B),
		KV("level3_cache_12MB_12way_assoc_64B_line", 0x4C),
		KV("level3_cache_16MB_16way_assoc_64B_line", 0x4D),
		KV("level2_cache_6MB_24way_assoc_64B_line", 0x4E),
		KV("instr_tlb_4KB_pages_32_entries", 0x4F),
		KV("instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_64_entries",
				0x50),
		KV("instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_128_entries",
				0x51),
		KV("instr_tlb_4KB_2MB_or_4MB_pages_fully_assoc_256_entries",
				0x52),
		KV("level1_data_tlb0_4MB_pages_4way_assoc_16_entries", 0x56),
		KV("level1_data_tlb0_4KB_pages_4way_assoc_16_entries", 0x57),
		KV("level1_data_tlb0_4KB_pages_fully_assoc_16_entries", 0x59),
		KV("data_tlb_4KB_or_4MB_pages_fully_assoc_64_entries", 0x5B),
		KV("data_tlb_4KB_or_4MB_pages_fully_assoc_128_entries", 0x5C),
		KV("data_tlb_4KB_or_4MB_pages_fully_assoc_256_entries", 0x5D),
		KV("level1_data_cache_16KB_8way_assoc_sectored_64B_line", 0x60),
		KV("level1_data_cache_8KB_4way_assoc_sectored_64B_line", 0x66),
		KV("level1_data_cache_16KB_4way_assoc_sectored_64B_line", 0x67),
		KV("level1_data_cache_32KB_4way_assoc_sectored_64B_line", 0x68),
		KV("trace_cache_12K_uops_8way_assoc", 0x70),
		KV("trace_cache_16K_uops_8way_assoc", 0x71),
		KV("trace_cache_32K_uops_8way_assoc", 0x72),
		KV("trace_cache_64K_uops_8way_assoc", 0x73),
		KV("level2_cache_1MB_4way_assoc_64B_line", 0x78),
		KV("level2_cache_128KB_8way_assoc_sectored_64B_line", 0x79),
		KV("level2_cache_256KB_8way_assoc_sectored_64B_line", 0x7A),
		KV("level2_cache_512KB_8way_assoc_sectored_64B_line", 0x7B),
		KV("level2_cache_1MB_8way_assoc_sectored_64B_line", 0x7C),
		KV("level2_cache_2MB_8way_assoc_sectored_64B_line", 0x7D),
		KV("level2_cache_512KB_2way_assoc_64B_line", 0x7F),
		KV("level2_cache_512KB_8way_assoc_64B_line", 0x80),
		KV("level2_cache_256KB_8way_assoc_32B_line", 0x82),
		KV("level2_cache_512KB_8way_assoc_32B_line", 0x83),
		KV("level2_cache_1MB_8way_assoc_32B_line", 0x84),
		KV("level2_cache_2MB_8way_assoc_32B_line", 0x85),
		KV("level2_cache_512KB_4way_assoc_64B_line", 0x86),
		KV("level2_cache_1MB_8way_assoc_64B_line", 0x87),
		KV("instr_tlb_4KB_pages_4way_assoc_128_entries", 0xB0),
		KV("instr_tlb_4MB_or_2MB_pages_4way_assoc_4_or_8_entries",
				0xB1),
		KV("data_tlb_4KB_pages_4way_assoc_128_entries", 0xB3),
		KV("data_tlb1_4KB_pages_4way_assoc_256_entries", 0xB4),
		KV("data_tlb1_4KB_pages_4way_assoc_64_entries", 0xBA),
		KV("data_tlb_4KB_4MB_pages_4way_assoc_8_entries", 0xC0),
		KV("64B_prefetching", 0xF0),
		KV("128B_prefetching", 0xF1));
}

}  // namespace device
}  // namespace hwpp
