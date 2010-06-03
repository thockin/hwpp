#include "pp.h"
#include "util/printfxx.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

#include "mem_binding.h"
#include "driver.h"
#include "util/filesystem.h"

namespace pp { 

#define MEM_DEVICE	"/dev/mem"

/* constructor */
MemIo::MemIo(const MemAddress &address, const string &device)
    : m_address(address)
{
	open_device(device);
}

/* destructor */
MemIo::~MemIo()
{
	// m_file will close() when it's last reference goes away
}

const MemAddress &
MemIo::address() const
{
	return m_address;
}

// We do this differently than most other drivers because we are using
// mmap() under the covers, and we want to use single exact-width memory
// accesses.
Value
MemIo::read(const Value &address, const BitWidth width) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	switch (width) {
	    case BITS8:
		return do_read<uint8_t>(address);
	    case BITS16:
		return do_read<uint16_t>(address);
	    case BITS32:
		return do_read<uint32_t>(address);
	    case BITS64:
		return do_read<uint64_t>(address);
	    default:
		/* we already did bounds and width checking */
		break;
	}
	do_io_error("can't get here");
	return -1;
}

// We do this differently than most other drivers because we are using
// mmap() under the covers, and we want to use single exact-width memory
// accesses.
void
MemIo::write(const Value &address, const BitWidth width,
    const Value &value) const
{
	/* make sure this is a valid access */
	check_width(width);
	check_bounds(address, BITS_TO_BYTES(width));

	switch (width) {
	    case BITS8:
		return do_write<uint8_t>(address, value);
	    case BITS16:
		return do_write<uint16_t>(address, value);
	    case BITS32:
		return do_write<uint32_t>(address, value);
	    case BITS64:
		return do_write<uint64_t>(address, value);
	    default:
		/* we already did bounds and width checking */
		break;
	}
}

void
MemIo::do_io_error(const string &str) const
{
	throw Driver::IoError(to_string(m_address) + ": " + str);
}

void
MemIo::open_device(string device)
{
	if (device == "")
		device = MEM_DEVICE;

	m_file = filesystem::File::open(device, O_RDONLY | O_SYNC);
	return;
}

filesystem::FileMappingPtr
MemIo::map(const Value &offset, size_t length) const
{
	if (offset.get_uint()+length > m_address.size) {
		do_io_error(sprintfxx("can't access register 0x%x", offset));
	}

	return m_file->mmap(m_address.base+offset.get_uint(), length);
}

void
MemIo::check_width(BitWidth width) const
{
	switch (width) {
	    case BITS8:
	    case BITS16:
	    case BITS32:
	    case BITS64:
		break;
	    default:
		do_io_error(sprintfxx("unsupported register width %d", width));
	}
}

void
MemIo::check_bounds(const Value &offset, size_t bytes) const
{
	/* we support 64 bit memory */
	if (offset < 0 || (offset+bytes) > MASK(64)) {
		do_io_error(sprintfxx("invalid register: %d bytes @ 0x%x",
		                      bytes, offset));
	}
}

template<typename Tdata>
Value
MemIo::do_read(const Value &offset) const
{
	filesystem::FileMappingPtr mapping = map(offset, sizeof(Tdata));
	Tdata *ptr = (Tdata *)mapping->address();
	Tdata data = *ptr;
	return Value(data);
}

template<typename Tdata>
void
MemIo::do_write(const Value &offset, const Value &value) const
{
	/* see if we are already open RW or can change to RW */
	if (m_file->mode() == O_RDONLY) {
		m_file->reopen(O_RDWR | O_SYNC);
	}

	filesystem::FileMappingPtr mapping = map(offset, sizeof(Tdata));
	Tdata *ptr = (Tdata *)mapping->address();
	Tdata data = value.get_uint();
	*ptr = data;
}

}  // namespace pp
