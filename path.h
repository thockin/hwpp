// Copyright 2007 Google Inc. All Rights Reserved.

#ifndef PP_PATH_HPP__
#define PP_PATH_HPP__

#include "pp/pp.h"
#include <list>
#include <stdexcept>
#include <ostream>
#include <boost/iterator_adaptors.hpp>

namespace pp {

//
// This template class is a thin wrapper to make iterators work for
// Path objects.  This is largely based on the boost example
// code for boost::iterator_facade.
//
template<typename Titer, typename Tval>
class PathIterator
    : public boost::iterator_facade<PathIterator<Titer, Tval>, Tval,
      typename std::iterator_traits<Titer>::iterator_category>
{
	friend class boost::iterator_core_access;
	template<class,class> friend class PathIterator;

    public:
	// default constructor
	PathIterator() {}

	// implicit conversion from the underlying iterator
	PathIterator(Titer it): m_it(it) {}

	// implicit conversion from non-const to const_iterator
	template<class Tother>
	PathIterator(const PathIterator<Titer, Tother> &other)
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
	equal(const PathIterator<Titer, Tthat> &that) const
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
	distance_to(const PathIterator<Titer, Tthere> &there) const
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

class Path
{
    public:
	// a single path element
	class Element
	{
	    public:
		enum array_mode {
			ARRAY_NONE = 0,
			ARRAY_INDEX,
			ARRAY_APPEND,
		};

	    private:
		string m_name;
		enum array_mode m_array_mode;
		int m_array_index;
		bool m_is_bookmark;

	    public:
		// explicit ctor from string
		// throws:
		// 	Path::InvalidError
		explicit
		Element(const string &str)
		    : m_name(), m_array_mode(ARRAY_NONE), m_array_index(0),
		      m_is_bookmark(false)
		{
			parse(str);
		}
		explicit
		Element(const char *str)
		    : m_name(), m_array_mode(ARRAY_NONE), m_array_index(0),
		      m_is_bookmark(false)
		{
			parse(str);
		}

		string
		to_string() const;

		bool
		equals(const Element &other) const;

		const string &
		name() const;

		bool
		is_array() const;

		enum array_mode
		array_mode() const;

		int
		array_index() const;

		bool
		is_bookmark() const;

	    private:
		void
		parse(const string &input);

		void
		parse_error(const string &name);
	};

	// a path not found error
	struct NotFoundError: public std::runtime_error
	{
		explicit NotFoundError(const std::string &str)
		    : runtime_error(str)
		{
		}
	};

	// an invalid path error
	struct InvalidError: public std::runtime_error
	{
		explicit InvalidError(const std::string &str)
		    : runtime_error(str)
		{
		}
	};

    private:
	typedef std::list<Element> Tlist;
	typedef Tlist::iterator Titer;

	// member variables
	Tlist m_list;
	bool m_absolute;

    public:
	typedef PathIterator<Titer, Element> iterator;
	typedef PathIterator<Titer, const Element> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	// default constructor
	Path()
	    : m_list(), m_absolute(false)
	{
	}
	// implicit conversion from string
	// throws:
	// 	Path::InvalidError
	Path(const string &path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	// implicit conversion from char* (for string-literal conversion)
	// throws:
	// 	Path::InvalidError
	Path(const char *path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	// explicit conversion from Path::Element
	explicit
	Path(const Element &elem)
	    : m_list(), m_absolute(false)
	{
		m_list.push_back(elem);
	}
	// explicit conversion from 2 Path::Elements
	Path(const Element &elem1, const Element &elem2)
	    : m_list(), m_absolute(false)
	{
		m_list.push_back(elem1);
		m_list.push_back(elem2);
	}
	// copy constructor
	Path(const Path &that)
	    : m_list(that.m_list), m_absolute(that.m_absolute)
	{
	}

	// destructor
	~Path()
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
		Path *p = const_cast<Path *>(this);
		return p->begin();
	}
	const_iterator
	end() const
	{
		Path *p = const_cast<Path *>(this);
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
		Path *p = const_cast<Path *>(this);
		return p->rbegin();
	}
	const_reverse_iterator
	rend() const
	{
		Path *p = const_cast<Path *>(this);
		return p->rend();
	}

	// get the number of elements
	Tlist::size_type
	size() const
	{
		return m_list.size();
	}

	// check if the path is initialized (i.e. holds a valid path)
	bool
	is_initialized() const
	{
		return (m_absolute || !m_list.empty());
	}

	// access the first/last values
	// throws:
	// 	std::out_of_range
	Element &
	front()
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.front();
	}
	Element &
	back()
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.back();
	}
	const Element &
	front() const
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.front();
	}
	const Element &
	back() const
	{
		if (m_list.empty()) {
			throw std::out_of_range("path is empty");
		}
		return m_list.back();
	}

	//
	// NOTE: all of the operations which modify the internal list are
	// ignorant of whether a path is absolute or not.  The "correct"
	// behavior is not clear, so it is left up to the caller.
	//

	// add elements onto a path
	void
	push_back(const Path &path)
	{
		const_iterator it = path.begin();
		while (it != path.end()) {
			m_list.push_back(*it);
			it++;
		}
	}
	void
	push_back(const Element &elem)
	{
		m_list.push_back(elem);
	}

	// remove items from the front or back
	// throws:
	// 	std::out_of_range
	Element
	pop_front()
	{
		Element old_front = front();
		m_list.pop_front();
		return old_front;
	}
	Element
	pop_back()
	{
		Element old_back = back();
		m_list.pop_back();
		return old_back;
	}

	// insert one or more elements
	void
	insert(iterator pos, const Element &elem)
	{
		m_list.insert(pos.get(), elem);
	}
	void
	insert(iterator pos, iterator first, iterator last)
	{
		m_list.insert(pos.get(), first.get(), last.get());
	}

	// erase one or more elements
	iterator
	erase(iterator pos)
	{
		return m_list.erase(pos.get());
	}
	iterator
	erase(iterator first, iterator last)
	{
		return m_list.erase(first.get(), last.get());
	}

	// splice in a second path
	void
	splice(iterator pos, const Path &path)
	{
		Path tmp(path);
		m_list.splice(pos.get(), tmp.m_list);
	}

	// reset everything
	void
	clear()
	{
		m_absolute = false;
		m_list.clear();
	}

	// handle absolute vs. relative paths
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
	void
	set_absolute(bool val)
	{
		m_absolute = val;
	}

	string
	to_string() const;

	bool
	equals(const Path &other) const;

    private:
	void
	append(const string &str);
};

// Stream out a path element.
inline std::ostream &
operator<<(std::ostream& o, const Path::Element &element)
{
	return o << element.to_string();
}

// Allow comparison of path elements.
inline bool
operator==(const Path::Element &left, const Path::Element &right)
{
	return left.equals(right);
}
inline bool
operator!=(const Path::Element &left, const Path::Element &right)
{
	return !(left == right);
}
// Allow comparison between path elements and strings, without converting
// the strings to elements.
inline bool
operator==(const Path::Element &left, const string &right)
{
	return (left.to_string() == right);
}
inline bool
operator==(const string &left, const Path::Element &right)
{
	return (right == left);
}
inline bool
operator!=(const Path::Element &left, const string &right)
{
	return !(left == right);
}
inline bool
operator!=(const string &left, const Path::Element &right)
{
	return !(left == right);
}
// These are needed to disambiguate comparisons to string-literals (which
// could convert to string or Path::Element).
inline bool
operator==(const Path::Element &left, const char *right)
{
	return (left.to_string() == right);
}
inline bool
operator==(const char *left, const Path::Element &right)
{
	return (right == left);
}
inline bool
operator!=(const Path::Element &left, const char *right)
{
	return !(left == right);
}
inline bool
operator!=(const char *left, const Path::Element &right)
{
	return !(left == right);
}

// Stream out a path.
inline std::ostream &
operator<<(std::ostream& o, const Path &path)
{
	return o << path.to_string();
}

// Allow comparison of paths.
inline bool
operator==(const Path &left, const Path &right)
{
	return left.equals(right);
}
inline bool
operator!=(const Path &left, const Path &right)
{
	return !(left == right);
}
// Allow comparison between paths and strings, without converting the
// strings to paths.
inline bool
operator==(const Path &left, const string &right)
{
	return to_string(left) == right;
}
inline bool
operator==(const string &left, const Path &right)
{
	return (right == left);
}
inline bool
operator!=(const Path &left, const string &right)
{
	return !(left == right);
}
inline bool
operator!=(const string &left, const Path &right)
{
	return !(left == right);
}
// Allow comparison between paths and char* (string literals), without
// converting the strings to paths.  This is needed because there is an
// implicit ctor for Path from char*, which makes some ambiguous calls.
inline bool
operator==(const Path &left, const char *right)
{
	return (left == string(right));
}
inline bool
operator==(const char *left, const Path &right)
{
	return (right == left);
}
inline bool
operator!=(const Path &left, const char *right)
{
	return !(left == right);
}
inline bool
operator!=(const char *left, const Path &right)
{
	return !(left == right);
}

// Allow simple path appending and catenation.
inline Path &
operator+=(Path &left, const Path &right)
{
	// return the original Path, the arg here is a reference
	left.push_back(right);
	return left;
}
inline Path
operator+(Path left, const Path &right)
{
	// return a new Path, the arg here is a copy
	left.push_back(right);
	return left;
}

}  // namespace pp

#endif // PP_PATH_HPP__
