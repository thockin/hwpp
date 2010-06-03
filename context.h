/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_CONTEXT_H__
#define PP_CONTEXT_H__

#include "pp.h"
#include "scope.h"
#include "path.h"
#include "dirent.h"
#include "register.h"
#include "field.h"
#include "datatype.h"

namespace pp {

/*
 * This encapsulates a context of the PP tree.
 */
class Context;
typedef boost::shared_ptr<Context> ContextPtr;
#define new_pp_context(...) ::pp::ContextPtr(new ::pp::Context(__VA_ARGS__))

class Context
{
    private:
	Path::Element m_name;
	boost::weak_ptr<Scope> m_scope;
	bool m_readonly;

    public:
	Context(const string &n, const ScopePtr &s)
	    : m_name(n), m_scope(s), m_readonly(false)
	{
	}
	Context(const Path::Element &n, const ScopePtr &s)
	    : m_name(n), m_scope(s), m_readonly(false)
	{
	}

	/*
	 * Take a read-only snapshot of the current context
	 */
	ContextPtr
	snapshot() const
	{
		ContextPtr new_ctxt = new_pp_context(*this);
		new_ctxt->m_readonly = true;
		return new_ctxt;
	}

	/*
	 * getter methods
	 */
	string
	name() const
	{
		return m_name.to_string();
	}
	ScopePtr
	scope() const
	{
		return m_scope.lock();
	}
	bool
	is_readonly() const
	{
		return m_readonly;
	}

	/*
	 * Encapsulate for ease of use.
	 */

	const ConstBindingPtr &
	binding() const
	{
		return scope()->binding();
	}

	void
	add_datatype(const string &name, const DatatypePtr &datatype)
	{
		scope()->add_datatype(name, datatype);
	}

	ConstDatatypePtr
	resolve_datatype(const string &name) const
	{
		return scope()->resolve_datatype(name);
	}

	Path
	resolve_path(const string &path_str) const
	{
		return scope()->resolve_path(path_str);
	}

	void
	add_dirent(const string &name, const DirentPtr &dirent)
	{
		scope()->add_dirent(name, dirent);
	}
	void
	add_dirent(const Path::Element &name, const DirentPtr &dirent)
	{
		scope()->add_dirent(name, dirent);
	}

	bool
	dirent_defined(const Path &path) const
	{
		return scope()->dirent_defined(path);
	}

	ConstDirentPtr
	lookup_dirent(const Path &path, unsigned flags=0) const
	{
		return scope()->lookup_dirent(path, flags);
	}

	void
	add_bookmark(const string &name)
	{
		return scope()->add_bookmark(name);
	}
};

}  // namespace pp

#endif // PP_CONTEXT_H__
