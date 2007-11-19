/* Copyrioht (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_IO_LINUX_IO_IO_H__
#define PP_DRIVERS_IO_LINUX_IO_IO_H__

#include <stdint.h>
#include <vector>
#include <algorithm>

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

#define IO_DEVICE	"/dev/port"

/*
 * Linux-specific port IO
 */
class linux_io_io: public io_io
{
    public:
	/* constructors */
	explicit
	linux_io_io(pp_regaddr base, pp_regaddr size)
	    : m_address(io_address(base, size))
	{
		open_device();
	}
	explicit
	linux_io_io(const io_address &address)
	    : m_address(address)
	{
		open_device();
	}

	/* destructor */
	virtual
	~linux_io_io()
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
		    default:
			break;
		}
		throw do_io_error(to_string(
		    boost::format("unsupported register width %d")
		    %width));
	}

    private:
	io_address m_address;
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
			m_file = fs::file::open(IO_DEVICE, O_RDONLY);
			return;
		} catch (fs::not_found_error &e) {
			/* the device seems to not exist */
			throw do_io_error("can't open " IO_DEVICE);
		}
	}

	void
	seek(const pp_regaddr offset) const
	{
		if (offset >= m_address.size) {
			throw do_io_error(to_string(
			    boost::format("can't access register 0x%x")
			    %offset));
		}

		m_file->seek(m_address.base+offset, SEEK_SET);
	}

	template<typename Tdata>
	pp_value
	do_read(const pp_regaddr offset) const
	{
		seek(offset);
		Tdata data;
		if (m_file->read(&data, sizeof(data)) != sizeof(data)) {
			throw do_io_error(to_string(
			    boost::format("can't read register 0x%x")
			    %offset));
		}
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

		seek(offset);
		Tdata data = value;
		if (m_file->write(&data, sizeof(data)) != sizeof(data)) {
			throw do_io_error(to_string(
			    boost::format("can't write register 0x%x")
			    %offset));
		}
	}
};

#endif // PP_DRIVERS_IO_LINUX_IO_IO_H__
