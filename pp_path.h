// Copyright 2007 Google Inc. All Rights Reserved.
// Author: lesleyn@google.com(Lesley Northam)

#ifndef PP_PATH_HPP__
#define PP_PATH_HPP__

#include "pp.h"
#include <list>
#include <iostream>
#include <string>
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
	bool equal(const pp_path_iterator<Titer, Tthat> &that) const {
		return(this->m_it == that.m_it);
	}

	/* move the iterator forward by one */
	void increment() {
		++m_it;
	}

	/* move the iterator backward by one */
	void decrement() {
		--m_it;
	}

	/* move the iterator forward or backward by n */
	void advance(const std::ptrdiff_t n) {
		m_it += n;
	}

	/* figure out the distance to another iterator */
	template<typename Tthere>
	std::ptrdiff_t distance_to(
	    const pp_path_iterator<Titer, Tthere> &there) const {
		return there.m_it - this->m_it;
	}

	/* get at the referent */
	Tval& dereference() const {
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
	typedef std::list<std::string> Tlist;
	typedef Tlist::iterator Titer;

    public:
	typedef pp_path_iterator<Titer, std::string> iterator;
	typedef pp_path_iterator<Titer, const std::string> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/* default constructor */
	explicit pp_path() : m_list() {}

	/* copy constructor */
	pp_path(const pp_path &that) : m_list(that.m_list) {}

	/* implicit conversion from string */
	pp_path(const std::string &path) {
		//FIXME: take delimiter as an argument, too
		/* Create a path from the input string */
		std::string tmp = path;
		std::string path_item;

		/* the location of the delimiter */
		std::string::size_type last_loc = 0;

		/*
		 * Until we can find no more /
		 * note that this function will self-correct excess /'s, for
		 * example: /red/orange/yellow/ the function does not create
		 * an empty string that would occur after the final /.  And in
		 * the path //red//orange, it will not add extra empty strings
		 * between the double slashes.  This algorithm also does not
		 * care whether you lead or end with a /, it finds only valid
		 * non-empty strings and creates the path from them.
		 *
		 * Valid is defined as any non-empty string between a pair of
		 * /'s.
		 */
		while (last_loc != std::string::npos) {
			/* find the location of / */
			last_loc = tmp.find_first_of(delim());
			if (last_loc == 0) {
				tmp = tmp.substr(1, tmp.length());
			} else {
				/* non-empty strings */
				if (tmp.length() > 0) {
					path_item = tmp.substr(0, last_loc);
					m_list.push_back(path_item);
					tmp = tmp.substr(last_loc+1,
					    tmp.length()+1);
				}
			}
		}
	}

	/* destructor */
	~pp_path() {}

	/* iterator functionality */
	iterator begin() {
		return m_list.begin();
	}

	iterator end() {
		return m_list.end();
	}

	const_iterator begin() const {
		pp_path *p = const_cast<pp_path *>(this);
		return p->begin();
	}

	const_iterator end() const {
		pp_path *p = const_cast<pp_path *>(this);
		return p->end();
	}

	reverse_iterator rbegin() {
		return m_list.rbegin();
	}

	reverse_iterator rend() {
		return m_list.rend();
	}

	const_reverse_iterator rbegin() const {
		pp_path *p = const_cast<pp_path *>(this);
		return p->rbegin();
	}

	const_reverse_iterator rend() const {
		pp_path *p = const_cast<pp_path *>(this);
		return p->rend();
	}

	/* size functionality */
	Tlist::size_type size() const {
		return m_list.size();
	}

	Tlist::size_type max_size() const {
		return m_list.max_size();
	}

	/* check if the list is empty */
	bool empty() const {
		return m_list.empty();
	}

	/* access the first/last values */
	std::string &front() {
		return m_list.front();
	}

	std::string &back() {
		return m_list.back();
	}

	const std::string &front() const {
		return m_list.front();
	}

	const std::string &back() const {
		return m_list.back();
	}

	/* push and pop functions */
	void push_front(const std::string &item) {
		//FIXME: check for / characters?
		m_list.push_front(item);
	}
	void push_front(const pp_path &path) {
		const_reverse_iterator rit = path.rbegin();
		while (rit != path.rend()) {
			m_list.push_front(*rit);
			rit++;
		}
	}

	void push_back(const std::string &item) {
		//FIXME: check for / characters?
		m_list.push_back(item);
	}
	void push_back(const pp_path &path) {
		const_iterator it = path.begin();
		while (it != path.end()) {
			m_list.push_back(*it);
			it++;
		}
	}

	void pop_front() {
		m_list.pop_front();
	}

	void pop_back() {
		m_list.pop_back();
	}

	/* Clear function */
	void clear() {
		m_list.clear();
	}

	/* A function for testing if two paths are equal */
	bool equals(const pp_path &item) const {
		/*
		 * If the two lists are different lengths, exit,
		 * they cannot be equal
		 * */
		 if (m_list.size() != item.size()) {
			return false;
		 }

		/*
		 * Iterate through both lists at the same rate comparing
		 * each element
		 * */
		const_iterator my_iter = begin();
		const_iterator your_iter = item.begin();

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

	std::string delim() const {
		return "/";
	}

    private:
	/* the list that is being wrapped */
	Tlist m_list;
};

inline std::ostream &
operator<<(std::ostream& o, const pp_path &path)
{
	pp_path::const_iterator it = path.begin();
	while (it != path.end()) {
		o << *it;
		it++;
		if (it != path.end()) {
			o << path.delim();
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
operator==(const pp_path &path, const std::string &str)
{
	return path.equals(pp_path(str));
}

inline bool
operator==(const std::string &str, const pp_path &path)
{
	return path.equals(pp_path(str));
}

inline bool
operator!=(const pp_path &left, const pp_path &right)
{
	return !(left == right);
}

inline bool
operator!=(const pp_path &path, const std::string &str)
{
	return !(path == str);
}

inline bool
operator!=(const std::string &str, const pp_path &path)
{
	return !(path == str);
}

inline pp_path &
operator+=(pp_path &left, const pp_path &right)
{
	/* return the original pp_path, the arg here is a reference */
	left.push_back(right);
	return left;
}

inline pp_path &
operator+=(pp_path &path, const std::string &str)
{
	/* return the original pp_path, the arg here is a reference */
	path.push_back(str);
	return path;
}

inline std::string &
operator+=(std::string &str, const pp_path &path)
{
	/* return the original string */
	str += path.delim() + to_string(path);
	return str;
}

inline pp_path
operator+(pp_path left, const pp_path &right)
{
	/* return a new pp_path, the arg here is a copy */
	left.push_back(right);
	return left;
}

inline pp_path
operator+(pp_path path, const std::string &str)
{
	/* return a new pp_path, the arg here is a copy */
	path.push_back(str);
	return path;
}

inline std::string
operator+(std::string str, const pp_path &path)
{
	/* return a new string */
	str += path.delim() + to_string(path);
	return str;
}

#endif // PP_PATH_HPP__
