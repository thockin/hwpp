// Copyright 2007 Google Inc. All Rights Reserved.

#ifndef PP_PATH_HPP__
#define PP_PATH_HPP__

#include "pp.h"
#include <list>
#include <stdexcept>
#include <ostream>
#include <boost/iterator_adaptors.hpp>

//
// This template class is a thin wrapper to make iterators work for
// pp_path objects.  This is largely based on the boost example
// code for boost::iterator_facade.
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
		// 	pp_path::invalid_error
		explicit
		element(const string &str)
		    : m_name(), m_array_mode(ARRAY_NONE), m_array_index(0),
		      m_is_bookmark(false)
		{
			parse(str);
		}
		explicit
		element(const char *str)
		    : m_name(), m_array_mode(ARRAY_NONE), m_array_index(0),
		      m_is_bookmark(false)
		{
			parse(str);
		}

		string
		to_string() const;

		bool
		equals(const element &other) const;

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
	struct not_found_error: public std::runtime_error
	{
		explicit not_found_error(const std::string &str)
		    : runtime_error(str)
		{
		}
	};

	// an invalid path error
	struct invalid_error: public std::runtime_error
	{
		explicit invalid_error(const std::string &str)
		    : runtime_error(str)
		{
		}
	};

    private:
	typedef std::list<element> Tlist;
	typedef Tlist::iterator Titer;

	// member variables
	Tlist m_list;
	bool m_absolute;

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
	// throws:
	// 	pp_path::invalid_error
	pp_path(const string &path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	// implicit conversion from char* (for string-literal conversion)
	// throws:
	// 	pp_path::invalid_error
	pp_path(const char *path)
	    : m_list(), m_absolute(false)
	{
		append(path);
	}
	// explicit conversion from pp_path::element
	explicit
	pp_path(const element &elem)
	    : m_list(), m_absolute(false)
	{
		m_list.push_back(elem);
	}
	// explicit conversion from 2 pp_path::elements
	pp_path(const element &elem1, const element &elem2)
	    : m_list(), m_absolute(false)
	{
		m_list.push_back(elem1);
		m_list.push_back(elem2);
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

	//
	// NOTE: all of the operations which modify the internal list are
	// ignorant of whether a path is absolute or not.  The "correct"
	// behavior is not clear, so it is left up to the caller.
	//

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
	void
	push_back(const element &elem)
	{
		m_list.push_back(elem);
	}

	// remove items from the front or back
	// throws:
	// 	std::out_of_range
	element
	pop_front()
	{
		element old_front = front();
		m_list.pop_front();
		return old_front;
	}
	element
	pop_back()
	{
		element old_back = back();
		m_list.pop_back();
		return old_back;
	}

	// insert one or more elements
	void
	insert(iterator pos, const element &elem)
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
	splice(iterator pos, const pp_path &path)
	{
		pp_path tmp(path);
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
	equals(const pp_path &other) const;

    private:
	void
	append(const string &str);
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
// the strings to elements.
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
// These are needed to disambiguate comparisons to string-literals (which
// could convert to string or pp_path::element).
inline bool
operator==(const pp_path::element &left, const char *right)
{
	return (left.to_string() == right);
}
inline bool
operator==(const char *left, const pp_path::element &right)
{
	return (right == left);
}
inline bool
operator!=(const pp_path::element &left, const char *right)
{
	return !(left == right);
}
inline bool
operator!=(const char *left, const pp_path::element &right)
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
