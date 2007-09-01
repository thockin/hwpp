/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_IO_IO_BINDING_H__
#define PP_DRIVERS_IO_IO_BINDING_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

#include "pp_binding.h"
#include "io_driver.h"
#include "linux_io_io.h"

/*
 * io_binding - IO binding for register spaces
 *
 * Constructors:
 *	(pp_regaddr_t address, io_io *io?)
 *
 * Notes:
 */
class io_binding: public pp_binding
{
    public:
	explicit io_binding(io_address address, io_io *io = NULL)
	    : m_address(address), m_io(io ? io : new linux_io_io(address))
	{
	}

	virtual ~io_binding()
	{
	}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const
	{
		return m_io->read(address, width);
	}

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const
	{
		return m_io->write(address, width, value);
	}

	virtual string
	to_string() const
	{
		return ::to_string(m_address);
	}

	const io_address &
	address()
	{
		return m_address;
	}

    private:
	const io_address m_address;
	boost::shared_ptr<io_io> m_io;
};
typedef boost::shared_ptr<io_binding> io_binding_ptr;

#define new_io_binding(...) io_binding_ptr(new io_binding(__VA_ARGS__))

#endif // PP_DRIVERS_IO_IO_BINDING_H__
