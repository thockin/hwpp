/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_ARRAY_H__
#define PP_ARRAY_H__

#include "pp/pp.h"
#include "pp/util/printfxx.h"
#include <vector>
#include <stdexcept>
#include "pp/path.h"
#include "pp/dirent.h"

namespace pp {

//
// Array - an array of dirents.
//
class Array: public Dirent
{
    public:
	explicit Array(DirentType type)
	    : Dirent(DIRENT_TYPE_ARRAY), m_type(type)
	{
	}
	virtual ~Array()
	{
	}

	//
	// Add a dirent to this array.
	// Throws:
	// 	Dirent::ConversionError		- wrong DirentType
	//
	void
	append(const DirentPtr &dirent)
	{
		if (dirent->dirent_type() != m_type) {
			throw Dirent::ConversionError(
			    sprintfxx("can't append %s to %s[]",
			              dirent->dirent_type(), m_type));
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
	const ConstDirentPtr &
	at(size_t index) const
	{
		return m_vector.at(index);
	}
	const ConstDirentPtr &
	operator[](size_t index) const
	{
		return at(index);
	}

	DirentType
	array_type() const
	{
		return m_type;
	}

    private:
	std::vector<ConstDirentPtr> m_vector;
	DirentType m_type;
};
typedef boost::shared_ptr<Array> ArrayPtr;
typedef boost::shared_ptr<const Array> ConstArrayPtr;

#define new_pp_array(...) ::pp::ArrayPtr(new ::pp::Array(__VA_ARGS__))

// const
inline ConstArrayPtr
array_from_dirent(const ConstDirentPtr &dirent)
{
	if (dirent->is_array()) {
		return static_pointer_cast<const Array>(dirent);
	}
	throw Dirent::ConversionError("non-array dirent used as array");
}
// non-const
inline ArrayPtr
array_from_dirent(const DirentPtr &dirent)
{
	const ConstDirentPtr &const_dirent = dirent;
	return const_pointer_cast<Array>(array_from_dirent(const_dirent));
}

}  // namespace pp

#endif // PP_ARRAY_H__
