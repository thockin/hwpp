// Copyright 2007 Google Inc. All Rights Reserved.
// Author: lesleyn@google.com(Lesley Northam)

#ifndef PP_PATH_HPP__
#define PP_PATH_HPP__

#include <list>
#include <stdexcept>
#include <boost/iterator_adaptors.hpp>

/* forward declaration of path class */
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
	friend class pp_path;

    public:
	/* default constructor */
	pp_path_iterator() {}

	/* implicit conversion from the underlying iterator */
	pp_path_iterator(Titer it): m_it(it) {}

	/* implicit conversion from non-const to const_iterator */
	template<class Tother>
	pp_path_iterator(const pp_path_iterator<Titer, Tother> &other)
	    : m_it(other.m_it), m_trap(other.m_trap) {}

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


/* The pp_path Object */
class pp_path 
{
    /* typedefs for list and iterator */
    typedef std::list<std::string> Tlist;
    typedef Tlist::iterator Titer;

    public:
	typedef std::string value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef pp_path_iterator<Titer, std::string> iterator;
	typedef pp_path_iterator<Titer, const std::string > const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/* Constructors */
	explicit pp_path() : m_list() {};
	explicit pp_path(const std::string  &path) {
	    /* Create a path from the input string */
	    std::string temp_path = path;
	    std::string path_item;

	    /* the location of the / */
	    unsigned int last_location = 1;

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
             * */
	    while (last_location != std::string::npos) {
		/* find the location of / */
		last_location = temp_path.find_first_of("/");
		if (last_location == 0) {
		    temp_path = temp_path.substr(1, temp_path.length());
		} else {
		    /* non-empty strings */
		    if (temp_path.length() > 0) {
			path_item = temp_path.substr(0, last_location);
			m_list.push_back(path_item);
			temp_path = temp_path.substr(last_location+1, 
                                                     temp_path.length()+1);
		    }
		}
	    }
	};

	/* Destructor */
	~pp_path() {};

	/* Iterator Functionality */
	iterator begin() {
	    return m_list.begin();
	};

	iterator end() {
	    return m_list.end();
	};

	const_iterator begin() const {
	    pp_path *p = const_cast<pp_path *>(this);
	    return p->begin();
	};

	const_iterator end() const {
	    pp_path *p = const_cast<pp_path *>(this);
	    return p->end();
	};

	reverse_iterator rbegin() {
	    return m_list.rbegin();
	};

	reverse_iterator rend() {
	    return m_list.rend();
	};

	const_reverse_iterator rbegin() const {
	    pp_path *p = const_cast<pp_path *>(this);
	    return p->rbegin();
	};

	const_reverse_iterator rend() const {
	    pp_path *p = const_cast<pp_path *>(this);
	    return p->rend();
	};

	/* Size Functionality */
	size_type size() const {
	    return m_list.size();
	};

	size_type max_size() const {
	    return m_list.max_size();
	};

	/* A function that returns true if the list is empty */
	bool empty() const {
	    return m_list.empty();
	};

	/* access the first/last values */
	reference front() {
	    return m_list.front();
        };
    
	reference back() {
	    return m_list.back();
	};
    
	const_reference front() const {
	    return m_list.front();
	};

	const_reference back() const {
	    return m_list.back();
	};

	/* Push and pop functions */
	void push_front(const_reference item) {
	    m_list.push_front(item);
	};

	void push_back(const_reference item) {
	    m_list.push_back(item);
	};

	void pop_front() {
	    m_list.pop_front();
	};

	void pop_back() {
	    m_list.pop_back();
	};

	/* Clear function */
	void clear() {
	    m_list.clear();
	};

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
	    const_iterator my_pp_path_iterator;
	    const_iterator your_pp_path_iterator;
	    my_pp_path_iterator = begin();
	    your_pp_path_iterator = item.begin();
      
	    while (my_pp_path_iterator != end()) {
        
		/* If any element comparison is non-equal, return false */
		if ((*my_pp_path_iterator).compare(*your_pp_path_iterator) != 0)
                {
		    return false;
		}

		my_pp_path_iterator++;
		your_pp_path_iterator++;
	    }

	    /* 
             * Return true at the end, as we would have exited as false prior 
             * */
	    return true;
	};
        
    private:

	/* The list that is being wrapped */
	Tlist m_list;
};

#endif // PP_PATH_HPP__
