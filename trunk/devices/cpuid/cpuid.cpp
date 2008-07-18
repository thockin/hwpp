#include "pp.h"
#include <vector>
#include "device_init.h"
#include "fake_language.h"
#include "drivers.h"
#include "pp_scope.h"
#include "pp_driver.h"
#include "pp_datatypes.h"
#include "generic_device.h"

void
cpuid_datatypes_init()
{
	/* cpuid_vendor_t */
	ENUM("cpuid_vendor_t",
		KV("intel",     pp_value("0x6c65746e49656e69756e6547")),
		KV("amd",       pp_value("0x444d416369746e6568747541")),
		KV("umc",       pp_value("0x20434d5520434d5520434d55")),
		KV("cyrix",     pp_value("0x64616574736e497869727943")),
		KV("nexgen",    pp_value("0x6e65766972446e654778654e")),
		KV("centaur",   pp_value("0x736c756148727561746e6543")),
		KV("rise",      pp_value("0x657369526573695265736952")),
		KV("sis",       pp_value("0x205369532053695320536953")),
		KV("transmeta", pp_value("0x3638784d54656e69756e6547")),
		KV("nsc",       pp_value("0x43534e2079622065646f6547")));

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
}

static void
cpuid_discovered(const std::vector<pp_value> &args)
{
	pp_value cpu = args[0];

	//FIXME: include standard MSRs?
	CPUID_SCOPE("cpuid." + to_string(cpu), cpu);
	CLOSE_SCOPE();
}

class cpuid_discovery {
    public:
	explicit
	cpuid_discovery()
	{
		// register a catch-all discovery rule
		std::vector<pp_value> args;
		pp_register_discovery("cpuid", args, cpuid_discovered);
	}
};

static cpuid_discovery the_cpuid_discovery;
