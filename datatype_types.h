/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DATATYPES_H__
#define HWPP_DATATYPES_H__

#include "hwpp.h"
#include "util/printfxx.h"
#include "datatype.h"
#include "util/keyed_vector.h"
#include "language.h"
#include "util/bit_buffer.h"

namespace hwpp {

/*
 * MultiDatatype - datatype for combining multiple subtypes based
 *		       on the value being read
 *
 * NOTE: range is between min and max inclusive
 */
class MultiDatatype: public Datatype
{
    private:
	struct hwpp_multi_range
	{
		hwpp_multi_range(const ConstDatatypePtr &datatype,
		               Value min, Value max)
		    : type(datatype), low(min), high(max) {}
		ConstDatatypePtr type;
		Value low;
		Value high;
	};
	std::vector<hwpp_multi_range> m_parts;

	static bool
	val_in_range(const Value &value, const hwpp_multi_range &range) {
		return ((value >= range.low) && (value <= range.high));
	}

    public:
	MultiDatatype()
	{
	}

	virtual ~MultiDatatype()
	{
	}

	/*
	 * MultiDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the representation of the 'value' argument,
	 * evaluated with the datatype (if any) assigned to the given value's
	 * range.
	 *
	 */
	virtual string
	evaluate(const Value &value) const
	{
		for (size_t i = 0; i < m_parts.size(); i++) {
			if (val_in_range(value, m_parts[i])) {
				return m_parts[i].type->evaluate(value);
			}
		}

		// Value out of range; return unknown indicator
		return sprintfxx("<!%d!>", value);
	}

	/*
	 * MultiDatatype::lookup(str)
	 * MultiDatatype::lookup(value) -- simply validate it's in range
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an multi type, that means converting a string to
	 * it's corresponding value, or validating that a numeric value is
	 * a valid option. 
	 *
	 * WARNING: in case the given string could map to multiple different
	 * Values, the lowest possible value is returned
	 *
	 * This will throw Datatype::InvalidError if the lookup is not a
	 * valid value for this enum.
	 *
	 */
	virtual Value
	lookup(const string &str) const
	{
		for (size_t i = 0; i < m_parts.size(); i++) {
			try {
				Value val = m_parts[i].type->lookup(str);
				if (val_in_range(val, m_parts[i])) {
					return val;
				}
			} catch (Datatype::InvalidError &e) {
				// Keep trying on exception
			}
		}
		throw Datatype::InvalidError("Could not match given string "
				"to a range: " + str);
	}

	virtual Value
	lookup(const Value &value) const
	{
		for (size_t i = 0; i < m_parts.size(); i++) {
			if (val_in_range(value, m_parts[i])) {
				return value;
			}
		}
		throw Datatype::InvalidError("Given value out of range: " +
				to_string(value));
	}

	/*
	 * MultiDatatype::add_range(hwpp_multi_range)
	 * MultiDatatype::add_range(datatype, min, max)
	 *
	 * Add a new range->datatype mapping to this multi datatype.
	 *
	 */
	void
	add_range(const hwpp_multi_range &range)
	{
		add_range(range.type, range.low, range.high);
	}
	void
	add_range(const ConstDatatypePtr &datatype,
		  Value min, Value max)
	{
		if (min > max) {
			throw Datatype::InvalidError(sprintfxx(
			    "Minimum value %d cannot be higher "
			    "than maximum value %d", min, max));
		}

		// If the vector is empty, we can add it right away
		if (m_parts.size() == 0) {
			m_parts.push_back(
				hwpp_multi_range(datatype, min, max));
			return;
		}

		// See if we can add it in the middle
		std::vector<hwpp_multi_range>::iterator iter;
		for (iter = m_parts.begin(); iter != m_parts.end(); iter++) {
			if (min > iter->high) {
				continue;
			} else if (max < iter->low) {
				m_parts.insert(iter, hwpp_multi_range(
							datatype, min, max));
				return;
			} else {
				throw Datatype::InvalidError(
					"Cannot have overlapping ranges");
			}
		}

		// Range is higher than everything else; add to end
		m_parts.push_back(
			hwpp_multi_range(datatype, min, max));
	}
};
typedef boost::shared_ptr<MultiDatatype> MultiDatatypePtr;

#define new_hwpp_multi_datatype(...) \
		::hwpp::MultiDatatypePtr(new ::hwpp::MultiDatatype(__VA_ARGS__))

/*
 * EnumDatatype - datatype for enumerated values.
 *
 * NOTE: Enum keys must be unique.
 */
class EnumDatatype: public Datatype
{
    private:
	util::KeyedVector<string, Value> m_values;
	string m_unknown;
	bool m_custom_unknown;

    public:
	EnumDatatype()
	    : m_custom_unknown(false)
	{
	}
	explicit EnumDatatype(const util::KeyedVector<string,
	                          Value> &values)
	    : m_values(values), m_custom_unknown(false)
	{
	}
	virtual ~EnumDatatype()
	{
	}

	/*
	 * EnumDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values[i] == value) {
				return m_values.key_at(i);
			}
		}
		if (m_custom_unknown) {
			return m_unknown;
		}
		return sprintfxx("<!%d!>", value);
	}

	/*
	 * EnumDatatype::lookup(str)
	 * EnumDatatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an enum type, that means converting a string to
	 * it's corresponding value, or validating that a numeric value is
	 * a valid option.
	 *
	 * This will throw Datatype::InvalidError if the lookup is not a
	 * valid value for this enum.
	 */
	virtual Value
	lookup(const string &str) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values.key_at(i) == str) {
				return m_values[i];
			}
		}
		throw Datatype::InvalidError(str);
	}
	virtual Value
	lookup(const Value &value) const
	{
		for (size_t i=0; i < m_values.size(); i++) {
			if (m_values[i] == value) {
				return m_values[i];
			}
		}
		throw Datatype::InvalidError(to_string(value));
	}

	/*
	 * EnumDatatype::add_value(name, value)
	 *
	 * Add a possible value to this enumeration.
	 */
	void
	add_value(const string &name, const Value &value)
	{
		DASSERT_MSG(m_values.find(name) == m_values.end(),
				"adding duplicate enum key: "
					+ name + " = " + to_string(value));
		m_values.insert(name, value);
	}

	/*
	 * EnumDatatype::set_unknown(name)
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
typedef boost::shared_ptr<EnumDatatype> EnumDatatypePtr;

#define new_hwpp_enum_datatype(...) \
		::hwpp::EnumDatatypePtr(new ::hwpp::EnumDatatype(__VA_ARGS__))

/*
 * BoolDatatype - datatype for boolean values.
 */
class BoolDatatype: public EnumDatatype
{
    public:
	BoolDatatype(const string &true_str, const string &false_str)
	{
		add_value(true_str, 1);
		add_value(false_str, 0);
		set_unknown(true_str);
	}

	virtual Value
	lookup(const Value &value) const
	{
		return value == 0 ? 0 : 1;
	}
	virtual Value
	lookup(const string &str) const
	{
		return EnumDatatype::lookup(str);
	}
};
typedef boost::shared_ptr<BoolDatatype> BoolDatatypePtr;

#define new_hwpp_bool_datatype(...) \
		::hwpp::BoolDatatypePtr(new ::hwpp::BoolDatatype(__VA_ARGS__))

/*
 * BitmaskDatatype - datatype for bitmasks.
 *
 * NOTE: Bit names must be unique.
 * //FIXME: this is operating on bit numbers, not masks - should it?
 */
class BitmaskDatatype: public Datatype
{
    private:
	util::KeyedVector<string, Value> m_bits;

    public:
	BitmaskDatatype()
	{
	}
	explicit BitmaskDatatype(const util::KeyedVector<string, Value> &bits)
	    : m_bits(bits)
	{
	}
	virtual ~BitmaskDatatype()
	{
	}

	/*
	 * BitmaskDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const
	{
		string ret("");
		Value myval = value;

		for (size_t i=0; myval != 0 && i < m_bits.size(); i++) {
			Value b = m_bits[i];
			Value mask = (1 << b);
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
			if ((myval & Value(1)) == 1) {
				if (!ret.empty()) {
					ret += " ";
				}
				ret += sprintfxx("<!%d!>", unknown);
			}
			myval >>= 1;
			unknown++;
		}

		return ret;
	}

	/*
	 * BitmaskDatatype::lookup(str)
	 * BitmaskDatatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For a bitmask type, this means converting a string to
	 * it's corresponding bit value.
	 *
	 * This will throw Datatype::InvalidError if the string is not a
	 * valid value in this bitmask.
	 */
	virtual Value
	lookup(const string &str) const
	{
		//FIXME: need to support multiple bits
		for (size_t i=0; i < m_bits.size(); i++) {
			if (m_bits.key_at(i) == str) {
				return m_bits[i];
			}
		}
		throw Datatype::InvalidError(str);
	}
	virtual Value
	lookup(const Value &value) const
	{
		//FIXME: need to support multiple bits
		for (size_t i=0; i < m_bits.size(); i++) {
			if (m_bits.at(i) == value) {
				return m_bits[i];
			}
		}
		throw Datatype::InvalidError(to_string(value));
	}

	/*
	 * BitmaskDatatype::add_bit(name, value)
	 *
	 * Add a named bit to this bitmask.
	 */
	void
	add_bit(const string &name, const Value &value)
	{
		DASSERT_MSG(m_bits.find(name) == m_bits.end(),
				"adding duplicate bitmask key: "
					+ name + " = " + to_string(value));
		m_bits.insert(name, value);
	}
};
typedef boost::shared_ptr<BitmaskDatatype> BitmaskDatatypePtr;

#define new_hwpp_bitmask_datatype(...) \
		::hwpp::BitmaskDatatypePtr(new ::hwpp::BitmaskDatatype(__VA_ARGS__))

/*
 * StringDatatype - datatype for string values
 *
 * This datatype interprets a given value as a string in little-endian order,
 * meaning that the least significant byte in the input is the first
 * character in the string. For example, 0x6f6c6c6548 produces "Hello" (where
 * 0x48 == 'H').
 *
 * Internal non-printing characters are ignored for display purposes. For
 * example, 0x6f6c6c00006548 also produces "Hello". However, they are
 * preserved in the string so that the same Value is returned when
 * using lookup() as that which was given to evaluate() in order to produce
 * the string in the first place. That is, lookup() and evaluate() are inverse.
 */
class StringDatatype: public Datatype
{
    public:
	StringDatatype()
	{
	}
	virtual ~StringDatatype()
	{
	}

	/*
	 * StringDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype. This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const
	{
		util::BitBuffer bits = value.to_bitbuffer();
		return string((char *)bits.get(), bits.size_bytes());
	}

	/*
	 * StringDatatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype. For a string type, this is a no-op.
	 */
	virtual Value
	lookup(const Value &value) const
	{
		return value;
	}
	virtual Value
	lookup(const string &str) const
	{
		util::BitBuffer bits(BYTES_TO_BITS(str.size()),
			       (uint8_t *)str.data());
		return Value(bits);
	}

};
typedef boost::shared_ptr<StringDatatype> StringDatatypePtr;

#define new_hwpp_string_datatype(...) \
		::hwpp::StringDatatypePtr(new ::hwpp::StringDatatype(__VA_ARGS__))

/*
 * IntDatatype - datatype for signed integer values.
 *
 * Notes:
 * 	This class makes a private copy of the 'units' argument.
 */
class IntDatatype: public Datatype
{
    public:
	explicit IntDatatype(const string &units = "")
	    : m_units(units)
	{
	}
	virtual ~IntDatatype()
	{
	}

	/*
	 * IntDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const
	{
		string ret = to_string(value);
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}

	/*
	 * IntDatatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an int type, this is a no-op.
	 */
	virtual Value
	lookup(const Value &value) const
	{
		return value;
	}
	virtual Value
	lookup(const string &str) const
	{
		try {
			return Value(str);
		} catch (std::invalid_argument &e) {
			throw Datatype::InvalidError(str);
		}
	}

    protected:
	string m_units;
};
typedef boost::shared_ptr<IntDatatype> IntDatatypePtr;

#define new_hwpp_int_datatype(...) \
		::hwpp::IntDatatypePtr(new ::hwpp::IntDatatype(__VA_ARGS__))

/*
 * HexDatatype - datatype for hexadecimal values.
 */
class HexDatatype: public IntDatatype
{
    public:
	explicit HexDatatype(const BitWidth width = BITS0,
	                         const string &units = "")
	    : IntDatatype(units), m_width(width)
	{
	}
	virtual ~HexDatatype()
	{
	}

	/*
	 * HexDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const
	{
		string fmt = "0x%0" + to_string(m_width/4) + "x";
		string ret = sprintfxx(fmt, value);
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}

    private:
	BitWidth m_width;
};
typedef boost::shared_ptr<HexDatatype> HexDatatypePtr;

#define new_hwpp_hex_datatype(...) \
		::hwpp::HexDatatypePtr(new ::hwpp::HexDatatype(__VA_ARGS__))

/*
 * TransformDatatype - datatype wrapper to perform a transform on data
 * before passing off to another datatype.
 */
template<typename Tdefunc, typename Tenfunc>
class TransformDatatype: public Datatype
{
    private:
	ConstDatatypePtr m_real_type;
	Tdefunc m_decode_func;
	Tenfunc m_encode_func;

    public:
	TransformDatatype(const ConstDatatypePtr &real_type,
	    const Tdefunc &decode_func, const Tenfunc &encode_func)
	    : m_real_type(real_type), m_decode_func(decode_func),
	      m_encode_func(encode_func)
	{
	}
	virtual ~TransformDatatype()
	{
	}

	/*
	 * TransformDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &raw) const
	{
		Value cooked = m_decode_func(raw);
		return m_real_type->evaluate(cooked);
	}

	/*
	 * TransformDatatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.
	 */
	virtual Value
	lookup(const Value &cooked) const
	{
		Value tmp = m_real_type->lookup(cooked);
		return m_encode_func(tmp);
	}
	virtual Value
	lookup(const string &cooked) const
	{
		Value tmp = m_real_type->lookup(cooked);
		return m_encode_func(tmp);
	}
};
typedef boost::shared_ptr<Datatype> TransformDatatypePtr;

// This has to be a template, because template argument inference only
// happens on functions, not classes.
template<typename Tdefunc, typename Tenfunc>
TransformDatatypePtr
new_hwpp_transform_datatype(const ConstDatatypePtr &real_type,
    const Tdefunc &decode_func, const Tenfunc &encode_func)
{
	return TransformDatatypePtr(
	    new ::hwpp::TransformDatatype<Tdefunc, Tenfunc>(real_type,
	        decode_func, encode_func));
}

/*
 * FixedDatatype - datatype for fixed-point values.
 *
 * Notes:
 * 	This class makes a private copy of the 'units' argument.
 */
class FixedDatatype: public Datatype
{
    public:
	explicit FixedDatatype(int nbits, const string &units = "")
	    : m_nbits(nbits), m_units(units)
	{}
	virtual ~FixedDatatype()
	{}

	/*
	 * FixedDatatype::evaluate(value)
	 *
	 * Evaluate a value against this datatype.  This method returns a
	 * string containing the evaluated representation of the 'value'
	 * argument.
	 */
	virtual string
	evaluate(const Value &value) const
	{
		Value numer = value & ((Value(1) << m_nbits) - 1);
		Value denom = Value::pow(2, m_nbits);
		Value scalar = Value::pow(10, m_nbits);

		Value decimal = (numer * scalar) / denom;
		Value integral = value >> m_nbits;

		// right align things like 0.500 to 0.5
		while (decimal > 0 && (decimal % 10) == 0) {
			decimal /= 10;
		}
		string ret = sprintfxx("%d.%d", integral, decimal);
		if (!m_units.empty()) {
			ret += " ";
			ret += m_units;
		}
		return ret;
	}

	/*
	 * FixedDatatype::lookup(value)
	 *
	 * Lookup the value of a (potentially valid) evaluation for this
	 * datatype.  For an int type, this is a no-op.
	 */
	virtual Value
	lookup(const Value &value) const
	{
		return value;
	}
	virtual Value
	lookup(const string &str) const
	{
		try {
			//FIXME:
			return Value(str);
		} catch (std::invalid_argument &e) {
			throw Datatype::InvalidError(str);
		}
	}

    protected:
	int m_nbits;
	string m_units;
};
typedef boost::shared_ptr<FixedDatatype> FixedDatatypePtr;

#define new_hwpp_fixed_datatype(...) \
		::hwpp::FixedDatatypePtr(new ::hwpp::FixedDatatype(__VA_ARGS__))

}  // namespace hwpp

#endif // HWPP_DATATYPES_H__
