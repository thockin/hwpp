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
class pp_context
{
    private:
	string m_name;
	pp_scope_ptr m_scope;
	bool m_readonly;

    public:
	pp_context(): m_readonly(false)
	{
	}
	pp_context(const string &n, pp_scope_ptr s)
	    : m_name(n), m_scope(s), m_readonly(false)
	{
	}

	/*
	 * Take a read-only snapshot of the current context
	 */
	pp_context
	snapshot() const
	{
		pp_context tmp(*this);
		tmp.m_readonly = true;
		return tmp;
	}

	/*
	 * getter methods
	 */
	const string &
	name() const
	{
		return m_name;
	}
	pp_scope *
	scope()
	{
		return m_scope.get();
	}
	const pp_scope *
	scope() const
	{
		return m_scope.get();
	}
	bool
	is_valid() const
	{
		return m_scope ? true : false;
	}
	bool
	is_readonly() const
	{
		return m_readonly;
	}

	/*
	 * Encapsulate for ease of use.
	 */

	const pp_binding *
	binding() const
	{
		return m_scope->binding();
	}

	void
	add_datatype(const string &name, pp_const_datatype_ptr datatype)
	{
		m_scope->add_datatype(name, datatype);
	}
	void
	add_datatype(pp_const_datatype_ptr datatype)
	{
		m_scope->add_datatype(datatype);
	}

	const pp_datatype *
	resolve_datatype(const string &name) const
	{
		return m_scope->resolve_datatype(name);
	}

	void
	add_dirent(const pp_context &ctxt)
	{
		m_scope->add_dirent(ctxt.m_name, ctxt.m_scope);
	}
	void
	add_dirent(const string &name, pp_dirent_ptr dirent)
	{
		m_scope->add_dirent(name, dirent);
	}

	bool
	dirent_defined(const pp_path &path) const
	{
		return m_scope->dirent_defined(path);
	}

	const pp_dirent *
	lookup_dirent(pp_path path) const
	{
		return m_scope->lookup_dirent(path);
	}

	const pp_register *
	lookup_register(const pp_path &path) const
	{
		return m_scope->lookup_register(path);
	}

	const pp_field *
	lookup_field(const pp_path &path) const
	{
		return m_scope->lookup_field(path);
	}

	const pp_scope *
	lookup_scope(const pp_path &path) const
	{
		return m_scope->lookup_scope(path);
	}
};

#endif // PP_PP_CONTEXT_H__
