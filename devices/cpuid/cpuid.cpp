#include "pp.h"
#include <vector>
#include "device_init.h"
#include "utils.h"
#include "drivers.h"
#include "pp_scope.h"
#include "pp_driver.h"
#include "pp_datatypes.h"
#include "generic_device.h"

void
cpuid_datatypes_init()
{
	/* cpu_vendor_t */
	ENUM("cpu_vendor_t",
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
}

static void
cpuid_discovered(const std::vector<pp_value> &args)
{
	pp_value cpu = args[0];

	//FIXME: include standard MSRs?
	OPEN_SCOPE("cpu[" + to_string(cpu) + "]");
	cpuid_generic_device(cpu);
	CLOSE_SCOPE();
}

class cpuid_discovery {
    public:
	explicit
	cpuid_discovery()
	{
		// register a catch-all discovery rule
		std::vector<pp_value> args;
		register_discovery("cpuid", args, cpuid_discovered);
	}
};

static cpuid_discovery the_cpuid_discovery;
