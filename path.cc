// Copyright 2008 Google Inc. All Rights Reserved.

#include "path.h"

#include <ctype.h>
#include <vector>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "util/strings.h"

namespace hwpp {

//
// Path::Element
//

string
Path::Element::to_string() const
{
	string ret;
	if (is_array()) {
		ret = name() + "[";
		if (m_array_mode == ARRAY_INDEX) {
			ret += strings::to_string(m_array_index);
		}
		ret += "]";
	} else if (is_bookmark()) {
		ret = "$" + name();
	} else {
		ret = name();
	}
	return ret;
}

bool
Path::Element::equals(const Element &other) const
{
	return (to_string() == other.to_string());
}

const string &
Path::Element::name() const
{
	return m_name;
}

bool
Path::Element::is_array() const
{
	return (m_array_mode != ARRAY_NONE);
}

enum Path::Element::array_mode
Path::Element::array_mode() const
{
	return m_array_mode;
}

int
Path::Element::array_index() const
{
	return m_array_index;
}

bool
Path::Element::is_bookmark() const
{
	return m_is_bookmark;
}

void
Path::Element::parse(const string &input)
{
	enum {
		ST_START,
		ST_GOT_LEADING_SPECIAL,
		ST_BODY,
		ST_GOT_LEADING_DOT,
		ST_GOT_ARRAY_OPEN,
		ST_GOT_ARRAY_INDEX_NEGATIVE,
		ST_DETECT_ARRAY_INDEX_BASE,
		ST_ARRAY_INDEX,
		ST_DONE,
	} state = ST_START;
	int idx_base = 10;
	int idx_sign = 1;

	for (size_t i = 0; i < input.size(); i++) {
		char c = input[i];

		switch (state) {
		    case ST_START:
			if (c == '%') {
				m_name += c;
				state = ST_GOT_LEADING_SPECIAL;
			} else if (c == '$') {
				m_is_bookmark = true;
				state = ST_GOT_LEADING_SPECIAL;
			} else if (isalpha(c) || c == '_') {
				m_name += c;
				state = ST_BODY;
			} else if (c == '.') {
				state = ST_GOT_LEADING_DOT;
				m_name += c;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_GOT_LEADING_SPECIAL:
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
				state = ST_GOT_ARRAY_OPEN;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_GOT_LEADING_DOT:
			if (c == '.') {
				m_name += c;
				state = ST_DONE;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_GOT_ARRAY_OPEN:
			if (c == ']') {
				m_array_mode = ARRAY_APPEND;
				state = ST_DONE;
			} else if (c == '-') {
				idx_sign = -1;
				m_array_mode = ARRAY_INDEX;
				state = ST_GOT_ARRAY_INDEX_NEGATIVE;
			} else if (c == '0') {
				m_array_mode = ARRAY_INDEX;
				m_array_index = 0;
				state = ST_DETECT_ARRAY_INDEX_BASE;
			} else if (isdigit(c)) {
				idx_base = 10;
				m_array_mode = ARRAY_INDEX;
				m_array_index = c - '0';
				state = ST_ARRAY_INDEX;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_GOT_ARRAY_INDEX_NEGATIVE:
			if (c == '0') {
				m_array_index = 0;
				state = ST_DETECT_ARRAY_INDEX_BASE;
			} else if (isdigit(c)) {
				idx_base = 10;
				m_array_index = c - '0';
				state = ST_ARRAY_INDEX;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_DETECT_ARRAY_INDEX_BASE:
			if (tolower(c) == 'x') {
				idx_base = 16;
				state = ST_ARRAY_INDEX;
			} else if (isdigit(c) && c <= '7') {
				idx_base = 8;
				m_array_index += c - '0';
				state = ST_ARRAY_INDEX;
			} else if (c == ']') {
				idx_base = 10;
				m_array_index *= idx_sign;
				state = ST_DONE;
			} else {
				parse_error(input);
				return;
			}
			break;
		    case ST_ARRAY_INDEX:
			if (idx_base == 10 && isdigit(c)) {
				m_array_index *= idx_base;
				m_array_index += c - '0';
				state = ST_ARRAY_INDEX;
			} else if (idx_base == 8 && isdigit(c) && c <= '7') {
				m_array_index *= idx_base;
				m_array_index += c - '0';
				state = ST_ARRAY_INDEX;
			} else if (idx_base == 16 && isxdigit(c)) {
				m_array_index *= idx_base;
				if (c >= '0' && c <= '9') {
					m_array_index += c - '0';
				} else {
					m_array_index += tolower(c) - 'a';
				}
				state = ST_ARRAY_INDEX;
			} else if (c == ']') {
				m_array_index *= idx_sign;
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

	if (m_name == "") {
		parse_error(input);
	}
}

void
Path::Element::parse_error(const string &name)
{
	throw Path::InvalidError("invalid path element '" + name + "'");
}


//
// Path
//

string
Path::to_string() const
{
	string result;

	if (is_absolute()) {
		result += "/";
	}

	Path::const_iterator it = begin();
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
Path::equals(const Path &other) const
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
Path::append(const string &str)
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

	// Add each non-empty part to the list (leading and trailing
	// delimiters yield blank parts) as a Path::Element.
	for (size_t i = 0; i < parts.size(); i++) {
		if (parts[i].length() != 0) {
			m_list.push_back(Element(parts[i]));
		}
	}
}

}  // namespace hwpp
