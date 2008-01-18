/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_REGBITS_H__
#define PP_PP_REGBITS_H__

#include "pp.h"
#include "pp_register.h"

/*
 * pp_regbits - a bit range from a register.
 *
 * Constructors:
 * 	(const pp_register *reg, const int shift, const pp_value &mask)
 */
class pp_regbits
{
    public:
	// create an empty regbits
	pp_regbits(): m_register(NULL), m_mask(0)
	{}
	// create a simple regbits
	pp_regbits(const pp_register *reg)
	{
		// use the full register
		init(reg, reg->width() - 1, 0);
	}
	pp_regbits(const pp_register *reg, unsigned bit)
	{
		// use a single bit
		init(reg, bit, bit);
	}
	pp_regbits(const pp_register *reg, unsigned hi_bit, unsigned lo_bit)
	{
		// use the specified bits
		init(reg, hi_bit, lo_bit);
	}
	// add more bits onto a regbits (always adding at the LSB)
	pp_regbits
	operator+(const pp_regbits &that) const
	{
		pp_regbits result;
		if (m_register) {
			// simple this becomes complex result
			result.m_sub_bits.push_back(*this);
		} else {
			// complex this
			result.m_sub_bits = this->m_sub_bits;
		}
		result.m_sub_bits.push_back(that);
		return result;
	}
	pp_regbits &
	operator+=(const pp_regbits &that)
	{
		if (m_register) {
			// simple this becomes complex
			m_sub_bits.push_back(*this);
			m_register = NULL;
			m_mask = 0;
		}
		m_sub_bits.push_back(that);
		return *this;
	}

	/*
	 * pp_regbits::read()
	 *
	 * Read the value of this regbits.  The resulting value is
	 * right-justified.
	 */
	pp_value
	read() const
	{
		pp_value result = 0;
		if (m_register) {
			// simple
			result = m_register->read();
			result >>= m_shift;
			result &= m_mask;
		} else {
			// complex (MSB first)
			for (size_t i = 0; i < m_sub_bits.size(); i++) {
				result <<= m_sub_bits[i].width();
				result |= m_sub_bits[i].read();
			}
		}
		return result;
	}

	/*
	 * pp_regbits::write(value)
	 *
	 * Write a value to this regbits.  The value is assumed to be
	 * right-justified.
	 */
	void
	write(const pp_value &value) const
	{
		if (m_register) {
			// simple
			pp_value myval = value;
			myval &= m_mask;
			myval <<= m_shift;
			pp_value tmp = m_register->read();
			tmp ^= (tmp & (m_mask << m_shift));
			tmp |= myval;
			m_register->write(tmp);
		} else {
			// complex (MSB first)
			pp_value myval = value;
			for (int i = m_sub_bits.size()-1; i >= 0; i--) {
				m_sub_bits[i].write(myval);
				myval >>= m_sub_bits[i].width();
			}
		}
	}

	/*
	 * pp_regbits::width()
	 *
	 * Return the width of this regbits, in bits.
	 */
	unsigned long
	width() const
	{
		unsigned long result = 0;
		if (m_register) {
			// simple
			result = m_mask.popcount();
		} else {
			// complex
			for (size_t i = 0; i < m_sub_bits.size(); i++) {
				result += m_sub_bits[i].width();
			}
		}
		return result;
	}

    private:
	// a simple regbits populates these
	const pp_register *m_register;
	unsigned m_shift;
	pp_value m_mask;
	// a complex regbits populates this (most significant bits first)
	std::vector<pp_regbits> m_sub_bits;

	void
	init(const pp_register *reg, unsigned hi_bit, unsigned lo_bit)
	{
		// sanity checks first
		DASSERT_MSG(reg, "NULL register");
		if (hi_bit < lo_bit) {
			throw std::out_of_range("bad bit range: ["
			    + to_string(hi_bit) + ":"
			    + to_string(lo_bit) + "]");
		}
		if (hi_bit >= (unsigned)reg->width()) {
			throw std::out_of_range("bad bit range: ["
			    + to_string(hi_bit) + ":"
			    + to_string(lo_bit) + "]: register is only "
			    + to_string(reg->width()) + " bits");
		}

		// do this only after sanity checks
		m_register = reg;
		m_shift = lo_bit;
		m_mask = PP_MASK((hi_bit - lo_bit) + 1);
	}
};

#endif // PP_PP_REGBITS_H__
