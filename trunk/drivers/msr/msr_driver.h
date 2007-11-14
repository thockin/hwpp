/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MSR_MSR_DRIVER_H__
#define PP_DRIVERS_MSR_MSR_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"

/*
 * msr_address
 */
struct msr_address
{
	/* constructors */
	msr_address()
	    : cpu(-1)
	{
	}
	msr_address(int c)
	    : cpu(cpu)
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
 * Abstract MSR IO
 */
class msr_io
{
    public:
	/* destructor */
	virtual
	~msr_io() {}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const = 0;

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) = 0;
};

/*
 * msr_driver - MSR driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class msr_driver: public pp_driver
{
    public:
	explicit msr_driver();
	virtual ~msr_driver();

	/*
	 * msr_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * msr_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_regaddr> &args) const;
};

#define new_msr_driver(...) pp_driver_ptr(new msr_driver(__VA_ARGS__))

#endif // PP_DRIVERS_MSR_MSR_DRIVER_H__
