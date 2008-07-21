#include "pp.h"

#include <stdint.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "cpuid_binding.h"
#include "pp_driver.h"
#include "filesystem.h"
#include "simple_regex.h"

#define CPU_SYSFS_DIR		"/sys/devices/system/cpu"

/* constructor */
cpuid_io::cpuid_io(const cpuid_address &address)
    : m_address(address)
{
}

/* destructor */
cpuid_io::~cpuid_io()
{
}

const cpuid_address &
cpuid_io::address() const
{
	return m_address;
}

pp_value
cpuid_io::read(const pp_value &address, const pp_bitwidth width) const
{
	// make sure this is a valid access
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	// save original affinity
	cpu_set_t new_set, orig_set;

	if (sched_getaffinity(getpid(), sizeof(orig_set), &orig_set) < 0) {
		do_io_error("cannot get CPU affinity");
	}

	// set affinity to desired CPU
	CPU_ZERO(&new_set);
	CPU_SET(m_address.cpu, &new_set);
	if (sched_setaffinity(getpid(), sizeof(new_set), &new_set) < 0) {
		do_io_error(to_string(
		    boost::format("cannot set affinity to CPU %d")
		    %m_address.cpu));
	}

	// call CPUID
	uint32_t regs[4];
	unsigned int function = pp_value(address & PP_MASK(32)).get_uint();
	unsigned int argument = 
		pp_value((address >> 32) & PP_MASK(32)).get_uint();
	asm volatile(
		"cpuid"
		: "=a" (regs[0]),
		  "=b" (regs[1]),
		  "=c" (regs[2]),
		  "=d" (regs[3])
		: "0"  (function),
		  "2"  (argument)
		);
	
	// set affinity back to original
	if (sched_setaffinity(getpid(), sizeof(orig_set), &orig_set) < 0) {
		do_io_error("cannot reset CPU affinity");
	}

	bitbuffer bitbuf(width);
	memcpy(bitbuf.get(), regs, width/CHAR_BIT);

	return pp_value(bitbuf);
}

void
cpuid_io::write(const pp_value &address, const pp_bitwidth width,
    const pp_value &value) const
{
	// do nothing - CPUID is read-only
}

void
cpuid_io::enumerate(std::vector<cpuid_address> *addresses)
{
	// This does not currently support non sysfs systems, but it could.
	fs::directory_ptr dir = fs::directory::open(CPU_SYSFS_DIR);

	fs::direntry_ptr de;
	while ((de = dir->read())) {
		// each CPU gets a dir
		if (!de->is_dir())
			continue;
		// each dir is named cpuN
		if (de->name().substr(0,3) != "cpu")
			continue;

		// parse the file name
		regex re("^cpu[0-9]+$");
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

		addresses->push_back(cpuid_address(cpu));
	}
	std::sort(addresses->begin(), addresses->end());
}

void
cpuid_io::do_io_error(const string &str) const
{
	throw pp_driver::io_error(to_string(m_address) + ": " + str);
}

void
cpuid_io::check_width(pp_bitwidth width) const
{
	switch (width) {
	    case BITS128:
		break;
	    default:
		do_io_error(to_string(
		    boost::format("unsupported register width %d") %width));
	}
}

void
cpuid_io::check_bounds(const pp_value &offset, unsigned bytes) const
{
	/* CPUID has a 64 bit address space */
	if (offset < 0 || offset > PP_MASK(64)) {
		do_io_error(to_string(
		    boost::format("invalid register: %d bytes @ 0x%x")
		    %bytes %offset));
	}
}
