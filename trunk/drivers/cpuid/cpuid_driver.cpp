#include "pp.h"
#include "pp_datatypes.h"
#include "cpuid_driver.h"
#include "cpuid_binding.h"

// this forces linkage and avoids the static initialization order fiasco
pp_driver *
load_cpuid_driver()
{
	static cpuid_driver the_driver;
	return &the_driver;
}

cpuid_driver::cpuid_driver()
{
	system("modprobe cpuid >/dev/null 2>&1");
}

cpuid_driver::~cpuid_driver()
{
}

string
cpuid_driver::name() const
{
	return "cpuid";
}

pp_binding_ptr
cpuid_driver::new_binding(const std::vector<pp_value> &args) const
{
	if (args.size() != 2) {
		throw pp_driver::args_error("cpuid<>: <cpu,function>");
	}

	pp_value cpu = args[0];
	pp_value function = args[1];

	if (cpu < 0) {
		throw pp_driver::args_error("cpuid<>: invalid cpu");
	}
	if (function < 0) {
		throw pp_driver::args_error("cpuid<>: invalid function");
	}

	return new_cpuid_binding(
		cpuid_address(cpu.get_uint(), function.get_uint()));
}

void
cpuid_driver::discover(pp_scope *platform) const
{
	std::vector<cpuid_address> addresses;

	/* find all CPUID addresses */
	cpuid_io::enumerate(&addresses);

	/* for each CPUID device in the system */
	std::vector<cpuid_address>::iterator it;
	for (it = addresses.begin(); it != addresses.end(); it++) {
		/* check if anyone registered for this device */
		const discovery_request *dr = find_discovery_request(*it);
		std::vector<pp_value> args;
		args.push_back(it->cpu);
		args.push_back(it->function);
		if (dr) {
			/* call the callback */
			dr->function(args);
		} else if (m_catchall) {
			/* call the catchall */
			m_catchall(args);
		}
	}
}

void
cpuid_driver::register_discovery(const std::vector<pp_value> &args,
    discovery_callback function)
{
	if (args.size() == 0) {
		if (m_catchall) {
			throw pp_driver::args_error(
			    "cpuid discovery: catchall already defined");
		}
		m_catchall = function;
		return;
	}

	if (args.size() != 7) {
		throw pp_driver::args_error(
		    "cpuid discovery: <vendor, family_min, family_max, "
		    "model_min, model_max, stepping_min, stepping_max>");
	}

	discovery_request dr;
	dr.vendor = args[0] & PP_MASK(12*8);
	dr.family_min = args[1].get_uint();
	dr.family_max = args[2].get_uint();
	dr.model_min = args[3].get_uint();
	dr.model_max = args[4].get_uint();
	dr.stepping_min = args[5].get_uint();
	dr.stepping_max = args[6].get_uint();
	dr.function = function;
	m_callbacks.push_back(dr);
}

const cpuid_driver::discovery_request *
cpuid_driver::find_discovery_request(const cpuid_address &addr) const
{
	pp_value vendor;
	pp_value family;
	pp_value model;
	pp_value stepping;

	// This is the ordering the CPU vendors use.  I don't know why.
	cpuid_io dev(cpuid_address(addr.cpu, 0));
	vendor = dev.read(1, BITS32)
		| (dev.read(3, BITS32)<<32)
		| (dev.read(2, BITS32)<<64);

	//FIXME: This might have to be different per vendor.  This is AMD.
	dev = cpuid_io(cpuid_address(addr.cpu, 1));
	pp_value tmp = dev.read(1, BITS32);
	family = ((tmp & pp_value(0xf00)) >> 8)
	    + ((tmp & pp_value(0xff00000)) >> 20);
	model = ((tmp & pp_value(0xf0000)) >> 12)
	    | ((tmp & pp_value(0xf0)) >> 4);
	stepping = tmp & pp_value(0xf);

	for (size_t i = 0; i < m_callbacks.size(); i++) {
		if ((vendor == m_callbacks[i].vendor)
		 && (family >= m_callbacks[i].family_min
		  && family <= m_callbacks[i].family_max)
		 && (model >= m_callbacks[i].model_min
		  && model <= m_callbacks[i].model_max)
		 && (stepping >= m_callbacks[i].stepping_min
		  && stepping <= m_callbacks[i].stepping_max)) {
			return &m_callbacks[i];
		}
	}

	return NULL;
}
