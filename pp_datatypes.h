/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DATATYPES_H__
#define PP_PP_DATATYPES_H__

#include "pp_datatype.h"
#include "keyed_vector.h"

/*
 * pp_enum - datatype for enumerated values.
 *
 * Constructors:
 * 	()
 *
 * Notes:
 */
class pp_enum: public pp_datatype
{
    public:
	explicit pp_enum(): m_default("<!!unknown!!>") {}
	virtual ~pp_enum() {}

	/*
	 * pp_enum::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values[i] == value) {
				return m_values.key_at(i);
			}
		}
		return m_default;
	}

	/*
	 * pp_enum::lookup(str)
	 * pp_enum::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an enum type, that means converting a string to
	 * it's corresponding value, or validating that a numeric value is
	 * a valid option.
	 *
	 * This will throw pp_datatype_invalid_error if the lookup is not a
	 * valid value for this enum.
	 */
	virtual pp_value
	lookup(const string &str) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values.key_at(i) == str) {
				return m_values[i];
			}
		}
		throw pp_datatype_invalid_error(str);
	}
	virtual pp_value
	lookup(const pp_value &value) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values[i] == value) {
				return m_values[i];
			}
		}
		throw pp_datatype_invalid_error(to_string(value));
	}

	/*
	 * pp_enum::add_value(name, value)
	 *
	 * Add a possible value to this enumeration.
	 */
	void
	add_value(const string &name, const pp_value &value)
	{
		DASSERT_MSG(m_values.find(name) == m_values.end(),
				"adding duplicate enum key: "
					+ name + " = " + to_string(value));
		m_values.insert(name, value);
	}

	/*
	 * pp_enum::set_default(name)
	 *
	 * Use a string for unknown enumerated values.
	 */
	void
	set_default(const string &name)
	{
		m_default = name;
	}

    private:
	keyed_vector<string, pp_value> m_values;
	string m_default;
};
typedef boost::shared_ptr<pp_enum> pp_enum_ptr;

#define new_pp_enum(...) pp_enum_ptr(new pp_enum(__VA_ARGS__))

/*
 * pp_bool - datatype for boolean values.
 *
 * Constructors:
 * 	(const string &true_str, const string &false_str)
 *
 * Notes:
 */
class pp_bool: public pp_enum
{
    public:
	explicit
	pp_bool(const string &true_str, const string &false_str)
	{
		add_value(true_str, 1);
		add_value(false_str, 0);
		set_default(true_str);
	}

	virtual pp_value
	lookup(const pp_value &value) const
	{
		return !!value;
	}
};
typedef boost::shared_ptr<pp_bool> pp_bool_ptr;

#define new_pp_bool(...) pp_bool_ptr(new pp_bool(__VA_ARGS__))

/*
 * pp_bitmask - datatype for bitmasks.
 * //FIXME: this is operating on bit numbers, not masks - should it?
 *
 * Constructors:
 * 	()
 *
 * Notes:
 */
class pp_bitmask: public pp_datatype
{
    public:
	explicit pp_bitmask(): m_default("") {}
	virtual ~pp_bitmask() {}

	/*
	 * pp_bitmask::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		string ret("");
		pp_value myval = value;

		for (size_t i=0; myval != 0 && i < m_bits.size(); i++) {
			pp_value b = m_bits[i];
			pp_value mask = (1 << b);
			if (myval & mask) {
				if (!ret.empty()) {
					ret += " ";
				}
				ret += m_bits.key_at(i);
				myval &= ~mask;
			}
		}

		int unknown = 0;
		while (myval) {
			if (myval & 1) {
				if (!ret.empty()) {
					ret += " ";
				}
				ret += boost::format("<!!bit%d!!>") %unknown;
			}
			myval >>= 1;
			unknown++;
		}

		if (ret == "") {
			ret = m_default;
		}
		return ret;
	}

	/*
	 * pp_bitmask::lookup(str)
	 * pp_bitmask::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For a bitmask type, this means converting a string to
	 * it's corresponding bit value.
	 *
	 * This will throw pp_datatype_invalid_error if the string is not a
	 * valid value in this bitmask.
	 */
	virtual pp_value
	lookup(const string &str) const
	{
		//FIXME: need to support multiple bits
		for (size_t i=0; i < m_bits.size(); i++) {
			if (m_bits.key_at(i) == str) {
				return m_bits[i];
			}
		}
		throw pp_datatype_invalid_error(str);
	}
	virtual pp_value
	lookup(const pp_value &value) const
	{
		return 0; //FIXME: missing
	}

	/*
	 * pp_bitmask::add_bit(name, value)
	 *
	 * Add a named bit to this bitmask.
	 */
	void
	add_bit(const string &name, const pp_value &value)
	{
		DASSERT_MSG(m_bits.find(name) == m_bits.end(),
				"adding duplicate bitmask key: "
					+ name + " = " + to_string(value));
		m_bits.insert(name, value);
	}

	/*
	 * pp_bitmask::set_default(name)
	 *
	 * Use a string for unknown enumerated values.
	 */
	void
	set_default(const string &name)
	{
		m_default = name;
	}

    private:
	string m_default;
	keyed_vector<string, pp_value> m_bits;
};
typedef boost::shared_ptr<pp_bitmask> pp_bitmask_ptr;

#define new_pp_bitmask(...) pp_bitmask_ptr(new pp_bitmask(__VA_ARGS__))

/*
 * pp_int - datatype for signed integer values.
 *
 * Constructors:
 * 	(const string &units = "")
 *
 * Notes:
 * 	This class makes a private copy of the 'units' argument.
 */
class pp_int: public pp_datatype
{
    public:
	explicit pp_int(const string &units = ""): m_units(units) {}
	virtual ~pp_int() {}

	/*
	 * pp_int::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		pp_svalue svalue(value);
		string ret = to_string(svalue);
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}

	/*
	 * pp_int::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an int type, this is a no-op.
	 */
	virtual pp_value
	lookup(const pp_value &value) const
	{
		return value;
	}
	virtual pp_value
	lookup(const string &str) const
	{
		return pp_value_from_string(str);
	}

    protected:
	string m_units;
};
typedef boost::shared_ptr<pp_int> pp_int_ptr;

#define new_pp_int(...) pp_int_ptr(new pp_int(__VA_ARGS__))

/*
 * pp_uint - datatype for unsigned integer values.
 *
 * Constructors:
 * 	(const string &units = "")
 *
 * Notes:
 * 	This class makes a private copy of the 'units' argument.
 */
class pp_uint: public pp_int
{
    public:
	explicit pp_uint(const string &units = ""): pp_int(units) {}
	virtual ~pp_uint() {}

	/*
	 * pp_uint::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		string ret = to_string(value);
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}
};
typedef boost::shared_ptr<pp_uint> pp_uint_ptr;

#define new_pp_uint(...) pp_uint_ptr(new pp_uint(__VA_ARGS__))

/*
 * pp_hex - datatype for hexadecimal values.
 *
 * Constructors:
 * 	(const pp_bitwidth width, const string &units="")
 *
 * Notes:
 */
class pp_hex: public pp_int
{
    public:
	explicit pp_hex(const pp_bitwidth width = BITS0,
			const string &units = "")
			: pp_int(units), m_width(width) {}
	virtual ~pp_hex() {}

	/*
	 * pp_hex::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		string fmt = "0x%0" + to_string(m_width/4) + "x";
		string ret = to_string(boost::format(fmt) %value);
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}

    private:
	pp_bitwidth m_width;
};
typedef boost::shared_ptr<pp_hex> pp_hex_ptr;

#define new_pp_hex(...) pp_hex_ptr(new pp_hex(__VA_ARGS__))

#endif // PP_PP_DATATYPES_H__
