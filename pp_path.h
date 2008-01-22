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

/* forward declaration */
class pp_path;

/*
 * This template class is a thin wrapper to make iterators work for
 * pp_path objects.  This is largely based on the boost example
 * code for boost::iterator_facade.
 *
 * Borrowed from keyed_vector.h
 */
template<typename Titer, typename Tval>
class pp_path_iterator
    : public boost::iterator_facade<pp_path_iterator<Titer, Tval>, Tval,
      typename std::iterator_traits<Titer>::iterator_category>
{
	friend class boost::iterator_core_access;
	template<class,class> friend class pp_path_iterator;

    public:
	/* default constructor */
	pp_path_iterator() {}

	/* implicit conversion from the underlying iterator */
	pp_path_iterator(Titer it): m_it(it) {}

	/* implicit conversion from non-const to const_iterator */
	template<class Tother>
	pp_path_iterator(const pp_path_iterator<Titer, Tother> &other)
	    : m_it(other.m_it), m_trap(other.m_trap) {}

	/* get the underlying iterator */
	const Titer&
	get() const
	{
		return m_it;
	}

    private:
	/* check for equality */
	template<typename Tthat>
	bool
	equal(const pp_path_iterator<Titer, Tthat> &that) const
	{
		return(this->m_it == that.m_it);
	}

	/* move the iterator forward by one */
	void
	increment()
	{
		++m_it;
	}

	/* move the iterator backward by one */
	void
	decrement()
	{
		--m_it;
	}

	/* move the iterator forward or backward by n */
	void
	advance(const std::ptrdiff_t n)
	{
		m_it += n;
	}

	/* figure out the distance to another iterator */
	template<typename Tthere>
	std::ptrdiff_t
	distance_to(const pp_path_iterator<Titer, Tthere> &there) const
	{
		return there.m_it - this->m_it;
	}

	/* get at the referent */
	Tval &
	dereference() const
	{
		return *m_it;
	}

    private:
	/* the actual underlying iterator */
	Titer m_it;

	/* a trap to catch const_iterator to iterator assignment */
	Tval *m_trap;
};

class pp_path
{
	typedef std::list<string> Tlist;
	typedef Tlist::iterator Titer;

    public:
	typedef pp_path_iterator<Titer, string> iterator;
	typedef pp_path_iterator<Titer, const string> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/* default constructor */
	explicit pp_path()
	    : m_list(), m_absolute(false)
	{
	}

	/* copy constructor */
	pp_path(const pp_path &that)
	    : m_list(that.m_list), m_absolute(that.m_absolute)
	{
	}

	/* implicit conversion from string */
	pp_path(const string &path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	/* implicit conversion from char* */
	pp_path(const char *path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}

	/* destructor */
	~pp_path()
	{
	}

	/* iterator functionality */
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

	/* size functionality */
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

	/* check if the list is empty */
	bool
	empty() const
	{
		return m_list.empty();
	}

	/* access the first/last values */
	string &
	front()
	{
		return m_list.front();
	}

	string &
	back()
	{
		return m_list.back();
	}

	const string &
	front() const
	{
		return m_list.front();
	}

	const string &
	back() const
	{
		return m_list.back();
	}

	void
	push_back(const pp_path &path)
	{
		const_iterator it = path.begin();
		while (it != path.end()) {
			m_list.push_back(*it);
			it++;
		}
	}

	string
	pop_front()
	{
		string old_front = m_list.front();
		m_list.pop_front();
		m_absolute = false;
		return old_front;
	}

	void
	pop_back()
	{
		m_list.pop_back();
	}

	/* reset everything */
	void
	clear()
	{
		m_absolute = false;
		m_list.clear();
	}

	/* handle absolute vs relative paths */
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

	/* test if two paths are equal */
	bool
	equals(const pp_path &that) const
	{
		/* if they are different lengths they cannot be equal */
		if (m_list.size() != that.size()) {
			return false;
		}
		/* if only one is absolute, they cannot be equal */
		if (is_absolute() != that.is_absolute()) {
			return false;
		}

		/*
		 * Iterate through both lists at the same rate comparing
		 * each element.
		 */
		const_iterator my_iter = begin();
		const_iterator your_iter = that.begin();

		while (my_iter != end()) {
			/* if any elements are non-equal, return false */
			if (my_iter->compare(*your_iter) != 0) {
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
		/* special case for "" */
		if (str.size() == 0)
			return;

		std::vector<string> parts;

		/*
		 * Note: this function will self-correct excess '/',
		 * for example: given "/red/orange/yellow/", it does not
		 * create an empty part after the final '/'.  Given
		 * the path "//red//orange", it will compact the duplicate
		 * '/'.
		 */
		boost::split(parts, str, boost::is_any_of("/"));

		/*
		 * Determine if the path is relative or absolute by the first
		 * element of the vector. If it is an empty string, then the
		 * path is absolute.
		 */
		if (parts.size() > 0 && parts[0].length() == 0) {
			m_absolute = true;
		}

		/* add each non-empty part to the list */
		for (size_t i = 0; i < parts.size(); i++) {
			if (parts[i].length() != 0) {
				m_list.push_back(parts[i]);
			}
		}
	}
};

inline std::ostream &
operator<<(std::ostream& o, const pp_path &path)
{
	if (path.is_absolute()) {
		o << "/";
	}

	pp_path::const_iterator it = path.begin();
	while (it != path.end()) {
		o << *it;
		it++;
		if (it != path.end()) {
			o << "/";
		}
	}
	return o;
}

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

inline pp_path &
operator+=(pp_path &left, const pp_path &right)
{
	/* return the original pp_path, the arg here is a reference */
	left.push_back(right);
	return left;
}

inline pp_path
operator+(pp_path left, const pp_path &right)
{
	/* return a new pp_path, the arg here is a copy */
	left.push_back(right);
	return left;
}

#endif // PP_PATH_HPP__
