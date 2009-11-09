/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_FIELDS_H__
#define PP_FIELDS_H__

#include "pp/field.h"
#include "pp/datatype.h"
#include "pp/register.h"
#include "pp/regbits.h"
#include "pp/rwprocs.h"
#include "pp/context.h"
#include "runtime.h"
#include <vector>

namespace pp {

/*
 * DirectField - a field that maps directly to register bits.
 */
class DirectField: public Field
{
    public:
	DirectField(const ConstDatatypePtr &datatype, const RegBits &regbits)
	    : Field(datatype), m_regbits(regbits)
	{
	}
	virtual ~DirectField()
	{
	}

	/*
	 * DirectField::describe()
	 */
	virtual string
	describe() const
	{
		return m_regbits.describe();
	}

	/*
	 * DirectField::read()
	 *
	 * Read the current value of this field.
	 *
	 * Throws: Driver::IoError
	 */
	virtual Value
	read() const
	{
		return m_regbits.read();
	}

	/*
	 * DirectField::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: Driver::IoError
	 */
	virtual void
	write(const Value &value) const
	{
		m_regbits.write(value);
	}

    private:
	RegBits m_regbits;
};
typedef boost::shared_ptr<DirectField> DirectFieldPtr;

#define new_pp_direct_field(...) \
	::pp::DirectFieldPtr(new ::pp::DirectField(__VA_ARGS__))

/*
 * ProcField - a field that is a procedure.
 *
 * Notes:
 */
class ProcField: public Field
{
    public:
	ProcField(const ConstDatatypePtr &datatype,
	    const RwProcsPtr &access)
	    : Field(datatype), m_access(access),
	      m_context(pp::runtime::context_snapshot())
	{
	}
	virtual ~ProcField()
	{
	}

	/*
	 * ProcField::describe()
	 */
	virtual string
	describe() const
	{
		return "procedures";
	}

	/*
	 * ProcField::read()
	 *
	 * Read the current value of this field.
	 *
	 * Throws: Driver::IoError
	 */
	virtual Value
	read() const
	{
		pp::runtime::context_push(m_context);
		Value ret = m_access->read();
		pp::runtime::context_pop();
		return ret;
	}

	/*
	 * ProcField::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: Driver::IoError
	 */
	virtual void
	write(const Value &value) const
	{
		pp::runtime::context_push(m_context);
		m_access->write(value);
		pp::runtime::context_pop();
	}

    private:
	RwProcsPtr m_access;
	ContextPtr m_context;
};
typedef boost::shared_ptr<ProcField> ProcFieldPtr;

#define new_pp_proc_field(...) \
	::pp::ProcFieldPtr(new ::pp::ProcField(__VA_ARGS__))

/*
 * ConstantField - a field that returns a constant value.
 */
class ConstantField: public Field
{
    public:
	ConstantField(const ConstDatatypePtr &datatype,
	    const Value &value)
	    : Field(datatype), m_value(value)
	{
	}
	virtual ~ConstantField()
	{
	}

	/*
	 * ConstantField::describe()
	 */
	virtual string
	describe() const
	{
		return "constant";
	}

	/*
	 * ConstantField::read()
	 *
	 * Read the current value of this field.
	 */
	virtual Value
	read() const
	{
		return m_value;
	}

	/*
	 * ConstantField::write(value)
	 *
	 * Write a value to this field.
	 *
	 * Throws: Driver::IoError
	 */
	virtual void
	write(const Value &value) const
	{
		(void)value;
		// discard writes
	}

    private:
	Value m_value;
};
typedef boost::shared_ptr<ConstantField> ConstantFieldPtr;

#define new_pp_constant_field(...) \
	::pp::ConstantFieldPtr(new ::pp::ConstantField(__VA_ARGS__))

}  // namespace pp

#endif // PP_FIELDS_H__
