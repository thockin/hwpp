#include "hwpp.h"
#include "util/printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "cpuid_binding.h"
#include "driver.h"
#include "util/bit_buffer.h"

namespace hwpp { 

/* constructor */
CpuidIo::CpuidIo(const CpuidAddress &address)
    : m_address(address)
{
}

/* destructor */
CpuidIo::~CpuidIo()
{
}

const CpuidAddress &
CpuidIo::address() const
{
	return m_address;
}

Value
CpuidIo::read(const Value &address, const BitWidth width) const
{
	// make sure this is a valid access
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	// save original affinity
	cpu_set_t new_set, orig_set;

	if (sched_getaffinity(getpid(), sizeof(orig_set), &orig_set) < 0) {
		do_io_error(sprintfxx("can't get CPU affinity: %s",
		                    strerror(errno)));
	}

	// set affinity to desired CPU
	CPU_ZERO(&new_set);
	CPU_SET(m_address.cpu, &new_set);
	if (sched_setaffinity(getpid(), sizeof(new_set), &new_set) < 0) {
		do_io_error(sprintfxx("can't set CPU affinity to %d: %s",
		                      m_address.cpu, strerror(errno)));
	}

	// call CPUID
	uint32_t regs[4];
	unsigned int function = Value(address & MASK(32)).as_uint();
	unsigned int argument = 
		Value((address >> 32) & MASK(32)).as_uint();
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
		do_io_error(sprintfxx("can't reset CPU affinity: %s",
		                      strerror(errno)));
	}

	util::BitBuffer bitbuf(width);
	memcpy(bitbuf.get(), regs, width/CHAR_BIT);

	return Value(bitbuf);
}

void
CpuidIo::write(const Value &address, const BitWidth width,
    const Value &value) const
{
	(void)address;
	(void)width;
	(void)value;
	// do nothing - CPUID is read-only
}

void
CpuidIo::do_io_error(const string &str) const
{
	throw Driver::IoError(to_string(m_address) + ": " + str);
}

void
CpuidIo::check_width(BitWidth width) const
{
	switch (width) {
	    case BITS128:
		break;
	    default:
		do_io_error(sprintfxx("unsupported register width %d", width));
	}
}

void
CpuidIo::check_bounds(const Value &offset, unsigned bytes) const
{
	/* CPUID has a 64 bit address space */
	if (offset < 0 || offset > MASK(64)) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}

}  // namespace hwpp
