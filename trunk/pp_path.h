// Copyright 2007 Google Inc. All Rights Reserved.
// Author: lesleyn@google.com(Lesley Northam)

#ifndef PP_PATH_HPP__
#define PP_PATH_HPP__

#include "pp.h"
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/iterator_adaptors.hpp>

//
// This template class is a thin wrapper to make iterators work for
// pp_path objects.  This is largely based on the boost example
// code for boost::iterator_facade.
//
// Borrowed from keyed_vector.h
//
template<typename Titer, typename Tval>
class pp_path_iterator
    : public boost::iterator_facade<pp_path_iterator<Titer, Tval>, Tval,
      typename std::iterator_traits<Titer>::iterator_category>
{
	friend class boost::iterator_core_access;
	template<class,class> friend class pp_path_iterator;

    public:
	// default constructor
	pp_path_iterator() {}

	// implicit conversion from the underlying iterator
	pp_path_iterator(Titer it): m_it(it) {}

	// implicit conversion from non-const to const_iterator
	template<class Tother>
	pp_path_iterator(const pp_path_iterator<Titer, Tother> &other)
	    : m_it(other.m_it), m_trap(other.m_trap) {}

	// get the underlying iterator
	const Titer&
	get() const
	{
		return m_it;
	}

    private:
	// check for equality
	template<typename Tthat>
	bool
	equal(const pp_path_iterator<Titer, Tthat> &that) const
	{
		return(this->m_it == that.m_it);
	}

	// move the iterator forward by one
	void
	increment()
	{
		++m_it;
	}

	// move the iterator backward by one
	void
	decrement()
	{
		--m_it;
	}

	// move the iterator forward or backward by n
	void
	advance(const std::ptrdiff_t n)
	{
		m_it += n;
	}

	// figure out the distance to another iterator
	template<typename Tthere>
	std::ptrdiff_t
	distance_to(const pp_path_iterator<Titer, Tthere> &there) const
	{
		return there.m_it - this->m_it;
	}

	// get at the referent
	Tval &
	dereference() const
	{
		return *m_it;
	}

    private:
	// the actual underlying iterator
	Titer m_it;

	// a trap to catch const_iterator to iterator assignment
	Tval *m_trap;
};

class pp_path
{
    public:
	// a single path element
	class element
	{
	    public:
		explicit
		element(const string &str)
		{
			//FIXME: should do better parsing
			m_string = str;
		}

		string
		to_string() const
		{
			return m_string;
		}

		bool
		equals(const element &other) const
		{
			return (m_string == other.m_string);
		}

	    private:
		string m_string;
	};

    private:
	typedef std::list<element> Tlist;
	typedef Tlist::iterator Titer;

    public:
	typedef pp_path_iterator<Titer, element> iterator;
	typedef pp_path_iterator<Titer, const element> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	// default constructor
	pp_path()
	    : m_list(), m_absolute(false)
	{
	}
	// implicit conversion from string
	pp_path(const string &path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	// implicit conversion from char* (for string-literal conversion)
	pp_path(const char *path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	// copy constructor
	pp_path(const pp_path &that)
	    : m_list(that.m_list), m_absolute(that.m_absolute)
	{
	}

	// destructor
	~pp_path()
	{
	}

	// iterator functionality
	iterator
	begin()
	{
		return m_list.begin();
	}
	iterator
	end()
	{
		return m_list.end();
	}
	const_iterator
	begin() const
	{
		pp_path *p = const_cast<pp_path *>(this);
		return p->begin();
	}
	const_iterator
	end() const
	{
		pp_path *p = const_cast<pp_path *>(this);
		return p->end();
	}

	reverse_iterator
	rbegin()
	{
		return m_list.rbegin();
	}
	reverse_iterator
	rend()
	{
		return m_list.rend();
	}
	const_reverse_iterator
	rbegin() const
	{
		pp_path *p = const_cast<pp_path *>(this);
		return p->rbegin();
	}
	const_reverse_iterator
	rend() const
	{
		pp_path *p = const_cast<pp_path *>(this);
		return p->rend();
	}

	// size functionality
	Tlist::size_type
	size() const
	{
		return m_list.size();
	}
	Tlist::size_type
	max_size() const
	{
		return m_list.max_size();
	}

	// check if the list is empty
	bool
	empty() const
	{
		return m_list.empty();
	}

	// access the first/last values
	// throws:
	// 	std::out_of_range
	element &
	front()
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.front();
	}
	element &
	back()
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.back();
	}
	const element &
	front() const
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.front();
	}
	const element &
	back() const
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.back();
	}

	// add elements onto a path
	void
	push_back(const pp_path &path)
	{
		const_iterator it = path.begin();
		while (it != path.end()) {
			m_list.push_back(*it);
			it++;
		}
	}

	// remove items from the front or back
	// throws:
	// 	std::out_of_range
	element
	pop_front()
	{
		element old_front = front();
		m_list.pop_front();
		m_absolute = false;
		return old_front;
	}
	element
	pop_back()
	{
		element old_back = back();
		m_list.pop_back();
		return old_back;
	}

	// reset everything
	void
	clear()
	{
		m_absolute = false;
		m_list.clear();
	}

	// handle absolute vs relative paths
	bool
	is_absolute() const
	{
		return m_absolute;
	}
	bool
	is_relative() const
	{
		return !m_absolute;
	}

	string
	to_string() const
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
	equals(const pp_path &other) const
	{
		// if they are different lengths they cannot be equal
		if (m_list.size() != other.size()) {
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

    private:
	Tlist m_list;
	bool m_absolute;

	void
	append(const string &str)
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
};

// Stream out a path element.
inline std::ostream &
operator<<(std::ostream& o, const pp_path::element &element)
{
	return o << element.to_string();
}

// Allow comparison of path elements.
inline bool
operator==(const pp_path::element &left, const pp_path::element &right)
{
	return left.equals(right);
}
inline bool
operator!=(const pp_path::element &left, const pp_path::element &right)
{
	return !(left == right);
}
// Allow comparison between path elements and strings, without converting
// the strings to elements.  Comparison to char* will work because the
// char* will be converted to string.
inline bool
operator==(const pp_path::element &left, const string &right)
{
	return (left.to_string() == right);
}
inline bool
operator==(const string &left, const pp_path::element &right)
{
	return (right == left);
}
inline bool
operator!=(const pp_path::element &left, const string &right)
{
	return !(left == right);
}
inline bool
operator!=(const string &left, const pp_path::element &right)
{
	return !(left == right);
}

// Stream out a path.
inline std::ostream &
operator<<(std::ostream& o, const pp_path &path)
{
	return o << path.to_string();
}

// Allow comparison of paths.
inline bool
operator==(const pp_path &left, const pp_path &right)
{
	return left.equals(right);
}
inline bool
operator!=(const pp_path &left, const pp_path &right)
{
	return !(left == right);
}
// Allow comparison between paths and strings, without converting the
// strings to paths.
inline bool
operator==(const pp_path &left, const string &right)
{
	return to_string(left) == right;
}
inline bool
operator==(const string &left, const pp_path &right)
{
	return (right == left);
}
inline bool
operator!=(const pp_path &left, const string &right)
{
	return !(left == right);
}
inline bool
operator!=(const string &left, const pp_path &right)
{
	return !(left == right);
}
// Allow comparison between paths and char* (string literals), without
// converting the strings to paths.  This is needed because there is an
// implicit ctor for pp_path from char*, which makes some ambiguous calls.
inline bool
operator==(const pp_path &left, const char *right)
{
	return (left == string(right));
}
inline bool
operator==(const char *left, const pp_path &right)
{
	return (right == left);
}
inline bool
operator!=(const pp_path &left, const char *right)
{
	return !(left == right);
}
inline bool
operator!=(const char *left, const pp_path &right)
{
	return !(left == right);
}

// Allow simple path appending and catenation.
inline pp_path &
operator+=(pp_path &left, const pp_path &right)
{
	// return the original pp_path, the arg here is a reference
	left.push_back(right);
	return left;
}
inline pp_path
operator+(pp_path left, const pp_path &right)
{
	// return a new pp_path, the arg here is a copy
	left.push_back(right);
	return left;
}

#endif // PP_PATH_HPP__
