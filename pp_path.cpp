// Copyright 2007 Google Inc. All Rights Reserved.

#include "pp.h"
#include "pp_path.h"
#include <stdexcept>
#include <vector>
#include <boost/algorithm/string.hpp>

//
// pp_path::element
//

string
pp_path::element::to_string() const
{
	return m_string;
}

bool
pp_path::element::equals(const element &other) const
{
	return (m_string == other.m_string);
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
	if (my_str.size() == 0)
		return;

	std::vector<string> parts;

	// Note: this function will self-correct excess '/',
	// for example: given "/red/orange/yellow/", it does not
	// create an empty part after the final '/'.  Given
	// the path "//red//orange", it will compact the duplicate
	// '/'.
	boost::split(parts, my_str, boost::is_any_of("/"));

	// Determine if the path is relative or absolute by the first
	// element of the vector. If it is an empty string, then the
	// path is absolute.
	if (parts.size() > 0 && parts[0].length() == 0) {
		m_absolute = true;
	}

	// add each non-empty part to the list
	for (size_t i = 0; i < parts.size(); i++) {
		if (parts[i].length() != 0) {
			m_list.push_back(element(parts[i]));
		}
	}
}
