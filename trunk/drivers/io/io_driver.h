/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_IO_IO_DRIVER_H__
#define PP_DRIVERS_IO_IO_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"

/*
 * io_address
 */
struct io_address
{
	/* constructors */
	io_address(uint16_t b = -1, uint16_t s = -1): base(b), size(s)
	{
	}

	uint16_t base;
	uint16_t size;
};

inline std::ostream &
operator<<(std::ostream& out, const io_address &addr)
{
	out << boost::format("io<0x%04x,0x%04x>") %addr.base %addr.size;
	return out;
}

/*
 * Abstract port IO
 */
class io_io
{
    public:
	/* destructor */
	virtual
	~io_io() {}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const = 0;

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) = 0;
};

/*
 * io_driver - IO driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class io_driver: public pp_driver
{
    public:
	explicit io_driver();
	virtual ~io_driver();

	/*
	 * io_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * io_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_regaddr> &args) const;
};

#define new_io_driver(...) pp_driver_ptr(new io_driver(__VA_ARGS__))

#endif // PP_DRIVERS_IO_IO_DRIVER_H__
