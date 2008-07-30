/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_ARRAY_H__
#define PP_PP_ARRAY_H__

#include "pp.h"
#include <vector>
#include <stdexcept>
#include "pp_path.h"
#include "pp_dirent.h"

//
// pp_array - an array of dirents.
//
class pp_array: public pp_dirent
{
    public:
	explicit pp_array(pp_dirent_type type)
	    : pp_dirent(PP_DIRENT_ARRAY), m_type(type)
	{
	}
	virtual ~pp_array()
	{
	}

	//
	// Add a dirent to this array.
	// Throws:
	// 	pp_dirent::conversion_error	- wrong pp_dirent_type
	//
	void
	append(const pp_dirent_ptr &dirent)
	{
		if (dirent->dirent_type() != m_type) {
			throw pp_dirent::conversion_error("can't append "
			    + to_string(dirent->dirent_type())
			    + " to " + to_string(m_type) + "[]");
		}
		m_vector.push_back(dirent);
	}

	//
	// Return the number of dirents in this array.
	//
	size_t
	size() const
	{
		return m_vector.size();
	}

	//
	// Provide access to the dirent at an index.
	// Throws:
	// 	std::out_of_range	- index is out of range
	//
	const pp_dirent_const_ptr &
	at(size_t index) const
	{
		return m_vector.at(index);
	}
	const pp_dirent_const_ptr &
	operator[](size_t index) const
	{
		return at(index);
	}

	pp_dirent_type
	array_type() const
	{
		return m_type;
	}

    private:
	std::vector<pp_dirent_const_ptr> m_vector;
	pp_dirent_type m_type;
};
typedef boost::shared_ptr<pp_array> pp_array_ptr;
typedef boost::shared_ptr<const pp_array> pp_array_const_ptr;

#define new_pp_array(...) pp_array_ptr(new pp_array(__VA_ARGS__))

// const
inline pp_array_const_ptr
pp_array_from_dirent(const pp_dirent_const_ptr &dirent)
{
	if (dirent->is_array()) {
		return static_pointer_cast<const pp_array>(dirent);
	}
	throw pp_dirent::conversion_error("non-array dirent used as array");
}
// non-const
inline pp_array_ptr
pp_array_from_dirent(const pp_dirent_ptr &dirent)
{
	const pp_dirent_const_ptr &const_dirent = dirent;
	return const_pointer_cast<pp_array>(
	       pp_array_from_dirent(const_dirent));
}

#endif // PP_PP_ARRAY_H__
