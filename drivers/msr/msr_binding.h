/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_MSR_BINDING_H__
#define PP_DRIVERS_MSR_MSR_BINDING_H__

#include "pp.h"
#include "pp_binding.h"
#include "pp_driver.h"
#include "filesystem.h"

/*
 * msr_address
 */
struct msr_address
{
	/* constructor */
	msr_address()
	    : cpu(-1)
	{
	}
	explicit
	msr_address(int c)
	    : cpu(c)
	{
	}

	int cpu;
};

inline bool
operator<(const msr_address &left, const msr_address &right)
{
	return (left.cpu < right.cpu);
}

inline std::ostream &
operator<<(std::ostream& out, const msr_address &addr)
{
	out << "msr<" << addr.cpu << ">";
	return out;
}

/*
 * msr_io - Linux-specific MSR access
 */

class msr_io
{
    public:
	msr_io(const msr_address &address,
	    const string &devdir = "", int major = -1, int minor = -1);
	~msr_io();

	const msr_address &
	address() const;

	pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const;

	void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) const;

    private:
	msr_address m_address;
	fs::file_ptr m_file;

	pp_driver_io_error
	do_io_error(const string &str) const;

	void
	open_device(string devdir, int major, int minor);

	void
	seek(const pp_regaddr offset) const;

	template<typename Tdata>
	pp_value
	do_read(const pp_regaddr offset) const;

	template<typename Tdata>
	void
	do_write(const pp_regaddr offset, const pp_value value) const;
};

/*
 * msr_binding - MSR binding for register spaces
 *
 * Constructors:
 *	(msr_address address)
 */
typedef simple_binding<msr_io, msr_address> msr_binding;

#define new_msr_binding(...) pp_binding_ptr(new msr_binding(__VA_ARGS__))

#endif // PP_DRIVERS_MSR_MSR_BINDING_H__
