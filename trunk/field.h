/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_FIELD_H__
#define PP_FIELD_H__

#include "pp.h"
#include "dirent.h"
#include "datatype.h"
#include <stdexcept>

namespace pp {

/*
 * Field - abstract base class for a data field.
 */
class Field: public Dirent
{
    public:
	explicit Field(const ConstDatatypePtr &datatype)
	    : Dirent(DIRENT_TYPE_FIELD), m_datatype(datatype) {}
	virtual ~Field() {}

	/*
	 * Field::describe()
	 *
	 * Get a string which describes this field
	 */
	virtual string
	describe() const = 0;

	/*
	 * Field::read()
	 *
	 * Read the current value of this field.
	 */
	virtual Value
	read() const = 0;

	/*
	 * Field::write(value)
	 *
	 * Write a value to this field.
	 */
	virtual void
	write(const Value &value) const = 0;

	/*
	 * Field::evaluate()
	 *
	 * Evaluate the value of this field against it's datatype. This
	 * method returns a string containing the evaluated representation
	 * of the field.
	 */
	virtual string
	evaluate() const
	{
		Value value = read();
		return m_datatype->evaluate(value);
	}

	/*
	 * Field::lookup(str)
	 * Field::lookup(value)
	 *
	 * Lookup the numeric value of a (potentially valid) evaluation for
	 * this field.
	 *
	 * This can throw Datatype::InvalidError.
	 */
	virtual Value
	lookup(const string &str) const
	{
		return m_datatype->lookup(str);
	}
	virtual Value
	lookup(const Value &value) const
	{
		return m_datatype->lookup(value);
	}

	/*
	 * Field::compare(str)
	 * Field::compare(value)
	 *
	 * Compare a field against another argument, according to the
	 * rules of this datatype.  Return an integer which indicates
	 * whether the lhs is less than, equal to, or greater than the rhs
	 * (negative, zero, positive return code, respectively).
	 *
	 * This can throw Datatype::InvalidError.
	 */
	virtual int
	compare(const string &comparator) const
	{
		return m_datatype->compare(read(), comparator);
	}
	virtual int
	compare(const Value &comparator) const
	{
		return m_datatype->compare(read(), comparator);
	}

	/*
	 * Field::test(str)
	 * Field::test(value)
	 *
	 * Perform a logical AND or a field and an argument.
	 */
	virtual Value
	test(const string &comparator) const
	{
		return m_datatype->test(read(), comparator);
	}
	virtual Value
	test(const Value &comparator) const
	{
		return m_datatype->test(read(), comparator);
	}

    private:
	ConstDatatypePtr m_datatype;
};
typedef boost::shared_ptr<Field> FieldPtr;
typedef boost::shared_ptr<const Field> ConstFieldPtr;

// const
inline ConstFieldPtr
field_from_dirent(const ConstDirentPtr &dirent)
{
	if (dirent->is_field()) {
		return static_pointer_cast<const Field>(dirent);
	}
	throw Dirent::ConversionError("non-field dirent used as field");
}
// non-const
inline FieldPtr
field_from_dirent(const DirentPtr &dirent)
{
	const ConstDirentPtr &const_dirent = dirent;
	return const_pointer_cast<Field>(field_from_dirent(const_dirent));
}

}  // namespace pp

#endif // PP_FIELD_H__
