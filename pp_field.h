/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_FIELD_H__
#define PP_PP_FIELD_H__

#include "pp.h"
#include "pp_dirent.h"
#include "pp_datatype.h"

#include <stdexcept>

/*
 * pp_field - abstract base class for a data field.
 *
 * Constructors:
 * 	(const pp_datatype_const_ptr &datatype)
 *
 * Notes:
 */
class pp_field: public pp_dirent
{
    public:
	explicit pp_field(const pp_datatype_const_ptr &datatype)
	    : pp_dirent(PP_DIRENT_FIELD), m_datatype(datatype) {}
	virtual ~pp_field() {}

	/*
	 * pp_field::describe()
	 *
	 * Get a string which describes this field
	 */
	virtual string
	describe() const = 0;

	/*
	 * pp_field::read()
	 *
	 * Read the current value of this field.
	 */
	virtual pp_value
	read() const = 0;

	/*
	 * pp_field::write(value)
	 *
	 * Write a value to this field.
	 */
	virtual void
	write(const pp_value &value) const = 0;

	/*
	 * pp_field::evaluate()
	 *
	 * Evaluate the value of this field against it's datatype. This
	 * method returns a string containing the evaluated representation
	 * of the field.
	 */
	virtual string
	evaluate() const
	{
		pp_value value = read();
		return m_datatype->evaluate(value);
	}

	/*
	 * pp_field::lookup(str)
	 * pp_field::lookup(value)
	 *
	 * Lookup the numeric value of a (potentially valid) evaluation for
	 * this field.
	 *
	 * This can throw pp_datatype::invalid_error.
	 */
	virtual pp_value
	lookup(const string &str) const
	{
		return m_datatype->lookup(str);
	}
	virtual pp_value
	lookup(const pp_value &value) const
	{
		return m_datatype->lookup(value);
	}

	/*
	 * pp_field::compare(str)
	 * pp_field::compare(value)
	 *
	 * Compare a field against another argument, according to the
	 * rules of this datatype.  Return an integer which indicates
	 * whether the lhs is less than, equal to, or greater than the rhs
	 * (negative, zero, positive return code, respectively).
	 *
	 * This can throw pp_datatype::invalid_error.
	 */
	virtual int
	compare(const string &comparator) const
	{
		return m_datatype->compare(read(), comparator);
	}
	virtual int
	compare(const pp_value &comparator) const
	{
		return m_datatype->compare(read(), comparator);
	}

	/*
	 * pp_field::test(str)
	 * pp_field::test(value)
	 *
	 * Perform a logical AND or a field and an argument.
	 */
	virtual pp_value
	test(const string &comparator) const
	{
		return m_datatype->test(read(), comparator);
	}
	virtual pp_value
	test(const pp_value &comparator) const
	{
		return m_datatype->test(read(), comparator);
	}

    private:
	pp_datatype_const_ptr m_datatype;
};
typedef boost::shared_ptr<pp_field> pp_field_ptr;
typedef boost::shared_ptr<const pp_field> pp_field_const_ptr;

// const
inline pp_field_const_ptr
pp_field_from_dirent(const pp_dirent_const_ptr &dirent)
{
	if (dirent->is_field()) {
		return static_pointer_cast<const pp_field>(dirent);
	}
	throw pp_dirent::conversion_error("non-field dirent used as field");
}
// non-const
inline pp_field_ptr
pp_field_from_dirent(const pp_dirent_ptr &dirent)
{
	const pp_dirent_const_ptr &const_dirent = dirent;
	return const_pointer_cast<pp_field>(
	       pp_field_from_dirent(const_dirent));
}

#endif // PP_PP_FIELD_H__
