/* Copyright (c) Tim Hockin, 2010 */
#ifndef PP_UTIL_POINTER_H__
#define PP_UTIL_POINTER_H__

#include <boost/scoped_ptr.hpp>
#include "pp/util/assert.h"

namespace util {

struct NullPointerError: public std::runtime_error
{
	explicit NullPointerError(const std::string &str)
	    : runtime_error(str)
	{
	}
};

// This is a scoped_ptr<> that can never be NULL.
template<typename Tptr>
class NeverNullScopedPtr : public boost::scoped_ptr<Tptr>
{
 private:
	// This allows us to strip the const off of Tptr, if present.  This
	// allows us to have const and non-const ctors.
	template<typename T>
	struct UnConst {
		typedef T type;
	};
	template<typename T>
	struct UnConst<const T> {
		typedef T type;
	};

 public:
	explicit
	NeverNullScopedPtr(typename UnConst<Tptr>::type *pointer)
	    : boost::scoped_ptr<Tptr>(pointer)
	{
		if (pointer == NULL) {
			throw NullPointerError(
			    "NeverNullScopedPtr initialized to NULL");
		}
	}
	explicit
	NeverNullScopedPtr(const typename UnConst<Tptr>::type *pointer)
	    : boost::scoped_ptr<Tptr>(pointer)
	{
		if (pointer == NULL) {
			throw NullPointerError(
			    "NeverNullScopedPtr initialized to NULL");
		}
	}
	// This catches static initializations to NULL.
	template<typename T>
	explicit
	NeverNullScopedPtr(T catches_null)
	{
		// This symbol does not exist, but makes for better errors.
		dont_init_NeverNullScopedPtr_to_NULL(catches_null);
	}

	void
	reset(typename UnConst<Tptr>::type *pointer)
	{
		if (pointer == NULL) {
			throw NullPointerError(
			    "NeverNullScopedPtr reset to NULL");
		}
		boost::scoped_ptr<Tptr>::reset(pointer);
	}
	void
	reset(const typename UnConst<Tptr>::type *pointer)
	{
		if (pointer == NULL) {
			throw NullPointerError(
			    "NeverNullScopedPtr reset to NULL");
		}
		boost::scoped_ptr<Tptr>::reset(pointer);
	}
	// This catches static reset()s to NULL.
	template<typename T>
	void
	reset(T catches_null)
	{
		// This symbol does not exist, but makes for better errors.
		dont_reset_NeverNullScopedPtr_to_NULL(catches_null);
	}

	void
	swap(NeverNullScopedPtr &other)
	{
		boost::scoped_ptr<Tptr>::swap(other);
	}

	// This prevents swap() with NULL.  It's never defined.
	void
	swap(boost::scoped_ptr<Tptr> &other);
};

// This is a scoped_ptr<> that can be NULL.
template<typename Tptr>
class MaybeNullScopedPtr : public boost::scoped_ptr<Tptr>
{
 public:
	explicit
	MaybeNullScopedPtr(Tptr *pointer = NULL)
	    : boost::scoped_ptr<Tptr>(pointer)
	{
	}
};

}  // namespace util

#endif // PP_UTIL_POINTER_H__
