/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MEM_MEM_BINDING_H__
#define PP_DRIVERS_MEM_MEM_BINDING_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

#include "pp_binding.h"
#include "mem_driver.h"
#include "linux_mem_io.h"

/*
 * mem_binding - IO binding for register spaces
 *
 * Constructors:
 *	(pp_regaddr_t address, mem_io *io?)
 *
 * Notes:
 */
class mem_binding: public pp_binding
{
    public:
	explicit mem_binding(mem_address address, mem_io *io = NULL)
	    : m_address(address), m_io(io ? io : new linux_mem_io(address))
	{
	}

	virtual ~mem_binding()
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

	const mem_address &
	address()
	{
		return m_address;
	}

    private:
	const mem_address m_address;
	boost::shared_ptr<mem_io> m_io;
};
typedef boost::shared_ptr<mem_binding> mem_binding_ptr;

#define new_mem_binding(...) mem_binding_ptr(new mem_binding(__VA_ARGS__))

#endif // PP_DRIVERS_MEM_MEM_BINDING_H__
