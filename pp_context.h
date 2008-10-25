/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_PP_CONTEXT_H__
#define PP_PP_CONTEXT_H__

#include "pp.h"
#include "pp_scope.h"
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_register.h"
#include "pp_field.h"
#include "pp_datatype.h"

/*
 * This encapsulates a context of the PP tree.
 */
class pp_context;
typedef boost::shared_ptr<pp_context> pp_context_ptr;
#define new_pp_context(...) pp_context_ptr(new pp_context(__VA_ARGS__))

class pp_context
{
    private:
	pp_path::element m_name;
	boost::weak_ptr<pp_scope> m_scope;
	bool m_readonly;

    public:
	pp_context(const string &n, const pp_scope_ptr &s)
	    : m_name(n), m_scope(s), m_readonly(false)
	{
	}
	pp_context(const pp_path::element &n, const pp_scope_ptr &s)
	    : m_name(n), m_scope(s), m_readonly(false)
	{
	}

	/*
	 * Take a read-only snapshot of the current context
	 */
	pp_context_ptr
	snapshot() const
	{
		pp_context_ptr new_ctxt = new_pp_context(*this);
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
	pp_scope_ptr
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

	const pp_binding_const_ptr &
	binding() const
	{
		return scope()->binding();
	}

	void
	add_datatype(const string &name, const pp_datatype_ptr &datatype)
	{
		scope()->add_datatype(name, datatype);
	}

	pp_datatype_const_ptr
	resolve_datatype(const string &name) const
	{
		return scope()->resolve_datatype(name);
	}

	pp_path
	resolve_path(const string &path_str) const
	{
		return scope()->resolve_path(path_str);
	}

	void
	add_dirent(const string &name, const pp_dirent_ptr &dirent)
	{
		scope()->add_dirent(name, dirent);
	}
	void
	add_dirent(const pp_path::element &name, const pp_dirent_ptr &dirent)
	{
		scope()->add_dirent(name, dirent);
	}

	bool
	dirent_defined(const pp_path &path) const
	{
		return scope()->dirent_defined(path);
	}

	pp_dirent_const_ptr
	lookup_dirent(pp_path path, unsigned flags=0) const
	{
		return scope()->lookup_dirent(path, flags);
	}

	void
	add_bookmark(const string &name)
	{
		return scope()->add_bookmark(name);
	}
};

#endif // PP_PP_CONTEXT_H__
