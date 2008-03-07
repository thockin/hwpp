/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_SHARED_OBJECT_H__
#define PP_SHARED_OBJECT_H__

#include <stdexcept>
#include <dlfcn.h>
#include <boost/shared_ptr.hpp>

/*
 * This is a simple wrapper for loading and managing shared object files.
 * It does reference counting (via shared_ptr) on the "handle" and will
 * unload it only when the last reference is released.
 */
class shared_object
{
    private:
	static const unsigned default_flags = (RTLD_NOW | RTLD_LOCAL);
	boost::shared_ptr<void> m_handle;
	string m_path;

	// create a shared_ptr to a handle
	boost::shared_ptr<void>
	make_handle_ptr(void *ptr = NULL)
	{
		if (ptr == NULL) {
			return boost::shared_ptr<void>();
		}
		return boost::shared_ptr<void>(ptr, dlclose);
	}

    public:
	// something went wrong loading the object
	struct load_error: public std::runtime_error
	{
		explicit load_error(const string &str)
		    : runtime_error(str)
		{
		}
	};
	// a requested symbol lookup failed
	struct symbol_not_found_error: public std::runtime_error
	{
		explicit symbol_not_found_error(const string &str)
		    : runtime_error(str)
		{
		}
	};

	shared_object()
	    : m_handle(make_handle_ptr()), m_path("")
	{
	}
	shared_object(const string &path, unsigned flags = default_flags)
	    : m_handle(make_handle_ptr()), m_path("")
	{
		open(path, flags);
	}
	~shared_object()
	{
		// the dtor for m_handle does the dirty work
	}

	//
	// Open a new shared object file.  See the man-page for dlopen() for
	// details about flags and path handling.
	//
	void
	open(const string &path, unsigned flags = default_flags)
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
	lookup_symbol(const string &symbol) const
	{
		if (handle() == NULL) {
			throw symbol_not_found_error(symbol);
		}

		void *sym;

		// check for errors as per the man page
		dlerror();
		sym = dlsym(m_handle.get(), symbol.c_str());
		if (sym == NULL && dlerror()) {
			throw symbol_not_found_error(symbol);
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
	const string &
	path() const
	{
		return m_path;
	}
};

#endif // PP_SHARED_OBJECT_H__
