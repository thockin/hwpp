/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_SHARED_OBJECT_H__
#define PP_SHARED_OBJECT_H__

#include <stdexcept>
#include <string>
#include <dlfcn.h>
#include <boost/shared_ptr.hpp>

namespace util {

//
// This is a simple wrapper for loading and managing shared object files.
// It does reference counting (via shared_ptr) on the "handle" and will
// unload it only when the last reference is released.
//
class SharedObject
{
    public:
	// something went wrong loading the object
	struct load_error: public std::runtime_error
	{
		explicit load_error(const std::string &str)
		    : runtime_error(str)
		{
		}
	};
	// the handle was NULL
	struct invalid_handle_error: public std::runtime_error
	{
		explicit invalid_handle_error(const std::string &str)
		    : runtime_error(str)
		{
		}
	};
	// a requested symbol lookup failed
	struct symbol_not_found_error: public std::runtime_error
	{
		explicit symbol_not_found_error(const std::string &str)
		    : runtime_error(str)
		{
		}
	};

    private:
	// member variables
	boost::shared_ptr<void> m_handle;
	std::string m_path;

    public:
	// these are the default flags with which to open a SharedObject
	static const unsigned DEFAULT_FLAGS = (RTLD_NOW | RTLD_LOCAL);

	// default ctor - does not open anything
	SharedObject()
	    : m_handle(), m_path("")
	{
	}
	// ctor - open a shared object file
	explicit
	SharedObject(const std::string &path, unsigned flags=DEFAULT_FLAGS)
	    : m_handle(), m_path(path)
	{
		open(path, flags);
	}
	// the default copy ctor is correct
	// the default assignment operator is correct
	// the dtor for m_handle does the dirty work

	// Open a new shared object file.  See the man-page for dlopen() for
	// details about flags and path handling.
	void
	open(const std::string &path, unsigned flags = DEFAULT_FLAGS)
	{
		void *handle = dlopen(path.c_str(), flags);
		if (handle == NULL) {
			throw load_error(dlerror());
		}
		m_handle = make_handle_ptr(handle);
		m_path = path;
	}

	// drop our reference to the current shared object
	void
	close()
	{
		m_handle = make_handle_ptr();
	}

	// get a pointer to a public symbol in the current shared object
	void *
	lookup_symbol(const std::string &symbol) const
	{
		if (handle() == NULL) {
			throw invalid_handle_error(symbol);
		}

		// check for errors as per the man page
		dlerror();
		void *sym = dlsym(m_handle.get(), symbol.c_str());
		const char *error = dlerror();
		if (sym == NULL && error) {
			throw symbol_not_found_error(error);
		}

		return sym;
	}

	// get the raw handle
	const void *
	handle() const
	{
		return m_handle.get();
	}

	// get the path that was used to open the shared object file
	const std::string &
	path() const
	{
		return m_path;
	}

    private:
	// create a shared_ptr to a handle, with an appropriate deleter
	boost::shared_ptr<void>
	make_handle_ptr(void *ptr = NULL)
	{
		if (ptr == NULL) {
			return boost::shared_ptr<void>();
		}
		return boost::shared_ptr<void>(ptr, dlclose);
	}
};

} // namespace util

#endif // PP_SHARED_OBJECT_H__
