/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_REGISTER_H__
#define PP_PP_REGISTER_H__

#include "pp.h"
#include "pp_dirent.h"

/*
 * pp_register - abstract base class for a register
 *
 * Constructors:
 * 	(const pp_bitwidth width)
 *
 * Notes:
 */
class pp_register: public pp_dirent
{
    protected:
	pp_bitwidth m_width;

    public:
	explicit pp_register(const pp_bitwidth width)
	    : pp_dirent(PP_DIRENT_REGISTER), m_width(width)
	{
	}
	virtual ~pp_register()
	{
	}

	/*
	 * pp_register::width()
	 *
	 * Get the access width of this register.
	 */
	pp_bitwidth
	width() const
	{
		return m_width;
	}

	/*
	 * pp_register::describe()
	 *
	 * Get a string which describes this register
	 */
	virtual string
	describe() const = 0;

	/*
	 * pp_register::read()
	 *
	 * Read the value of this register.
	 */
	virtual pp_value
	read() const = 0;

	/*
	 * pp_register::write(value)
	 *
	 * Write a value to this register.
	 */
	virtual void
	write(const pp_value &value) const = 0;
};
typedef boost::shared_ptr<pp_register> pp_register_ptr;
typedef boost::shared_ptr<const pp_register> pp_register_const_ptr;

// const
inline pp_register_const_ptr
pp_register_from_dirent(const pp_dirent_const_ptr &dirent)
{
	if (dirent->is_register()) {
		return static_pointer_cast<const pp_register>(dirent);
	}
	throw pp_dirent::conversion_error(
	    "non-register dirent used as register");
}
// non-const
inline pp_register_ptr
pp_register_from_dirent(const pp_dirent_ptr &dirent)
{
	const pp_dirent_const_ptr &const_dirent = dirent;
	return const_pointer_cast<pp_register>(
	       pp_register_from_dirent(const_dirent));
}

#endif // PP_PP_REGISTER_H__
