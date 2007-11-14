/* Copyrioht (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_LINUX_MSR_IO_H__
#define PP_DRIVERS_MSR_LINUX_MSR_IO_H__

#include <stdint.h>
#include <vector>
#include <algorithm>

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

#define MSR_DEVICE_DIR	"/dev/cpu"

/*
 * Linux-specific MSR IO
 */
class linux_msr_io: public msr_io
{
    public:
	/* constructors */
	explicit
	linux_msr_io(int cpu)
	    : m_address(msr_address(cpu))
	{
		open_device();
	}
	explicit
	linux_msr_io(const msr_address &address)
	    : m_address(address)
	{
		open_device();
	}

	/* destructor */
	virtual
	~linux_msr_io()
	{
		m_file->close();
	}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		switch (width) {
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
	msr_address m_address;
	fs::file_ptr m_file;

	pp_driver_io_error
	do_io_error(const string &str) const
	{
		return pp_driver_io_error(to_string(m_address) + ": " + str);
	}

	void
	open_device()
	{
		string filename;

		/* try to open it through /dev */
		filename = to_string(
		    boost::format("%s/%d/msr") %MSR_DEVICE_DIR %m_address.cpu);
		try {
			m_file = fs::file::open(filename, O_RDONLY);
			return;
		} catch (fs::not_found_error e) {
			/* do nothing yet */
		}

		/* fall back on a self-made device node */
		#if 0 //FIXME:
		filename = to_string(
		    boost::format("%s/%02x/%02x.%x")
		    %MSR_PROCFS_DIR %m_address.bus %m_address.device
		    %m_address.function);
		#endif
		try {
			m_file = fs::file::open(filename, O_RDONLY);
			return;
		} catch (fs::not_found_error e) {
			/* the device seems to not exist */
			throw do_io_error("device does not exist");
		}
	}

	void
	seek(const pp_regaddr offset) const
	{
		/* MSRs have 32 bit addresses */
		if (offset >= 0x100000000ULL) {
			throw do_io_error(to_string(
			    boost::format("can't access register 0x%x")
			    %offset));
		}

		m_file->seek(offset, SEEK_SET);
	}

	template<typename Tdata>
	pp_value
	do_read(const pp_regaddr offset) const
	{
		seek(offset);
		Tdata data;
		if (m_file->read(&data, sizeof(data)) != sizeof(data)) {
			throw do_io_error(to_string(
			    boost::format("error reading register 0x%x")
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
			    boost::format("error writing register 0x%x")
			    %offset));
		}
	}
};

#endif // PP_DRIVERS_MSR_LINUX_MSR_IO_H__
