#include "hwpp.h"
#include <vector>
#include "fake_language.h"
#include "devices/cpuid/generic_device.h"
#include "devices/msr/generic_device.h"

namespace hwpp {
namespace device {

static void
cpu_discovered(const std::vector<Value> &args)
{
	Value cpu = args[0];

	OPEN_SCOPE("cpu." + to_string(cpu));
	BOOKMARK("cpu");

		CPUID_SCOPE("cpuid", cpu);
		CLOSE_SCOPE();

		MSR_SCOPE("msr", cpu);
		CLOSE_SCOPE();

	CLOSE_SCOPE();
	ALIAS("cpu[]", "cpu." + to_string(cpu));
}

class CpuDiscovery
{
    public:
	CpuDiscovery()
	{
		// register a catch-all discovery rule
		std::vector<Value> args;
		register_discovery("cpu", args, cpu_discovered);
	}
};

static CpuDiscovery the_cpu_discovery;

}  // namespace device
}  // namespace hwpp
