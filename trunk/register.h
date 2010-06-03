/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_REGISTER_H__
#define PP_REGISTER_H__

#include "pp.h"
#include "dirent.h"

namespace pp {

/*
 * Register - abstract base class for a register
 */
class Register: public Dirent
{
    protected:
	BitWidth m_width;

    public:
	explicit Register(const BitWidth width)
	    : Dirent(DIRENT_TYPE_REGISTER), m_width(width)
	{
	}
	virtual ~Register()
	{
	}

	/*
	 * Register::width()
	 *
	 * Get the access width of this register.
	 */
	BitWidth
	width() const
	{
		return m_width;
	}

	/*
	 * Register::describe()
	 *
	 * Get a string which describes this register
	 */
	virtual string
	describe() const = 0;

	/*
	 * Register::read()
	 *
	 * Read the value of this register.
	 */
	virtual Value
	read() const = 0;

	/*
	 * Register::write(value)
	 *
	 * Write a value to this register.
	 */
	virtual void
	write(const Value &value) const = 0;
};
typedef boost::shared_ptr<Register> RegisterPtr;
typedef boost::shared_ptr<const Register> ConstRegisterPtr;

// const
inline ConstRegisterPtr
register_from_dirent(const ConstDirentPtr &dirent)
{
	if (dirent->is_register()) {
		return static_pointer_cast<const Register>(dirent);
	}
	throw Dirent::ConversionError(
	    "non-register dirent used as register");
}
// non-const
inline RegisterPtr
register_from_dirent(const DirentPtr &dirent)
{
	const ConstDirentPtr &const_dirent = dirent;
	return const_pointer_cast<Register>(
	       register_from_dirent(const_dirent));
}

}  // namespace pp

#endif // PP_REGISTER_H__
