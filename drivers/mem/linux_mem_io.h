/* Copyrioht (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MEM_LINUX_MEM_IO_H__
#define PP_DRIVERS_MEM_LINUX_MEM_IO_H__

#include <stdint.h>
#include <vector>
#include <algorithm>

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

#define MEM_DEVICE	"/dev/mem"

/*
 * Linux-specific port IO
 */
class linux_mem_io: public mem_io
{
    public:
	/* constructors */
	explicit
	linux_mem_io(pp_regaddr base, pp_regaddr size)
	    : m_address(mem_address(base, size))
	{
		open_device();
	}
	explicit
	linux_mem_io(const mem_address &address)
	    : m_address(address)
	{
		open_device();
	}

	/* destructor */
	virtual
	~linux_mem_io()
	{
		m_file->close();
	}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
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
			break;
		}
		throw do_io_error(to_string(
		    boost::format("unsupported register width %d")
		    %width));
	}

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value)
	{
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
			break;
		}
		throw do_io_error(to_string(
		    boost::format("unsupported register width %d")
		    %width));
	}

    private:
	mem_address m_address;
	fs::file_ptr m_file;

	pp_driver_io_error
	do_io_error(const string &str) const
	{
		return pp_driver_io_error(to_string(m_address) + ": " + str);
	}

	void
	open_device()
	{
		try {
			m_file = fs::file::open(MEM_DEVICE, O_RDONLY);
			return;
		} catch (fs::not_found_error e) {
			/* the device seems to not exist */
			throw do_io_error("can't open " MEM_DEVICE);
		}
	}

	fs::file_mapping_ptr
	map(const pp_regaddr offset, std::size_t length) const
	{
		if (offset >= m_address.size) {
			throw do_io_error(to_string(
			    boost::format("can't access register 0x%x")
			    %offset));
		}

		return m_file->mmap(m_address.base+offset, length);
	}

	template<typename Tdata>
	pp_value
	do_read(const pp_regaddr offset) const
	{
		fs::file_mapping_ptr mapping = map(offset, sizeof(Tdata));
		Tdata *ptr = (Tdata *)mapping->address();
		Tdata data = *ptr;
		return data;
	}

	template<typename Tdata>
	void
	do_write(const pp_regaddr offset, const pp_value value)
	{
		/* see if we are already open RW or can change to RW */
		if (m_file->mode() == O_RDONLY) {
			m_file->reopen(O_RDWR);
		}

		fs::file_mapping_ptr mapping = map(offset, sizeof(Tdata));
		Tdata *ptr = (Tdata *)mapping->address();
		Tdata data = value;
		*ptr = data;
	}
};

#endif // PP_DRIVERS_MEM_LINUX_MEM_IO_H__
