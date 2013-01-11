/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_BINDING_H__
#define HWPP_BINDING_H__

#include "hwpp.h"
#include <stdexcept>
#include <ostream>

namespace hwpp {

/*
 * Binding - abstract base class for bound register spaces.
 */
class Binding
{
    public:
	Binding() {}
	virtual ~Binding() {}

	/*
	 * Binding::read(address, width)
	 *
	 * Read from a register in this binding.
	 *
	 * Throws: Driver::IoError
	 */
	virtual Value
	read(const Value &address, const BitWidth width) const = 0;

	/*
	 * Binding::write(address, width, value)
	 *
	 * Write to a register in this binding.
	 *
	 * Throws: Driver::IoError
	 */
	virtual void
	write(const Value &address, const BitWidth width,
	    const Value &value) const = 0;

	/*
	 * Binding::to_string()
	 *
	 * Return a string representing this binding.
	 */
	virtual string
	to_string() const = 0;
};
typedef boost::shared_ptr<Binding> BindingPtr;
typedef boost::shared_ptr<const Binding> ConstBindingPtr;

inline std::ostream &
operator<<(std::ostream& o, const Binding &binding)
{
	return o << binding.to_string();
}

/*
 * SimpleBinding - a simple binding
 */
template<class Tio, class Taddress>
class SimpleBinding: public Binding
{
    public:
	explicit SimpleBinding(Taddress address)
	    : m_io(address)
	{
	}

	virtual Value
	read(const Value &address, const BitWidth width) const
	{
		return m_io.read(address, width);
	}

	virtual void
	write(const Value &address, const BitWidth width,
	    const Value &value) const
	{
		return m_io.write(address, width, value);
	}

	virtual string
	to_string() const
	{
		return ::to_string(m_io.address());
	}

	const Taddress &
	address()
	{
		return m_io.address();
	}

    private:
	Tio m_io;
};

}  // namespace hwpp

#endif // HWPP_BINDING_H__
