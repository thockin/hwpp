// Copyright (c) Tim Hockin, 2007-2008
#ifndef KEYED_VECTOR_HPP__
#define KEYED_VECTOR_HPP__

//
// keyed_vector.cpp
//
// Tim Hockin <thockin@hockin.org>
// 2007
//

#include <vector>
#include <map>
#include <stdexcept>
#include <boost/iterator_adaptors.hpp>
#include "pp.h"
#include "debug.h"

//
// This template class is a thin wrapper to make iterators work for
// keyed_vector objects.  This is largely based on the boost example
// code for boost::iterator_facade.
//
template<typename Titer, typename Tval>
class keyvec_iter
    : public boost::iterator_facade<keyvec_iter<Titer, Tval>, Tval,
      typename std::iterator_traits<Titer>::iterator_category>
{
	friend class boost::iterator_core_access;
	template<class,class> friend class keyvec_iter;

    public:
	// default constructor
	keyvec_iter() {}

	// implicit conversion from the underlying iterator
	keyvec_iter(Titer it): m_it(it) {}

	// implicit conversion from iterator to const_iterator
	template<class Tother>
	keyvec_iter(const keyvec_iter<Titer, Tother> &other)
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
	equal(const keyvec_iter<Titer, Tthat> &that) const
	{
		return (this->m_it == that.m_it);
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
	distance_to(const keyvec_iter<Titer, Tthere> &there) const
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

//
// template class keyed_vector<Tkey, Tval>
//
// This template class implements a vector in which each stored object
// (value) has a specific key.  Indexing can be done by integer or by key.
// When indexing by integer or iterating the data is returned in insert
// order.
//
// This is a brief overview of how it works.  First, there is an STL
// vector of Tval which holds the data elements, in insert order.  Then,
// There is an STL map of Tkey->int.  This holds the key elements and the
// int index of the value in the value vector.  Lastly, there is a vector
// of map iterators, where the index in the vector is parallel to the
// value vector.  This allows you to access the data in order (via the
// vectors), or to access it randomly (by the map).
//
// STL guarantees that map iterators do not get invalidated unless the
// item pointed to is removed from the map.  We rely on this when we store
// iterators.  When an item is removed from any position except the back
// of the vectors, all subsequent entries must have their indices updated
// in the map.
//
// Notes:
//   - The 'Tkey' type must have an == operator.
//   - Because of the dual modes of indexing, the 'Tkey' type can not be an
//     integral primitive.
//   - Keys must be unique.  Inserting a duplicate key will over-write the
//     original value.
//
template<typename Tkey, typename Tval>
class keyed_vector
{
	typedef std::vector<Tval> Tval_vector;
	typedef typename Tval_vector::iterator Tval_iter;
	typedef std::map<Tkey, int> Tkey_map;
	typedef typename Tkey_map::iterator Tkey_iter;
	typedef std::vector<Tkey_iter> Tkeyptr_vector;
	typedef typename Tkeyptr_vector::iterator Tkeyptr_iter;

	// the underlying data representation
	Tkey_map m_keys;
	Tval_vector m_values;
	Tkeyptr_vector m_keyptrs;

    public:
	typedef Tval value_type;
	typedef Tkey key_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef keyvec_iter<Tval_iter, Tval> iterator;
	typedef keyvec_iter<Tval_iter, const Tval> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	// default ctor
	keyed_vector()
	{
	}
	// copy ctor
	keyed_vector(const keyed_vector &other)
	{
		*this = other;
	}
	// assignment operator
	keyed_vector &
	operator=(const keyed_vector &other)
	{
		// We can't just copy each element, because m_keyptrs
		// holds iterators into m_keys.
		for (size_type i = 0; i < other.size(); i++) {
			insert(other.key_at(i), other.at(i));
		}
		return *this;
	}
	// swap data
	void
	swap(keyed_vector &other)
	{
		m_keys.swap(other.m_keys);
		m_values.swap(other.m_values);
		m_keyptrs.swap(other.m_keyptrs);
	}

	// get a forward iterator
	iterator
	begin()
	{
		return m_values.begin();
	}
	iterator
	end()
	{
		return m_values.end();
	}
	const_iterator
	begin() const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->begin();
	}
	const_iterator
	end() const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->end();
	}
	// get a reverse iterator
	reverse_iterator
	rbegin()
	{
		return m_values.rbegin();
	}
	reverse_iterator
	rend()
	{
		return m_values.rend();
	}
	const_reverse_iterator
	rbegin() const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->rbegin();
	}
	const_reverse_iterator
	rend() const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->rend();
	}

	 // get meta-data
	size_type
	size() const
	{
		DASSERT(m_keys.size() == m_values.size());
		return m_values.size();
	}
	size_type
	max_size() const
	{
		return min(m_keys.max_size(), m_values.max_size());
	}
	size_type
	capacity() const
	{
		return m_values.capacity();
	}
	bool
	empty() const
	{
		return m_values.empty();
	}

	// get the first or last value - can throw std::out_of_range
	reference
	front()
	{
		bounds_check(0);
		return m_values.front();
	}
	reference
	back()
	{
		bounds_check(0);
		return m_values.back();
	}
	const_reference
	front() const
	{
		bounds_check(0);
		return m_values.front();
	}
	const_reference
	back() const
	{
		bounds_check(0);
		return m_values.back();
	}

	// get the value at an int index - can throw std::out_of_range
	reference
	at(size_type index)
	{
		bounds_check(index);
		return m_values[index];
	}
	reference
	operator[](size_type index)
	{
		return at(index);
	}
	const_reference
	at(size_type index) const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->at(index);
	}
	const_reference
	operator[](size_type index) const
	{
		return at(index);
	}

	// get the value at a Tkey index - can throw std::out_of_range
	reference
	at(const Tkey &index)
	{
		iterator it = find(index);
		if (it != end()) {
			return *it;
		}
		throw std::out_of_range("key not found: " + to_string(index));
	}
	reference
	operator[](const Tkey &index)
	{
		return at(index);
	}
	const_reference
	at(const Tkey &index) const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->at(index);
	}
	const_reference
	operator[](const Tkey &index) const
	{
		return at(index);
	}

	// get the const key at an int index - can throw std::out_of_range
	const Tkey &
	key_at(size_type index) const
	{
		bounds_check(index);
		return m_keyptrs[index]->first;
	}

	// check for the existence of a key - does not throw
	bool
	has_key(const Tkey &key) const
	{
		return (find(key) != end());
	}

	// get the value for a key, or end() if not found - does not throw
	iterator
	find(const Tkey &key)
	{
		Tkey_iter found = m_keys.find(key);
		if (found == m_keys.end()) {
			return end();
		}
		return iter_at(found->second);
	}
	const_iterator
	find(const Tkey &key) const
	{
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->find(key);
	}

	// add a unique key-value pair, or overwrite the value if the key
	// already exists - same as insert()
	void
	push_back(const Tkey &key, const Tval &value)
	{
		insert(key, value);
	}

	// remove the first or last item - can throw std::out_of_range
	void
	pop_front()
	{
		bounds_check(0);
		erase(m_keyptrs.front()->first);
	}
	void
	pop_back()
	{
		bounds_check(0);
		erase(m_keyptrs.back()->first);
	}

	// add a unique key-value pair, or overwrite the value if the key
	// already exists
	iterator
	insert(const Tkey &key, const Tval &value)
	{
		std::pair<Tkey_iter, bool> ret;
		typename Tkey_map::value_type key_val(key, -1);

		// try to insert the key
		ret = m_keys.insert(key_val);
		if (ret.second) {
			// the key was inserted into the map: add the
			// value to the vectors, and to point to the key
			m_values.push_back(value);
			m_keyptrs.push_back(ret.first);
			ret.first->second = size()-1;
			return (m_values.end() - 1);
		} else {
			// the key already existed in the map: update the
			// value, but leave the vectors alone
			m_values[ret.first->second] = value;
			return (iter_at(ret.first->second));
		}
	}

	// remove a value by key
	iterator
	erase(const Tkey &key)
	{
		iterator found = find(key);
		if (found != end()) {
			return erase(found);
		}
		return end();
	}
	// remove value(s) by iterator(s)
	iterator
	erase(iterator pos)
	{
		size_type index = pos - begin();
		iterator ret = pos+1;

		m_keys.erase(m_keyptrs[index]);
		m_keyptrs.erase(keyptr_iter_at(index));
		m_values.erase(val_iter_at(index));

		// shuffle the indices which are stored in the map
		for (size_type i = index; i < size(); i++) {
			m_keyptrs[i]->second--;
		}

		return ret;
	}
	iterator
	erase(iterator first, iterator last)
	{
		while (first < last) {
			erase(first);
			first++;
		}
		return first;
	}

    private:
	// check index bounds
	void
	bounds_check(size_type index) const
	{
		if (index >= size()) {
			throw std::out_of_range("index out of range: "
			    + to_string(index));
		}
	}

	// get various iterators by index - these are internal, so no
	// bounds checking is needed
	iterator
	iter_at(size_type index)
	{
		return val_iter_at(index);
	}
	Tkeyptr_iter
	keyptr_iter_at(size_type index)
	{
		return (m_keyptrs.begin() + index);
	}
	Tval_iter
	val_iter_at(size_type index)
	{
		return (m_values.begin() + index);
	}
};

#endif // KEYED_VECTOR_HPP__
