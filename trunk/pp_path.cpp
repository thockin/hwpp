// Copyright 2008 Google Inc. All Rights Reserved.

#include "pp.h"
#include "pp_path.h"
#include <stdexcept>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <ctype.h>

//
// pp_path::element
//

string
pp_path::element::to_string() const
{
	string ret = m_name;
	if (m_is_array) {
		ret += "[";
		if (m_index >= 0) {
			ret += ::to_string(m_index);
		}
		ret += "]";
	}
	return ret;
}

bool
pp_path::element::equals(const element &other) const
{
	return (to_string() == other.to_string());
}

const string &
pp_path::element::name() const
{
	return m_name;
}

bool
pp_path::element::is_array() const
{
	return m_is_array;
}

int
pp_path::element::array_index() const
{
	return m_index;
}

void
pp_path::element::parse(const string &input)
{
	enum {
		ST_START,
		ST_PERCENT,
		ST_BODY,
		ST_DOT,
		ST_ARRAY,
		ST_ARRAY_INDEX_BASE,
		ST_ARRAY_INDEX,
		ST_DONE,
	} state = ST_START;
	int idx_base = 10;

	for (size_t i = 0; i < input.size(); i++) {
		char c = input[i];

		switch (state) {
		    case ST_START:
			if (c == '%') {
				m_name += c;
				state = ST_PERCENT;
			} else if (c == '^') {
				m_name += c;
				state = ST_DONE;
			} else if (isalpha(c) || c == '_') {
				m_name += c;
				state = ST_BODY;
			} else if (c == '.') {
				state = ST_DOT;
				m_name += c;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_PERCENT:
			if (isalpha(c) || c == '_') {
				m_name += c;
				state = ST_BODY;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_BODY:
			if (isalnum(c) || c == '_' || c == '.') {
				m_name += c;
				state = ST_BODY;
			} else if (c == '[') {
				m_is_array = true;
				state = ST_ARRAY;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_DOT:
			if (c == '.') {
				m_name += c;
				state = ST_DONE;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_ARRAY:
			if (c == ']') {
				m_index = -1;
				state = ST_DONE;
			} else if (c == '0') {
				m_index = 0;
				state = ST_ARRAY_INDEX_BASE;
			} else if (isdigit(c)) {
				idx_base = 10;
				m_index = c - '0';
				state = ST_ARRAY_INDEX;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_ARRAY_INDEX_BASE:
			if (c == 'x') {
				idx_base = 16;
				state = ST_ARRAY_INDEX;
			} else if (isdigit(c) && c <= '7') {
				idx_base = 8;
				m_index += c - '0';
				state = ST_ARRAY_INDEX;
			} else if (c == ']') {
				idx_base = 10;
				state = ST_DONE;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_ARRAY_INDEX:
			if (idx_base == 10 && isdigit(c)) {
				m_index *= idx_base;
				m_index += c - '0';
				state = ST_ARRAY_INDEX;
			} else if (idx_base == 8 && isdigit(c) && c <= '7') {
				m_index *= idx_base;
				m_index += c - '0';
				state = ST_ARRAY_INDEX;
			} else if (idx_base == 16 && isxdigit(c)) {
				m_index *= idx_base;
				if (c >= '0' && c <= '9') {
					m_index += c - '0';
				} else {
					m_index += tolower(c) - 'a';
				}
				state = ST_ARRAY_INDEX;
			} else if (c == ']') {
				state = ST_DONE;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_DONE:
			parse_error(input);
			return;
		}
	}
}

void
pp_path::element::parse_error(const string &name)
{
	throw pp_path::invalid_error("invalid path element '" + name + "'");
}


//
// pp_path
//

string
pp_path::to_string() const
{
	string result;

	if (is_absolute()) {
		result += "/";
	}

	pp_path::const_iterator it = begin();
	while (it != end()) {
		result += it->to_string();
		it++;
		if (it != end()) {
			result += "/";
		}
	}

	return result;
}

bool
pp_path::equals(const pp_path &other) const
{
	// if they are different lengths, they cannot be equal
	if (size() != other.size()) {
		return false;
	}
	// if only one is absolute, they cannot be equal
	if (is_absolute() != other.is_absolute()) {
		return false;
	}

	// Iterate through both lists at the same rate comparing
	// each element.
	const_iterator my_iter = begin();
	const_iterator your_iter = other.begin();

	while (my_iter != end()) {
		// if any elements are non-equal, return false
		if (!my_iter->equals(*your_iter)) {
			return false;
		}
		my_iter++;
		your_iter++;
	}

	return true;
}

void
pp_path::append(const string &str)
{
	// discard leading and trailing whitespace
	string my_str = boost::algorithm::trim_copy(str);

	// special case for ""
	if (my_str.size() == 0) {
		return;
	}

	std::vector<string> parts;

	// Note: this function will self-correct excess delimiters.
	// For example: given "/red/orange/yellow/", it does not
	// create an empty part after the final '/'.  Given
	// the path "//red//orange", it will compact the duplicate
	// delimiters.
	boost::split(parts, my_str, boost::is_any_of("/"),
	    boost::token_compress_on);

	// Determine if the path is relative or absolute by the first
	// element of the vector. If it is an empty string, then the
	// path is absolute.
	if (parts.size() > 0 && parts[0].length() == 0) {
		m_absolute = true;
	}

	// add each non-empty part to the list (leading and trailing
	// delimiters yield blank parts).
	for (size_t i = 0; i < parts.size(); i++) {
		if (parts[i].length() != 0) {
			m_list.push_back(element(parts[i]));
		}
	}
}