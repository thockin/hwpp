/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_MSR_BINDING_H__
#define PP_DRIVERS_MSR_MSR_BINDING_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

#include "pp_binding.h"
#include "msr_driver.h"
#include "linux_msr_io.h"

/*
 * msr_binding - MSR binding for register spaces
 *
 * Constructors:
 *	(msr_address address, msr_io *io?)
 *
 * Notes:
 */
//FIXME: make a generic_binding that wraps this up
//FIXME: use a template for the IO argument
class msr_binding: public pp_binding
{
    public:
	explicit msr_binding(msr_address address, msr_io *io = NULL)
	    : m_address(address), m_io(io ? io : new linux_msr_io(address))
	{
	}

	virtual ~msr_binding()
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

	const msr_address &
	address()
	{
		return m_address;
	}

    private:
	msr_address m_address;
	boost::shared_ptr<msr_io> m_io;
};
typedef boost::shared_ptr<msr_binding> msr_binding_ptr;

#define new_msr_binding(...) msr_binding_ptr(new msr_binding(__VA_ARGS__))

#endif // PP_DRIVERS_MSR_MSR_BINDING_H__
