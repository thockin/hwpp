/* Copyright (c) Tim Hockin, 2007 */
#ifndef KEYED_VECTOR_HPP__
#define KEYED_VECTOR_HPP__

/*
 * keyed_vector.cpp
 *
 * Tim Hockin <thockin@hockin.org>
 * 2007
 */

#include <vector>
#include <map>
#include <stdexcept>
#include <boost/iterator_adaptors.hpp>

/* forward declare */
template<typename Tkey, typename Tval> class keyed_vector;

/*
 * This template class is a thin wrapper to make iterators work for
 * keyed_vector objects.  This is largely based on the boost example
 * code for boost::iterator_facade.
 */
template<typename Titer, typename Tval>
class keyvec_iter
    : public boost::iterator_facade<keyvec_iter<Titer, Tval>, Tval,
      typename std::iterator_traits<Titer>::iterator_category>
{
	friend class boost::iterator_core_access;
	template<class,class> friend class keyvec_iter;

    public:
	/* default constructor */
	keyvec_iter() {}

	/* implicit conversion from the underlying iterator */
	keyvec_iter(Titer it): m_it(it) {}

	/* implicit conversion from iterator to const_iterator */
	template<class Tother>
	keyvec_iter(const keyvec_iter<Titer, Tother> &other)
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
	bool equal(const keyvec_iter<Titer, Tthat> &that) const {
		return (this->m_it == that.m_it);
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
	    const keyvec_iter<Titer, Tthere> &there) const {
		return there.m_it - this->m_it;
	}

	/* get at the referent */
	Tval& dereference() const {
		return m_it->second;
	}

    private:
	/* the actual underlying iterator */
	Titer m_it;

	/* a trap to catch const_iterator to iterator assignment */
	Tval *m_trap;
};

/*
 * template class keyed_vector<Tkey, Tval>
 *
 * This template class implements a vector in which each stored object
 * (value) has a specific key.  Indexing can be done by integer or by key.
 * When indexing by integer or iterating the data is returned in insert
 * order.
 *
 * Notes:
 *   - The 'Tkey' type must have an == operator.
 *   - Because of the dual modes of indexing, the 'Tkey' type can not be an
 *     integral primitive.
 *   - This is currently a very naive O(n) implementation of key lookups.
 *     There is room for improvement.
 *   - Indexing by 'Tkey' returns the first match.  Use find() or at() to
 *     find additional matches.
 */
template<typename Tkey, typename Tval>
class keyed_vector
{
	typedef std::pair<Tkey, Tval> Tpair;
	typedef std::vector<Tpair> Tvector;
	typedef typename Tvector::iterator Titer;

    public:
	typedef Tval value_type;
	typedef Tkey key_type;
	typedef Tpair pair_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef keyvec_iter<Titer, Tval> iterator;
	typedef keyvec_iter<Titer, const Tval> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	explicit keyed_vector(): m_vector() {}
	explicit keyed_vector(size_type n): m_vector(n) {};
	~keyed_vector() {}

	/* get an iterator */
	iterator begin() {
		return m_vector.begin();
	}
	iterator end() {
		return m_vector.end();
	}
	const_iterator begin() const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->begin();
	}
	const_iterator end() const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->end();
	}
	reverse_iterator rbegin() {
		return m_vector.rbegin();
	}
	reverse_iterator rend() {
		return m_vector.rend();
	}
	const_reverse_iterator rbegin() const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->rbegin();
	}
	const_reverse_iterator rend() const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->rend();
	}

	/* size related methods */
	size_type size() const {
		return m_vector.size();
	}
	size_type max_size() const {
		return m_vector.max_size();
	}
	size_type capacity() const {
		return m_vector.capacity();
	}
	bool empty() const {
		return m_vector.empty();
	}
	void reserve(size_type n) {
		m_vector.reserve(n);
	}

	/* get the value at an int index - can throw std::out_of_range */
	reference at(size_type index) {
		return kviter_at(index)->second;
	}
	reference operator[](size_type index) {
		return at(index);
	}
	const_reference at(size_type index) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->kviter_at(index)->second;
	}
	const_reference operator[](size_type index) const {
		return at(index);
	}

	/* get the value at the first matching Tkey index - can throw
	 * std::out_of_range */
	reference at(const Tkey &index) {
		return kviter_at(index)->second;
	}
	reference operator[](const Tkey &index) {
		return at(index);
	}
	const_reference at(const Tkey &index) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->kviter_at(index)->second;
	}
	const_reference operator[](const Tkey &index) const {
		return at(index);
	}

	/* get the value at the next matching Tkey index - can throw
	 * std::out_of_range */
	reference at(const Tkey &index, const iterator &from) {
		return kviter_at(index, from.get())->second;
	}
	const_reference at(const Tkey &index, const iterator &from) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->kviter_at(index, from.get())->second;
	}

	/* access the first/last values */
	reference front() {
		return m_vector.front().second;
	}
	reference back() {
		return m_vector.back().second;
	}
	const_reference front() const {
		return m_vector.front().second;
	}
	const_reference back() const {
		return m_vector.back().second;
	}

	/* get the const key at an int index - can throw std::out_of_range */
	const Tkey& key_at(size_type index) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->kviter_at(index)->first;
	}

	/* get the const pair at an int index - can throw std::out_of_range */
	const Tpair& pair_at(size_type index) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return *p->kviter_at(index);
	}

	/* check for the existence of a key - does not throw */
	bool has_key(const Tkey &key) const {
		return (find(key) != end());
	}
	bool has_key(const Tkey &key, const iterator &from) const {
		return (find(key, from) != end());
	}

	/* look up a key, return end() if not found - does not throw */
	iterator find(const Tkey &key) {
		return find(key, m_vector.begin());
	}
	iterator find(const Tkey &key, const iterator &from) {
		try {
			return kviter_at(key, from.get());
		} catch (std::out_of_range) {
			return m_vector.end();
		}
	}
	const_iterator find(const Tkey &key) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->find(key);
	}
	const_iterator find(const Tkey &key, const iterator &from) const {
		keyed_vector *p = const_cast<keyed_vector *>(this);
		return p->find(key, from);
	}

	/* push a value onto the end */
	void push_back(const Tkey &key, const Tval &value) {
		m_vector.push_back(Tpair(key, value));
	}

	/* pop a value from the end */
	void pop_back() {
		m_vector.pop_back();
	}

	/* add a unique key-value pair, or overwrite the value if the key
	 * already exists */
	iterator insert(const Tkey &key, const Tval &value) {
		try {
			Titer it = kviter_at(key);
			it->second = value;
			return it;
		} catch (std::out_of_range) {
			m_vector.push_back(Tpair(key, value));
			return (m_vector.end() - 1);
		}
	}

	/* remove a value by key */
	void remove(const Tkey &key) {
		try {
			Titer it = kviter_at(key);
			m_vector.erase(it);
		} catch (std::out_of_range) {
		}
	}

	/* remove a value or values by iterator */
	iterator erase(iterator pos) {
		return m_vector.erase(pos.get());
	}
	iterator erase(iterator first, iterator last) {
		return m_vector.erase(first.get(), last.get());
	}

    private:
	/* the underlying vector */
	Tvector m_vector;

	/* get the pair iterator at an int index */
	Titer kviter_at(size_type index) {
		if (index < m_vector.size()) {
			Titer it = m_vector.begin();
			it += index;
			return it;
		}
		throw std::out_of_range("index out of range");
	}

	/* get the pair iterator at a Tkey index */
	Titer kviter_at(const Tkey &index) {
		return kviter_at(index, m_vector.begin());
	}
	Titer kviter_at(const Tkey &index, Titer it) {
		while (it != m_vector.end()) {
			if (it->first == index) {
				return it;
			}
			it++;
		}
		throw std::out_of_range("key not found");
	}
};

#endif // KEYED_VECTOR_HPP__
