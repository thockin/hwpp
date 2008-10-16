#include "pp.h"
#include <vector>
#include "device_init.h"
#include "fake_language.h"
#include "drivers.h"
#include "../cpuid/generic_device.h"
#include "../msr/generic_device.h"


static void
cpu_discovered(const std::vector<pp_value> &args)
{
	pp_value cpu = args[0];

	OPEN_SCOPE("cpu." + to_string(cpu));
	BOOKMARK("cpu");

		CPUID_SCOPE("cpuid", cpu);
		CLOSE_SCOPE();

		MSR_SCOPE("msr", cpu);
		CLOSE_SCOPE();

	CLOSE_SCOPE();
	ALIAS("cpu[]", "cpu." + to_string(cpu));
}

class cpu_discovery {
    public:
	explicit
	cpu_discovery()
	{
		// register a catch-all discovery rule
		std::vector<pp_value> args;
		pp_register_discovery("cpu", args, cpu_discovered);
	}
};

static cpu_discovery the_cpu_discovery;
