/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_DATATYPES_H__
#define PP_PP_DATATYPES_H__

#include "pp.h"
#include "pp_datatype.h"
#include "keyed_vector.h"
#include "language.h"

/*
 * pp_multi_datatype - datatype for combining multiple subtypes based
 *		       on the value being read
 *
 * NOTE: range is between min and max inclusive
 */
class pp_multi_datatype: public pp_datatype
{
    private:
	struct pp_multi_range
	{
		pp_multi_range(const pp_datatype_const_ptr &datatype,
		               pp_value min, pp_value max)
		    : type(datatype), low(min), high(max) {}
		pp_datatype_const_ptr type;
		pp_value low;
		pp_value high;
	};
	std::vector<pp_multi_range> m_parts;

	static bool
	val_in_range(const pp_value &value,
		     const pp_multi_range &range) {
		return ((value >= range.low) && (value <= range.high));
	}

    public:
	pp_multi_datatype()
	{
	}

	virtual ~pp_multi_datatype()
	{
	}

	/*
	 * pp_multi_datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the representation of the 'value' argument,
	 * evaluated with the datatype (if any) assigned to the given value's
	 * range.
	 *
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		for (size_t i = 0; i < m_parts.size(); i++) {
			if (val_in_range(value, m_parts[i])) {
				return m_parts[i].type->evaluate(value);
			}
		}

		// Value out of range; return unknown indicator
		return to_string(boost::format("<!%d!>") %value);
	}

	/*
	 * pp_multi_datatype::lookup(str)
	 * pp_multi_datatype::lookup(value) -- simply validate it's in range
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an multi type, that means converting a string to
	 * it's corresponding value, or validating that a numeric value is
	 * a valid option. 
	 *
	 * WARNING: in case the given string could map to multiple different
	 * pp_values, the lowest possible value is returned
	 *
	 * This will throw pp_datatype::invalid_error if the lookup is not a
	 * valid value for this enum.
	 *
	 */
	virtual pp_value
	lookup(const string &str) const
	{
		for (size_t i = 0; i < m_parts.size(); i++) {
			try {
				pp_value val = m_parts[i].type->lookup(str);
				if (val_in_range(val, m_parts[i])) {
					return val;
				}
			} catch (pp_datatype::invalid_error &e) {
				// Keep trying on exception
			}
		}
		throw pp_datatype::invalid_error("Could not match given string "
				"to a range: " + str);
	}

	virtual pp_value
	lookup(const pp_value &value) const
	{
		for (size_t i = 0; i < m_parts.size(); i++) {
			if (val_in_range(value, m_parts[i])) {
				return value;
			}
		}
		throw pp_datatype::invalid_error("Given value out of range: " +
				to_string(value));
	}

	/*
	 * pp_multi_datatype::add_range(pp_multi_range)
	 * pp_multi_datatype::add_range(datatype, min, max)
	 *
	 * Add a new range->datatype mapping to this multi datatype.
	 *
	 */
	void
	add_range(const pp_multi_range &range)
	{
		add_range(range.type, range.low, range.high);
	}
	void
	add_range(const pp_datatype_const_ptr &datatype,
		  pp_value min, pp_value max)
	{
		if (min > max) {
			throw pp_datatype::invalid_error("Minimum value " +
					to_string(min) + " cannot be higher "
					"than maximum value " + to_string(max));
		}

		// If the vector is empty, we can add it right away
		if (m_parts.size() == 0) {
			m_parts.push_back(
				pp_multi_range(datatype, min, max));
			return;
		}

		// See if we can add it in the middle
		std::vector<pp_multi_range>::iterator iter;
		for (iter = m_parts.begin(); iter != m_parts.end(); iter++) {
			if (min > iter->high) {
				continue;
			} else if (max < iter->low) {
				m_parts.insert(iter, pp_multi_range(
							datatype, min, max));
				return;
			} else {
				throw pp_datatype::invalid_error(
					"Cannot have overlapping ranges");
			}
		}

		// Range is higher than everything else; add to end
		m_parts.push_back(
			pp_multi_range(datatype, min, max));
	}
};
typedef boost::shared_ptr<pp_multi_datatype> pp_multi_datatype_ptr;

#define new_pp_multi_datatype(...) \
		pp_multi_datatype_ptr( \
			new pp_multi_datatype(__VA_ARGS__))

/*
 * pp_enum_datatype - datatype for enumerated values.
 *
 * NOTE: Enum keys must be unique.
 */
class pp_enum_datatype: public pp_datatype
{
    private:
	keyed_vector<string, pp_value> m_values;
	string m_unknown;
	bool m_custom_unknown;

    public:
	pp_enum_datatype()
	    : m_custom_unknown(false)
	{
	}
	explicit pp_enum_datatype(const keyed_vector<string, pp_value> &values)
	    : m_values(values), m_custom_unknown(false)
	{
	}
	virtual ~pp_enum_datatype()
	{
	}

	/*
	 * pp_enum_datatype::evaluate(value)
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
		if (m_custom_unknown) {
			return m_unknown;
		}
		return to_string(boost::format("<!%d!>") %value);
	}

	/*
	 * pp_enum_datatype::lookup(str)
	 * pp_enum_datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an enum type, that means converting a string to
	 * it's corresponding value, or validating that a numeric value is
	 * a valid option.
	 *
	 * This will throw pp_datatype::invalid_error if the lookup is not a
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
		throw pp_datatype::invalid_error(str);
	}
	virtual pp_value
	lookup(const pp_value &value) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values[i] == value) {
				return m_values[i];
			}
		}
		throw pp_datatype::invalid_error(to_string(value));
	}

	/*
	 * pp_enum_datatype::add_value(name, value)
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
	 * pp_enum_datatype::set_unknown(name)
	 *
	 * Use a string for unknown enumerated values.
	 */
	void
	set_unknown(const string &name)
	{
		m_unknown = name;
		m_custom_unknown = true;
	}
};
typedef boost::shared_ptr<pp_enum_datatype> pp_enum_datatype_ptr;

#define new_pp_enum_datatype(...) \
		pp_enum_datatype_ptr(new pp_enum_datatype(__VA_ARGS__))

/*
 * pp_bool_datatype - datatype for boolean values.
 */
class pp_bool_datatype: public pp_enum_datatype
{
    public:
	pp_bool_datatype(const string &true_str, const string &false_str)
	{
		add_value(true_str, 1);
		add_value(false_str, 0);
		set_unknown(true_str);
	}

	virtual pp_value
	lookup(const pp_value &value) const
	{
		return value == 0 ? 0 : 1;
	}
	virtual pp_value
	lookup(const string &str) const
	{
		return pp_enum_datatype::lookup(str);
	}
};
typedef boost::shared_ptr<pp_bool_datatype> pp_bool_datatype_ptr;

#define new_pp_bool_datatype(...) \
		pp_bool_datatype_ptr(new pp_bool_datatype(__VA_ARGS__))

/*
 * pp_bitmask_datatype - datatype for bitmasks.
 *
 * NOTE: Bit names must be unique.
 * //FIXME: this is operating on bit numbers, not masks - should it?
 */
class pp_bitmask_datatype: public pp_datatype
{
    private:
	keyed_vector<string, pp_value> m_bits;

    public:
	pp_bitmask_datatype()
	{
	}
	explicit pp_bitmask_datatype(const keyed_vector<string, pp_value> &bits)
	    : m_bits(bits)
	{
	}
	virtual ~pp_bitmask_datatype()
	{
	}

	/*
	 * pp_bitmask_datatype::evaluate(value)
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
			if ((myval & mask) != 0) {
				if (!ret.empty()) {
					ret += " ";
				}
				ret += m_bits.key_at(i);
				myval ^= (myval & mask);
			}
		}

		int unknown = 0;
		while (myval != 0) {
			if ((myval & pp_value(1)) == 1) {
				if (!ret.empty()) {
					ret += " ";
				}
				ret += boost::format("<!%d!>") %unknown;
			}
			myval >>= 1;
			unknown++;
		}

		return ret;
	}

	/*
	 * pp_bitmask_datatype::lookup(str)
	 * pp_bitmask_datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For a bitmask type, this means converting a string to
	 * it's corresponding bit value.
	 *
	 * This will throw pp_datatype::invalid_error if the string is not a
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
		throw pp_datatype::invalid_error(str);
	}
	virtual pp_value
	lookup(const pp_value &value) const
	{
		//FIXME: need to support multiple bits
		for (size_t i=0; i < m_bits.size(); i++) {
			if (m_bits.at(i) == value) {
				return m_bits[i];
			}
		}
		throw pp_datatype::invalid_error(to_string(value));
	}

	/*
	 * pp_bitmask_datatype::add_bit(name, value)
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
};
typedef boost::shared_ptr<pp_bitmask_datatype> pp_bitmask_datatype_ptr;

#define new_pp_bitmask_datatype(...) \
		pp_bitmask_datatype_ptr(new pp_bitmask_datatype(__VA_ARGS__))

/*
 * pp_string_datatype - datatype for string values
 *
 * This datatype interprets a given value as a string in little-endian order,
 * meaning that the least significant byte in the input is the first
 * character in the string. For example, 0x6f6c6c6548 produces "Hello" (where
 * 0x48 == 'H').
 *
 * Internal non-printing characters are ignored for display purposes. For
 * example, 0x6f6c6c00006548 also produces "Hello". However, they are
 * preserved in the string so that the same pp_value is returned when
 * using lookup() as that which was given to evaluate() in order to produce
 * the string in the first place. That is, lookup() and evaluate() are inverse.
 */
class pp_string_datatype: public pp_datatype
{
    public:
	pp_string_datatype()
	{
	}
	virtual ~pp_string_datatype()
	{
	}

	/*
	 * pp_string_datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype. This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		bitbuffer bits = value.get_bitbuffer();
		return string((char *)bits.get(), bits.size_bytes());
	}

	/*
	 * pp_string_datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype. For a string type, this is a no-op.
	 */
	virtual pp_value
	lookup(const pp_value &value) const
	{
		return value;
	}
	virtual pp_value
	lookup(const string &str) const
	{
		bitbuffer bits(BYTES_TO_BITS(str.size()),
			       (uint8_t *)str.data());
		return pp_value(bits);
	}

};
typedef boost::shared_ptr<pp_string_datatype> pp_string_datatype_ptr;

#define new_pp_string_datatype(...) \
		pp_string_datatype_ptr(new pp_string_datatype(__VA_ARGS__))

/*
 * pp_int_datatype - datatype for signed integer values.
 *
 * Notes:
 * 	This class makes a private copy of the 'units' argument.
 */
class pp_int_datatype: public pp_datatype
{
    public:
	explicit pp_int_datatype(const string &units = "")
	    : m_units(units)
	{
	}
	virtual ~pp_int_datatype()
	{
	}

	/*
	 * pp_int_datatype::evaluate(value)
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

	/*
	 * pp_int_datatype::lookup(value)
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
		try {
			return pp_value(str);
		} catch (std::invalid_argument &e) {
			throw pp_datatype::invalid_error(str);
		}
	}

    protected:
	string m_units;
};
typedef boost::shared_ptr<pp_int_datatype> pp_int_datatype_ptr;

#define new_pp_int_datatype(...) \
		pp_int_datatype_ptr(new pp_int_datatype(__VA_ARGS__))

/*
 * pp_hex_datatype - datatype for hexadecimal values.
 */
class pp_hex_datatype: public pp_int_datatype
{
    public:
	explicit pp_hex_datatype(const pp_bitwidth width = BITS0,
	                         const string &units = "")
	    : pp_int_datatype(units), m_width(width)
	{
	}
	virtual ~pp_hex_datatype()
	{
	}

	/*
	 * pp_hex_datatype::evaluate(value)
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
typedef boost::shared_ptr<pp_hex_datatype> pp_hex_datatype_ptr;

#define new_pp_hex_datatype(...) \
		pp_hex_datatype_ptr(new pp_hex_datatype(__VA_ARGS__))

/*
 * pp_transform_datatype - datatype wrapper to perform a transform on data
 * before passing off to another datatype.
 */
template<typename Tdefunc, typename Tenfunc>
class pp_transform_datatype: public pp_datatype
{
    private:
	pp_datatype_const_ptr m_real_type;
	Tdefunc m_decode_func;
	Tenfunc m_encode_func;

    public:
	pp_transform_datatype(const pp_datatype_const_ptr &real_type,
	    const Tdefunc &decode_func, const Tenfunc &encode_func)
	    : m_real_type(real_type), m_decode_func(decode_func),
	      m_encode_func(encode_func)
	{
	}
	virtual ~pp_transform_datatype()
	{
	}

	/*
	 * pp_transform_datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &raw) const
	{
		pp_value cooked = m_decode_func(raw);
		return m_real_type->evaluate(cooked);
	}

	/*
	 * pp_transform_datatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.
	 */
	virtual pp_value
	lookup(const pp_value &cooked) const
	{
		pp_value tmp = m_real_type->lookup(cooked);
		return m_encode_func(tmp);
	}
	virtual pp_value
	lookup(const string &cooked) const
	{
		pp_value tmp = m_real_type->lookup(cooked);
		return m_encode_func(tmp);
	}
};
typedef boost::shared_ptr<pp_datatype> pp_transform_datatype_ptr;

// This has to be a template, because template argument inference only
// happens on functions, not classes.
template<typename Tdefunc, typename Tenfunc>
pp_transform_datatype_ptr
new_pp_transform_datatype(const pp_datatype_const_ptr &real_type,
    const Tdefunc &decode_func, const Tenfunc &encode_func)
{
	return pp_transform_datatype_ptr(
	    new pp_transform_datatype<Tdefunc, Tenfunc>(real_type,
	        decode_func, encode_func));
}

/*
 * pp_fixed_datatype - datatype for fixed-point values.
 *
 * Notes:
 * 	This class makes a private copy of the 'units' argument.
 */
class pp_fixed_datatype: public pp_datatype
{
    public:
	explicit pp_fixed_datatype(int nbits, const string &units = "")
	    : m_nbits(nbits), m_units(units)
	{}
	virtual ~pp_fixed_datatype()
	{}

	/*
	 * pp_fixed_datatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const pp_value &value) const
	{
		pp_value numer = value & ((pp_value(1) << m_nbits) - 1);
		pp_value denom = pp_value::pow(2, m_nbits);
		pp_value scalar = pp_value::pow(10, m_nbits);

		pp_value decimal = (numer * scalar) / denom;
		pp_value integral = value >> m_nbits;

		// right align things like 0.500 to 0.5
		while (decimal > 0 && (decimal % 10) == 0) {
			decimal /= 10;
		}
		string ret = integral.to_string() + "." + decimal.to_string();
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}

	/*
	 * pp_fixed_datatype::lookup(value)
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
		try {
			//FIXME:
			return pp_value(str);
		} catch (std::invalid_argument &e) {
			throw pp_datatype::invalid_error(str);
		}
	}

    protected:
	int m_nbits;
	string m_units;
};
typedef boost::shared_ptr<pp_fixed_datatype> pp_fixed_datatype_ptr;

#define new_pp_fixed_datatype(...) \
		pp_fixed_datatype_ptr(new pp_fixed_datatype(__VA_ARGS__))

#endif // PP_PP_DATATYPES_H__
