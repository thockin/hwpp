#include "pp.h"
#include "pp_datatypes.h"
#include "cpu_driver.h"
#include "cpu_address.h"

#include <stdint.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "filesystem.h"
#include "simple_regex.h"
#include "bit_buffer.h"

#define CPU_SYSFS_DIR	"/sys/devices/system/cpu"


// this forces linkage and avoids the static initialization order fiasco
pp_driver *
load_cpu_driver()
{
	static cpu_driver the_driver;
	return &the_driver;
}

cpu_driver::cpu_driver()
{
	system("modprobe cpuid >/dev/null 2>&1");
}

cpu_driver::~cpu_driver()
{
}

string
cpu_driver::name() const
{
	return "cpu";
}

void
cpu_driver::discover() const
{
	std::vector<cpu_address> addresses;

	// find all CPU addresses
	enumerate(CPU_SYSFS_DIR, &addresses);

	// for each CPU device in the system
	std::vector<cpu_address>::iterator it;
	for (it = addresses.begin(); it != addresses.end(); it++) {
		// check if anyone registered for this device
		const discovery_request *dr = find_discovery_request(*it);
		if (dr && dr->function == NULL) {
			continue;
		}

		// call the callback
		std::vector<pp_value> args;
		args.push_back(it->cpu);
		if (dr) {
			// call the callback
			dr->function(args);
		} else if (m_catchall) {
			// call the catchall
			m_catchall(args);
		}
	}
}

void
cpu_driver::register_discovery(const std::vector<pp_value> &args,
    discovery_callback function)
{
	if (args.size() == 0) {
		if (m_catchall) {
			throw pp_driver::args_error(
			    "cpu discovery: catchall already defined");
		}
		m_catchall = function;
		return;
	}

	if (args.size() != 7) {
		throw pp_driver::args_error(
		    "cpu discovery: <vendor, family_min, family_max, "
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

const cpu_driver::discovery_request *
cpu_driver::find_discovery_request(const cpu_address &addr) const
{
	pp_value vendor;
	pp_value family;
	pp_value model;
	pp_value stepping;
	pp_value tmp;

	// This is the ordering the CPU vendors use.  I don't know why.
	tmp = cpuid(addr, 0);
	vendor = ((((tmp >> 32*1) & PP_MASK(32)) << 0)
		| (((tmp >> 32*3) & PP_MASK(32)) << 32)
		| (((tmp >> 32*2) & PP_MASK(32)) << 64));

	tmp = cpuid(addr, 1);
	family = (tmp & pp_value(0xf00)) >> 8;
	model = (tmp & pp_value(0xf0)) >> 4;
	stepping = tmp & pp_value(0xf);

	// handle slight differences in AMD and Intel specs
	if (vendor == pp_value("0x6c65746e49656e69756e6547")) {
		// intel
		family += (tmp & pp_value(0xff00000)) >> 20;
		model |= (tmp & pp_value(0xf0000)) >> 12;
	} else if (vendor == pp_value("0x444d416369746e6568747541")) {
		// amd
		if (family == 0xf) {
			family += (tmp & pp_value(0xff00000)) >> 20;
			model |= (tmp & pp_value(0xf0000)) >> 12;
		}
	}

	DTRACE(TRACE_DISCOVERY, "discovery: cpu "
			+ to_string(boost::format("0x%x") %vendor)
			+ " " + to_string(family)
			+ " " + to_string(model)
			+ " " + to_string(stepping));

	for (size_t i = 0; i < m_callbacks.size(); i++) {
		if ((vendor == m_callbacks[i].vendor)
		 && (family >= m_callbacks[i].family_min
		  && family <= m_callbacks[i].family_max)
		 && (model >= m_callbacks[i].model_min
		  && model <= m_callbacks[i].model_max)
		 && (stepping >= m_callbacks[i].stepping_min
		  && stepping <= m_callbacks[i].stepping_max)) {
			DTRACE(TRACE_DISCOVERY,
					"discovery: cpu found match for "
					+ to_string(addr));
			return &m_callbacks[i];
		}
	}

	DTRACE(TRACE_DISCOVERY, "discovery: cpu no match for "
			+ to_string(addr));
	return NULL;
}

pp_value
cpu_driver::cpuid(const cpu_address &address, unsigned function)
{
	// save original affinity
	cpu_set_t new_set, orig_set;

	if (sched_getaffinity(getpid(), sizeof(orig_set), &orig_set) < 0) {
		do_io_error(address, "cannot get CPU affinity");
	}

	// set affinity to desired CPU
	CPU_ZERO(&new_set);
	CPU_SET(address.cpu, &new_set);
	if (sched_setaffinity(getpid(), sizeof(new_set), &new_set) < 0) {
		do_io_error(address, to_string(
		    boost::format("cannot set affinity to CPU %d")
		    %address.cpu));
	}

	// call CPUID
	uint32_t regs[4];
	asm volatile(
		"cpuid"
		: "=a" (regs[0]),
		  "=b" (regs[1]),
		  "=c" (regs[2]),
		  "=d" (regs[3])
		: "0"  (function)
		);
	
	// set affinity back to original
	if (sched_setaffinity(getpid(), sizeof(orig_set), &orig_set) < 0) {
		do_io_error(address, "cannot reset CPU affinity");
	}

	util::BitBuffer bitbuf(128);
	memcpy(bitbuf.get(), regs, 128/CHAR_BIT);

	return pp_value(bitbuf);
}

void
cpu_driver::enumerate(const string &path, std::vector<cpu_address> *addresses)
{
	fs::directory_ptr dir = fs::directory::open(path);

	fs::direntry_ptr de;
	while ((de = dir->read())) {
		// each CPU gets a dir
		if (!de->is_dir())
			continue;
		// each dir is named cpuN
		if (de->name().substr(0,3) != "cpu")
			continue;

		// parse the file name
		util::SimpleRegex re("^cpu[0-9]+$");
		if (!re.matches(de->name())) {
			continue;
		}

		std::istringstream iss(de->name());
		char c;
		int cpu = (unsigned)-1;
		// name comes in as "cpuN", but all we care about is N
		iss >> c >> c >> c >> cpu;
		// just in case
		if (cpu < 0) {
			continue;
		}

		addresses->push_back(cpu_address(cpu));
	}
	std::sort(addresses->begin(), addresses->end());
}

void
cpu_driver::do_io_error(const cpu_address &address, const string &str)
{
	throw pp_driver::io_error(to_string(address) + ": " + str);
}
