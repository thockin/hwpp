/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_DRIVERS_MEM_MEM_DRIVER_H__
#define PP_DRIVERS_MEM_MEM_DRIVER_H__

#include "pp.h"
#include "pp_driver.h"

/*
 * mem_address
 */
struct mem_address
{
	/* constructors */
	mem_address(uint64_t b = -1, uint64_t s = -1): base(b), size(s)
	{
	}

	uint64_t base;
	uint64_t size;
};

inline std::ostream &
operator<<(std::ostream& out, const mem_address &addr)
{
	out << boost::format("mem<0x%016llx,0x%016llx>") %addr.base %addr.size;
	return out;
}

/*
 * Abstract mem IO
 */
class mem_io
{
    public:
	/* destructor */
	virtual
	~mem_io() {}

	virtual pp_value
	read(const pp_regaddr address, const pp_bitwidth width) const = 0;

	virtual void
	write(const pp_regaddr address, const pp_bitwidth width,
	    const pp_value value) = 0;
};

/*
 * mem_driver - IO driver plugin.
 *
 * Constructors:
 *	none
 *
 * Notes:
 */
class mem_driver: public pp_driver
{
    public:
	explicit mem_driver();
	virtual ~mem_driver();

	/*
	 * mem_driver::name()
	 *
	 * Get the name of this driver.
	 */
	virtual string
	name() const;

	/*
	 * mem_driver::new_binding(args)
	 *
	 * Create a new pp_binding.
	 *
	 * Throws: pp_driver_args_error
	 */
	virtual pp_binding_ptr
	new_binding(const std::vector<pp_regaddr> &args) const;
};

#define new_mem_driver(...) pp_driver_ptr(new mem_driver(__VA_ARGS__))

#endif // PP_DRIVERS_MEM_MEM_DRIVER_H__
